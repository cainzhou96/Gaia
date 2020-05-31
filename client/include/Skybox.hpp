//
//  Skybox.hpp
//  Gaia
//
//  Created by Wenlin Mao on 5/3/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "core.h"
#include "util.h"

#ifndef Skybox_hpp
#define Skybox_hpp

class Skybox{
public:
    Skybox();
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();
    
    void update();
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);

private:
    unsigned int cubemapTexture;
    
    GLuint VAO;
    GLuint VBO, EBO;
    

};


#endif /* Skybox_hpp */
