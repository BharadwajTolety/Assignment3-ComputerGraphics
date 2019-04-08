#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "GLScenegraphRenderer.h"
#include "INode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "IVertexData.h"
#include "PolygonMesh.h"
#include <string>
#include <map>
using namespace std;

namespace sgraph
{

  /**
 * A specific implementation of this scene graph. This implementation is still independent
 * of the rendering technology (i.e. OpenGL)
 * \author Amit Shesh
 */

  class Scenegraph
  {
    /**
     * The root of the scene graph tree
     */
  protected:
    INode *root;



    /**
     * A map to store the (name,node) pairs. A map is chosen for efficient search
     */
    map<string,INode *> nodes;

    map<string,string> textures;

    std::vector<util::Light> lights;
    /**
     * The associated renderer for this scene graph. This must be set before attempting to
     * render the scene graph
     */
    GLScenegraphRenderer *renderer;

    std::vector<HitRecord> hitrecords;

  public:
    Scenegraph()
    {
      root = NULL;
    }

    ~Scenegraph()
    {
      dispose();
    }

    void dispose()
    {

      if (root!=NULL)
        {
          delete root;
          root = NULL;
        }
    }


    void setLights(std::vector<util::Light> lights){
        this->lights = lights;
    }

    /**
     * Sets the renderer, and then adds all the meshes to the renderer.
     * This function must be called when the scene graph is complete, otherwise not all of its
     * meshes will be known to the renderer
     * \param renderer The IScenegraphRenderer object that will act as its renderer
     * \throws Exception
     */
    template <class VertexType>
    void setRenderer(GLScenegraphRenderer *renderer,map<string,
                     util::PolygonMesh<VertexType> >& meshes) throw(runtime_error)
    {
      this->renderer = renderer;

      //now add all the meshes
      for (typename map<string,util::PolygonMesh<VertexType> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++)
        {
          this->renderer->addMesh<VertexType>(it->first,it->second);
        }
      for (auto tex_pair: textures){
          //textures
          const string name = tex_pair.first;
          const string path = tex_pair.second;
          this->renderer->addTexture(name, path);
      }
    }


    /**
     * Set the root of the scenegraph, and then pass a reference to this scene graph object
     * to all its node. This will enable any node to call functions of its associated scene graph
     * \param root
     */

    void makeScenegraph(INode *root)
    {
      this->root = root;
      this->root->setScenegraph(this);

    }

    /**
     * Draw this scene graph. It delegates this operation to the renderer
     * \param modelView
     */
    void draw(stack<glm::mat4>& modelView) {
      if ((root!=NULL) && (renderer!=NULL))
        {
          renderer->draw(root,modelView);
        }
    }

    void animate(float time)
    {

    }

    void addNode(const string& name, INode *node) {
      nodes[name]=node;
    }


    INode *getRoot()
    {
      return root;
    }

    /**
     * Update this scene graph's bounding box. It delegates this operation to the renderer
     * \param modelView
     */
    void updateBoundingBox(stack<glm::mat4>& modelView)
    {
        if((root != NULL) && (renderer != NULL))
        {
            renderer->updateBoundingBox(root, modelView);
        }
    }

    map<string, INode *> getNodes()
    {
      return nodes;
    }

    void addTexture(const string& name, const string& path)
    {
      textures[name] = path;
    }

