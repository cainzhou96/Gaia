#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "core.h"

class Window {
public:
    Window(int width, int height, std::string title);
    ~Window();
    GLFWwindow* getWindow();
    void displayCallback();
    std::pair<int, int> getFrameBufferSize();
    void setGameStart(bool start);
    void setGameOver(bool over);
    void setId(int player_id);
    void setPlayerNum(int num);
    void setTime(std::string t);
    void setScore(int s);
    void setOppoScore(int os);
    bool getRestart();
    void updateWindow();
  
private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;
    bool closed;
    
    bool game_start;
    bool game_over;
    bool game_restart;
    int user_id;
    int player_num;
    std::string time;
    int score;
    int oppo_score;
    GLuint my_image_texture;
    GLuint my_image_logo;

    GLFWwindow* createWindow(int width, int height, std::string title);
    bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
    void setupGui();
    void cleanupGui();
};

#endif
