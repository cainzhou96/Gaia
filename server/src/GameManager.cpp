#include "GameManager.hpp"
#include "constant.h"
#include "PhysicsConstant.h"

using namespace std;
namespace pt = boost::property_tree;

glm::vec3 start1 = glm::vec3(62, 0, -62); 
glm::vec3 start2 = glm::vec3(189, 0, -189); 

GameManager::GameManager(): updateTerrain(false), round(0), game_start(true){
    currTime = "";
    //startTime = clock();
    startTime = time(NULL);
    totalGameTime = TOTAL_GAME_TIME;
    scoreManager = new ScoreManager(10);
    terrain = new Terrain(251, 251, 1.0f);
    terrain->computeBoundingBoxes();

    terrain->reset(round + SEED_OFFSET);
    //cout << "Just after the constructor in GameManager.cpp, now we have: " << terrain->height.size() << endl;
    // for(int i=0; i<terrain->height.size(); i++){
    //     if(i < 50){
    //         cout << terrain->height[i] << " ";
    //     }
    // }
    //scoreManager->UpdateScoreYCorrd(terrain);
    //scoreFlag = -1;
    sphere1 = new Sphere(MASS, RADIUS);
    sphere1->move(glm::vec3(start1.x,RADIUS,start1.z));

    sphere2 = new Sphere(MASS, RADIUS);
    sphere2->move(glm::vec3(start2.x, RADIUS,start2.z));

    for (int i = 0; i < 4 ; i++){
        mutex_arr[i].lock();
        edited_terrains.push_back({});
        mutex_arr[i].unlock();
    }
}

// void GameManager::UpdateScore(){
//     //obj.score++;
//     // Need to determine which team to add score
// }

void GameManager::setStartTime() {
    startTime = time(NULL);
}

int GameManager::UpdateTime(){
    string finishedTime = "";
    endTime = time(NULL);
    //timeSignal = 1;
    //float duration = totalGameTime - (float)(endTime-startTime) / CLOCKS_PER_SEC;
    double duration = totalGameTime - difftime(endTime, startTime);
    finishedTime = finishedTime + to_string((int)duration/60);
    if((int)duration % 60 < 10){
        finishedTime += ":0" + to_string((int)duration%60);
    } else {
        finishedTime += ":" + to_string((int)duration%60);
    }
    if(duration <= 0){
        // Send a signal to announce game ends
        duration = 0;
        //timeSignal = 0;
        finishedTime = "0:00";
        return 0;
    }
    //cout << finishedTime << endl;
    currTime = finishedTime;
    return 1;
}

