#ifndef _SCENEXMLREADER_H_
#define _SCENEXMLREADER_H_

#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <QXmlDefaultHandler>
#include <qxml.h>
#include "ObjImporter.h"
#include "INode.h"
#include "TransformNode.h"
#include "LeafNode.h"
#include "GroupNode.h"
#include "ScenegraphInfo.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

namespace sgraph
{
  template <class T> class MyHandler;
  /**
 * A SAX parser for parsing the scene graph and compiling an {@link
 * sgraph.IScenegraph} object from it.
 *
 * \author Amit Shesh
 */
  class SceneXMLReader {
  public:
    template <class K>
    static sgraph::ScenegraphInfo<K> importScenegraph(const string& filename) throw(runtime_error)
    {

      MyHandler<K> handler;
      QFile xmlFile(QString::fromStdString(filename));
      if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw runtime_error("Could not open file: "+filename);
      QXmlInputSource source(&xmlFile);

      QXmlSimpleReader reader;


      reader.setContentHandler(&handler);

      bool answer = reader.parse(source);

      sgraph::ScenegraphInfo<K> info;
      info.scenegraph = NULL;

      if (answer)
        {
          info.scenegraph = handler.getScenegraph();
          info.meshes = handler.getMeshes();
          info.lights = handler.getLights();
        }
      else
        {
          printf("Parsing unsuccessful because %s\n",handler.errorString().toLatin1().constData());
        }

      return info;
    }
  };



  template<class K>
  class MyHandler: public QXmlDefaultHandler
  {
  private:
    sgraph::Scenegraph *scenegraph;
    map<string,util::PolygonMesh<K>> meshes;
    INode *node;
    TransformNode *transformnode;
    stack<INode *> stackNodes;
    stack<TransformNode *> transformNodes;
    std::vector<util::Light> Lights;
    glm::mat4 transform;
    util::Material material;
    util::Light light;
    map<string, sgraph::INode *> subgraph;
    vector<float> data;
    bool lightSignal;// using to specify the light and material

  public:
    sgraph::Scenegraph *getScenegraph() {
      return scenegraph;
    }

    map<string,util::PolygonMesh<K>> getMeshes()
    {
      return meshes;
    }

    std::vector<util::Light> getLights(){
        return Lights;
    }

    MyHandler()
    {
    }

    bool startDocument()
    {
      node = NULL;
      scenegraph = new sgraph::Scenegraph();
      transform = glm::mat4(1.0);
      //light = util::Light();
      lightSignal = false;
      return true;
    }

