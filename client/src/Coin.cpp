//
//  Coin.cpp
//  Gaia
//
//  Created by Wenlin Mao on 5/30/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Coin.hpp"


Coin::Coin() : Model("model/dogecoin.obj", false){
}

Coin::~Coin(){
}

void Coin::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader){
    
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    Model::Draw(shader);
}

void Coin::update() {
    spin(0.7f);
    Model::update();
}

void Coin::spin(float deg)
{
    // Update the model matrix by multiplying a rotation matrix
    rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}


