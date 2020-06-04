//
//  ScoreManager.hpp
//  Gaia
//
//  Created by Marino Wang on 5/29/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef ScoreManager_hpp
#define ScoreManager_hpp

#include "core.h"
#include "Terrain.hpp"
#include "constant.h"


class ScoreManager{
public:
    ScoreManager();
    std::vector<glm::vec3> scoreStatus;
    void UpdateScoreYCorrd(Terrain* terrain);
    
    
    
    
    
    
};
#endif /* ScoreManager_hpp */
