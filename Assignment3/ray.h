#ifndef RAY_H
#define RAY_H
#include <glm/glm.hpp>
#include <iostream>

class Ray{
private:
    glm::vec4 startpoint;
    glm::vec4 direction;
public:
    Ray(){

    }
    ~Ray(){

    }
    void setStartPoint(glm::vec4 startpoint){
        if (startpoint.w == 0)
            std::cout << "The startpoint should be a point!\n";
        else
            this->startpoint = startpoint;
    }
    void setDirection(glm::vec4 direction){
        if (direction.w == 1)
            std::cout << "The direction should be a vector!\n";
        else
            this->direction = direction;
    }
    glm::vec4 getStartPoint(){
        return this->startpoint;
    }
    glm::vec4 getDirection(){
        return this->direction;
    }
};

#endif // RAY_H
