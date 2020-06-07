//
//  Client.h
//  Gaia
//
//  Created by Cain on 4/16/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Client_h
#define Client_h

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "engine.h"
#include "Window.h"
#include "shader.h"
#include "IO_handler.hpp"
#include "Camera.hpp"
#include "AudioManager.hpp"
#include "ScoreManager.hpp"
#include <ctime>
#include "constant.h"

class Client {
public:
    Client(int width, int height);
    ~Client();
    bool initialize();

    // Draw and update
    void idleCallback();
    void displayCallback();

    void run();

private:
    Window* window;
    int width;
    int height;
    // Camera Matrices
    glm::mat4 projection;

    // Shader Program ID
    GLuint shaderProgram;
    GLuint skyboxProgram;
    GLuint terrainProgram;
    GLuint toonProgram;
    GLuint modelProgram;
    GLuint multiTextureProgram;
    
    // Constructors and Destructors
    bool initializeProgram();
    bool initializeObjects();
    void setupOpenglSettings();
    void printVersions();
    void setupCallbacks();
    static void checkCollisions(Sphere* sphere);

    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void setMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // flag
    static int isMouseButtonDown; // 0 for release, 1 for left, 2 for right (temperary unused)
    static glm::vec2 clickPos;
    static glm::vec2 releasePos;
    
    
    // objects
    static Sphere* sphere_player1;
    static Sphere* sphere_player2;
    static Arrow* arrow_player1;
    static Arrow* arrow_player2;

    static Sphere* sphere_mouse; // for testing purpose
    static Terrain* terrain;
    static Camera* camera;
    static Skybox* skybox;
    static std::vector<Coin*> coins;
  
    // Store sphere absolute position for camera, since the position stored is always 0
    static glm::vec3 sphere1_pos;
    static glm::vec3 sphere2_pos;
    
    static glm::vec2 mousePos;
    static bool mouseControl;

    // IO Handler
    static IO_handler* io_handler;

    // Decode message from server and update client side graphic
    static void updateFromServer(std::string msg);
    
    // Timer
    static void updateTime();
    static time_t timeStart;
    static time_t timeNow;
    static int totalTime;
    static bool inGame;
    
    // Transform screen coordinate to world coordinate
    static glm::vec2 screenPointToWorld(glm::vec2 mousePos);
    
    
    // Audio
    static AudioManager* audioManager;
    
    // Score
    static ScoreManager* scoreManager;
    
    // GameManager
    static bool game_wait;
    static bool game_start;
    static bool game_over;
    static bool game_restart;
    static bool restart_send;
    static int player_num;
    static int round_num;
    static int player_id;
    static string currTime;
    static int score;
    static int oppo_score;

    // movement
    static bool forward;
    static bool backward;
    static bool left;
    static bool right;

    // Network
    static boost::asio::io_service io_service;
    static tcp::endpoint endpoint;
    static chat_client c;
    static boost::thread t;
    static std::string msg;
    
};

#endif /* Client_h */