void GameManager::update1(char op, glm::vec3 lookat) {
    glm::vec3 newPos;
    glm::vec3 right = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));
    lookat = glm::normalize(lookat);

    float speed = SPEED;

    switch (op) {
    case 'w': {
        glm::vec3 f = glm::vec3(lookat.x * speed, 0, lookat.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        //r.y += sphere1->getRadius() / 2.0f; 
        sphere1->applyMoveForce(f, r);
        break;
    }
    case 'a': {
        glm::vec3 f = glm::vec3(-right.x * speed, 0, -right.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        //r.y += sphere1->getRadius() / 2.0f;
        sphere1->applyMoveForce(f, r);
        break;
    }
    case 's': {
        glm::vec3 f = glm::vec3(-lookat.x * speed, 0, -lookat.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        //r.y += sphere1->getRadius() / 2.0f;
        sphere1->applyMoveForce(f, r);
        break;
    }
    case 'd': {
        glm::vec3 f = glm::vec3(right.x * speed, 0, right.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        //r.y += sphere1->getRadius() / 2.0f;
        sphere1->applyMoveForce(f, r);
        break;
    }
    }
}

void GameManager::update2(char op, glm::vec3 lookat) {
    glm::vec3 newPos;
    glm::vec3 right = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));

    lookat = glm::normalize(lookat);

    float speed = SPEED;

    switch (op) {
    case 'w': {
        glm::vec3 f = glm::vec3(lookat.x * speed, 0, lookat.z * speed) * sphere1->mass;
        glm::vec3 r = sphere2->getCenter();
        //r.y += sphere2->getRadius() / 2.0f;
        sphere2->applyMoveForce(f, r);
        break;
    }
    case 'a': {
        glm::vec3 f = glm::vec3(-right.x * speed, 0, -right.z * speed) * sphere1->mass;
        glm::vec3 r = sphere2->getCenter();
        //r.y += sphere2->getRadius() / 2.0f;
        sphere2->applyMoveForce(f, r);
        break;
    }
    case 's': {
        glm::vec3 f = glm::vec3(-lookat.x * speed, 0, -lookat.z * speed) * sphere1->mass;
        glm::vec3 r = sphere2->getCenter();
        //r.y += sphere2->getRadius() / 2.0f;
        sphere2->applyMoveForce(f, r);
        break;
    }
    case 'd': {
        glm::vec3 f = glm::vec3(right.x * speed, 0, right.z * speed) * sphere1->mass;
        glm::vec3 r = sphere2->getCenter();
        //r.y += sphere2->getRadius() / 2.0f;
        sphere2->applyMoveForce(f, r);
        break;
    }
    }
}

/*
void GameManager::update1(string op, glm::vec3 lookat) {
    glm::vec3 newPos;
    right1 = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));
    lookat1 = glm::normalize(lookat);

    float speed = SPEED;
    if (op == "w_u") {
        w1 = false;
    }
    else if (op == "a_u") {
        a1 = false;
    }
    else if (op == "s_u") {
        s1 = false;
    }
    else if (op == "d_u") {
        d1 = false;
    }
    else if (op == "w_d") {
        w1 = true;
    }
    else if (op == "a_d") {
        a1 = true;
    }
    else if (op == "s_d") {
        a1 = true;
    }
    else if (op == "d_d") {
        d1 = true;
    }

}

void GameManager::update2(string op, glm::vec3 lookat) {
    glm::vec3 newPos;
    right2 = glm::normalize(glm::cross(lookat, glm::vec3(0.0f, 1.0f, 0.0f)));
    lookat2 = glm::normalize(lookat);

    float speed = SPEED;
    if (op == "w_u") {
        w2 = false;
    }
    else if (op == "a_u") {
        a2 = false;
    }
    else if (op == "s_u") {
        s2 = false;
    }
    else if (op == "d_u") {
        d2 = false;
    }
    else if (op == "w_d") {
        w2 = true;
    }
    else if (op == "a_d") {
        a2 = true;
    }
    else if (op == "s_d") {
        a2 = true;
    }
    else if (op == "d_d") {
        d2 = true;
    }

}
*/

void GameManager::editTerrain(std::vector<glm::vec2> & editPoints, float height){
    //cout << "editing terrain..." << endl;
    //std::cout << editPoints[0][0] << ", " << editPoints[0][1] << " & " << editPoints[1][0] << ", " << editPoints[1][1] << std::endl;

    glm::vec2 sT = glm::vec2(editPoints[0][0] * TERRAIN_RES, editPoints[0][1] * -TERRAIN_RES);
    glm::vec2 eT = glm::vec2(editPoints[1][0] * TERRAIN_RES, editPoints[1][1] * -TERRAIN_RES);
    std::vector<glm::vec2> temp = { sT, eT };
    for (int i = 0; i < 4; ++i)
    {
        edited_terrains[i].push_back(std::to_string(sT[0]) + "," + std::to_string(sT[1]) + ","
            + std::to_string(eT[0]) + "," + std::to_string(eT[1]) + "," + std::to_string(height));
    }
    if (abs(temp[0][0] - temp[1][0]) <= 3 && abs(temp[0][1] - temp[1][1]) <= 3) {
        terrain->editPoint(temp[0], height);
    }
    else {
        terrain->edit(temp, height);
    }
}

void GameManager::handle_input(string data, int id){
    std::string key_op = "";
    std::string mouse_op = "";

    std::vector<glm::vec2> editPoints;
    float height = PEN_HEIGHT;
    glm::vec3 camLookatFront = glm::vec3(0.0);
    decode(id, data, key_op, mouse_op, camLookatFront, editPoints);
    //cout << camLookatFront.x << " " << camLookatFront.y << " " << camLookatFront.z << " " << endl;
    int player_id = (id - 1 + round) % 4 + 1;
    if(key_op != ""){
        cout << "player id: " << player_id << ", operation: "<< key_op << endl;
        if(player_id == 1){
            update1(key_op.at(0), camLookatFront);
            //update1(key_op, camLookatFront);
        }else if(player_id == 2){
            update2(key_op.at(0), camLookatFront);
            //update2(key_op, camLookatFront);
        }
    }
    if(!editPoints.empty()){
        if(mouse_op.compare("l") == 0){
            mutex_terrain.lock(); 
            editTerrain(editPoints, height);
            mutex_terrain.unlock(); 
            
            //this->scoreManager->ScoreBeenEaten(1, scoreManager->scoreStatus[0].x, scoreManager->scoreStatus[0].z);

        } 
        else if(mouse_op.compare("r") == 0){
            mutex_terrain.lock(); 
            editTerrain(editPoints, 0.0f);
            mutex_terrain.unlock(); 

            //terrain->setHeight(114,12,height*-1);
            //std::cout << "Y value at hardcode point is: " << terrain->getHeight(114,12) << std::endl;
            //std::cout << "Y value at hardcode point2 is: " << terrain->getHeight(110, 20) << std::endl;
            //this->scoreManager->ScoreBeenEaten(1, 55.0f, -10.0f);
            //this->scoreManager->ScoreBeenEaten(1, scoreManager->scoreStatus[scoreManager->scoreCount - 1].x, scoreManager->scoreStatus[scoreManager->scoreCount - 1].z);
            //this->scoreManager->ScoreBeenEaten(1, scoreManager->scoreStatus[scoreManager->scoreCount - 1].x, scoreManager->scoreStatus[scoreManager->scoreCount - 1].z);




            //this->scoreManager->UpdateScoreYCorrd(this->terrain);
            //scoreFlag = 1;
        }

        updateTerrain = true;
    }

}


string GameManager::encode(int id)
{
    transM1 = sphere1->getModel();
    transM2 = sphere2->getModel();
    pt::ptree root;
    pt::ptree obj;

    pt::ptree obj1;
    pt::ptree obj2;

    pt::ptree m1;
    pt::ptree m2;

    pt::ptree matrix1[16];
    pt::ptree matrix2[16];

    pt::ptree height_root;

    pt::ptree timeNode;
    pt::ptree scoreNode;


    obj1.put("id", 1);
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            matrix1[4*i+j].put("", transM1[i][j]);
        }
    }
    for(int i=0;i<16;i++){
        m1.push_back(std::make_pair("", matrix1[i]));
    }
    obj1.add_child("transformation", m1);

    obj2.put("id", 2);
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            matrix2[4*i+j].put("", transM2[i][j]);
        }
    }

    for(int i=0;i<16;i++){
        m2.push_back(std::make_pair("", matrix2[i]));
    }
    obj2.add_child("transformation", m2);
    obj.push_back(std::make_pair("", obj1));
    obj.push_back(std::make_pair("", obj2));
    
    //if(updateTerrain){
        // build and add current height map node to root

    while(!edited_terrains[id - 1].empty()){
        pt::ptree node;
        mutex_arr[id - 1].lock();
        string res = edited_terrains[id - 1].back();
        edited_terrains[id - 1].pop_back();
        mutex_arr[id - 1].unlock();
        node.put("", res);
        height_root.push_back(std::make_pair("", node));
    }

    pt::ptree tempNodeS1;
    pt::ptree tempNodeS2;
    tempNodeS1.put("", scoreManager->scoreT1);
    tempNodeS2.put("", scoreManager->scoreT2);
    scoreNode.push_back(std::make_pair("", tempNodeS1));
    scoreNode.push_back(std::make_pair("", tempNodeS2));

    // TODO： Change 10
    //const int T = this->scoreManager->scoreCount * 3 + 1;

    // HARDCODE FOR SCORE
    pt::ptree scoreCoordinate[10*3];
    pt::ptree scoreManagerNode;

    //scoreCoordinate[0].put("", this->scoreFlag);

    // if(scoreFlag != 0){
    //     if(scoreFlag == -1){
    //         std::cout << "Sending initial scoreCoord " << std::endl;
    //     } else if(scoreFlag == 1){
    //         std::cout << "Click update scoreCoord" << std::endl;
    //     }
    // }

    // if(scoreFlag != 0){
    //     scoreFlag =0;
    // }
    for(int i=0; i<this->scoreManager->scoreCount; i++){
        scoreCoordinate[i*3].put("", scoreManager->scoreStatus[i].x);
        scoreCoordinate[i*3+1].put("", scoreManager->scoreStatus[i].y);
        scoreCoordinate[i*3+2].put("", scoreManager->scoreStatus[i].z);
    }
    // while(!scoreManager->scoreStatus.empty()){
    //     pt::ptree nodet1, nodet2, nodet3;
    //     nodet1.put("", scoreManager->scoreStatus.back().x);
    //     nodet2.put("", scoreManager->scoreStatus.back().y);
    //     nodet3.put("", scoreManager->scoreStatus.back().z);
    //     scoreManagerNode.push_back(std::make_pair("", nodet1));
    //     scoreManagerNode.push_back(std::make_pair("", nodet2));
    //     scoreManagerNode.push_back(std::make_pair("", nodet3));
    //     scoreManager->scoreStatus.pop_back();
    // }

    for(int i=0; i<this->scoreManager->scoreCount*3; i++){
        scoreManagerNode.push_back(std::make_pair("", scoreCoordinate[i]));
    }

    // Add time to root
    pt::ptree tempNodeT;

    string finishedTime = "";
    endTime = time(NULL);
    //timeSignal = 1;
    //float duration = totalGameTime - (float)(endTime-startTime) / CLOCKS_PER_SEC;
    double duration = totalGameTime - difftime(endTime, startTime); 
    finishedTime = finishedTime + to_string((int)duration/60);
    if((int)duration % 60 < 10){
        finishedTime += ":0" + to_string((int)duration%60);
    } else {
        finishedTime += ":" + to_string((int)duration%60);
    }
    //cout << finishedTime << endl;


    tempNodeT.put("", finishedTime);
    timeNode.push_back(std::make_pair("",tempNodeT));

    root.add_child("Obj", obj);
    
    root.add_child("edited_points" ,height_root);
    
    root.add_child("Score", scoreNode);

    root.add_child("Time", timeNode);

    root.add_child("ScoreManager", scoreManagerNode);

    root.put("Header", "update");

    stringstream ss;
    write_json(ss, root, false);
    return ss.str() + '\n';
}