    bool startElement( const QString & namespaceURI,const QString & localName,const QString & qName,const QXmlAttributes & atts)
    {
      if (qName.compare("scene")==0)
        {
          stackNodes.push(new sgraph::GroupNode(scenegraph, "Root of scene graph"));
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("group")==0)
        {
          string name = "";
          string copyof = "";
          string fromfile = "";

          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                name = atts.value(i).toLatin1().constData();
              else if (atts.qName(i).compare("copyof")==0)
                copyof = atts.value(i).toLatin1().constData();
              else if (atts.qName(i).compare("from")==0)
                fromfile = atts.value(i).toLatin1().constData();
            }
          if ((copyof.length() > 0) && (subgraph.count(copyof)==1))
            {
              node = subgraph[copyof]->clone();
              node->setName(name);
            }
          else if (fromfile.length() > 0)
            {
              sgraph::ScenegraphInfo<K> tempsginfo;
              tempsginfo = sgraph::SceneXMLReader::importScenegraph<K>(fromfile);

              std::vector<util::Light> childlights = tempsginfo.lights;

              // leave room for lights
              for(std::vector<util::Light>::iterator i = childlights.begin();
                  i != childlights.end();
                  i++){
                  stack<TransformNode *> temptransformnodes = transformNodes;
                  while(!temptransformnodes.empty()){
                      // we need to transform the childlight's position/direction, and spotdirection

                      // transform the light's position/direction
                      glm::vec4 childlightpos = i->getPosition();
                      childlightpos = temptransformnodes.top()->getAnimationTransform() *
                              temptransformnodes.top()->getTransform() *
                              childlightpos;
                      if(childlightpos.w != 0)
                          i->setPosition(childlightpos.x, childlightpos.y, childlightpos.z);
                      else
                          i->setDirection(childlightpos.x, childlightpos.y, childlightpos.z);

                      // transform the light's spotdirection, if it is not a spotlight, the vector will be(0, 0, 0, 0)
                      glm::vec4 childlightspotdirection = i->getSpotDirection();
                      childlightspotdirection = temptransformnodes.top()->getAnimationTransform() *
                              temptransformnodes.top()->getTransform() *
                              childlightspotdirection;
                      i->setSpotDirection(childlightspotdirection.x, childlightspotdirection.y, childlightspotdirection.z);

                      util::Light childlight = util::Light();
                      // set the childlight's position/direction
                      if(i->getPosition().w != 0)
                          childlight.setPosition(i->getPosition().x, i->getPosition().y, i->getPosition().z);
                      else
                          childlight.setDirection(i->getPosition().x, i->getPosition().y, i->getPosition().z);

                      // set the childlight's ambient
                      childlight.setAmbient(i->getAmbient().x, i->getAmbient().y, i->getAmbient().z);

                      // set the childlight's diffuse
                      childlight.setDiffuse(i->getDiffuse().x, i->getDiffuse().y, i->getDiffuse().z);

                      // set the childlight's specular
                      childlight.setSpecular(i->getSpecular().x, i->getSpecular().y, i->getSpecular().z);

                      // set the childlight's spotdirection
                      childlight.setSpotDirection(i->getSpotDirection().x, i->getSpotDirection().y, i->getSpotDirection().z);

                      // set the childlight's spotangle
                      childlight.setSpotAngle(i->getSpotCutoff());

                      //push the new i into the Lights
                      Lights.push_back(childlight);

                      // pop the transformation stack
                      temptransformnodes.pop();
                  }
              }

              node = new sgraph::GroupNode(scenegraph,name);

              for (typename map<string,util::PolygonMesh<K>>::iterator it=tempsginfo.meshes.begin();
                   it!=tempsginfo.meshes.end();it++)
                {
                  meshes[it->first] = it->second;
                }
              //rename all the nodes in tempsg to prepend with the name of the group node
              map<string, INode *> nodes = tempsginfo.scenegraph->getNodes();
              for (map<string,INode *>::iterator it=nodes.begin();it!=nodes.end();it++)
                {
                  //cout << it->first << endl;
                  it->second->setName(it->second->getName());
                  scenegraph->addNode(it->second->getName(), it->second);
                }

              node->addChild(tempsginfo.scenegraph->getRoot());
            }
          else
            {
              node = new sgraph::GroupNode(scenegraph, name);
            }
          stackNodes.top()->addChild(node);

          stackNodes.push(node);
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("transform")==0)
        {
          string name = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                name = atts.value(i).toLatin1().constData();
                //cout << "transform node name :" << name << endl;
            }
          node = new sgraph::TransformNode(scenegraph, name);
          stackNodes.top()->addChild(node);

