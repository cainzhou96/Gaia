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
#include <ctime>
#include "Terrain.hpp"
#include <set>


class ScoreManager{
public:
    
    ScoreManager(int num);
    void GenerateScore();
    void UpdateScoreYCorrd(Terrain* terrain);
    void ScoreBeenEaten(int whichSphere, float scoreX, float scoreY);
    bool GenerateNewOne(int index);

    // All scores position
    std::vector<glm::vec3> scoreStatus;
    

    // Is triggered by player
    std::vector<glm::vec3> beenEaten;


    int scoreCount;     
    
    int scoreT1 = 0;
    int scoreT2 = 0;
    
    
    
};
#endif /* ScoreManager_hpp */
