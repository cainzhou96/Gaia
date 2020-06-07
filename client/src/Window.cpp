#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Window::Window(int width, int height, std::string title) {
  this->width = width;
  this->height = height;
  this->title = title;
    this->game_start = false;
    this->game_over = false;
    this->game_restart = false;
  
  window = createWindow(width, height, title);
  if (!window) {
    throw "Unable to create a window. ";
  }
  setupGui(); 

  int my_image_width = 0;
  int my_image_height = 0;
  this->my_image_texture = 0;
  bool ret = LoadTextureFromFile("textures/mainpage.jpg", &my_image_texture, &my_image_width, &my_image_height);
  IM_ASSERT(ret);

  this->my_image_logo = 0;
  ret = LoadTextureFromFile("textures/logo.png", &my_image_logo, &my_image_width, &my_image_height);
  IM_ASSERT(ret);
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

void Window::setOppoScore(int os) {
    oppo_score = os;
}

bool Window::getRestart(){
    return game_restart;
}

void Window::updateWindow() {
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();

    // Swap buffers.
    glfwSwapBuffers(window);
}

void Window::displayCallback()
{
    // feed inputs to dear imgui, start new frame
  
    if(game_start && !game_over){
        game_restart = false;
        std::string player_type = "uninitialize";
        std::string player_team = "uninitialize";
        std::string opponent_team = "uninitialize";
        ImGui::Begin("Player Info");
        if(user_id == 1 || user_id == 2){
            player_type = "Sphere Player";
        }
        else if(user_id == 3 || user_id == 4){
            player_type = "Terrain Player";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        if(user_id == 1 || user_id == 3){
            player_team = "Team Yellow";
            opponent_team = "Team Blue";
        }
        else if(user_id == 2 || user_id == 4){
            player_team = "Team Blue";
            opponent_team = "Team Yellow";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        ImGui::SetWindowFontScale(1.8);
        ImGui::Text("Player Type: %s", player_type.c_str());
        ImGui::Text("Player Team: %s", player_team.c_str());
        ImGui::End();
        
      ImGui::Begin("Time");
      ImGui::SetWindowFontScale(1.8);
      ImGui::Text("Remaining time: %s", time.c_str());
      ImGui::End();
        
      ImGui::Begin("Score Board");
      ImGui::SetWindowFontScale(1.8);
      if (player_team.compare("Team Yellow") == 0) {
          ImGui::Text("->%s score<-: %d", player_team.c_str(), score);
          ImGui::Text("   %s score   : %d", opponent_team.c_str(), oppo_score);
      }
      else {
          ImGui::Text("->%s score<-: %d", player_team.c_str(), score);
          ImGui::Text(" %s score : %d", opponent_team.c_str(), oppo_score);
      }
      ImGui::End();

      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
      ImGui::Begin("Gaia");
      ImGui::Image((void*)(intptr_t)my_image_logo, ImVec2(200, 100));
      ImGui::PopStyleColor();
      ImGui::End();

    }
    else if(game_over){
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 1.0f));
        ImGui::Begin("Times out");
        ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(1800, 1000));
        ImGui::PopStyleColor();
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::Begin("Game Over!");
        ImGui::SetWindowFontScale(1.8);
        ImGui::NewLine();
        
        if (score > oppo_score) {
            ImGui::Text("You Win!!!");
            ImGui::Text("Your final score is: %d", score);
        }
        else if (score < oppo_score) {
            ImGui::Text("You Lose!!!");
            ImGui::Text("Your final score is: %d", score);
        }
        else{
            ImGui::Text("It's a Tie!!!");
            ImGui::Text("Your final score is: %d", score);
        }
        ImGui::NewLine();
        ImGui::Text("Thanks for playing!");
        ImGui::NewLine();
        if(ImGui::Button("Restart")){
            game_restart = true;
        }
        if(ImGui::Button("Quit")){
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if(game_restart){
            ImGui::NewLine();
            ImGui::Text("Waiting for other players...");
        }
        ImGui::PopStyleColor();
        ImGui::End();
    }
    else if(!game_start){
        game_restart = false;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 1.0f));
        ImGui::Begin("Welcome");
        std::string player_type = "uninitialize";
        std::string player_team = "uninitialize";
        if(user_id == 1 || user_id == 2){
            player_type = "Sphere Player";
        }
        else if(user_id == 3 || user_id == 4){
            player_type = "Terrain Player";
        }
        else{
            //std::cout << "unrecognized id" << std::endl;
        }
        if(user_id == 1 || user_id == 3){
            player_team = "Team Yellow";
        }
        else if(user_id == 2 || user_id == 4){
            player_team = "Team Blue";
        }
        else{
                //std::cout << "unrecognized id" << std::endl;
        }
        if(player_num == 0){
            player_num = user_id;
        }

        ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(1800, 1000));
        ImGui::PopStyleColor();
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::Begin("Welcome to Gaia");
        ImGui::SetWindowFontScale(1.8);
        ImGui::NewLine();
        ImGui::Text("Waiting for players to join...");
        ImGui::NewLine();
        ImGui::Text("You are: %s", player_type.c_str());
        ImGui::Text("Your team: %s", player_team.c_str());
        ImGui::NewLine();
        ImGui::Text("Current players joined: %d", player_num);
        ImGui::PopStyleColor();
        ImGui::End();
    }
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

bool Window::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

void Window::cleanupGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
