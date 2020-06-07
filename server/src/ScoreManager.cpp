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

    scoreStatus.clear();

    srand((unsigned) time(0));
    std::set<float> xValue;
    std::set<float> zValue;


    while(xValue.size() < this->scoreCount){
        xValue.insert(rand()%190+30);
    }
    while(zValue.size() < this->scoreCount){
        zValue.insert(rand()%190+30);
    }

    std::vector<float> xV(xValue.size());
    std::vector<float> zV(zValue.size());

    std::copy(xValue.begin(), xValue.end(), xV.begin());
    std::copy(zValue.begin(), zValue.end(), zV.begin());

    std::random_shuffle(xV.begin(), xV.end());
    std::random_shuffle(zV.begin(), zV.end());

    //std::random_shuffle(xValue.begin(), xValue.end());
    //std::shuffle(zValue.begin(), zValue.end());

    for(int i=0; i<this->scoreCount; i++){
        //auto itx = xValue.begin();
        //auto itz = zValue.begin();
        auto itx = xV.begin();
        auto itz = zV.begin();
        this->scoreStatus.push_back(glm::vec3(*itx, -1.0f, (*itz)*-1.0f));
        //xValue.erase(itx);
        //zValue.erase(itz);
        xV.erase(itx);
        zV.erase(itz);
        
    }
    xValue.clear();
    zValue.clear();
    
    // For test purpose, hardcoded score position
    //this->scoreStatus[0] = glm::vec3(57.0f, -1.0f, -6.0f);
    //this->scoreStatus[4] = glm::vec3(55.0f, -1.0f, -10.0f);
}

void ScoreManager::UpdateScoreYCorrd(Terrain* terrain){
    //std::cout << "Get Called" << std::endl;
    for(int i=0; i<this->scoreStatus.size(); i++){
        this->scoreStatus[i].y = terrain->getHeight(this->scoreStatus[i].x*TERRAIN_RES, this->scoreStatus[i].z*-TERRAIN_RES);
    }

    // Debug Message
    //for(int i=0; i<this->scoreCount; i++){
    //    std::cout << this->scoreStatus[i].x << " " << this->scoreStatus[i].y << " " << this->scoreStatus[i].z << std::endl;
    //}
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
    int tempx = rand() % 190 + 30;
    int tempz = (rand() % 190 + 30) * -1;
    //scoreStatus[index].x = (rand() % 230 + 10);
    //scoreStatus[index].z = (rand() % 230 + 10) * -1;
    for (int i = 0; i < scoreStatus.size(); i++) {
        //if (i != index) {
        if (scoreStatus[i].x == tempx && scoreStatus[i].z == tempz) {
            return false;
        }
        //}
    }
    scoreStatus[index].x = tempx;
    scoreStatus[index].z = tempz;
    return true;
}

