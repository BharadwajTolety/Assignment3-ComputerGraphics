#ifndef HITRECORD_H
#define HITRECORD_H
#include <glm/glm.hpp>
#include "Material.h"
#include "TextureImage.h"
#include <iostream>
using namespace std;

class HitRecord{
private:
    float time;
    glm::vec4 intersectPoint;// in the view coordinate system
    glm::vec4 normal;// in the view coordinate system
    util::Material material;
    string textureName;
    util::TextureImage textureImage;
    glm::mat4 textureMatrix;
    glm::vec4 textureCoodinate;
//    float absorption;
//    float reflection;
//    float transparency;
public:
    HitRecord(){

    }
    ~HitRecord(){

    }
//    void setAbsorption(float absorption){
//        this->absorption = absorption;
//    }
//    void setReflection(float reflection){
//        this->reflection = reflection;
//    }
//    void setTransparency(float transparency){
//        this->transparency = transparency;
//    }
    void setTime(float time){
        this->time = time;
    }
    void setIntersectPoint(glm::vec4 intersectPoint){
//        cout << "crash here\n";
        if(intersectPoint.w == 0)
            cout << "the intersectPoint should be a point!\n";
        else{
//            cout << "set intersectPoint\n";
//            cout << "x : " << intersectPoint.x << endl
//                 << "y : " << intersectPoint.y << endl
//                 << "z : " << intersectPoint.z << endl
//                 << "w : " << intersectPoint.w << endl;
            this->intersectPoint = intersectPoint;
            //cout << "set success\n";
        }
    }
    void setNormal(glm::vec4 normal){
        if(normal.w == 1)
            cout << "the normal should be a vector!\n";
        else{
//            cout << "set normal:\n"
//                 << "x : " << normal.x << endl
//                 << "y : " << normal.y << endl
//                 << "z : " << normal.z << endl
//                 << "w : " << normal.w << endl;
            this->normal = normal;
        }
    }
    void setMaterial(util::Material material){
        this->material = material;
    }
    void setTextureName(string textureName){
        this->textureName = textureName;
    }
    void setTextureMatrix(glm::mat4 textureMatrix){
        this->textureMatrix = textureMatrix;
    }
    void setTextureCoordinate(glm::vec4 textureCoodinate){
        this->textureCoodinate = textureCoodinate;
    }
    void setTextureImage(util::TextureImage textureImage){
        this->textureImage = textureImage;
    }

    util::TextureImage getTextureImage(){
        return this->textureImage;
    }

    float getTime(){
        return this->time;
    }
    glm::vec4 getIntersectPoint(){
        return this->intersectPoint;
    }
    glm::vec4 getNormal(){
        return this->normal;
    }
    util::Material getMaterial(){
        return this->material;
    }
    string getTextureName(){
        return this->textureName;
    }
    glm::mat4 getTextureMatrix(){
        return this->textureMatrix;
    }
    glm::vec4 getTextureCoordinate(){
        return this->textureCoodinate;
    }
//    float getAbsorption(){
//        return this->absorption;
//    }
//    float getReflection(){
//        return this->reflection;
//    }
//    float getTransparency(){
//        return this->transparency;
//    }
};

#endif // HITRECORD_H
