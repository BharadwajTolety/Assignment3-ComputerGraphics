#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractNode
{
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;

    util::Light light;

    string textureName;

    util::TextureImage textureImage;

    glm::vec4 minBounds;// store Bounding Box in LeafNodes

    glm::vec4 maxBounds;

    glm::vec4 center;

    glm::mat4 texturematrix;

    bool existTextureTransform;

public:
    LeafNode(const string& instanceOf,sgraph::Scenegraph *graph,const string& name)
        :AbstractNode(graph,name), existTextureTransform(false)
    {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) throw(runtime_error)
    {
        material = mat;
    }

    /**
     * Set texture ID of the texture to be used for this leaf
     * \param name
     */
    void setTextureName(const string& name) throw(runtime_error)
    {
        textureName = name;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    INode *clone()
    {
        LeafNode *newclone = new LeafNode(this->objInstanceName,scenegraph,name);
        newclone->setMaterial(this->getMaterial());
        newclone->setTextureName(this->textureName);
        newclone->setTextureMatrix(this->texturematrix);
        newclone->setExistTextureTransform(this->existTextureTransform);
        return (INode*)newclone;
    }

    void setMinBounds(glm::vec4 boundingBoxMin){
        this->minBounds.x = boundingBoxMin.x;
        this->minBounds.y = boundingBoxMin.y;
        this->minBounds.z = boundingBoxMin.z;
        this->minBounds.w = boundingBoxMin.w;
    }


    /**
     * Delegates to the scene graph for rendering. This has two advantages:
     * <ul>
     *     <li>It keeps the leaf light.</li>
     *     <li>It abstracts the actual drawing to the specific implementation of the scene graph renderer</li>
     * </ul>
     * \param context the generic renderer context {@link sgraph.IScenegraphRenderer}
     * \param modelView the stack of modelview matrices
     * \throws runtime_error
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView) throw(runtime_error)
    {
        if (objInstanceName.length()>0)
        {
            context.drawMesh(objInstanceName,material,textureName,modelView.top(),texturematrix);
            this->textureImage = context.getTextureImage(this->textureName);
        }
    }

    /**
     * Delegates to the scene graph for updateing mesh bounding box.
     * \param context the generic renderer context
     * \param modelView the stack of modelview matrices
     * \throws runtime_error
     */
    void updateBoundingBox(GLScenegraphRenderer& context,stack<glm::mat4>& modelView) throw(runtime_error)
    {
        this->setCenterofBoundingBox();;
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

    void setMaxBounds(glm::vec4 boundingBoxMax){
        this->maxBounds.x = boundingBoxMax.x;
        this->maxBounds.y = boundingBoxMax.y;
        this->maxBounds.z = boundingBoxMax.z;
        this->maxBounds.w = boundingBoxMax.w;
    }

    void setMinBounds(const glm::vec4& _boundingBoxMin){
        minBounds = _boundingBoxMin;
    }

    glm::vec4 getMinBounds(){
        return this->minBounds;
    }
    glm::vec4 getMaxBounds(){
        return this->maxBounds;
    }

    void setExistTextureTransform(bool existTextureTransform){
        this->existTextureTransform = existTextureTransform;
    }

    void setTextureMatrix(const glm::mat4& texturematrix) throw(runtime_error){
        cout << "Texture matrix set!\n";
        this->setExistTextureTransform(true);
        this->texturematrix = texturematrix;
    }

    bool retrieveRecord(Ray ray, stack<glm::mat4>& modelView, HitRecord& hitrecord)
    {
        glm::vec4 startPoint = glm::inverse(modelView.top()) * ray.getStartPoint();
        glm::vec4 direction = glm::inverse(modelView.top()) * ray.getDirection();

        if(this->objInstanceName == "box")
        {
            float tMin, tMax, tXMin, tXMax, tYMin, tYMax, tZMin, tZMax;
            if (direction.x < 0)
            {
                tXMin = (0.5 - startPoint.x) / direction.x;
                tXMax = (- 0.5 - startPoint.x) / direction.x;
                if (direction.y < 0)
                {
                    tYMin = (0.5 - startPoint.y) / direction.y;
                    tYMax = (- 0.5 - startPoint.y) / direction.y;
                    if (direction.z < 0)
                    {
                        tZMin = (0.5 - startPoint.z) / direction.z;
                        tZMax = (- 0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else
                        {
                            if (tMax <= 0)
                                return false;
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                    else if (direction.z == 0)
                    {
                        if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                            return false;
                        else {
                            tMin = max(tXMin, tYMin);
                            tMax = min(tXMax, tYMax);
                            if (tMin > tMax)
                                return false;
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                    else if (direction.z > 0)
                    {
                        tZMin = (- 0.5 - startPoint.z) / direction.z;
                        tZMax = (0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0)
                                return false;
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                }
                else if (direction.y == 0)
                {
                    if ((startPoint.y < - 0.5) || (startPoint.y > 0.5))
                        return false;
                    else
                    {
                        if (direction.z < 0)
                        {
                            tZMin = (0.5 - startPoint.z) / direction.z;
                            tZMax = (- 0.5 - startPoint.z) / direction.z;
                            tMin = max(tXMin, tZMin);
                            tMax = min(tXMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0)
                                    return false;
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                        else if (direction.z == 0)
                        {
                            if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                                return false;
                            else
                            {
                                tMin = tXMin;
                                tMax = tXMax;
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0)
                                        return false;
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                        }
                        else if (direction.z > 0)
                        {
                            tZMin = (- 0.5 - startPoint.z) / direction.z;
                            tZMax = (0.5 - startPoint.z) / direction.z;
                            tMin = max(tXMin, tZMin);
                            tMax = min(tXMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else
                            {
                                if (tMax <= 0)
                                    return false;
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    }
                }
                else if (direction.y > 0)
                {
                    tYMin = (- 0.5 - startPoint.y) / direction.y;
                    tYMax = (0.5 - startPoint.y) / direction.y;
                    if (direction.z < 0)
                    {
                        tZMin = (0.5 - startPoint.z) / direction.z;
                        tZMax = (- 0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else
                        {
                            if (tMax <= 0)
                                return false;
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                    else if (direction.z == 0)
                    {
                        if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                            return false;
                        else
                        {
                            tMin = max(tXMin, tYMin);
                            tMax = min(tXMax, tYMax);
                            if (tMin > tMax)
                                return false;
                            else
                            {
                                if (tMax <= 0)
                                    return false;
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    }
                    else if (direction.z > 0)
                    {
                        tZMin = (- 0.5 - startPoint.z) / direction.z;
                        tZMax = (0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0)
                                return false;
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                }
            }
            else if (direction.x == 0)
            {
                if ((startPoint.x < - 0.5) || (startPoint.x > 0.5))
                    return false;
                else {
                    if (direction.y < 0){
                        tYMin = (0.5 - startPoint.y) / direction.y;
                        tYMax = (- 0.5 - startPoint.y) / direction.y;
                        if (direction.z < 0)
                        {
                            tZMin = (0.5 - startPoint.z) / direction.z;
                            tZMax = (- 0.5 - startPoint.z) / direction.z;
                            tMin = max(tYMin, tZMin);
                            tMax = min(tYMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                        else if (direction.z == 0)
                        {
                            if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                                return false;
                            else {
                                tMin = tYMin;
                                tMax = tYMax;
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0){
                                        return false;
                                    }
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                        }
                        else if (direction.z > 0)
                        {
                            tZMin = (- 0.5 - startPoint.z) / direction.z;
                            tZMax = (0.5 - startPoint.z) / direction.z;
                            tMin = max(tYMin, tZMin);
                            tMax = min(tYMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    }
                    else if (direction.y == 0)
                    {
                        if ((startPoint.y < - 0.5) || (startPoint.y > 0.5))
                            return false;
                        else {
                            if (direction.z < 0){
                                tZMin = (0.5 - startPoint.z) / direction.z;
                                tZMax = (- 0.5 - startPoint.z) / direction.z;
                                tMin = tZMin;
                                tMax = tZMax;
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0){
                                        return false;
                                    }
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                            else if (direction.z == 0)
                            {
                                return false;
                            }
                            else if (direction.z > 0)
                            {
                                tZMin = (- 0.5 - startPoint.z) / direction.z;
                                tZMax = (0.5 - startPoint.z) / direction.z;
                                tMin = max(tYMin, tZMin);
                                tMax = min(tYMax, tZMax);
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0){
                                        return false;
                                    }
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    else if (direction.y > 0)
                    {
                        tYMin = (- 0.5 - startPoint.y) / direction.y;
                        tYMax = (0.5 - startPoint.y) / direction.y;
                        if (direction.z < 0){
                            tZMin = (0.5 - startPoint.z) / direction.z;
                            tZMax = (- 0.5 - startPoint.z) / direction.z;
                            tMin = max(tYMin, tZMin);
                            tMax = min(tYMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                        else if (direction.z == 0)
                        {
                            if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                                return false;
                            else {
                                tMin = tYMin;
                                tMax = tYMax;
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0){
                                        return false;
                                    }
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                        }
                        else if (direction.z > 0)
                        {
                            tZMin = (- 0.5 - startPoint.z) / direction.z;
                            tZMax = (0.5 - startPoint.z) / direction.z;
                            tMin = max(tYMin, tZMin);
                            tMax = min(tYMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            else if (direction.x > 0)
            {
                tXMin = (- 0.5 - startPoint.x) / direction.x;
                tXMax = (0.5 - startPoint.x) / direction.x;
                if (direction.y < 0){
                    tYMin = (0.5 - startPoint.y) / direction.y;
                    tYMax = (- 0.5 - startPoint.y) / direction.y;
                    if (direction.z < 0){
                        tZMin = (0.5 - startPoint.z) / direction.z;
                        tZMax = (- 0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0){
                                return false;
                            }
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    } else if (direction.z == 0){
                        if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                            return false;
                        else {
                            tMin = max(tXMin, tYMin);
                            tMax = min(tXMax, tYMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    } else if (direction.z > 0){
                        tZMin = (- 0.5 - startPoint.z) / direction.z;
                        tZMax = (0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0){
                                return false;
                            } else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                }
                else if (direction.y == 0)
                {
                    if ((startPoint.y < - 0.5) || (startPoint.y > 0.5))
                        return false;
                    else {
                        if (direction.z < 0){
                            tZMin = (0.5 - startPoint.z) / direction.z;
                            tZMax = (- 0.5 - startPoint.z) / direction.z;
                            tMin = max(tXMin, tZMin);
                            tMax = min(tXMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                } else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        } else if (direction.z == 0){
                            if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                                return false;
                            else {
                                tMin = tXMin;
                                tMax = tXMax;
                                if (tMin > tMax)
                                    return false;
                                else{
                                    if (tMax <= 0){
                                        return false;
                                    }
                                    else
                                    {
                                        if (tMin <= 0){
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                        } else {
                                            hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                        }
                                        return true;
                                    }
                                }
                            }
                        } else if (direction.z > 0){
                            tZMin = (- 0.5 - startPoint.z) / direction.z;
                            tZMax = (0.5 - startPoint.z) / direction.z;
                            tMin = max(tXMin, tZMin);
                            tMax = min(tXMax, tZMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    }
                }
                else if (direction.y > 0)
                {
                    tYMin = (- 0.5 - startPoint.y) / direction.y;
                    tYMax = (0.5 - startPoint.y) / direction.y;
                    if (direction.z < 0){
                        tZMin = (0.5 - startPoint.z) / direction.z;
                        tZMax = (- 0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0){
                                return false;
                            }
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    } else if (direction.z == 0){
                        if ((startPoint.z < - 0.5) || (startPoint.z > 0.5))
                            return false;
                        else {
                            tMin = max(tXMin, tYMin);
                            tMax = min(tXMax, tYMax);
                            if (tMin > tMax)
                                return false;
                            else{
                                if (tMax <= 0){
                                    return false;
                                }
                                else
                                {
                                    if (tMin <= 0){
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                    } else {
                                        hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                    }
                                    return true;
                                }
                            }
                        }
                    } else if (direction.z > 0){
                        tZMin = (- 0.5 - startPoint.z) / direction.z;
                        tZMax = (0.5 - startPoint.z) / direction.z;
                        tMin = max(max(tXMin, tYMin), tZMin);
                        tMax = min(min(tXMax, tYMax), tZMax);
                        if (tMin > tMax)
                            return false;
                        else{
                            if (tMax <= 0){
                                return false;
                            }
                            else
                            {
                                if (tMin <= 0){
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMax);
                                } else {
                                    hitrecord = generateHitRecord(modelView, startPoint, direction, tMin);
                                }
                                return true;
                            }
                        }
                    }
                }
            }

        }
        else if (this->objInstanceName == "sphere")
        {
            // radius = 1.0, center at (0, 0, 0, 1)
            float centerX = 0.0f;
            float centerY = 0.0f;
            float centerZ = 0.0f;
            float radius = 1.0f;
            float A = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
            float B = 2 * (direction.x * (startPoint.x - centerX) + direction.y * (startPoint.y - centerY) + direction.z * (startPoint.z - centerZ));
            float C = (startPoint.x - centerX) * (startPoint.x - centerX)
                    + (startPoint.y - centerY) * (startPoint.y - centerY)
                    + (startPoint.z - centerZ) * (startPoint.z - centerZ)
                    - radius * radius;

            float tFirst = ((- B) + sqrt(B * B - 4 * A * C)) / (2 * A);
            float tSecond = ((- B) - sqrt(B * B - 4 * A * C)) / (2 * A);

            if (A == 0.0)
                return false;
            if ((B * B - 4 * A * C) < 0)
                return false;
            else
            {
                if (tFirst == tSecond)
                {
                    if (tFirst > 0.0f) {
                        hitrecord = generateHitRecordSphere(modelView, startPoint, direction, tFirst);
                    }
                }
                else if (tFirst < tSecond)
                {
                    if (tFirst <= 0.0){
                        if (tSecond > 0.0f ){
                            hitrecord = generateHitRecordSphere(modelView, startPoint, direction, tSecond);
                        }
                    } else {
                        hitrecord = generateHitRecordSphere(modelView, startPoint, direction, tFirst);
                    }
                }
                else if (tFirst > tSecond)
                {
                    if (tSecond <= 0.0){
                        if (tFirst > 0.0f){
                            hitrecord = generateHitRecordSphere(modelView, startPoint, direction, tFirst);
                        }
                    } else {
                        hitrecord = generateHitRecordSphere(modelView, startPoint, direction, tSecond);
                    }
                }
                return true;
            }
        }
        else if (this->objInstanceName == "cylinder")
        {
            float radius = 1.0f;
            float A = direction.x * direction.x + direction.z * direction.z;
            float B = 2 * (startPoint.x * direction.x + startPoint.z * direction.z);
            float C = startPoint.x * startPoint.x + startPoint.z * startPoint.z - radius * radius;

            float distance = sqrt((startPoint.x * startPoint.x) + (startPoint.z * startPoint.z));
            if (direction.y == 0.0f && distance <= radius)
            {
                float tFirst = ((- B) + sqrt((B * B) - (4 * A * C))) / 2.0 * A;
                float tSecond = ((- B) - sqrt((B * B) - (4 * A * C))) / 2.0 * A;
                if (tFirst < tSecond)
                {
                    if (tFirst <= 0)
                    {
                        if (tSecond <= 0)
                        {
                            return false;
                        }
                        else
                        {
                            hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tSecond);
                            return true;
                        }
                    }
                    else
                    {
                        hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tFirst);
                        return true;
                    }
                }
                else if (tFirst == tSecond)
                {
                    if (tFirst <= 0)
                    {
                        return false;
                    }
                    else
                    {
                        hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tFirst);
                        return true;
                    }
                }
                else if (tFirst > tSecond)
                {
                    if (tSecond <= 0){
                        if (tFirst <= 0){
                            return false;
                        }
                        else
                        {
                            hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tFirst);
                            return true;
                        }
                    } else {
                        hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tSecond);
                    }
                }
            }
            else
            {
                // direction.y != 0
                float tHitSurfaceFirst = 0.0f;
                float tHitSurfaceSecond = 0.0f;
                float tHitUp = 0.0f;
                float tHitBottom = 0.0f;

                if (A == 0)
                {
                    // direction.x == 0, and direction.z == 0
                    // in this condition, the ray can only hit the up or bottom
                    tHitUp = (1.0 - startPoint.y) / direction.y;
                    tHitBottom = (0.0 - startPoint.y) / direction.y;
                    if (tHitUp < tHitBottom)
                    {
                        if (tHitUp <= 0)
                        {
                            if (tHitBottom <= 0)
                            {
                                return false;
                            }
                            else
                            {
                                hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tHitBottom);
                                return true;
                            }
                        }
                        else
                        {
                            hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tHitUp);
                            return true;
                        }
                    }
                    else if (tHitUp == tHitBottom)
                    {
                        if (tHitUp <= 0)
                        {
                            return false;
                        }
                        else
                        {
                            hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tHitUp);
                            return true;
                        }
                    }
                    else if (tHitUp > tHitBottom)
                    {
                        if (tHitBottom <= 0)
                        {
                            if (tHitUp <= 0)
                            {
                                return false;
                            }
                            else
                            {
                                hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tHitUp);
                                return true;
                            }
                        }
                        else
                        {
                            hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, tHitBottom);
                            return true;
                        }
                    }
                }

                else if ((B * B - 4 * A * C) < 0)
                {
                    // The ray will not hit the surface
                    return false;
                }

                else
                {
                    tHitSurfaceFirst = ((- B) + sqrt(B * B - 4 * A * C)) / (2 * A);
                    tHitSurfaceSecond = ((- B) - sqrt(B * B - 4 * A * C)) / (2 * A);
                    tHitUp = (1.0 - startPoint.y) / direction.y;
                    tHitBottom = (0.0 - startPoint.y) / direction.y;
                    // intersection with surface First
                    glm::vec4 intersectionPointA = glm::vec4(startPoint.x + tHitSurfaceFirst * direction.x,
                                                             startPoint.y + tHitSurfaceFirst * direction.y,
                                                             startPoint.z + tHitSurfaceFirst * direction.z,
                                                             1.0f);
                    // intersection with surface Second
                    glm::vec4 intersectionPointB = glm::vec4(startPoint.x + tHitSurfaceSecond * direction.x,
                                                             startPoint.y + tHitSurfaceSecond * direction.y,
                                                             startPoint.z + tHitSurfaceSecond * direction.z,
                                                             1.0f);
                    // intersection with Up
                    glm::vec4 intersectionPointC = glm::vec4(startPoint.x + tHitUp * direction.x,
                                                             startPoint.y + tHitUp * direction.y,
                                                             startPoint.z + tHitUp * direction.z,
                                                             1.0f);
                    // intersection with Bottom
                    glm::vec4 intersectionPointD = glm::vec4(startPoint.x + tHitBottom * direction.x,
                                                             startPoint.y + tHitBottom * direction.y,
                                                             startPoint.z + tHitBottom * direction.z,
                                                             1.0f);
                    vector<float> timeVector;
                    timeVector.push_back(tHitSurfaceFirst);
                    timeVector.push_back(tHitSurfaceSecond);
                    timeVector.push_back(tHitUp);
                    timeVector.push_back(tHitBottom);
                    if ((intersectionPointA.y < 0.0f) || (intersectionPointA.y > 1.0f)){
                        timeVector[0] = - 1.0;
                    }
                    if ((intersectionPointB.y < 0.0f) || (intersectionPointB.y > 1.0f)){
                        timeVector[1] = - 1.0;
                    }
                    float bottomDistance = sqrt((intersectionPointC.x) * (intersectionPointC.x) + (intersectionPointC.z) * (intersectionPointC.z));
                    if (bottomDistance >= radius){
                        timeVector[2] = - 1.0;
                    }
                    float upDistance = sqrt((intersectionPointD.x) * (intersectionPointD.x) + (intersectionPointD.z) * (intersectionPointD.z));
                    if (upDistance >= radius){
                        timeVector[3] = - 1.0;
                    }
                    vector<float> timeHit;
                    for (int i = 0; i < timeVector.size(); i++){
                        if (timeVector[i] > 0.0f)
                            timeHit.push_back(timeVector[i]);
                    }
                    if (timeHit.empty())
                    {
                        return false;
                    }
                    else
                    {
                        float time = timeHit[0];
                        for (int i = 0; i < timeHit.size(); i++){
                            if (timeHit[i] <= time)
                                time = timeHit[i];
                        }
                        hitrecord = generateHitRecordCylinder(modelView, startPoint, direction, time);
                        return true;
                    }
                }
            }
        }

    }

    std::vector<HitRecord> raycast(Ray ray, stack<glm::mat4>& modelView, std::vector<HitRecord> hitrecords)
    {
        HitRecord record;
        if (retrieveRecord(ray, modelView, record))
        {
            if (record.getTime() - 0.0f > 0.000001)
            {
                hitrecords.push_back(record);
            }
        }
        return hitrecords;
    }

    HitRecord generateHitRecord(stack<glm::mat4>& modelView, glm::vec4 startPoint, glm::vec4 direction, float time){
        float intersectionX = startPoint.x + direction.x * time;// in the obj coordinate
        float intersectionY = startPoint.y + direction.y * time;// in the obj coordinate
        float intersectionZ = startPoint.z + direction.z * time;// in the obj coordinate
        float intersectionW = 1.0f;// point

        glm::vec4 normal = glm::vec4(0.0);
        glm::vec4 textureCoordinate = glm::vec4(0.0);
        glm::vec4 intersection = glm::vec4(intersectionX, intersectionY, intersectionZ, intersectionW);
        // get the intersection point in the obj coordinate;

        if (fabs(intersection.x - (-0.5f)) <= 0.0001f)                  // left
        {
            normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
            float textureCoordinateS = 0.0f + (intersection.z - (- 0.5f)) / 4.0f;
            float textureCoordinateT = 0.25f + abs(intersection.y - 0.5f) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else if (fabs(intersection.x - (0.5f)) <= 0.0001f)              // right
        {
            normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            float textureCoordinateS = 0.5f + abs(intersection.z - 0.5f) / 4.0f;
            float textureCoordinateT = 0.25f + abs(intersection.y - 0.5f) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else if (fabs(intersection.y - (-0.5f)) <= 0.0001f)
        {// bottom face
            normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
            float textureCoordinateS = 0.25f + (intersection.x - (- 0.5f)) / 4.0f;
            float textureCoordinateT = 0.5f + abs(intersection.z - 0.5f) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else if (fabs(intersection.y - (0.5f)) <= 0.0001f)
        {// top face
            normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
            float textureCoordinateS = 0.25f + (intersection.x - (- 0.5f)) / 4.0f;
            float textureCoordinateT = 0.0f + (intersection.z - (- 0.5f)) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else if (fabs(intersection.z - (-0.5f)) <= 0.0001f)
        {// back face
            normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
            float textureCoordinateS = 0.75f + abs(intersection.x - 0.5f) / 4.0f;
            float textureCoordinateT = 0.25f + abs(intersection.y - 0.5f) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else if (fabs(intersection.z - (0.5f)) <= 0.0001f)
        {// front face
            normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
            float textureCoordinateS = 0.25f + (intersection.x - (- 0.5f)) / 4.0f;
            float textureCoordinateT = 0.25f + abs(intersection.y - 0.5f) / 4.0f;
            textureCoordinate = glm::vec4(textureCoordinateS, textureCoordinateT, 0.0, 0.0);
        }
        else
        {

        }
        HitRecord hitRecord;
        intersection = modelView.top() * intersection;
        // transform it to the view coordinate
        normal = glm::inverse(glm::transpose(modelView.top())) * normal;
        normal.w = 0.0f;
        hitRecord.setIntersectPoint(intersection);
        hitRecord.setNormal(normal);
        hitRecord.setTime(time);
        hitRecord.setMaterial(this->material);
        hitRecord.setTextureName(this->textureName);
        hitRecord.setTextureMatrix(this->texturematrix);
        hitRecord.setTextureCoordinate(textureCoordinate);
        hitRecord.setTextureImage(this->textureImage);
        return hitRecord;
    }

    HitRecord generateHitRecordCylinder(stack<glm::mat4>& modelView, glm::vec4 startPoint, glm::vec4 direction, float time){
        float intersectionX = startPoint.x + direction.x * time;
        float intersectionY = startPoint.y + direction.y * time;
        float intersectionZ = startPoint.z + direction.z * time;
        float intersectionW = 1.0f;//point
        float pi = 3.14;
        float radius = 1.0f;
        float width = 2 * pi * radius;
        float height = 3.0f;
        glm::vec4 normal = glm::vec4(0.0f);
        glm::vec4 textureCoordinate = glm::vec4(0.0f);
        glm::vec4 intersection = glm::vec4(intersectionX, intersectionY, intersectionZ, 1.0f);
        if ((intersectionY > - 0.05f) && (intersectionY < 0.05f)){
            // the point is on the bottom
            normal = glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f);
            float longtitude = atan2(- intersection.z, intersection.x);
            float S = ((longtitude + pi) / (2 * pi)) + 0.5;
            float distance = sqrt((intersection.x) * (intersection.x) + (intersection.z) * (intersection.z));
            float T = distance / height;
            textureCoordinate = glm::vec4(S, T, 0.0, 0.0);
        } else if ((intersectionY > 0.95f) && (intersectionY < 1.05f)){
            // the point is on the top
            normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
            float longtitude = atan2(- intersection.z, intersection.x);
            float S = ((longtitude + pi) / (2 * pi)) + 0.5;
            float distance = sqrt((intersection.x) * (intersection.x) + (intersection.z) * (intersection.z));
            float T = (height - distance) / height;
            textureCoordinate = glm::vec4(S, T, 0.0, 0.0);
        } else {
            // the point is on the surface
            normal = glm::normalize(glm::vec4(intersectionX, 0.0f, intersectionZ, 0.0f));
            float longtitude = atan2(- intersection.z, intersection.x);
            float S = ((longtitude + pi) / (2 * pi)) + 0.5;
            float distance = intersection.y + radius;
            float T = distance / height;
            textureCoordinate = glm::vec4(S, T, 0.0, 0.0);
        }
        HitRecord hitRecord;
        intersection = modelView.top() * intersection;
        // transform it to the view coordinate
        normal = glm::normalize(glm::inverse(glm::transpose(modelView.top())) * normal);
//        cout << "Sphere Hit Normal in View Coordinate System : \n"
//             << "x : " << normal.x << " y : " << normal.y << " z : " << normal.z << endl;
        hitRecord.setIntersectPoint(intersection);
        hitRecord.setNormal(normal);
        hitRecord.setTime(time);
        hitRecord.setMaterial(this->material);
        hitRecord.setTextureName(this->textureName);
        hitRecord.setTextureMatrix(this->texturematrix);
        hitRecord.setTextureCoordinate(textureCoordinate);
        hitRecord.setTextureImage(this->textureImage);
        return hitRecord;
    }

    HitRecord generateHitRecordSphere(stack<glm::mat4>& modelView, glm::vec4 startPoint, glm::vec4 direction, float time){
        //cout << "StartPoint : " << "x : " << startPoint.x << " y : " << startPoint.y << " z : " << startPoint.z << endl;
        float intersectionX = startPoint.x + direction.x * time;
        float intersectionY = startPoint.y + direction.y * time;
        float intersectionZ = startPoint.z + direction.z * time;
        float intersectionW = 1.0f;//point
        //cout << "Sphere Hit Time : " << time << endl;
        glm::vec4 normal = glm::vec4(0.0f);
        glm::vec4 textureCoordinate = glm::vec4(0.0f);
        glm::vec4 intersection = glm::vec4(intersectionX, intersectionY, intersectionZ, 1.0f);
        normal = glm::normalize(glm::vec4(intersection.x, intersection.y, intersection.z, 0.0f));
//        cout << "Sphere Hit Normal : \n"
//             << "x : " << normal.x << " y : " << normal.y << " z : " << normal.z << endl;
        float latitude = asin(intersection.y);
        float longtitude = atan2(- intersection.z, intersection.x);
        float pi = 3.14;
        float S = ((longtitude + pi) / (2 * pi)) + 0.5;
        float T = 1.0 - ((latitude + (pi / 2)) / pi);
        textureCoordinate = glm::vec4(S, T, 0.0, 1.0);
        HitRecord hitRecord;
        intersection = modelView.top() * intersection;
        // transform it to the view coordinate
        normal = glm::normalize(glm::inverse(glm::transpose(modelView.top())) * normal);
//        cout << "Sphere Hit Normal in View Coordinate System : \n"
//             << "x : " << normal.x << " y : " << normal.y << " z : " << normal.z << endl;
        if (intersection.w != 1.f)
        {
            std::cout << "error here!\n";
        }
        hitRecord.setIntersectPoint(intersection);
        hitRecord.setNormal(normal);
        hitRecord.setTime(time);
        hitRecord.setMaterial(this->material);
        hitRecord.setTextureName(this->textureName);
        hitRecord.setTextureMatrix(this->texturematrix);
        hitRecord.setTextureCoordinate(textureCoordinate);
        hitRecord.setTextureImage(this->textureImage);
        return hitRecord;
    }


};

}
#endif
