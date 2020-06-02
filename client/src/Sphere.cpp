//
//  Sphere.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Sphere.h"
#define EPSILON 0.0001f

Sphere::Sphere(){
}

Sphere::Sphere(float m, float r): Primitive(m), radius(r){
    numLon = 10;
    numLat = 10;
    
    mass = m;
    I0 = glm::mat3(0.0f);
    I0[0][0] = 2 * mass * radius * radius / 5;
    I0[1][1] = 2 * mass * radius * radius / 5;
    I0[2][2] = 2 * mass * radius * radius / 5;
    
    createVerts();
    createIndices();
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &EBO);
}

Sphere::Sphere(float mass, const glm::vec3& pos, const glm::vec3& vel, float r): Primitive(mass, pos, vel), radius(r){
    
    numLon = 10;
    numLat = 10;
    
    createVerts();
    createIndices();
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &EBO);
}

Sphere::~Sphere(){
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Sphere::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader){
    
    prepareDraw();
    
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

    // Bind the VAO
    glBindVertexArray(VAO);
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}


void Sphere::prepareDraw(){
    // std::cout << glm::to_string(model) << std::endl; 
    
    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to the first VBO - We will use it to store the vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the second VBO - We will use it to store the normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    // Generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
    
    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Sphere::createVerts(){
    for(int i=0; i<=numLat; i++){
        
        float theta = (i * PI) / numLat;
        
        for(int j=0; j<=numLon; j++){
            float phi = (j*2*PI)/numLon;
            float x = radius * glm::sin(theta) * glm::cos(phi);
            float y = radius * glm::cos(theta);
            float z = radius * glm::sin(theta) * glm::sin(phi);
            
            positions.push_back(glm::vec3(x,y,z));
            normals.push_back(glm::normalize(glm::vec3(x,y,z)));
        }
    }
}

void Sphere::createIndices(){
    for(int i=0; i<numLat; i++){
        for(int j=0; j<numLon; j++){
            
            int first = (i*(numLon+1))+j;
            int second = first + (numLon + 1);
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first+1);
            
            indices.push_back(second);
            indices.push_back(second+1);
            indices.push_back(first+1);
        }
    }
}

void Sphere::computeAreoForce(Wind* wind){
    
    glm::vec3 v = velocity - wind->windV;
    
    float a = PI * pow(radius, 2);
    
    glm::vec3 f_areo =  wind->dragCoeff * 0.5f * wind->airDensity * (float) pow(glm::length(v), 2) * a * -glm::normalize(v);
    
    //applyForce(f_areo);
}

void Sphere::reset(){
    Primitive::reset();
    
    positions.clear();
    normals.clear();
    indices.clear();
}

void Sphere::setRadius(float r){
    radius = r;
    
    positions.clear();
    normals.clear();
    indices.clear();
    
    createVerts();
    createIndices();
}

// translate to pos
void Sphere::move(const glm::vec3& pos){
    model[3] = glm::vec4(pos, 1);
    position = pos;
}

void Sphere::move(const glm::mat4& transform){
    model = transform;
}

/*
 * a, b, c forms a triangle and n is normal. Return a vector to translate the sphere to just
 * resolve intersection. Return vec3(0) if no collision.
 */
