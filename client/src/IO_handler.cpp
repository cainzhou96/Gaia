//
//  IO_handler.cpp
//  Gaia
//
//  Created by Marino Wang on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "IO_handler.hpp"

using namespace std;
namespace pt = boost::property_tree;

IO_handler::IO_handler(int type){
    ctype = type;
}

void IO_handler::SendKeyBoardInput(int direction, glm::vec3 lookat){
    
    switch(direction){
        case 0:
            currDir = "w";
            camLookAt = lookat;
            break;
        case 1:
            currDir = "a";
            camLookAt = lookat;
            break;
        case 2:
            currDir = "s";
            camLookAt = lookat;
            break;
        case 3:
            currDir = "d";
            camLookAt = lookat;
            break;
        
    }
    //ifPressed = true;
}

void IO_handler::SendMouseInput(int leftOrRight, glm::vec2 start, glm::vec2 end){
    if(leftOrRight == 0){
        cerr << "WHY YOU ARE HERE???" << endl;
        return;
    }
    else if(leftOrRight == 1){
        startPos = start;
        endPos = end;
        currBut = "l";

        //cout << endPos.x << " " << endPos.y << endl;
//        cerr << currBut << endl;
//        cout << "drag start: " << startPos[0] << ", " << startPos[1] << endl;
//        cout << "drag end: " << endPos[0] << ", " << endPos[1] << endl;
    }
    else if(leftOrRight == 2){
        startPos = start;
        endPos = end;
        currBut = "r";

//        cerr << currBut << endl;
//        cout << "drag start: " << startPos[0] << ", " << startPos[1] << endl;
//        cout << "drag end: " << endPos[0] << ", " << endPos[1] << endl;
    }
    else{
        cerr << "???????" << endl;
        return;
    }
    //ifClicked = true;
}

void IO_handler::SendPackage(chat_client* c){
    pt::ptree root;
    pt::ptree cmd;
    
    pt::ptree cmd_key;
    pt::ptree cmd_mouse;
    pt::ptree cam_lookatfront;
    pt::ptree cam_Prop;
    
    pt::ptree mouse_l;
    pt::ptree mouse_r;
    pt::ptree allPos[4];
    pt::ptree camPos[3];
    
    cmd_key.put("key", currDir);
    
    cmd_mouse.put("mouse", currBut);
        
    allPos[0].put("", startPos.x);
    allPos[1].put("", startPos.y);
    allPos[2].put("", endPos.x);
    allPos[3].put("", endPos.y);
        
    if(currBut.compare("l") == 0){
        for(int i=0; i<4; i++){
            mouse_l.push_back(std::make_pair("", allPos[i]));
        }
    }
    else if (currBut.compare("r") == 0) {
        for(int i=0; i<4; i++){
            mouse_r.push_back(std::make_pair("", allPos[i]));
        }
    }
    else {
        //cerr << "string comparision error in io_handller 100" << endl;
    }
    
    camPos[0].put("", camLookAt.x);
    camPos[1].put("", camLookAt.y);
    camPos[2].put("", camLookAt.z);
    for(int i=0; i<3; i++){
        cam_lookatfront.push_back(std::make_pair("", camPos[i]));
    }

    
    
    currBut = "";
    currDir = "";

    
    cmd_mouse.add_child("mouse_l", mouse_l);
    cmd_mouse.add_child("mouse_r", mouse_r);
    cam_Prop.add_child("cam_lookatfront", cam_lookatfront);

    cmd.push_back(std::make_pair("", cmd_key));
    cmd.push_back(std::make_pair("", cmd_mouse));
    
    // it's actually cam
    cmd.push_back(std::make_pair("", cam_Prop));
    
    root.add_child("cmd", cmd);
//    root.add_child("Height_map", parseTerrain(terrainVec));
    
    stringstream ss;
    write_json(ss, root, false);
    c->write(ss.str());
}

boost::property_tree::ptree IO_handler::parseTerrain(vector<glm::vec3>* terrainVec){
    pt::ptree root;
    pt::ptree testRoot;
    int ind = 0;
    for (auto pos: *terrainVec){
        pt::ptree node;
        node.put("", pos.y);
        root.push_back(make_pair("", node));
        ind++;
    }
    stringstream ss;
    testRoot.add_child("height_map", root);
    write_json(ss, testRoot, false);
    return root;
}


