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

class Coin : public Model {
public:
    Coin();
    ~Coin();
    
    // factory method generate coin
    // TODO: need to be deleted when remove
    static Coin* generateCoin(const glm::vec3& pos){
        Coin* res = new Coin();
        res->move(pos);
        return res;
    }

    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
    void update();
    //void reset();
    
private:
    void spin(float deg);
};

#endif /* Coin_hpp */
