//
//  Primitive.h
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Primitive_h
#define Primitive_h

#include "core.h"
#include "Wind.h"

class Primitive {
public:
    Primitive();
    Primitive(float m);
    Primitive(float mass, const glm::vec3& pos);
    Primitive(float mass, const glm::vec3& pos, const glm::vec3& vel);
    ~Primitive();

    void applyForce(glm::vec3& f) {
        force += f;
    }
    
    float getMass() { return mass; }
    glm::vec3 getPos() { return position; }
    glm::vec3 getNormal() { return normal; }
    glm::vec3 getVelocity() { return velocity; }
    glm::mat4 getModel() { return model; }
    glm::vec3 getColor() { return color; }

    
    void zeroNormal(){
        normal = glm::vec3(0.0f);
    }
    void zeroForce(){
        force = glm::vec3(0.0f);
    }
    
    void setPos(const glm::vec3& p){
        position = p;
    }
    
    void setVelocity(const glm::vec3& v){
        velocity = v;
    }
    
    void applyNormal(glm::vec3& n) {
        normal += n;
    }
    
    void normalizeNormal(){
        normal = glm::normalize(normal);
    }
    
    virtual void computeAreoForce(Wind* wind){}
    
    virtual void move (const glm::vec3& pos);
    void reset(); 
    void update(float deltaTime);     
    glm::vec3 momentum; 
    float mass; // constant
    
protected:
    glm::vec3 position; // evaluate frame by frame
    glm::vec3 velocity; // evaluate frame by frame
    glm::vec3 force;    // reset and recomput each frame
    glm::vec3 normal;
    glm::mat4 model;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};

#endif /* Primitive_h */