    QColor shade(HitRecord hitrecord, stack<glm::mat4> &modelView)
    {

        if (!lights.empty())
        {
            std::vector<util::Light> lightsCopy = this->lights;
            int lightsNum = lightsCopy.size();
            glm::vec4 position = hitrecord.getIntersectPoint();
            glm::vec4 normal = hitrecord.getNormal();
            glm::vec4 textCoordinate = hitrecord.getTextureCoordinate();
            glm::mat4 textureMatrix = hitrecord.getTextureMatrix();
            util::Material material = hitrecord.getMaterial();
            glm::vec4 lightVec = glm::vec4(1.0);
            glm::vec4 tNormal = glm::vec4(1.0);
            glm::vec4 normalView = glm::vec4(1.0);
            glm::vec4 viewVec = glm::vec4(1.0);
            glm::vec4 reflectVec = glm::vec4(1.0);
            glm::vec4 spotlightVec = glm::vec4(1.0);
            glm::vec4 ambient = glm::vec4(1.0);
            glm::vec4 diffuse = glm::vec4(1.0);
            glm::vec4 specular = glm::vec4(1.0);
            glm::vec4 fColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
            float sDotL, nDotL, rDotV = 0.0;
            int survival = 0;
            string name = hitrecord.getTextureName();
            string path = textures[name];
            util::TextureImage image = hitrecord.getTextureImage();

            textCoordinate = textureMatrix * glm::vec4(1.0 * textCoordinate.x, 1.0 * textCoordinate.y, 0.0, 1.0);
            float attenuation = 1.0f;
            float distance = 1.0f;
            for(std::vector<util::Light>::iterator it = lightsCopy.begin();
                it != lightsCopy.end();
                it++){
                util::Light light = *it;
                light.setPosition(modelView.top() * light.getPosition());
                if (light.getPosition().w!=0){
                    lightVec = glm::normalize(glm::vec4((light.getPosition().x - position.x) * 1.0f,
                                                        (light.getPosition().y - position.y) * 1.0f,
                                                        (light.getPosition().z - position.z) * 1.0f,
                                                        0.0f));

                }
                else{
                    lightVec = glm::normalize(glm::vec4(0.0f - light.getPosition().x,
                                                        0.0f - light.getPosition().y,
                                                        0.0f - light.getPosition().z,
                                                        0.0f));
                }

                tNormal = normal;
                normalView = glm::normalize(glm::vec4(tNormal.x,
                                                      tNormal.y,
                                                      tNormal.z,
                                                      0.0f));
                nDotL = glm::dot(normalView,lightVec);              // cos for REFLECTION

                viewVec = glm::normalize(glm::vec4(- position.x,
                                    - position.y,
                                    - position.z,
                                    0.0f));                         // from light to eye

                reflectVec = glm::normalize(glm::reflect(-lightVec,normalView));

                rDotV = max(glm::dot(reflectVec, viewVec), 0.0f);   // cos for SPECULAR

                ambient = glm::vec4(material.getAmbient().x * light.getAmbient().x,
                                    material.getAmbient().y * light.getAmbient().y,
                                    material.getAmbient().z * light.getAmbient().z,
                                    1.0f);
                diffuse = glm::vec4(material.getDiffuse().x * light.getDiffuse().x * max(nDotL, 0.0f) * 1.0f,
                                    material.getDiffuse().y * light.getDiffuse().y * max(nDotL, 0.0f) * 1.0f,
                                    material.getDiffuse().z * light.getDiffuse().z * max(nDotL, 0.0f) * 1.0f,
                                    1.0f);

                if (nDotL > 0)
                    specular = glm::vec4(material.getSpecular().x * light.getSpecular().x * pow(rDotV,material.getShininess()),
                                         material.getSpecular().y * light.getSpecular().y * pow(rDotV,material.getShininess()),
                                         material.getSpecular().z * light.getSpecular().z * pow(rDotV,material.getShininess()),
                                         1.0f);
                else
                    specular = glm::vec4(0, 0, 0, 0);

                if (light.getSpotCutoff() != 0){
                    spotlightVec = glm::normalize(glm::vec4(- lightVec.x,
                                                            - lightVec.y,
                                                            - lightVec.z,
                                                            - lightVec.w));
                    sDotL = glm::dot(spotlightVec, glm::normalize(light.getSpotDirection()));
                    if(sDotL >= cos((light.getSpotCutoff() / 2)))
                        survival = 1;
                    else
                        survival = 0;
                } else {
                    survival = 1;
                }

                glm::vec4 shadowPoint = glm::vec4(position.x, position.y, position.z, 1.0f);
                glm::vec4 shadowDirection = glm::vec4(0.0f);
                float shadowDistance = 0.0f;
                int shadowSurvival = 1;
                if (light.getPosition().w == 0.0f){
                    shadowDirection = glm::vec4(0.0f - light.getPosition().x,
                                                0.0f - light.getPosition().y,
                                                0.0f - light.getPosition().z,
                                                0.0f);
                } else {
                    shadowDirection = glm::vec4(light.getPosition().x - position.x,
                                                light.getPosition().y - position.y,
                                                light.getPosition().z - position.z,
                                                0.0f);
                    shadowDistance = sqrt((light.getPosition().x - position.x) * (light.getPosition().x - position.x)
                                          + (light.getPosition().y - position.y) * (light.getPosition().y - position.y)
                                          + (light.getPosition().z - position.z) * (light.getPosition().z - position.z));
                }
                glm::vec4 unitFudgeValue = glm::normalize(shadowDirection);
                float fudgeDegree = 1.0f;
                shadowPoint = glm::vec4(shadowPoint.x + fudgeDegree * unitFudgeValue.x,
                                        shadowPoint.y + fudgeDegree * unitFudgeValue.y,
                                        shadowPoint.z + fudgeDegree * unitFudgeValue.z,
                                        1.0f);

                Ray shadowRay = Ray();
                shadowRay.setDirection(shadowDirection);
                shadowRay.setStartPoint(shadowPoint);
                if (!hitrecords.empty())
                {
                    hitrecords.clear();;
                }
                hitrecords = this->root->raycast(shadowRay, modelView, hitrecords);
                if (hitrecords.empty())
                {
                    shadowSurvival = 1;
                }
                else
                {
                    float time = hitrecords.front().getTime();// default value;
                    HitRecord intersectPoint = hitrecords.front();
                    HitRecord temphitrecord;
                    for(std::vector<HitRecord>::iterator hitIt = hitrecords.begin(); hitIt != hitrecords.end(); hitIt++)
                    {
                        temphitrecord = *hitIt;
                        float hitrecordTime = temphitrecord.getTime();
                        if (hitrecordTime <= time){
                            time = hitrecordTime;
                            intersectPoint.setIntersectPoint(temphitrecord.getIntersectPoint());
                            intersectPoint.setMaterial(temphitrecord.getMaterial());
                            intersectPoint.setNormal(temphitrecord.getNormal());
                            intersectPoint.setTextureCoordinate(temphitrecord.getTextureCoordinate());
                            intersectPoint.setTextureMatrix(temphitrecord.getTextureMatrix());
                            intersectPoint.setTextureName(temphitrecord.getTextureName());
                            intersectPoint.setTime(temphitrecord.getTime());
                            intersectPoint.setTextureImage(temphitrecord.getTextureImage());
                        }
                    }
                    float intersectDistance = sqrt((intersectPoint.getIntersectPoint().x - shadowPoint.x)
                                                   * (intersectPoint.getIntersectPoint().x - shadowPoint.x)
                                                   + (intersectPoint.getIntersectPoint().y - shadowPoint.y)
                                                   * (intersectPoint.getIntersectPoint().y - shadowPoint.y)
                                                   + (intersectPoint.getIntersectPoint().z - shadowPoint.z)
                                                   * (intersectPoint.getIntersectPoint().z - shadowPoint.z));
                    if (intersectDistance <= shadowDistance)
                    {
                        shadowSurvival = 0;
                    }
                    else
                    {
                        shadowSurvival = 1;
                    }
                }

//                survival = survival * shadowSurvival;

                if (survival == 1)
                {
                    fColor = fColor + glm::vec4(ambient.x + diffuse.x + specular.x,
                                                ambient.y + diffuse.y + specular.y,
                                                ambient.z + diffuse.z + specular.z,
                                                0.0);

                }
                else
                {
                    fColor = fColor;
                }
                fColor = fColor * image.getColor(textCoordinate.x, textCoordinate.y);
            }
            QColor color;
            if (fColor.x > 1.0f)
            {
                fColor.x = 1.0f;
            }
            if (fColor.y > 1.0f)
            {
                fColor.y = 1.0f;
            }
            if (fColor.z > 1.0f)
            {
                fColor.z = 1.0f;
            }
            color.setRgbF(fColor.x, fColor.y, fColor.z, 1.0f);

            return color;
        }
        else
        {
            QColor color;
            color.setRgbF(1.0f, 1.0f, 1.0f, 1.0f);
            return color;
        }
    }


