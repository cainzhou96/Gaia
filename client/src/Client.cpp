//
//  Client.cpp
//  Gaia
//
//  Created by Cain on 4/16/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Client.h"

namespace pt = boost::property_tree;

Sphere* Client::sphere_player1;
Sphere* Client::sphere_player2;
Arrow* Client::arrow_player1;
Arrow* Client::arrow_player2;
Sphere* Client::sphere_mouse; // testing only
Terrain* Client::terrain;
Skybox* Client::skybox;
std::vector<Coin*> Client::coins;

int Client::player_id = 0;
string Client::currTime = "Time shoud not be this";
int Client::score = 0;
int Client::oppo_score = 0;
time_t Client::timeStart;
time_t Client::timeNow;
int Client::totalTime = 300;
bool Client::inGame = false;
bool Client::game_wait = false;
bool Client::game_start = (DEBUG) ? true : false;
bool Client::game_over = false;
bool Client::game_restart = false;
bool Client::restart_send = false;
int Client::player_num = 0;
int Client::round_num = 0;

boost::asio::io_service Client::io_service;
//tcp::endpoint Client::endpoint(ip::address::from_string("127.0.0.1"),8888);
tcp::endpoint Client::endpoint(ip::address::from_string("137.110.112.150"), 8888);
//tcp::endpoint Client::endpoint(ip::address::from_string("137.110.115.249"),8888);
//tcp::endpoint Client::endpoint(ip::address::from_string("99.10.121.88"),8080);

chat_client Client::c(io_service, endpoint);
boost::thread Client::t(boost::bind(&boost::asio::io_service::run, &io_service));
std::string Client::msg;

Camera* Client::camera;
glm::vec3 Client::sphere1_pos = glm::vec3(0.0f);
glm::vec3 Client::sphere2_pos = glm::vec3(0.0f);
glm::vec2 Client::mousePos = glm::vec2(INFINITY, INFINITY);

bool Client::mouseControl = true;
bool Client::forward = false;
bool Client::backward = false;
bool Client::left = false;
bool Client::right = false;

int Client::isMouseButtonDown = 0;
glm::vec2 Client::clickPos = glm::vec2(INFINITY, INFINITY);
glm::vec2 Client::releasePos = glm::vec2(INFINITY, INFINITY);

IO_handler* Client::io_handler;
AudioManager* Client::audioManager;
ScoreManager* Client::scoreManager;

Client::Client(int width, int height) {
    window = new Window(width, height, "Window");
    std::pair<int, int> windowSize = window->getFrameBufferSize();
    this->width = windowSize.first;
    this->height = windowSize.second;
    camera = new Camera(glm::vec3(120, 158, 42), glm::vec3(120, 5, -72.5f));

    projection = glm::perspective(glm::radians(60.0), double(width) / (double)height, 1.0, 1000.0);

    // Print OpenGL and GLSL versions.
    printVersions();
    // Setup OpenGL settings.
    setupOpenglSettings();

    setupCallbacks();
}

Client::~Client() {
    // Deallcoate the objects.
    delete sphere_player1;
    delete sphere_player2;
    delete sphere_mouse;
    delete terrain;
    delete camera;


    // Delete the shader program.
    glDeleteProgram(shaderProgram);

    delete window;
}

bool Client::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
    skyboxProgram = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
    terrainProgram = LoadShaders("shaders/terrain.vert", "shaders/terrain.frag", "shaders/terrain.geom");
    toonProgram = LoadShaders("shaders/toon.vert", "shaders/toon.frag");
    modelProgram = LoadShaders("shaders/model.vert", "shaders/model.frag");
    multiTextureProgram = LoadShaders("shaders/multitextureTerrain.vert", "shaders/multitextureTerrain.frag");
//    terrainProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");


    // Check the shader program.
    if (!shaderProgram)
    {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    // Check the shader program.
    if (!skyboxProgram)
    {
        std::cerr << "Failed to initialize skybox program" << std::endl;
        return false;
    }


    // Create io_handler (0 for balls)
    io_handler = new IO_handler(0);

    audioManager = new AudioManager();
    scoreManager = new ScoreManager();

    return true;
}

