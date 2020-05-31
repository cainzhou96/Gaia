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
    bool getRestart();
  
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

    GLFWwindow* createWindow(int width, int height, std::string title);
    void setupGui();
    void cleanupGui();
};

#endif
