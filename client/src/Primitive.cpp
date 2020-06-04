//
//  Primitive.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Primitive.h"

Primitive::Primitive() : mass (0), position(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), momentum(glm::vec3(0.0f)){
    model = glm::mat4(1.0f);
}

Primitive::Primitive(float m) : mass (m), position(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), momentum(glm::vec3(0.0f)) {
    model = glm::mat4(1.0f);
}

Primitive::Primitive(float mass, const glm::vec3& pos) : mass (mass), position(pos), velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), momentum(glm::vec3(0.0f)) {
    model = glm::mat4(1.0f);
}

Primitive::Primitive(float mass, const glm::vec3& pos, const glm::vec3& vel) : mass (mass), position(pos), velocity(vel), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), momentum(glm::vec3(0.0f)) {
    model = glm::mat4(1.0f);
}


Primitive::~Primitive(){
}

void Primitive::update(float deltaTime){
    // compute acceleration
    glm::vec3 accel = force / mass;
    velocity += accel * deltaTime;
    position += velocity * deltaTime;
}

void Primitive::reset(){
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    zeroForce();
    zeroNormal();
}

// translate to pos
void Primitive::move(const glm::vec3& pos){
    // actiavte the shader program
    model = glm::translate(model, (pos - position));
    position = pos;
}