bool Client::initializeObjects()
{
    vector<std::string> faces =
    {
        "textures/dark/posx.png",
        "textures/dark/negx.png",
        "textures/dark/posy.png",
        "textures/dark/negy.png",
        "textures/dark/posz.png",
        "textures/dark/negz.png",
    };
    skybox = new Skybox(faces);
    
    vector<std::string> faces_sp1 =
    {
        "textures/Saturn/posx.png",
        "textures/Saturn/negx.png",
        "textures/Saturn/posy.png",
        "textures/Saturn/negy.png",
        "textures/Saturn/posz.png",
        "textures/Saturn/negz.png",
    };

    std::vector<std::string> faces_sp2 =
    {
        "textures/Mercury/posx.png",
        "textures/Mercury/negx.png",
        "textures/Mercury/posy.png",
        "textures/Mercury/negy.png",
        "textures/Mercury/posz.png",
        "textures/Mercury/negz.png",
    };

    sphere_player1 = new Sphere(5.0f, RADIUS, faces_sp1);
    //sphere_player1->move(glm::vec3(64, RADIUS, -65));
    sphere_player1->move(glm::vec3(0, RADIUS * 2 + 2, 0));

    
    sphere_player2 = new Sphere(5.0f, RADIUS, faces_sp2);
    //sphere_player2->move(glm::vec3(58, RADIUS, -54));
    // testing only
    sphere_mouse = new Sphere(1.0f, 0.7f, faces_sp1);

    arrow_player1 = new Arrow(glm::vec3(1.0f, 1.0f, 0.0f));
    arrow_player2 = new Arrow(glm::vec3(0.0f, 0.0f, 1.0f));

    terrain = new Terrain(251, 251, 1.0f);
    terrain->reset(round_num + SEED_OFFSET);
    terrain->computeBoundingBoxes();
    
    //coins.push_back(Coin::generateCoin(glm::vec3(62.5, 0, -62.5)));

    // Hardcode value for score position count ,currently 10
    //for (int i = 0; i < 10; i++) {
    //    coins.push_back(Coin::generateCoin(glm::vec3(0.0f, 0.0f, 0.0f)));
    //}

    return true;
}