          stackNodes.push(node);
          transformnode = new sgraph::TransformNode(scenegraph, name);
          transformNodes.push(transformnode);
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("object")==0)
        {
          string name = "";
          string objectname = "";
          string textureName;
          bool hasTexture = false;
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                {
                  name = atts.value(i).toLatin1().constData();
                }
              else if (atts.qName(i).compare("instanceof")==0)
                {
                  objectname = atts.value(i).toLatin1().constData();
                }
              else if (atts.qName(i).compare("texture")==0)
                {
                  textureName = atts.value(i).toLatin1().constData();
                  cout << "Read Textures : "
                       << textureName << endl;
                  hasTexture = true;
                }
            }
          if (objectname.length() > 0)
            {
              node = new sgraph::LeafNode(objectname, scenegraph, name);
              if (hasTexture){
                  node->setTextureName(textureName);
              } else {
                  node->setTextureName("white");
              }
              glm::vec4 minBounds = meshes[objectname].getMinimumBounds();
              glm::vec4 maxBounds = meshes[objectname].getMaximumBounds();
              // set the bounding box for the leaf node.
              node->setMinBounds(minBounds);
              node->setMaxBounds(maxBounds);
//              cout << "Read Leaf Node!\n";
//              cout << "Leaf node's Bounding Box: \n";
//              cout << "x bound : " << node->getMinBounds().x << " to " << node->getMaxBounds().x << endl;
//              cout << "y bound : " << node->getMinBounds().y << " to " << node->getMaxBounds().y << endl;
//              cout << "z bound : " << node->getMinBounds().y << " to " << node->getMaxBounds().z << endl;

              stackNodes.top()->addChild(node);

              stackNodes.push(node);
              subgraph[stackNodes.top()->getName()] = stackNodes.top();
            }
        }
      else if (qName.compare("instance")==0)
        {
          string name = "";
          string path = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                {
                  name = atts.value(i).toLatin1().constData();
                }
              else if (atts.qName(i).compare("path")==0)
                {
                  path = atts.value(i).toLatin1().constData();
                  if (path.substr(path.length()-4).compare(".obj")!=0)
                    path = path + ".obj";
                }
            }
          if ((name.length() > 0) && (path.length() > 0))
            {
              util::PolygonMesh<K> mesh;
              ifstream in(path.c_str());
              mesh = util::ObjImporter<K>::importFile(in, false);
              meshes[name] = mesh;
            }
        }
      else if (qName.compare("image")==0)
        {
          string name = "";
          string path = "";
          for (int i = 0; i < atts.count(); i++)
          {
            if (atts.qName(i).compare("name")==0)
            {
              name = atts.value(i).toLatin1().constData();
            }
            else if (atts.qName(i).compare("path")==0)
            {
              path = atts.value(i).toLatin1().constData();
            }
          }
          if ((name.length()>0) && (path.length()>0))
          {
            scenegraph->addTexture(name,path);
            cout << "Scenegraph addTextures!\n";
          }
        }
      else if (qName.compare("light") == 0){
          lightSignal = true;
      }

      return true;
    }

    bool endElement( const QString&, const QString&, const QString& qName)
    {
      if (qName.compare("scene")==0)
        {
          if (stackNodes.top()->getName().compare("Root of scene graph")==0)
            scenegraph->makeScenegraph(stackNodes.top());
        }
      else if ((qName.compare("group")==0) ||
               (qName.compare("object")==0))
        {
          stackNodes.pop();
        }
      else if (qName.compare("transform") == 0){
          stackNodes.pop();
          transformNodes.pop();
      }
      else if (qName.compare("set")==0)
        {
          stackNodes.top()->setTransform(transform);
          transformNodes.top()->setTransform(transform);
          transform = glm::mat4(1.0);
        }
      else if (qName.compare("scale")==0)
        {
          if (data.size()!=3)
            return false;
          transform = transform * glm::scale(glm::mat4(1.0),glm::vec3(data[0],data[1],data[2]));
          data.clear();
        }
      else if (qName.compare("rotate")==0)
        {
          if (data.size()!=4)
            return false;
          transform = transform * glm::rotate(glm::mat4(1.0),
                                              glm::radians(data[0]),
              glm::vec3(data[1],data[2],data[3]));
          data.clear();
        }
      else if (qName.compare("translate")==0)
        {
          if (data.size()!=3)
            return false;
          transform = transform * glm::translate(glm::mat4(1.0),glm::vec3(data[0],data[1],data[2]));
          data.clear();
        }
      else if (qName.compare("light") == 0){
         Lights.push_back(light);
         light = util::Light();
         lightSignal = false;//reset light signal
         //cout << "light traverse\n";
      }
      else if ((qName.compare("ambient") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          light.setAmbient(data[0], data[1], data[2]);
          data.clear();
      }
      else if ((qName.compare("diffuse") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          light.setDiffuse(data[0], data[1], data[2]);
          data.clear();
      }
      else if ((qName.compare("specular") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          light.setSpecular(data[0], data[1], data[2]);
          data.clear();
      }
      else if ((qName.compare("position") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          stack<TransformNode *> temptransformNodes;
          temptransformNodes = transformNodes;
          glm::vec4 lightpos = glm::vec4(data[0], data[1], data[2], 1);
          while (! temptransformNodes.empty()){
              lightpos = temptransformNodes.top()->getAnimationTransform() *
                      temptransformNodes.top()->getTransform() * lightpos;
              temptransformNodes.pop();
              //cout << "light pos get transformed\n";
          }// leave room for animation_transform
          light.setPosition(lightpos.x, lightpos.y, lightpos.z);

          data.clear();
      }
      else if ((qName.compare("spotangle") == 0) && (lightSignal == true)){
          if (data.size() != 1)
              return false;
          light.setSpotAngle(data[0]);
          data.clear();
      }
      else if ((qName.compare("direction") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          stack<TransformNode *> temptransformNodes;
          temptransformNodes = transformNodes;
          glm::vec4 lightdirection = glm::vec4(data[0], data[1], data[2], 0);
          while (! temptransformNodes.empty()){
              lightdirection = temptransformNodes.top()->getAnimationTransform() *
                      temptransformNodes.top()->getAnimationTransform() * lightdirection;
              temptransformNodes.pop();
              //cout << "light direction transformed\n";
          }// leave room for animation_transform
          light.setDirection(lightdirection.x, lightdirection.y, lightdirection.z);
          data.clear();
      }
      else if ((qName.compare("spotdirection") == 0) && (lightSignal == true)){
          if (data.size() != 3)
              return false;
          stack<TransformNode *> temptransformNodes;
          temptransformNodes = transformNodes;
          glm::vec4 spotdirection = glm::vec4(data[0], data[1], data[2], 0);
          while (! temptransformNodes.empty()){
              spotdirection = temptransformNodes.top()->getAnimationTransform() *
                      temptransformNodes.top()->getAnimationTransform() * spotdirection;
              temptransformNodes.pop();
              //cout << "spot direction transformed\n";
          }// leave room for animation_transform
          light.setSpotDirection(spotdirection.x, spotdirection.y, spotdirection.z);
          data.clear();
      }
      else if (qName.compare("material")==0)
        {
          stackNodes.top()->setMaterial(material);
          material = util::Material();
        }
      else if (qName.compare("color")==0)
        {
          if (data.size()!=3)
            return false;
          material.setAmbient(data[0],data[1],data[2]);
          material.setEmission(material.getEmission());
          material.setDiffuse(material.getDiffuse());
          material.setSpecular(material.getSpecular());
          material.setShininess(material.getShininess());
          data.clear();
        }
      else if ((qName.compare("ambient") == 0) && (lightSignal == false))
        {
          if (data.size()!=3)
            return false;
          material.setAmbient(data[0],data[1],data[2]);
          data.clear();
        }
      else if ((qName.compare("diffuse") == 0) && (lightSignal == false))
        {
          if (data.size()!=3)
            return false;
          material.setDiffuse(data[0],data[1],data[2]);
          data.clear();
        }
      else if ((qName.compare("specular") == 0) && (lightSignal == false))
        {
          if (data.size()!=3)
            return false;
          material.setSpecular(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("emissive")==0)
        {
          if (data.size()!=3)
            return false;
          material.setEmission(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("shininess")==0)
        {
          if (data.size()!=1)
            return false;
          material.setShininess(data[0]);
          data.clear();
        }
      else if (qName.compare("absorption")==0)
        {
          if (data.size()!=1)
            return false;
          material.setAbsorption(data[0]);
          data.clear();
        }
      else if (qName.compare("reflection")==0)
        {
          if (data.size()!=1)
            return false;
          material.setReflection(data[0]);
          data.clear();
        }
      else if (qName.compare("transparency")==0)
        {
          if (data.size()!=1)
            return false;
          material.setTransparency(data[0]);
          data.clear();
        }
      else if (qName.compare("refractive")==0)
        {
          if (data.size()!=1)
            return false;
          material.setRefractiveIndex(data[0]);
          data.clear();
        }
    return true;

    }


    bool characters(const QString& text)
    {
      int c;
      float f;
      QString info;

      info = text;
      info.remove(QRegExp("[\n\t\r]*")); //remove tabs and newlines

      QStringList list = info.split(' ',QString::SkipEmptyParts);


      for (int i=0;i<list.length();i++)
        {
          c=sscanf(list.at(i).toLatin1().constData(),"%f",&f);
          //printf("Read number: %f\n",f);
          data.push_back(f);
          if (c!=1)
            return true;

        }

      //all the data numbers are in the data array

      return true;
    }

  };
}
#endif





