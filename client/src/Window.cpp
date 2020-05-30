#include "Window.h"

Window::Window(int width, int height, std::string title) {
  this->width = width;
  this->height = height;
  this->title = title;
    this->game_start = false;
    this->game_over = false;
  
  window = createWindow(width, height, title);
  if (!window) {
    throw "Unable to create a window. ";
  }
  setupGui(); 
}

Window::~Window() {
  // cleanup Gui
  cleanupGui();

  // Destroy the window.
  glfwDestroyWindow(window);
  
  // Terminate GLFW.
  glfwTerminate();
}

GLFWwindow* Window::createWindow(int width, int height, std::string title)
{
  // Initialize GLFW.
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return NULL;
  }
  
  // 4x antialiasing.
  glfwWindowHint(GLFW_SAMPLES, 4);
  
#ifdef __APPLE__
  // Apple implements its own version of OpenGL and requires special treatments
  // to make it uses modern OpenGL.
  
  // Ensure that minimum OpenGL version is 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Enable forward compatibility and allow a modern OpenGL context
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  
  // Create the GLFW window.
  GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  
  // unresizable window
  glfwSetWindowSizeLimits(window, this->width, this->height, this->width, this->height);
  
  // Check if the window could not be created.
  if (!window)
  {
    std::cerr << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    return NULL;
  }
  
  // Make the context of the window.
  glfwMakeContextCurrent(window);
  
#ifndef __APPLE__
  // On Windows and Linux, we need GLEW to provide modern OpenGL functionality.
  
  // Initialize GLEW.
  if (glewInit())
  {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return NULL;
  }
#endif
  
  // Set swap interval to 1.
  glfwSwapInterval(0);
  
  return window;
}

GLFWwindow* Window::getWindow() {
  return this->window;
}

void Window::setGameStart(bool start){
    game_start = start;
}
void Window::setGameOver(bool over){
    game_over = over;
}

void Window::setId(int player_id){
    user_id = player_id;
}

void Window::setPlayerNum(int num){
    player_num = num;
}

void Window::setTime(std::string t){
    time = t;
}
void Window::setScore(int s){
    score = s;
}

void Window::displayCallback()
{
  // feed inputs to dear imgui, start new frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  
    if(game_start && !game_over){
        std::string player_type = "uninitialize";
        std::string player_team = "uninitialize";
        ImGui::Begin("Player Info");
        if(user_id == 1 || user_id == 2){
            player_type = "Ball Player";
        }
        else if(user_id == 3 || user_id == 4){
            player_type = "Terrian Player";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        if(user_id == 1 || user_id == 3){
            player_team = "Team 1";
        }
        else if(user_id == 2 || user_id == 4){
            player_team = "Team 2";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        ImGui::Text("Player Type: %s", player_type.c_str());
        ImGui::Text("Player Team: %s", player_team.c_str());
        ImGui::End();
        
      ImGui::Begin("Time");
      ImGui::Text("Remaining time: %s", time.c_str());
      ImGui::End();
        
      ImGui::Begin("Score");
      ImGui::Text("Current score: %d", score);
      ImGui::End();
    }
    else if(game_over){
        ImGui::Begin("Game Over!");
        ImGui::Text("Thanks for playing!");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.5);
        ImGui::Text("Your final score is: %d", score);
        ImGui::NewLine();
        if(ImGui::Button("Restart")){
            std::cout << "Restart" << std::endl;
        }
        if(ImGui::Button("Quit")){
            std::cout << "Quit" << std::endl;
        }
        ImGui::End();
    }
    else if(!game_start){
        ImGui::Begin("Welcome to Gaia");
        ImGui::SetWindowFontScale(1.5);
        ImGui::Text("Waiting for players to join...");
        std::string player_type = "uninitialize";
        std::string player_team = "uninitialize";
        if(user_id == 1 || user_id == 2){
            player_type = "Ball Player";
        }
        else if(user_id == 3 || user_id == 4){
            player_type = "Terrian Player";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        if(user_id == 1 || user_id == 3){
            player_team = "Team 1";
        }
        else if(user_id == 2 || user_id == 4){
            player_team = "Team 2";
        }
        else{
                //std::cout << "unrecognized id" << std::endl;
        }
        if(player_num == 0){
            player_num = user_id;
        }
        ImGui::Text("You are: %s", player_type.c_str());
        ImGui::Text("Your team: %s", player_team.c_str());
        ImGui::NewLine();
        ImGui::Text("Current players joined: %d", player_num);
        ImGui::End();
    }
  
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  
  // Gets events, including input such as keyboard and mouse or window resizing
  glfwPollEvents();
  
  // Swap buffers.
  glfwSwapBuffers(window);
}

std::pair<int, int> Window::getFrameBufferSize() {
  int width = this->width;
  int height = this->height;
#ifdef __APPLE__
  // In case your Mac has a retina display.
  glfwGetFramebufferSize(window, &width, &height);
#endif
  return std::pair<int, int>(width, height); 
}

void Window::setupGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
}

void Window::cleanupGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
