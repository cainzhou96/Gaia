//
//  Arrow.hpp
//  Gaia
//
//  Created by Wenlin Mao on 5/30/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Arrow_hpp
#define Arrow_hpp


#include "core.h"
#include "Model.hpp"

class Arrow : public Model {
public:
    Arrow(): Model("model/arrow-obj/arrow.obj", false) {
        color = glm::vec3(1.0f);
        scale(glm::vec3(0.7f, 5.0f, 0.7f));
        setSize(20.0f);
    }
    Arrow(const glm::vec3& c) : Model("model/arrow-obj/arrow.obj", false) {
        color = c;
        scale(glm::vec3(0.7f, 5.0f, 0.7f));
        setSize(20.0f);
    }
    ~Arrow() {

    }

    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) {
        // actiavte the shader program
        glUseProgram(shader);

        // get the locations and send the uniforms to the shader
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

        Model::Draw(shader);
    }
    void update() {
        Model::update();
    }

private:
    glm::vec3 color;
};

#endif /* Arrow_hpp */
