//
//  Sphere.h
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h

#include "util.h"
#include "Primitive.h"

using namespace std;

class Sphere : public Primitive {
public:
    Sphere();
    Sphere(float m, float r, const std::vector<std::string>& faces);
    Sphere(float mass, const glm::vec3& pos, const glm::vec3& vel, float r,
           const std::vector<std::string>& faces);
    ~Sphere();
    
    glm::vec3 computeNormal();
    glm::vec3 computeSurfaceV();
    virtual void move (const glm::vec3& pos);
    virtual void move (const glm::mat4& transform);
    
    glm::vec3 getCenter() { return position; }
    float getRadius() { return radius; }
    
    void setRadius(float r);
    
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
    void reset();
    
    void prepareDraw();
    void createVerts();
    void createIndices();
    
    glm::vec3 checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n);
        
    glm::vec3 force = glm::vec3(0);
    glm::vec3 moveForce = glm::vec3(0);
    glm::vec3 moveMomentum = glm::vec3(0); 
    
private:
    float radius;
    int numLon;
    int numLat;
    unsigned int cubemapTexture;
    
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;
    
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<unsigned int> indices;
    
    const float PI = glm::pi<float>();
};

#endif /* Sphere_h */
