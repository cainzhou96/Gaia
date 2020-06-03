//
//  Coin.cpp
//  Gaia
//
//  Created by Wenlin Mao on 5/30/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Coin.hpp"


Coin::Coin(){
        
    coinModel = new Model("model/dogecoin.obj", false);
    
}

Coin::~Coin(){
    delete coinModel;
}


void Coin::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader){
    
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    coinModel->Draw(shader);
}

void Coin::move(const glm::vec3& pos){
    
    // actiavte the shader program
    model = glm::translate(model, (pos - coinModel->model_center));
    coinModel->model_center = pos;
}

void Coin::scale(float factor){
    model = glm::scale(model, glm::vec3(factor));
}