void GameManager::checkTerrainCollisions(Sphere* sphere) {
    std::vector<unsigned int>* indices = terrain->getIndices();
    std::vector<glm::vec3>* vertices = terrain->getVertices();
    std::vector<TerrainBoundingBox>* boxes = terrain->getBoundingBoxes();

    float elapsedTime = ELAPSED_TIME / 10.0f;
    glm::vec3 curForce = sphere->force;
    glm::vec3 curMoveForce = sphere->moveForce;
    glm::vec3 curTorque = sphere->torque;

    for (int k = 0; k < 10; k++) {
        sphere->force = curForce;
        sphere->moveForce = curMoveForce;
        sphere->torque = curTorque;
        bool hit = false; 
        for (int j = 0; j < boxes->size(); j++) {
            TerrainBoundingBox& box = (*boxes)[j];
            glm::vec2& tminPoint = box.minPoint;
            glm::vec2& tmaxPoint = box.maxPoint;
            glm::vec2 sminPoint(sphere->getCenter().x, sphere->getCenter().z);
            sminPoint += glm::vec2(-sphere->getRadius(), -sphere->getRadius());
            glm::vec2 smaxPoint(sphere->getCenter().x, sphere->getCenter().z);
            smaxPoint += glm::vec2(sphere->getRadius(), sphere->getRadius());

            if (sminPoint.x > tmaxPoint.x || tminPoint.x > smaxPoint.x || sminPoint.y > tmaxPoint.y || tminPoint.y > smaxPoint.y) { // not in box
                continue;
            }

            for (int i = 0; i < box.indices2triangles.size(); i++) {
                if (hit) continue; 
                int curInd = box.indices2triangles[i];
                glm::vec3& a = (*vertices)[(*indices)[curInd - 2]];
                glm::vec3& b = (*vertices)[(*indices)[curInd - 1]];
                glm::vec3& c = (*vertices)[(*indices)[curInd]];
                glm::vec3 n = -glm::normalize(glm::cross(c - a, b - a));
                if (glm::dot(n, glm::vec3(0, 1, 0)) < 0) { // little hack to make sure normals are upwards
                    n = -n;
                }

                hit = sphere->checkCollision(a, b, c, n, elapsedTime);

                //sphere->move(sphere->getCenter() + offset); // move to right position
            }
        }
        sphere->updatePosition(elapsedTime, hit);
        sphere->updateOrientation(elapsedTime);
        sphere->force = glm::vec3(0);
        sphere->moveForce = glm::vec3(0);
        sphere->torque = glm::vec3(0);
    }

    // if sphere has fallen off, freaking lift it up
    float height = terrain->getHeightAt(sphere->getCenter().x, sphere->getCenter().z);
    //std::cout << height << std::endl;
    //std::cout << sphere->getCenter().y + sphere->getRadius() << std::endl;
    if (height > sphere->getCenter().y + sphere->getRadius()) {
        glm::vec3 offset(0);
        offset.y = height - (sphere->getCenter().y - sphere->getRadius());
        sphere->move(sphere->getCenter() + offset);
    }
}