    QColor raycast(Ray ray, stack<glm::mat4> &modelView, int bounce){
        if (bounce == 2){
            // return background color
            QColor color;
            color.setRgbF(1.0, 1.0, 1.0, 1.0);
            return color;
        }
        if(!hitrecords.empty())
        {
            hitrecords.clear();
        }
        if (this->root != NULL)
        {
            hitrecords = this->root->raycast(ray, modelView, hitrecords);
        }
        if(!hitrecords.empty())
        {
            float time = hitrecords.front().getTime();// default value;
            HitRecord intersectPoint = hitrecords.front();
            HitRecord hitrecord;
            for(std::vector<HitRecord>::iterator it = hitrecords.begin(); it != hitrecords.end(); it++)
            {
                hitrecord = *it;
                float hitrecordTime = hitrecord.getTime();
                if (hitrecordTime <= time){
                    time = hitrecordTime;
                    intersectPoint.setIntersectPoint(hitrecord.getIntersectPoint());
                    intersectPoint.setMaterial(hitrecord.getMaterial());
                    intersectPoint.setNormal(hitrecord.getNormal());
                    intersectPoint.setTextureCoordinate(hitrecord.getTextureCoordinate());
                    intersectPoint.setTextureMatrix(hitrecord.getTextureMatrix());
                    intersectPoint.setTextureName(hitrecord.getTextureName());
                    intersectPoint.setTime(hitrecord.getTime());
                    intersectPoint.setTextureImage(hitrecord.getTextureImage());
                }
            }// find the nearest point
            QColor color = shade(intersectPoint, modelView);
            if(intersectPoint.getMaterial().getReflection() > 0.0f){
                // if the object is reflective
                glm::vec4 normal = intersectPoint.getNormal();
                normal = glm::normalize(glm::vec4(normal.x,
                                                  normal.y,
                                                  normal.z,
                                                  0.0f));
                glm::vec4 incident = ray.getDirection();
                incident = glm::normalize(incident);
                glm::vec4 reflectDirection = glm::reflect(incident, normal);
                glm::vec4 unitFudgeDirection = glm::normalize(reflectDirection);
                float fudgeDegree = 1.0f;
                glm::vec4 reflectPoint = glm::vec4(intersectPoint.getIntersectPoint().x + fudgeDegree * unitFudgeDirection.x,
                                                   intersectPoint.getIntersectPoint().y + fudgeDegree * unitFudgeDirection.y,
                                                   intersectPoint.getIntersectPoint().z + fudgeDegree * unitFudgeDirection.z,
                                                   1.0f);
                Ray reflectRay;
                reflectRay.setStartPoint(reflectPoint);
                reflectRay.setDirection(reflectDirection);

//                QColor colorReflect = this->raycast(reflectRay, modelView, ++bounce);
                QColor colorReflect;
                colorReflect.setRedF(1.0f);
                colorReflect.setGreenF(1.0f);
                colorReflect.setBlueF(1.0f);

                float colorRed = intersectPoint.getMaterial().getAbsorption() * color.redF()
                        + intersectPoint.getMaterial().getReflection() * colorReflect.redF();
                float colorGreen = intersectPoint.getMaterial().getAbsorption() * color.greenF()
                        + intersectPoint.getMaterial().getReflection() * colorReflect.greenF();
                float colorBlue = intersectPoint.getMaterial().getAbsorption() * color.blueF()
                        + intersectPoint.getMaterial().getReflection() * colorReflect.blueF();
                color.setRgbF(colorRed, colorGreen, colorBlue, 1.0f);
            } else {
                //color = color;
            }
            return color;
        } else {
            // return background color
            if (bounce == 1){
                //cout << "reflection not hit!\n";
            }
            QColor color;
            color.setRgbF(1.0, 1.0, 1.0, 1.0);
            return color;
        }
    }

  };
}
#endif
