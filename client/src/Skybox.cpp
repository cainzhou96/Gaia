//
//  Skybox.cpp
//  Gaia
//
//  Created by Wenlin Mao on 5/3/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Skybox.hpp"

// The 8 vertices of a cube.
std::vector<glm::vec3> vertices
{
    glm::vec3(-1, 1, 1),
    glm::vec3(-1, -1, 1),
    glm::vec3(1, -1, 1),
    glm::vec3(1, 1, 1),
    glm::vec3(-1, 1, -1),
    glm::vec3(-1, -1, -1),
    glm::vec3(1, -1, -1),
    glm::vec3(1, 1, -1)
};

// Each ivec3(v1, v2, v3) define a triangle consists of vertices v1, v2
// and v3 in counter-clockwise order.
std::vector<glm::ivec3> indices
{
    // Front face.
    glm::ivec3(0, 1, 2),
    glm::ivec3(2, 3, 0),
    // Back face.
    glm::ivec3(7, 6, 5),
    glm::ivec3(5, 4, 7),
    // Right face.
    glm::ivec3(3, 2, 6),
    glm::ivec3(6, 7, 3),
    // Left face.
    glm::ivec3(4, 5, 1),
    glm::ivec3(1, 0, 4),
    // Top face.
    glm::ivec3(4, 0, 3),
    glm::ivec3(3, 7, 4),
    // Bottom face.
    glm::ivec3(1, 5, 6),
    glm::ivec3(6, 2, 1),
};


Skybox::Skybox(const std::vector<std::string>& faces){
    
    cubemapTexture = loadCubemap(faces);
    
    // Generate a vertex array (VAO) and vertex buffer object (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind VBO to the bound VAO, and store the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 0 to pass the vertex data through to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Generate EBO, bind the EBO to the bound VAO, and send the index data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind the VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

Skybox::~Skybox()
{
    // Delete the VBO/EBO and the VAO.
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
    glm::mat4 model = glm::scale(glm::vec3(1000 / 2.f));
    
    // Actiavte the shader program
    glUseProgram(shader);
    
    glDepthMask(GL_FALSE);
    
    glm::mat4 view_tmp = glm::mat4(glm::mat3(view));
    
    // Get the shader variable locations and send the uniform data to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view_tmp));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
//    glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

    // Bind the VAO
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
    // Draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
   
    glDepthMask(GL_TRUE);
    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