void GameManager::decode(int id, string data, string & key_op, string & mouse_op, glm::vec3 & camLookatFront, vector<glm::vec2> & editPoints)
{
    float temp[4];
    // Read JSON from client
    try{
        if(data != ""){

            stringstream ss;
            ss << data;

            pt::ptree tar;
            pt::read_json(ss, tar);

            string header = tar.get<string>("Header");
            
            if(header.compare("restart") == 0){
                restartSet.insert(id);
                if(restartSet.size() == 4 && game_start == false){
                    restartGame();
                    round += 2;
                    restartSet.clear();
                    game_start = true;
                }
            }else if(header.compare("data") == 0){
                int i = 0;
                BOOST_FOREACH(const pt::ptree::value_type& child, tar.get_child("cmd")) {
                    if(i == 0){
                        key_op = child.second.get<std::string>("key");
                    }
                    else if (i == 1){
                        // Mouse
                        mouse_op = child.second.get<std::string>("mouse");

                        if(mouse_op.compare("l") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_l")){
                                temp[index] = stof(t.second.data());
                                index++;
                            }
                        } else if(mouse_op.compare("r") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_r")){
                                temp[index] = stof(t.second.data());
                                index++;
                            }
                        }
                    }
                    else if( i == 2){
                        // Cam
                        float temp1[3];
                        int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("cam_lookatfront")){
                                temp1[index] = stof(t.second.data());
                                index++;
                            }
                        camLookatFront.x = temp1[0];
                        camLookatFront.y = temp1[1];
                        camLookatFront.z = temp1[2];
                    }
                    i++;
                }
            }

        }
    }catch(...){
        std::cout << data << std::endl;
        std::cout << "--------decode exception---------" << std::endl;

    }
    if(mouse_op != ""){
        editPoints.push_back(glm::vec2(temp[0],temp[1]));
        editPoints.push_back(glm::vec2(temp[2],temp[3]));
    }
}

