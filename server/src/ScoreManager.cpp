//
//  ScoreManager.cpp 
//  Gaia
//
//  Created by Marino Wang on 5/29/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "ScoreManager.hpp"

ScoreManager::ScoreManager(int num){
    this->scoreCount = num;    
}

void ScoreManager::GenerateScore(){
    srand((unsigned) time(0));
    std::set<float> xValue;
    std::set<float> zValue;

    while(xValue.size() < this->scoreCount){
        xValue.insert(rand()%100+10);
    }
    while(zValue.size() < this->scoreCount){
        zValue.insert(rand()%100+10);
    }

    for(int i=0; i<this->scoreCount; i++){
        auto itx = xValue.begin();
        auto itz = zValue.begin();
        this->scoreStatus.push_back(glm::vec3(*itx, -1.0f, *itz));
        xValue.erase(itx);
        zValue.erase(itz);
    }
    
    //scoreStatus.erase(glm::vec3(0.0f,-1.0f,0.0f));

    int randomNumber = rand()%110;
}