glm::vec3 Sphere::checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n, float elapsedTime) {
    position = model[3];
    if (a == b || a == c || b == c) {
        return glm::vec3(0);
    }
    
    float t = -(glm::dot(a, n) - glm::dot(position, n));
    if (t < -(radius + 0.0001f) || t > (radius + 0.0001f)) {
        return glm::vec3(0);
    }
    
    glm::vec3 P = position + t * (-n);
    
    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - b;
    glm::vec3 e3 = a - c;
    glm::vec3 A = P - a;
    glm::vec3 B = P - b;
    glm::vec3 C = P - c;
    
    //if P is inside triangle
    if (glm::dot(n, glm::cross(e1, A)) >= 0 &&
        glm::dot(n, glm::cross(e2, B)) >= 0 &&
        glm::dot(n, glm::cross(e3, C)) >= 0) {
        glm::vec3 result;
        glm::vec3 pointPos;
        if (t > 0) {
            result = P - (position + radius * glm::normalize(P - position));
            pointPos = position + radius * glm::normalize(P - position);
        } else {
            result = P - (position + radius * glm::normalize(position - P));
            pointPos = position + radius * glm::normalize(position - P);
        }
        // collision
        glm::mat3 I = glm::mat3(model) * I0 * glm::transpose(glm::mat3(model));
        glm::vec3 omega = glm::inverse(I) * angMomentum;
        glm::vec3 velocity = (momentum + moveMomentum) / mass;
        glm::vec3 r = pointPos - position;
        glm::vec3 vr = velocity + glm::cross(omega, r);
        float e = 0.2f;
        glm::vec3 impulse = (1 + e) * (fmax(glm::dot(vr, -n), 0.0f) / (1/mass + glm::dot(glm::inverse(I) * (glm::cross(glm::cross(r, n), r)), n))) * n; // with bounce
        // glm::vec3 impulse = (fmax(glm::dot(vr, -n), 0.0f) / (1/mass + glm::dot(glm::inverse(I) * (glm::cross(glm::cross(r, n), r)), n))) * n; // without bounce
        // glm::vec3 impulse = fmax(glm::dot(vr, -n), 0.0f) * mass * n;
        // glm::vec3 impulse = (1 + e) * fmax(glm::dot(vr, -n), 0.0f) * mass * n;
        glm::vec3 reactionForce = impulse / elapsedTime;
        // std::cout << glm::to_string(reactionForce) << std::endl;
        applyForce(reactionForce, pointPos);
        
        // friction
        if (glm::length(vr) < EPSILON && glm::length(vr) > -EPSILON) { // is 0
            return result;
        }
        float ud = 0.2f;
        float us = 0.21f;
        float fd = ud * glm::length(reactionForce);
        float fs = us * glm::length(reactionForce);
        
        glm::vec3 t;
        if (glm::dot(vr, n) > EPSILON || glm::dot(vr, n) < -EPSILON) { // not 0
            t = vr - glm::dot(vr, n) * n;
            if (glm::length(t) > 0) {
                t = normalize(t);
            }
        } else if (glm::dot(force, n) > EPSILON || glm::dot(force, n) < -EPSILON) { // not 0
            t = glm::normalize(force - glm::dot(force, n) * n);
            if (glm::length(t) > 0) {
                t = normalize(t);
            }
        } else {
            t = glm::vec3(0);
        }
        glm::vec3 ff;
        if ((glm::dot(vr, t) < EPSILON && glm::dot(vr, t) > -EPSILON) && glm::dot(force, t) <= fs) {
            // vr*t is 0 and force*t <= fs
            ff = -glm::dot(force, t) * t;
        } else {
            ff = -fd * t;
        }
        if (glm::length(ff * elapsedTime / mass) > glm::length(vr - glm::dot(vr, n) * n)) {
            ff = - (vr - glm::dot(vr, n) * n) * mass / elapsedTime;
        }
        applyForce(ff, pointPos);
        
        return result;
    } else {
        return glm::vec3(0);
    }
}

void Sphere::applyForce(glm::vec3 f, glm::vec3 pos) {
    if (glm::length(f) > 0) {
        force += f;
        glm::vec3 r = pos - position;
        torque += glm::cross(r, f);
    }
}

void Sphere::updatePosition(float elapsedTime) {
    momentum += force * elapsedTime;
    moveMomentum += moveForce * elapsedTime;
    if (glm::dot(momentum, moveMomentum) < 0) {
        moveMomentum += glm::dot(glm::normalize(momentum), moveMomentum) * glm::normalize(moveMomentum);
        momentum -= glm::dot(glm::normalize(momentum), moveMomentum) * glm::normalize(moveMomentum);
    }

    //std::cout << glm::to_string(force) << std::endl;
    glm::vec3 dis = (momentum/mass) * elapsedTime;
    // std::cout << glm::to_string(dis) << std::endl;
    
    if (glm::length(moveMomentum) > 0) {
        glm::vec3 temp = 10.0f * glm::normalize(moveMomentum) * elapsedTime;
        if (glm::length(temp) >= glm::length(moveMomentum)) {
            moveMomentum = glm::vec3(0);
        } else {
            moveMomentum -= 10.0f * glm::normalize(moveMomentum) * elapsedTime;
        }
    }
    if (glm::length(moveMomentum) > 40.0f) {
        moveMomentum = 40.0f * glm::normalize(moveMomentum);
    }
    dis += (moveMomentum/mass) * elapsedTime;
    
    //std::cout << glm::to_string(dis) << std::endl;
    
    move(getCenter() + dis);
}

void Sphere::updateOrientation(float elapsedTime) {
    angMomentum += torque * elapsedTime;
    glm::mat3 I = glm::mat3(model) * I0 * glm::transpose(glm::mat3(model));
    glm::vec3 omega = glm::inverse(I) * angMomentum;
    float angle = glm::length(omega) * elapsedTime; // magnitude of omega
    if (angle != 0.0f) {
        glm::vec3 axis = omega;
        axis = glm::normalize(axis);
        model = glm::rotate(model, angle, axis);
    }
}
