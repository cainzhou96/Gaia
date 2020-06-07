#ifndef GameManager_hpp
#define GameManager_hpp

#include <boost/enable_shared_from_this.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <array>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/foreach.hpp>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

#include "core.h"
#include "Terrain.hpp"
#include "Sphere.h"
#include "ScoreManager.hpp"

using namespace std;

class GameManager{
public:
    GameManager();

    void setStartTime();
    int UpdateTime();
    //void UpdateScore();    

    void update1(char op, glm::vec3 lookat);
    void update2(char op, glm::vec3 lookat);
    //void update1(string op, glm::vec3 lookat);
    //void update2(string op, glm::vec3 lookat);

    void editTerrain(std::vector<glm::vec2> & editPoints, float height);
    void handle_input(string data, int id);
    void checkTerrainCollisions(Sphere* sphere);
    void checkSphereCollisions();
    void updatePhysics(); 
    void checkScoreCollision(); 
    void checkBounds(); 

    string encode(int id);
    void decode(int id, string data, string & key_op, string & mouse_op, glm::vec3 & camLookatFront, vector<glm::vec2> & editPoints);
    void restartGame();

    Terrain * terrain;
    ScoreManager* scoreManager;
    int scoreFlag;
    
    string currTime;
    int timeSignal = 0;
    // int scoreT1 = -1;
    // int scoreT2 = -2;

    time_t startTime;
    time_t endTime;
    float totalGameTime;

    Sphere* sphere1;
    Sphere* sphere2;
    
    glm::mat4 transM1, transM2; 

    bool updateTerrain;

    set<int> restartSet;
    int round;
    bool game_start;

    std::vector<vector<string>> edited_terrains;
    std::vector<string> edited_points;
    mutex mutex_arr[4];
    mutex mutex_terrain; 

    /*
    bool w1 = false;
    bool a1 = false;
    bool s1 = false;
    bool d1 = false;
    bool w2 = false;
    bool a2 = false;
    bool s2 = false;
    bool d2 = false;
    glm::vec3 lookat1;
    glm::vec3 lookat2;
    glm::vec3 right1;
    glm::vec3 right2;
    */
};

#endif 
