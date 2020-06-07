//
//  Wind.h
//  cse169
//
//  Created by Wenlin Mao on 2/27/20.
//  Copyright © 2020 Wenlin Mao. All rights reserved.
//

#ifndef Wind_h
#define Wind_h

#include "core.h"

class Wind {
public:
    Wind(){}
    Wind(glm::vec3 wd) : windV(wd){}
    Wind(glm::vec3 wd, float ad, float dc) : windV(wd), airDensity(ad), dragCoeff(dc){}
    ~Wind() {}
    
    void zeroWindV() { windV = glm::vec3(0.0f); }
    void adjustWindV(float dir, float value) {windV[dir] += value;}

    glm::vec3 windV;
    float airDensity = 0.2f;
    float dragCoeff = 0.2f;
};

#endif /* Wind_h */