void GameManager::restartGame(){
    cout << "restart " << endl;
    currTime = "";
    //startTime = clock();
    startTime = time(NULL);
    totalGameTime = TOTAL_GAME_TIME;

    sphere1->move(glm::vec3(start1.x, RADIUS, start1.z));
    sphere2->move(glm::vec3(start2.x, RADIUS, start2.z));
    sphere1->moveForce = glm::vec3(0); 
    sphere1->moveMomentum = glm::vec3(0); 
    sphere1->force = glm::vec3(0); 
    sphere1->momentum = glm::vec3(0); 
    sphere1->torque = glm::vec3(0); 
    sphere1->angMomentum = glm::vec3(0); 
    
    sphere2->moveForce = glm::vec3(0); 
    sphere2->moveMomentum = glm::vec3(0); 
    sphere2->force = glm::vec3(0); 
    sphere2->momentum = glm::vec3(0); 
    sphere2->torque = glm::vec3(0); 
    sphere2->angMomentum = glm::vec3(0); 

    //scoreManager->GenerateScore();
    scoreManager->scoreT1 = 0;
    scoreManager->scoreT2 = 0;
    terrain->reset(round + SEED_OFFSET);
}

void GameManager::checkSphereCollisions() {
    glm::vec3 sphere1Pos = sphere1->getCenter();
    glm::vec3 sphere2Pos = sphere2->getCenter();
    float sphere1Radius = sphere1->getRadius();
    float sphere2Radius = sphere2->getRadius();
    float delta = glm::length(sphere1Pos - sphere2Pos);
    if (delta < sphere1Radius + sphere2Radius) {
        glm::vec3 pointPos1; 
        glm::vec3 pointPos2; 
        glm::vec3 n; 
        if (delta == 0) {
            pointPos1 = sphere2->getCenter(); 
            pointPos2 = sphere1->getCenter(); 
            n = glm::vec3(0, 1, 0); 
        }
        else {
			pointPos1 = sphere1->getCenter() + sphere1->getRadius() * glm::normalize(sphere2Pos - sphere1Pos); 
			pointPos2 = sphere2->getCenter() + sphere2->getRadius() * glm::normalize(sphere1Pos - sphere2Pos); 
            n = glm::normalize(sphere2Pos - sphere1Pos);
        }
        glm::mat4 model1 = sphere1->getModel(); 
        glm::mat3 I1 = glm::mat3(model1) * sphere1->I0 * glm::transpose(glm::mat3(model1));
        glm::vec3 omega1 = glm::inverse(I1) * sphere1->angMomentum;
        glm::vec3 velocity1 = (sphere1->momentum + sphere1->moveMomentum) / sphere1->mass;
        glm::vec3 r1 = pointPos1 - sphere1->getCenter();
        glm::vec3 vr1 = velocity1 + glm::cross(omega1, r1);
        float m1 = sphere1->mass; 

        glm::mat4 model2 = sphere2->getModel(); 
        glm::mat3 I2 = glm::mat3(model2) * sphere2->I0 * glm::transpose(glm::mat3(model2));
        glm::vec3 omega2 = glm::inverse(I2) * sphere2->angMomentum;
        glm::vec3 velocity2 = (sphere2->momentum + sphere2->moveMomentum) / sphere2->mass;
        glm::vec3 r2 = pointPos2 - sphere2->getCenter();
        glm::vec3 vr2 = velocity2 + glm::cross(omega2, r2);
        float m2 = sphere2->mass; 

        glm::vec3 vr = vr2 - vr1; 
        float e = RESTITUTION; 
        float jr = (1 + e) * fmax(glm::dot(vr, -n), 0.0f) / ((1 / m1) + (1 / m2) + glm::dot(glm::inverse(I1) * (glm::cross(glm::cross(r1, n), r1)) + glm::inverse(I2) * (glm::cross(glm::cross(r2, n), r2)), n));
        glm::vec3 impulse1 = jr * (-n); 
        glm::vec3 impulse2 = jr * n; 
        glm::vec3 reactionForce1 = impulse1 / ELAPSED_TIME; 
        glm::vec3 reactionForce2 = impulse2 / ELAPSED_TIME; 
        sphere1->applyForce(reactionForce1, pointPos1); 
        sphere2->applyForce(reactionForce2, pointPos2); 
    }
}

