//
//  Sphere.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Sphere.h"

Sphere::Sphere(){
}


Sphere::Sphere(float m, float r, const std::vector<std::string>& faces): Primitive(m), radius(r){
    numLon = 10;
    numLat = 10;
    
    cubemapTexture = loadCubemap(faces);
    
    createVerts();
    createIndices();
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &EBO);
}

Sphere::Sphere(float mass, const glm::vec3& pos, const glm::vec3& vel, float r,
               const std::vector<std::string>& faces): Primitive(mass, pos, vel), radius(r){
    
    numLon = 10;
    numLat = 10;
    
    cubemapTexture = loadCubemap(faces);
    
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

    // Bind the VAO
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}


void Sphere::prepareDraw(){
    
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
    // actiavte the shader program
    float len = glm::length(pos - position);
    if (len > 0.0001f) {
        glm::vec3 dir = glm::normalize(pos - position);
        if (glm::length(glm::vec3(dir.x, 0, dir.z)) > 0.0001f) {
            glm::vec3 up = glm::vec3(0, 1, 0);
            glm::vec3 right = glm::cross(dir, up);
            float angle = - len / radius;
            glm::mat4 coef = glm::rotate(glm::mat4(1), angle, right);
            model = coef * model;
        }
    }

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
glm::vec3 Sphere::checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n) {
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
        if (t > 0) {
            return P - (position + radius * glm::normalize(P - position));
        } else {
            return P - (position + radius * glm::normalize(position - P));
        }
    } else {
        return glm::vec3(0);
    }
}
