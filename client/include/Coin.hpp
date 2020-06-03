//
//  Coin.hpp
//  Gaia
//
//  Created by Wenlin Mao on 5/30/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Coin_hpp
#define Coin_hpp


#include "core.h"
#include "Model.hpp"

class Coin {
public:
    Coin();
    ~Coin();
    
    // factory method generate coin
    // TODO: need to be deleted when remove
    static Coin* generateCoin(const glm::vec3& pos){
        Coin* res = new Coin();
        res->move(pos);
        res->scale(res->coinModel->scale_factor);
        return res;
    }
    
    void move (const glm::vec3& pos);
    void scale(float factor);
    
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
    void reset();
    
    glm::vec3 checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n);
    Model* coinModel;

    
private:
    unsigned int cubemapTexture;
    
    glm::mat4 model = glm::mat4(1.0f);
    
    
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;
    
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

#endif /* Coin_hpp */