void GameManager::updatePhysics() {
    /*
    float speed = SPEED;
    if (w1) {
        glm::vec3 f = glm::vec3(lookat1.x * speed, 0, lookat1.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        sphere1->applyMoveForce(f, r);
    }
    if (a1) {
        glm::vec3 f = glm::vec3(-right1.x * speed, 0, -right1.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        sphere1->applyMoveForce(f, r);
    }
    if (s1) {
        glm::vec3 f = glm::vec3(-lookat1.x * speed, 0, -lookat1.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        sphere1->applyMoveForce(f, r);
    }
    if (d1) {
        glm::vec3 f = glm::vec3(right1.x * speed, 0, right1.z * speed) * sphere1->mass;
        glm::vec3 r = sphere1->getCenter();
        sphere1->applyMoveForce(f, r);
    }
    if (w2) {
        glm::vec3 f = glm::vec3(lookat2.x * speed, 0, lookat2.z * speed) * sphere2->mass;
        glm::vec3 r = sphere2->getCenter();
        sphere2->applyMoveForce(f, r);
    }
    if (a2) {
        glm::vec3 f = glm::vec3(-right2.x * speed, 0, -right2.z * speed) * sphere2->mass;
        glm::vec3 r = sphere2->getCenter();
        sphere2->applyMoveForce(f, r);
    }
    if (s2) {
        glm::vec3 f = glm::vec3(-lookat2.x * speed, 0, -lookat2.z * speed) * sphere2->mass;
        glm::vec3 r = sphere2->getCenter();
        sphere2->applyMoveForce(f, r);
    }
    if (d2) {
        glm::vec3 f = glm::vec3(right2.x * speed, 0, right2.z * speed) * sphere2->mass;
        glm::vec3 r = sphere2->getCenter();
        sphere2->applyMoveForce(f, r);
    }
    */

    // add gravity
    sphere1->applyForce(glm::vec3(0, -9.8, 0) * sphere1->mass, sphere1->getCenter());
    sphere2->applyForce(glm::vec3(0, -9.8, 0) * sphere2->mass, sphere2->getCenter());

    checkSphereCollisions(); 
    mutex_terrain.lock(); 
    checkTerrainCollisions(sphere1);
    checkTerrainCollisions(sphere2);
    mutex_terrain.unlock(); 
    checkScoreCollision(); 
    checkBounds(); 
}

