#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "OpenGLFunctions.h"
#include "AbstractNode.h"
#include "TransformNode.h"
#include "glm/glm.hpp"
#include "Light.h"
#include <vector>
#include <stack>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

namespace sgraph
{
  /**
 * This class represents a group node in the scenegraph. A group node is simply a logical grouping
 * of other nodes. It can have an arbitrary number of children. Its children can be nodes of any type
 * \author Amit Shesh
 */
  class GroupNode:public AbstractNode
  {
    /**
     * A list of its children
     */
  protected:
    vector<INode *> children;
    glm::vec4 minBounds;
    glm::vec4 maxBounds;
    glm::vec4 center;

  public:
    GroupNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
    }

    ~GroupNode()
    {
      for (int i=0;i<children.size();i++)
        {
          delete children[i];
        }
    }

    /**
     * Searches recursively into its subtree to look for node with specified name.
     * \param name name of node to be searched
     * \return the node whose name this is if it exists within this subtree, null otherwise
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
      {
        return n;
      }

      int i=0;
      INode *answer = NULL;

      while ((i<children.size()) && (answer == NULL))
      {
        answer = children[i]->getNode(name);
        i++;
      }
      return answer;
    }

    /**
     * Sets the reference to the scene graph object for this node, and then recurses down
     * to children for the same
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      for (int i=0;i<children.size();i++)
      {
        children[i]->setScenegraph(graph);
      }
    }

    void setCenterofBoundingBox(){
        glm::vec4 minBounds = this->getMinBounds();
        glm::vec4 maxBounds = this->getMaxBounds();
        center.x = (minBounds.x + maxBounds.x) / 2.0f;
        center.y = (minBounds.y + maxBounds.y) / 2.0f;
        center.z = (minBounds.z + maxBounds.z) / 2.0f;
        center.w = (minBounds.w + maxBounds.w) / 2.0f;
    }

    glm::vec4 getCenterofBoundingBox(){
        return center;
    }

    /**
     * To draw this node, it simply delegates to all its children
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
      for (int i=0;i<children.size();i++)
        {
          children[i]->draw(context,modelView);
        }
    }

    /**
     * To update this node's bounding box, it simply delegates to all its children
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */
    void updateBoundingBox(GLScenegraphRenderer& context, stack<glm::mat4>& modelView){
        vector<glm::vec4> boundingBox;
        for (int i = 0; i < children.size(); i++){
            children[i]->updateBoundingBox(context, modelView);
            boundingBox.push_back(children[i]->getMinBounds());
            boundingBox.push_back(children[i]->getMaxBounds());
        }
        glm::vec4 minBounds = boundingBox[0];
        glm::vec4 maxBounds = boundingBox[0];
        for (int i = 0; i < boundingBox.size(); i++)
        {
            glm::vec4 p = boundingBox[i];

            if (p.x < minBounds.x)
            {
                minBounds.x = p.x;
            }

            if (p.x > maxBounds.x)
            {
                maxBounds.x = p.x;
            }

            if (p.y < minBounds.y)
            {
                minBounds.y = p.y;
            }

            if (p.y > maxBounds.y)
            {
                maxBounds.y = p.y;
            }

            if (p.z < minBounds.z)
            {
                minBounds.z = p.z;
            }

            if (p.z > maxBounds.z)
            {
                maxBounds.z = p.z;
            }
        }
        this->setMinBounds(minBounds);
        this->setMaxBounds(maxBounds);
        this->setCenterofBoundingBox();;
    }

    void setMinBounds(glm::vec4 minBounds){
        this->minBounds.x = minBounds.x;
        this->minBounds.y = minBounds.y;
        this->minBounds.z = minBounds.z;
        this->minBounds.w = minBounds.w;
    }
    void setMaxBounds(glm::vec4 maxBounds){
        this->maxBounds.x = maxBounds.x;
        this->maxBounds.y = maxBounds.y;
        this->maxBounds.z = maxBounds.z;
        this->maxBounds.w = maxBounds.w;
    }

    glm::vec4 getMinBounds(){
        return this->minBounds;
    }
    glm::vec4 getMaxBounds(){
        return this->maxBounds;
    }

    /**
     * Makes a deep copy of the subtree rooted at this node
     * \return a deep copy of the subtree rooted at this node
     */
    INode *clone()
    {
      vector<INode *> newc;

      for (int i=0;i<children.size();i++)
        {
          newc.push_back(children[i]->clone());
        }

      GroupNode *newgroup = new GroupNode(scenegraph,name);

      for (int i=0;i<children.size();i++)
        {
          try
          {
            newgroup->addChild(newc[i]);
          }
          catch (runtime_error e)
          {

          }
        }
      return newgroup;
    }

    /**
     * Since a group node is capable of having children, this method overrides the default one
     * in sgraph::AbstractNode and adds a child to this node
     * \param child
     * \throws runtime_error this class does not throw this exception
     */
    void addChild(INode *child) throw(runtime_error)
    {
      children.push_back(child);
      child->setParent(this);
    }

    /**
     * Get a list of all its children, for convenience purposes
     * \return a list of all its children
     */

    vector<INode *> getChildren()
    {
      return children;
    }

    std::vector<HitRecord> raycast(Ray ray, stack<glm::mat4>& modelView, std::vector<HitRecord> hitrecords){

        for (int i = 0; i < children.size(); i++){
            std::vector<HitRecord> childHitRecordList;
            std::vector<HitRecord> childHitRecord = this->children[i]->raycast(ray, modelView, childHitRecordList);

            if (!childHitRecord.empty())
            {
                for(std::vector<HitRecord>::iterator it = childHitRecord.begin(); it != childHitRecord.end(); it++)
                {
                    HitRecord tempHitRecord = *it;
//                    if (tempHitRecord.getIntersectPoint() == glm::vec4(0.0f))
//                    {
//                        continue;
//                    }
                    hitrecords.push_back(tempHitRecord);
                }
            }
        }
        return hitrecords;
    }
  };
}

#endif
