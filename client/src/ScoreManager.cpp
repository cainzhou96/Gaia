//
//  ScoreManager.cpp
//  Gaia
//
//  Created by Marino Wang on 5/29/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "ScoreManager.hpp"

ScoreManager::ScoreManager(){
    
}

void ScoreManager::UpdateScoreYCorrd(Terrain* terrain){
    //std::cout << "Get Called" << std::endl;
    for(int i=0; i<this->scoreStatus.size(); i++){
        this->scoreStatus[i].y = terrain->getHeight(this->scoreStatus[i].x*1, this->scoreStatus[i].z*-1) + COIN_SIZE/2;
    }

    // Debug Message
//    for(int i=0; i<this->scoreCount; i++){
//        std::cout << this->scoreStatus[i].x << " " << this->scoreStatus[i].y << " " << this->scoreStatus[i].z << std::endl;
//    }
}
