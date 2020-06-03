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
    GenerateScore();

    // Debug Message
    // for(int i=0; i<this->scoreCount; i++){
    //     std::cout << this->scoreStatus[i].x << " " << this->scoreStatus[i].y << " " << this->scoreStatus[i].z << std::endl;
    // }
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
        this->scoreStatus.push_back(glm::vec3(*itx, -1.0f, (*itz)*-1.0f));
        xValue.erase(itx);
        zValue.erase(itz);
    }
    
    // For test purpose, hardcoded score position
    this->scoreStatus[0] = glm::vec3(57.0f, -1.0f, -6.0f);
    this->scoreStatus[4] = glm::vec3(55.0f, -1.0f, -10.0f);
}

void ScoreManager::UpdateScoreYCorrd(Terrain* terrain){
    //std::cout << "Get Called" << std::endl;
    for(int i=0; i<this->scoreStatus.size(); i++){
        this->scoreStatus[i].y = terrain->getHeight(this->scoreStatus[i].x*2, this->scoreStatus[i].z*-2);
    }

    // Debug Message
    for(int i=0; i<this->scoreCount; i++){
        std::cout << this->scoreStatus[i].x << " " << this->scoreStatus[i].y << " " << this->scoreStatus[i].z << std::endl;
    }
}


void ScoreManager::ScoreBeenEaten(int whichSphere, float scoreX, float scoreZ){
    for(int i=0; i<scoreStatus.size(); i++){
        if(scoreStatus[i].x == scoreX && scoreStatus[i].z == scoreZ){
            if(whichSphere == 1){
                scoreT1++;
            }
            else if(whichSphere == 2){
                scoreT2++;
            }
            //scoreStatus.erase(scoreStatus.begin()+i);
            //scoreCount--;
            //if(scoreCount < 0){
              //  scoreCount = 0;
            //}
            
            bool result = GenerateNewOne(i);
            while (result == false) {
                result = GenerateNewOne(i);
            }

        }

    }
}


bool ScoreManager::GenerateNewOne(int index) {
    srand((unsigned)time(0));
    scoreStatus[index].x = (rand() % 100 + 10);
    scoreStatus[index].z = (rand() % 100 + 10) * -1;
    for (int i = 0; i < scoreStatus.size(); i++) {
        if (i != index) {
            if (scoreStatus[i].x == scoreStatus[index].x && scoreStatus[i].z == scoreStatus[index].z) {
                return false;
            }
        }
    }
    return true;
}

