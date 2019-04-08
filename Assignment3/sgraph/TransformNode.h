#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "Light.h"
using namespace std;
#include <vector>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

  /**
 * This node represents a transformation in the scene graph. It has only one child. The transformation
 * can be viewed as changing from its child's coordinate system to its parent's coordinate system
 * This also stores an animation transform that can be tweaked at runtime
 * \author Amit Shesh
 */
  class TransformNode: public AbstractNode
  {
    /**
     * Matrices storing the static and animation transformations separately, so that they can be
     * changed separately
     */
  protected:
    glm::mat4 transform,animation_transform;

    /**
     * A reference to its only child
     */
    INode *child;

    glm::vec4 minBounds;
    glm::vec4 maxBounds;
    glm::vec4 center;

  public:
    TransformNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
      this->transform = glm::mat4(1.0);
      animation_transform = glm::mat4(1.0);
      child = NULL;
    }

    ~TransformNode()
    {
        if (child!=NULL)
            delete child;
    }

    INode *clone()
    {
      INode *newchild;

      if (child!=NULL)
        {
          newchild = child->clone();
        }
      else
        {
          newchild = NULL;
        }

      TransformNode *newtransform = new TransformNode(scenegraph,name);
      newtransform->setTransform(this->transform);
      newtransform->setAnimationTransform(animation_transform);

      if (newchild!=NULL)
        {
            newtransform->addChild(newchild);
        }
      return newtransform;
    }

    /**
     * Determines if this node has the specified name and returns itself if so. Otherwise it recurses
     * into its only child
     * \param name name of node to be searched
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
        return n;

      if (child!=NULL)
        {
          return child->getNode(name);
        }

      return NULL;
    }

    /**
     * Since this node can have a child, it override this method and adds the child to itself
     * This will overwrite any children set for this node previously.
     * \param child the child of this node
     * \throws runtime_error if a child already exists
     */
    void addChild(INode *child) throw(runtime_error)
    {
      if (this->child!=NULL)
        throw runtime_error("Transform node already has a child");
      this->child = child;
      this->child->setParent(this);
    }

    /**
     * Draws the scene graph rooted at this node
     * After preserving the current top of the modelview stack, this "post-multiplies" its
     * animation transform and then its transform in that order to the top of the model view
     * stack, and then recurses to its child. When the child is drawn, it restores the modelview
     * matrix
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */

    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
      modelView.push(modelView.top());
      modelView.top() = modelView.top()
          * animation_transform
          * transform;
      if (child!=NULL)
        child->draw(context,modelView);
      modelView.pop();
    }

    /**
     * Update the scene graph's bounding box rooted at this node
     * After preserving the current top of the modelview stack, this "post-multiplies" its
     * animation transform and then its transform in that order to the top of the model view
     * stack, and then recurses to its child. When the child's bounding box is drawn,
     * it restores the modelview matrix
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */

    void updateBoundingBox(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top()
            * animation_transform
            * transform;
        if (child!=NULL){
          child->updateBoundingBox(context,modelView);
          glm::vec4 maxBounds = child->getMaxBounds();
          glm::vec4 minBounds = child->getMinBounds();
          vector<glm::vec4> boundingBox;//a bounding Box has 8 vertices(just like a box)
          // using maxBounds and minBounds to create the 8 vertices of bounding box
          // assuming the box is right in the front of us.
          // first : the left-front-bottom vertice
          boundingBox.push_back(glm::vec4(minBounds.x, minBounds.y, maxBounds.z, 1));
          // second : the left-front-top vertice
          boundingBox.push_back(glm::vec4(minBounds.x, maxBounds.y, maxBounds.z, 1));
          // third : the right-front-top vertice
          boundingBox.push_back(glm::vec4(maxBounds.x, maxBounds.y, maxBounds.z, 1));
          // 4th : the right-front-bottom vertice
          boundingBox.push_back(glm::vec4(maxBounds.x, minBounds.y, maxBounds.z, 1));
          // 5th : the left-back-bottom vertice
          boundingBox.push_back(glm::vec4(minBounds.x, minBounds.y, minBounds.z, 1));
          // 6th : the left-back-top vertice
          boundingBox.push_back(glm::vec4(minBounds.x, maxBounds.y, minBounds.z, 1));
          // 7th : the right-back-top vectice
          boundingBox.push_back(glm::vec4(maxBounds.x, maxBounds.y, minBounds.z, 1));
          // 8th : the right-back-bottom vertice
          boundingBox.push_back(glm::vec4(maxBounds.x, minBounds.y, minBounds.z, 1));
          for(int i = 0; i < boundingBox.size(); i++){
              boundingBox[i] = animation_transform * transform * boundingBox[i];
          }
          // recalculate the bounding box
          minBounds = boundingBox[0];
          maxBounds = boundingBox[0];

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

          // reset the bounding Box
          this->setMinBounds(minBounds);
          this->setMaxBounds(maxBounds);
          this->setCenterofBoundingBox();
        }
        modelView.pop();
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
     * Sets the animation transform of this node
     * \param mat the animation transform of this node
     */
    void setAnimationTransform(const glm::mat4& mat) throw(runtime_error)
    {
      animation_transform = mat;
    }

    /**
     * Gets the transform at this node (not the animation transform)
     */
    glm::mat4 getTransform()
    {
      return transform;
    }

    /**
     * Sets the transformation of this node
     * \param t
     * \throws runtime_error this implementation does not throw this exception
     */
    void setTransform(const glm::mat4& t) throw(runtime_error)
    {
      this->transform = t;
    }

    /**
     * Gets the animation transform of this node
     * \return
     */
    glm::mat4 getAnimationTransform()
    {
      return animation_transform;
    }

    /**
     * Sets the scene graph object of which this node is a part, and then recurses to its child
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      if (child!=NULL)
        {
          child->setScenegraph(graph);
        }
    }

    std::vector<HitRecord> raycast(Ray ray, stack<glm::mat4>& modelView, std::vector<HitRecord> hitrecords)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top()
            * animation_transform
            * transform;
        if (child!=NULL)
        {
            std::vector<HitRecord> childHitRecordList;
            std::vector<HitRecord> childHitRecord = child->raycast(ray, modelView, childHitRecordList);
            //cout << "Transform Node : " << "childHitRecord size : " << childHitRecord.size() << endl;
            if (!childHitRecord.empty())
            {
                for(std::vector<HitRecord>::iterator it = childHitRecord.begin(); it != childHitRecord.end(); it++)
                {
                    HitRecord tempHitRecord = *it;
                    hitrecords.push_back(tempHitRecord);
                }
            }
        }
        modelView.pop();
        return hitrecords;
    }

  };
}
#endif
