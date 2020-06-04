//
//  Sphere.h
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h


#include "Primitive.h"

using namespace std;

class Sphere : public Primitive {
public:
    Sphere();
    Sphere(float m, float r);
    Sphere(float mass, const glm::vec3& pos, const glm::vec3& vel, float r);
    ~Sphere();
    
    void computeAreoForce(Wind* wind);
    virtual void move (const glm::vec3& pos);
    
    glm::vec3 getCenter() { return position; }
    float getRadius() { return radius; }
    
    void setRadius(float r);
    
    bool checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n, float elapsedTime); 
    void updatePosition(float elapsedTime, bool hit);
    void updateOrientation(float elapsedTime);
    void applyForce(glm::vec3 f, glm::vec3 pos);
    void applyMoveForce(glm::vec3 f, glm::vec3 pos);

    glm::vec3 force = glm::vec3(0);
    glm::vec3 moveForce = glm::vec3(0);
    glm::vec3 moveMomentum = glm::vec3(0);
    glm::vec3 momentum = glm::vec3(0);
    glm::vec3 angMomentum = glm::vec3(0);
    glm::vec3 torque = glm::vec3(0);
    glm::mat3 I0 = glm::mat3(0);
    float mass; // constant
    
private:
    float radius;
    glm::vec3 position; 
    
    const float PI = glm::pi<float>();
};

#endif /* Sphere_h */
