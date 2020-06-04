//
//  IO_handler.hpp
//  Gaia
//
//  Created by Marino Wang on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef IO_handler_hpp
#define IO_handler_hpp

#include <stdio.h>
#include <iostream>
#include "my_client.hpp"
#include "core.h"
#include <string>


class IO_handler{

private:
    int ctype = -1;
    std::string currDir = "";
    std::string currBut = "";
    
    bool ifPressed = false;
    bool ifClicked = false;
    
public:
    
    IO_handler(int type);
    glm::vec2 startPos = glm::vec2(0.0);
    glm::vec2 endPos = glm::vec2(0.0);
    glm::vec3 camLookAt = glm::vec3(0.0);
    void SendRestart(chat_client* c);
    void SendKeyBoardInput(int direction, glm::vec3 lookat);
    void SendMouseInput(int leftOrRight, glm::vec2 start, glm::vec2 end);
    void SendPackage(chat_client* c);
    //void SendPackage(chat_client* c, std::vector<glm::vec3>* terrainVec);
    void PressW();
    void PressA();
    void PressS();
    void PressD(chat_client* c);
    
    boost::property_tree::ptree parseTerrain(std::vector<glm::vec3>* terrainVec);
    
};


#endif /* IO_handler_hpp */