void GameManager::checkScoreCollision() {
    for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
        glm::vec3 scorePos = scoreManager->scoreStatus[i]; 
        if (glm::length(sphere1->getCenter() - scorePos) < COIN_SIZE + sphere1->getRadius()) {
            scoreManager->ScoreBeenEaten(1, scorePos.x, scorePos.z); 
        }
        if (glm::length(sphere2->getCenter() - scorePos) < COIN_SIZE + sphere2->getRadius()) {
            scoreManager->ScoreBeenEaten(2, scorePos.x, scorePos.z);
        }
    }

}

void GameManager::checkBounds() {
    float offset = 5.0f; 
    glm::vec3 sphere1Pos = sphere1->getCenter(); 
    glm::vec3 sphere2Pos = sphere2->getCenter(); 
    if (sphere1Pos.x < 0 + offset) {
        sphere1Pos.x = 0 + offset + sphere1->getRadius();
    }
    if (sphere1Pos.x > 251.0f - offset) {
        sphere1Pos.x = 251.0f - offset - sphere1->getRadius();
    }
    if (sphere1Pos.z > 0 - offset) {
        sphere1Pos.z = 0 - offset - sphere1->getRadius();
    }
    if (sphere1Pos.z < -251.0f + offset) {
        sphere1Pos.z = -251.0f + offset + sphere1->getRadius();
    }
    if (sphere2Pos.x < 0 + offset) {
        sphere2Pos.x = 0 + offset + sphere2->getRadius();
    }
    if (sphere2Pos.x > 251.0f - offset) {
        sphere2Pos.x = 251.0f - offset - sphere2->getRadius();
    }
    if (sphere2Pos.z > 0 - offset) {
        sphere2Pos.z = 0 - offset - sphere2->getRadius();
    }
    if (sphere2Pos.z < -251.0f + offset) {
        sphere2Pos.z = -251.0f + offset + sphere2->getRadius();
    }
    sphere1->move(sphere1Pos); 
    sphere2->move(sphere2Pos); 
}