void Client::idleCallback() {

    /*
    // movement update
    if (forward) {
        io_handler->SendKeyBoardInput(0, false, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else {
        io_handler->SendKeyBoardInput(0, true, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    if (left) {
        io_handler->SendKeyBoardInput(1, false, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else {
        io_handler->SendKeyBoardInput(1, true, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    if (backward) {
        io_handler->SendKeyBoardInput(2, false, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else {
        io_handler->SendKeyBoardInput(2, true, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    if (right) {
        io_handler->SendKeyBoardInput(3, false, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else {
        io_handler->SendKeyBoardInput(3, true, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    */
    // movement update
    if (forward) {
        //glm::vec3 f = sphere_player1->moveForce;
        //f.x += 20.0f;
        //sphere_player1->moveForce = f;
        io_handler->SendKeyBoardInput(0, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else if (left) {
        //glm::vec3 f = sphere_player1->moveForce;
        //f.z += 20.0f;
        //sphere_player1->moveForce = f;
        io_handler->SendKeyBoardInput(1, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else if (backward) {
        //glm::vec3 f = sphere_player1->moveForce;
        //f.x -= 20.0f;
        //sphere_player1->moveForce = f;
        io_handler->SendKeyBoardInput(2, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }
    else if (right) {
        //glm::vec3 f = sphere_player1->moveForce;
        //f.z -= 20.0f;
        //sphere_player1->moveForce = f;
        io_handler->SendKeyBoardInput(3, camera->frontVector);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        io_handler->SendPackage(&c);
    }

    for (Coin* c : coins) {
        c->update();
    }
    arrow_player1->move(sphere1_pos + glm::vec3(0.0f, 15.0f, 0.0f));
    arrow_player1->update();

    arrow_player2->move(sphere2_pos + glm::vec3(0.0f, 15.0f, 0.0f));
    arrow_player2->update();

    window->setId(player_id);
    window->setTime(currTime);
    window->setScore(score);
    window->setOppoScore(oppo_score);
    game_restart = window->getRestart();
    if (game_restart && !restart_send) {
        // Send signal to server
        io_handler->SendRestart(&c);
        restart_send = true;
    }
    window->setPlayerNum(player_num);
    window->setGameStart(game_start);
    window->setGameOver(game_over);
}

void Client::displayCallback() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the objects
    
    for (Coin* c : coins){
        c->draw(camera->getView(), projection, modelProgram);
    }
    
    sphere_player1->draw(camera->getView(), projection, skyboxProgram);
    sphere_player2->draw(camera->getView(), projection, skyboxProgram);
    arrow_player1->draw(camera->getView(), projection, shaderProgram);
    arrow_player2->draw(camera->getView(), projection, shaderProgram);

    //terrain->draw(camera->getView(), projection, camera->getPos(), toonProgram);
    terrain->multiTextureDraw(camera->getView(), projection, camera->getPos(), multiTextureProgram);
    skybox->draw(camera->getView(), projection, skyboxProgram);
    
    //sphere_mouse->draw(camera->getView(), projection, skyboxProgram);
}

bool Client::initialize() {
    return initializeProgram() && initializeObjects();
}

void Client::setupOpenglSettings()
{

    // Set the viewport size.
    glViewport(0, 0, width, height);

    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor

    // glEnable(GL_CULL_FACE);
}

void Client::printVersions()
{
  // Get info of GPU and supported OpenGL version.
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
    << std::endl;

  //If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
  std::cout << "Supported GLSL version is: " <<
  glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

void Client::run() {
    if (!initialize())
    throw "Unable to initialize client";

    // Client Try
    try
    {
//        boost::asio::io_service io_service;
//        tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"),8888);
//        chat_client c(io_service, endpoint);
//        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
//        std::string msg;

        // Loop while GLFW window should stay open.
        while (!glfwWindowShouldClose(window->getWindow())) {
            
            // Main render display callback. Rendering of objects is done here. (Draw)

            player_id = (c.get_id()-1+round_num)%4+1;

            if(player_id == 1){

                // Might not need to be in while loop, save for now, might optimize later
                mouseControl = true;
                camera->setLookAt(glm::vec3(sphere1_pos.x, sphere1_pos.y,sphere1_pos.z));
                camera->eyePos = sphere1_pos + glm::normalize(camera->eyePos - camera->lookAtPos)* 60.0f;

            }
            else if(player_id == 2){
                mouseControl = true;
                camera->setLookAt(glm::vec3(sphere2_pos.x, sphere2_pos.y,sphere2_pos.z));
                camera->eyePos = sphere2_pos + glm::normalize(camera->eyePos - camera->lookAtPos)* 60.0f;
            }
            else{
                // camera for terrian player is fixed
                mouseControl = false;
                camera->setLookAt(glm::vec3(120, 5, -70));
                camera->eyePos = glm::vec3(120, 158, 42);
                sphere_mouse->draw(camera->getView(), projection, skyboxProgram);
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            displayCallback();
            window->displayCallback();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            window->updateWindow();

            //camera = new Camera(glm::vec3(60, 59, 21), glm::vec3(60, 5, -30));

            // Sphere player and Terrian player Camera Logic


            //cout << camera->getLookAtPos().x << " " << camera->getLookAtPos().y << " " << camera->getLookAtPos().z << endl;
            //cout << camera->getPos().x << " " << camera->getPos().y << " " << camera->getPos().z << endl;

            // Idle callback. Updating objects, etc. can be done here. (Update)
            idleCallback();

//            io_handler -> SendPackage(&c);
            updateFromServer(c.getMsg());
        }
        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void Client::errorCallback(int error, const char* description)
{
    // Print error.
    std::cerr << description << std::endl;
}

void Client::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    /*
      * TODO: Modify below to add your key callbacks.
      */

      // Check for a key press.
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:{
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            }
            // take user's io
            case GLFW_KEY_W:{
                forward = true;
                break;
            }
            case GLFW_KEY_A:{
                left = true;
                break;
            }
            case GLFW_KEY_S:{
                backward = true;
                break;
            }
            case GLFW_KEY_D:{
                right = true;
                break;
            }
            case GLFW_KEY_P:{
                // DEBUG: REMEMBER TO DELETE FOR RELEASE
                audioManager->PlaySounds(0);
                audioManager->PlaySounds(1);
                break;
            }
            case GLFW_KEY_M:{
                // DEBUG: REMEMBER TO DELETE FOR RELEASE
                if(audioManager->volumeControl == 0){
                    audioManager->volumeControl = 1;
                    audioManager->VolumeControl();
                } else {
                    audioManager->volumeControl = 0;
                    audioManager->VolumeControl();
                }
                break;
            }
            case GLFW_KEY_C:{
                for(int i=0; i<scoreManager->scoreStatus.size(); i++){
                    cout << "ScoreM: " << scoreManager->scoreStatus[i].x << " " << scoreManager->scoreStatus[i].y << " " << scoreManager->scoreStatus[i].z << endl;
                        // << "  ModelC: " << coins[i]->coinModel->model_center.x << " " << coins[i]->coinModel->model_center.y << " " << coins[i]->coinModel->model_center.z << endl;
                }
                break;
            }
            default:
                break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:{
                forward = false;
                break;
            }
            case GLFW_KEY_A:{
                left = false;
                break;
            }
            case GLFW_KEY_S:{
                backward = false;
                break;
            }
            case GLFW_KEY_D:{
                right = false;
                break;
            }
            default:
                break;
        }
    }
}

void Client::setupCallbacks()
{
    // Set the error callback.
    glfwSetErrorCallback(errorCallback);

    // Set the key callback.
    glfwSetKeyCallback(window->getWindow(), keyCallback);

    glfwSetCursorPosCallback(window->getWindow(), cursorPositionCallback);

    glfwSetMouseButtonCallback(window->getWindow(), setMouseButtonCallback);
}


void Client::setMouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);

    if(action == GLFW_PRESS){
        // Check to see if we need to set click start position
        if(clickPos.x == INFINITY && clickPos.y == INFINITY){
            clickPos = glm::vec2((float)xpos, (float)ypos);
        }

        if(button==GLFW_MOUSE_BUTTON_RIGHT){
            //cout << "RIGHT!!" << endl;
            isMouseButtonDown = 2;
        }
        else if(button==GLFW_MOUSE_BUTTON_LEFT){
            //cout << "LEFT!!" << endl;
            isMouseButtonDown = 1;
        }
        else{
            cout << "unknown click?" << endl;
        }

//        if(isMouseButtonDown > 0){
//            string leftOrRight = "left";
//            if(isMouseButtonDown == 2){
//                leftOrRight = "right";
//            }
//            cout << leftOrRight << " click on: " << xpos << ", " << ypos << endl;
//        }
    }
    else if (action == GLFW_RELEASE){
        releasePos = glm::vec2((float)xpos, (float)ypos);
        //cout << "drag start: " << clickPos[0] << ", " << clickPos[1] << endl;
        //cout << "drag end: " << releasePos[0] << ", " << releasePos[1] << endl;

        // send i/o to server
        //io_handler->SendMouseInput(isMouseButtonDown, clickPos, releasePos);

        glm::vec2 translatedCPos = screenPointToWorld(clickPos);
        glm::vec2 translatedRPos = screenPointToWorld(releasePos);
        if(player_id == 3 || player_id == 4){
            io_handler->SendMouseInput(isMouseButtonDown, translatedCPos, translatedRPos);
            io_handler -> SendPackage(&c);
        }
        cout << "finalPos x: " << translatedRPos.x << " finalPos y: " << translatedRPos.y << endl;
        isMouseButtonDown = 0;
        clickPos = glm::vec2(INFINITY, INFINITY);
        releasePos = glm::vec2(INFINITY, INFINITY);
    }
}

glm::vec2 Client::screenPointToWorld(glm::vec2 mousePos){
    glm::vec3 w;
    glm::vec3 u;
    glm::vec3 v;
    float fov = glm::radians(60.0f);
    float a, b, t;
    float wWidth = 1920.0f;
    float wHeight = 1080.0f;
    glm::vec3 rayDir;
    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 finalPos;

    w = glm::normalize(camera->getPos() - camera->getLookAtPos());
    u = glm::normalize(glm::cross(camera->getUpVector(), w));
    v = glm::cross(w,u);

    // Finally worked version
    a = glm::tan(fov/2) * (wWidth /wHeight) * ((mousePos.x - (float)wWidth/2) / (wWidth/2));
    b = glm::tan(fov/2) * (((wHeight/2) - mousePos.y)/ (wHeight/2));

    rayDir = glm::normalize(a*u + b*v - w);

    t = glm::dot((glm::vec3(0.0f, 0.0f, 0.0f) - camera->getPos()), normal)/glm::dot(rayDir, normal);
    finalPos = camera->getPos()+t * rayDir;

    //cout << "finalPos x: " << finalPos.x << " finalPos y: " << finalPos.y << " finalPos z: " << finalPos.z << endl;

    return glm::vec2(finalPos.x, finalPos.z);
}


void Client::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

    glm::mat4 mtx = glm::mat4(1.0f);
    glm::vec2 translatedMPos = screenPointToWorld(glm::vec2(xpos, ypos));
    if(translatedMPos.x >= 0 && translatedMPos.x <= 251 && translatedMPos.y <= 0 && translatedMPos.y >= -251){
        mtx[3] = glm::vec4(translatedMPos.x,2,translatedMPos.y,1);
        sphere_mouse->move(mtx);
    }

    if (mousePos.x  == INFINITY || mousePos.y == INFINITY) {
        mousePos.x = xpos;
        mousePos.y = ypos;

        //return;
    }


//    if(isMouseButtonDown > 0){
//        string leftOrRight = "left";
//        if(isMouseButtonDown == 2){
//            leftOrRight = "right";
//        }
//        cout << leftOrRight << " click on: " << xpos << ", " << ypos << endl;
//    }


    if (mouseControl) {
        float xoffset = xpos - mousePos.x;
        float yoffset = ypos - mousePos.y;
        mousePos.x = xpos;
        mousePos.y = ypos;
        camera->rotateAround(xoffset, yoffset);
    }
}


void Client::updateFromServer(string msg) {
    //cout << msg << endl;
    try{
        if(msg != ""){
            stringstream ss;
            ss << msg;

            pt::ptree tar;
            pt::read_json(ss, tar);
            string header = tar.get<string>("Header");
            // waiting room
            if(header.compare("wait") == 0){
                if (game_wait == false) {
                    // TODO::Play waiting room BGM
                    audioManager->PlayBackgroundMusic0();
                    cout << "11111" << endl;
                }
                string player = tar.get<string>("players");
                cout << "total players:" <<  player << endl;
                player_num = stoi(player);
                game_wait = true;
            }
            // game ends
            else if(header.compare("end") == 0){
                if (game_start == true && game_over == false) {
                    terrain->reset(round_num + SEED_OFFSET);
                    // TODO::Close main game BGM and play game over BGM
                    //scoreManager->scoreStatus.clear();
                    //coins.clear();
                    audioManager->PlayBackgroundMusic1();
                    cout << "33333" << endl;
                }
                game_over = true;
                game_start = false;
                //cout << "Game Ends" << endl;
            }
            else if(header.compare("update") == 0){

                glm::mat4 matrix1, matrix2;

                vector <float> height_map;

                if (game_start == false && game_over == true) {
                    round_num+=2;
                    // TODO::Close game over BGM and play main game BGM
                    //cout << "44444" << endl;
                    score = 0;
                    oppo_score = 0;
                    audioManager->PlayBackgroundMusic();
                }

                if (game_start == false && game_over == false) {
                    // TODO::Close waiting room BGM and play main game BGM
                    //cout << "22222" << endl;
                    audioManager->PlayBackgroundMusic();
                }
                game_wait = false;
                game_start = true;
                game_over = false;
                restart_send = false;
                //cout << player_id << "start!" << endl;

                int id = 1;
                BOOST_FOREACH(const pt::ptree::value_type& child,
                              tar.get_child("Obj")) {

                    if (id == 1){
                        int i=0;
                        BOOST_FOREACH(const pt::ptree::value_type& m,
                                      child.second.get_child("transformation")) {

                            matrix1[i/4][i%4] = stof(m.second.data());
                            //cout << matrix1[i/4][i%4] << ",";
                            i++;
                        }

                        //Store the difference for camera
                        glm::vec3 newPos = glm::vec3(matrix1[3][0], matrix1[3][1], matrix1[3][2]);
                        glm::vec3 diffPos = newPos - sphere1_pos;

                        if(player_id == 1){
                            camera->eyePos += diffPos;
                        }
                        // Store the absolute position
                        sphere1_pos = glm::vec3(matrix1[3][0], matrix1[3][1], matrix1[3][2]);
                        sphere_player1->move(matrix1);
                        // sphere_player1->move(glm::vec3(matrix1[3][0], matrix1[3][1], matrix1[3][2]));
                        //cout << matrix1[3][0] << " " << matrix1[3][1] << " " << matrix1[3][2] << endl;
                    }
                    else if(id == 2){
                        int i=0;
                        BOOST_FOREACH(const pt::ptree::value_type& m,
                                      child.second.get_child("transformation")) {
                            matrix2[i/4][i%4] = stof(m.second.data());
                            i++;
                        }

                        //Store the difference for camera
                        glm::vec3 newPos = glm::vec3(matrix2[3][0], matrix2[3][1], matrix2[3][2]);
                        glm::vec3 diffPos = newPos - sphere2_pos;

                        if(player_id == 2){
                            camera->eyePos += diffPos;
                        }

                        // Store the absolute position
                        sphere2_pos = glm::vec3(matrix2[3][0], matrix2[3][1], matrix2[3][2]);
                        sphere_player2->move(matrix2);
                        // sphere_player2->move(glm::vec3(matrix2[3][0], matrix2[3][1], matrix2[3][2]));
    //                    float x2 = stof(child.second.get<std::string>("x"));
    //                    float y2 = stof(child.second.get<std::string>("y"));
    //                    glm::vec3 pos2 = glm::vec3(x2, y2, 0);
    //                    sphere_player2->move(pos2);

                    }
                    else{
                        // id 3, 4 for terrian
                    }
                    id++;
                }

                int indexForScore = 0;
                BOOST_FOREACH(const pt::ptree::value_type& v, tar.get_child("Score")){
                    // Team 1 get their score
                    if((player_id == 1 || player_id == 3) && indexForScore == 0){
                        int tempS = stoi(v.second.data());
                        if (score != tempS) {
                            audioManager->PlaySounds(0);
                        }
                        score = tempS;
                        //score = stoi(v.second.data());
                    }
                    if((player_id == 2 || player_id == 4) && indexForScore == 1){
                        int tempS = stoi(v.second.data());
                        if (score != tempS) {
                            audioManager->PlaySounds(0);
                        }
                        score = tempS;
                        //score = stoi(v.second.data());
                    }
                    if ((player_id == 1 || player_id == 3) && indexForScore == 1) {
                        /*int tempS = stoi(v.second.data());
                        if (score != tempS) {
                            audioManager->PlaySounds(0);
                        }
                        score = tempS;*/
                        oppo_score = stoi(v.second.data());
                    }
                    if ((player_id == 2 || player_id == 4) && indexForScore == 0) {
                       /* int tempS = stoi(v.second.data());
                        if (score != tempS) {
                            audioManager->PlaySounds(0);
                        }
                        score = tempS;*/
                        oppo_score = stoi(v.second.data());
                    }
                    indexForScore++;
                }
                //cout << "Score: " << score << endl;

                //int timeSignal = 0;

                BOOST_FOREACH(const pt::ptree::value_type& v, tar.get_child("Time")){
                    currTime = v.second.data();
                }
                
                //vector<float> scoreCoordinate;
                //int indexForScoreM = 0;
                //float scoreFlag = -100;
                vector<float>scoreCoordFloat;
                BOOST_FOREACH(const pt::ptree::value_type& v, tar.get_child("ScoreManager")){
                    
                    //if(indexForScoreM == 0){
                    //    scoreFlag = stof(v.second.data());
//                        if(scoreFlag == 1){
//                            std::cout << "The score flag is " << scoreFlag <<std::endl;
//                        }
                    //}
                    //else {
                    //else if(scoreFlag == -1 || scoreFlag == 1){
//                        if(scoreFlag == -1){
//                            std::cout << "Getting initial scoreCoord" << std::endl;
//                        } else if(scoreFlag == 1){
//                            std::cout << "Updating click scoreCoord" << std::endl;
//                        }
                        scoreCoordFloat.push_back(stof(v.second.data()));
                    //}

                    //indexForScoreM++;
                }
                //cout << "scoreSize " << scoreCoordFloat.size() << endl;

                
                //if(scoreFlag == -1 || scoreFlag == 1){
                    // Update the coordinate of scoremanager
                //if(scoreCoordFloat.size() > 0){
                // Might need optimize, now rerender every frame

                if (scoreManager->scoreStatus.size() == 0) {
                    for (int i = 0; i < scoreCoordFloat.size(); i += 3) {
                        glm::vec3 tempD = glm::vec3(scoreCoordFloat[i], scoreCoordFloat[i + 1], scoreCoordFloat[i + 2]);
                        scoreManager->scoreStatus.push_back(tempD);
                    }
                    scoreManager->UpdateScoreYCorrd(terrain);
                    for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
                        coins.push_back(Coin::generateCoin(scoreManager->scoreStatus[i]));
                        //coins.push_back(Coin::generateCoin(glm::vec3(0.0f)));
                    }
                    cout << "initializing: " << scoreManager->scoreStatus.size() << endl;
                }
                else{

                    //for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
                        //if (scoreManager->scoreStatus[i].x != scoreCoordFloat[i * 3] && scoreManager->scoreStatus[i].z != scoreCoordFloat[i * 3 + 2]) {
                        //    audioManager->PlaySounds(0);
                        //    scoreManager->scoreStatus[i].x = scoreCoordFloat[i * 3];
                        //    scoreManager->scoreStatus[i].z = scoreCoordFloat[i * 3 + 2];
                        //    //scoreManager->scoreStatus[i].y = terrain->getHeight(scoreManager->scoreStatus[i].x, scoreManager->scoreStatus[i].z);
                        //    //coins[i]->move(scoreManager);
                        //    //coins[i]->move(glm::vec3(0.0f));
                        //}
                        
                    //}
                    scoreManager->scoreStatus.clear();
                    for(int i=0; i<scoreCoordFloat.size(); i+=3){
                        glm::vec3 tempD = glm::vec3(scoreCoordFloat[i], scoreCoordFloat[i+1], scoreCoordFloat[i+2]);
                        scoreManager->scoreStatus.push_back(tempD);
                    }

                    scoreManager->UpdateScoreYCorrd(terrain);
                    for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
                        //if (coins[i]->center.x != scoreManager->scoreStatus[i].x && coins[i]->center.z != scoreManager->scoreStatus[i].z) {
                           // audioManager->PlaySounds(0);
                            coins[i]->move(glm::vec3(scoreManager->scoreStatus[i].x, scoreManager->scoreStatus[i].y, scoreManager->scoreStatus[i].z));
                        //}
                        //coins.push_back(Coin::generateCoin(scoreManager->scoreStatus[i]));
                    }


                //if(scoreCoordFloat.size()/3 != scoreManager->scoreStatus.size()){
                    //scoreManager->scoreStatus.clear();

                    // Hard code point count 10 points for sound effect
                    //if (scoreCoordFloat.size() != 30) {
                    //audioManager->PlaySounds(0);
                    //}
          
                    /*for(int i=0; i<scoreCoordFloat.size(); i+=3){
                        glm::vec3 tempD = glm::vec3(scoreCoordFloat[i], scoreCoordFloat[i+1], scoreCoordFloat[i+2]);
                        scoreManager->scoreStatus.push_back(tempD);
                    }

                    scoreManager->UpdateScoreYCorrd(terrain);


                    int differenceC = coins.size() - scoreManager->scoreStatus.size();

                    int indexForD = 0;
                    while(indexForD < scoreManager->scoreStatus.size()){
                        if (scoreManager->scoreStatus[indexForD].x != coins[indexForD]->center.x && scoreManager->scoreStatus[indexForD].z != coins[indexForD]->center.z) {
                            delete coins[indexForD];
                            coins.erase(coins.begin() + indexForD);
                            coins.shrink_to_fit();
                            differenceC -= 1;
                            if (differenceC == 0) {
                                break;
                            }
                        }
                        else {
                            indexForD++;
                        }
                    }

                    while (differenceC != 0) {
                        int temp = coins.size() - 1;
                        delete coins[temp];
                        coins.erase(coins.begin() + temp);
                        differenceC--;
                    }*/
                    


                    // May need to call rerender logic here
                    //for (int i = 0; i < 10; i++) {
                    //    coins[i]->move(scoreManager->scoreStatus[i]);
                    //}

                    //for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
                        //coins.push_back(Coin::generateCoin(scoreManager->scoreStatus[i]));
                        //coins.push_back(Coin::generateCoin(glm::vec3(0.0f)));
                    //}
                    
                    
                }
                //else{
//                    for(int i=0; i<scoreCoordFloat.size(); i+=3){
//                        glm::vec3 tempD = glm::vec3(scoreCoordFloat[i], scoreCoordFloat[i+1], scoreCoordFloat[i+2]);
//                        if(tempD != scoreManager->scoreStatus[i/3]){
//                            cout << "score " << i/3 << " has changed y to " << tempD.y << endl;
//                            cout << "current status size is " << scoreManager->scoreStatus.size() << endl;
//                            scoreManager->scoreStatus[i/3] = tempD;
//
//                            // May need to call rerender logic here
//
//                        }
//                    }


                int i=0;


                std::vector<glm::vec2> edited_points;
                float height = 0.0f;
                BOOST_FOREACH(const pt::ptree::value_type& v,
                tar.get_child("edited_points")) {
                    //cout << v.second.data() << endl;
                    stringstream ss(v.second.data());
                    string res;
                    std::vector<float> res_list;
                    while(getline(ss, res, ',')){
                        res_list.push_back(stof(res));
                    }
                    i++;
                    edited_points.push_back(glm::vec2(res_list[0], res_list[1]));
                    edited_points.push_back(glm::vec2(res_list[2], res_list[3]));
                    height = res_list[4];
                }

                if(!edited_points.empty()){
                    
                    if (abs(edited_points[0][0]-edited_points[1][0]) <=3 && abs(edited_points[0][1] - edited_points[1][1]) <= 3) {
                        //cout << "..." << endl;
                        terrain->editPoint(edited_points[0], height);
                    }
 
                    else {
                        terrain->edit(edited_points, height);
                    }

                    scoreManager->UpdateScoreYCorrd(terrain);
                    //coins.clear();
                    for (int i = 0; i < scoreManager->scoreStatus.size(); i++) {
                        coins[i]->move(glm::vec3(scoreManager->scoreStatus[i].x, scoreManager->scoreStatus[i].y, scoreManager->scoreStatus[i].z));
                        //coins.push_back(Coin::generateCoin(scoreManager->scoreStatus[i]));
                    }

                    // Re rendering process here

                    //cout << "Currently Terrain Contains: " << terrain->height.size() << "height" << endl;;
                    //cout << "Y value at hardcode point is: " << terrain->getHeight(114, 12) << endl;
                    //cout << "Y value at hardcode point 2 is " << terrain->getHeight(110, 20) << endl;
                    //terrain->edit(edited_points, 10);
                }
//                if(!height_map.empty()){
//                    //std::cout << msg << std::endl;
//                    std::cout << "building..." << std::endl;
//                    //build mesh based on height map from server
//                    terrain->terrainBuildMesh(height_map);
//                }

            }
        }
    } catch (...){

    }

}

void Client::checkCollisions(Sphere* sphere) {

    std::vector<unsigned int>* indices = terrain->getIndices();
    std::vector<glm::vec3>* vertices = terrain->getVertices();
    std::vector<TerrainBoundingBox>* boxes = terrain->getBoundingBoxes();

    float elapsedTime = 0.03f / 20;
    for (int k = 0; k < 20; k++) {
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
                int curInd = box.indices2triangles[i];
                int ai = (*indices)[curInd-2];
                int bi = (*indices)[curInd-1];
                int ci = (*indices)[curInd];
                glm::vec3& a = (*vertices)[ai];
                glm::vec3& b = (*vertices)[bi];
                glm::vec3& c = (*vertices)[ci];
                glm::vec3 n = glm::normalize(glm::cross(c-a, b-a));
                if (glm::dot(n, glm::vec3(0, 1, 0)) < 0) { // little hack to make sure normals are upwards
                    n = -n;
                }

                glm::vec3 offset = sphere->checkCollision(a, b, c, n);
                if (glm::length(offset) < 0.0001f) { // clamp to avoid bouncing too many times
                    offset = glm::vec3(0);
                    continue;
                }

                sphere->move(sphere->getCenter() + offset); // move to right position
            }
            sphere->momentum += sphere->force * elapsedTime;
            glm::vec3 dis = (sphere->momentum/sphere->mass) * elapsedTime;

            if (glm::length(sphere->moveMomentum) > 0) {
                glm::vec3 temp = 10.0f * glm::normalize(sphere->moveMomentum) * elapsedTime;
                if (glm::length(temp) >= glm::length(sphere->moveMomentum)) {
                    sphere->moveMomentum = glm::vec3(0);
                } else {
                    sphere->moveMomentum -= 10.0f * glm::normalize(sphere->moveMomentum) * elapsedTime;
                }
            }
            sphere->moveMomentum += sphere->moveForce * elapsedTime;
            if (glm::length(sphere->moveMomentum) > 20.0f) {
                sphere->moveMomentum = 20.0f * glm::normalize(sphere->moveMomentum);
            }
            dis += (sphere->moveMomentum/sphere->mass) * elapsedTime;
            //std::cout << glm::to_string(dis) << std::endl;

            sphere->move(sphere->getCenter() + dis);
        }
    }
    sphere->force = glm::vec3(0);
    sphere->moveForce = glm::vec3(0);

    // if sphere has fallen off, freaking lift it up
    float height = terrain->getHeightAt(sphere->getCenter().x, sphere->getCenter().z);
    if (height > sphere->getCenter().y + sphere->getRadius()) {
        glm::vec3 offset(0);
        offset.y = height - (sphere->getCenter().y - sphere->getRadius());
        sphere->move(sphere->getCenter() + offset);
    }
}

// Local Timer Logic, save for now.
void Client::updateTime(){
     string finishedTime = "";
     timeNow = time(NULL);
     //float duration = totalGameTime - (float)(endTime-startTime) / CLOCKS_PER_SEC;
     double duration = difftime(timeNow, timeStart);
     finishedTime = finishedTime + to_string((int)duration/60) + ":" + to_string((int)duration%60);
     if(duration <= 0){
         // Send a signal to announce game ends
         duration = 0;
         finishedTime = "0:00";
     }
    currTime = finishedTime;
}
