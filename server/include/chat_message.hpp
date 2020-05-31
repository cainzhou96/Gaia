//
//  chat_message.hpp
//  game-server
//
//  Created by peizhenwu on 2020/4/18.
//  Copyright © 2020 peizhenwu. All rights reserved.
//

#ifndef chat_message_hpp
#define chat_message_hpp

#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <thread>
#include <chrono>
#include <ctime>
#include <deque>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
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


using namespace std;
namespace pt = boost::property_tree;


class chat_message
{
public:
    int x1, y1, x2, y2;

    string time = "";
    int score = -1;

    glm::mat4 transM1, transM2;

    vector <float> height_map;
    vector <string> edited_points;

    chat_message() : x1(5), x2(5), y1(5), y2(5)
    {
        transM1 = glm::mat4(1.0f);
        transM1[3] = glm::vec4(64,-10,-65,1);
        transM2 = glm::mat4(1.0f);
        transM2[3] = glm::vec4(58,0,-54,1);

        for (int i = 0; i < 16000; i++){
            height_map.push_back(1);
        }

    }
  
    void update1(glm::mat4 trans){
        transM1 = trans;
    }
    
    void update2(glm::mat4 trans){
        transM2 = trans;
    }

    std::string encode()
    {
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

        // build and add current height map node to root
        // for(int i = 0; i < height_map.size(); i++){
        //     pt::ptree node;
        //     node.put("", height_map[i]);
        //     height_root.push_back(std::make_pair("", node));
        //}

        scoreNode.put("", score);
        timeNode.put("", time);

        root.add_child("Obj", obj);

        root.add_child("height_map" ,height_root);

        root.add_child("Score", scoreNode);

        root.add_child("Time", timeNode);

        stringstream ss;
        write_json(ss, root, false);
        return ss.str();
    }

    void decode(string data, string & key_op, string & mouse_op, vector<glm::vec2> & editPoints) {
        float temp[4];
        //std::cout << data << std::endl;
        // Read JSON from client
        try{
            if(data != ""){
                stringstream ss;
                ss << data;

                pt::ptree tar;
                pt::read_json(ss, tar);

                int i = 0;
                BOOST_FOREACH(const pt::ptree::value_type& child, tar.get_child("cmd")) {
                    if(i == 0){
                        key_op = child.second.get<std::string>("key");
                    }
                    else{
                        // Mouse
                        mouse_op = child.second.get<std::string>("mouse");

                        if(mouse_op.compare("l") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_l")){
                                temp[index] = stof(t.second.data());
                                index++;
    //                                    if(temp[index]!= 0){
    //                                        cout << temp[index] << " ";
    //                                    }
                            }
                        } else if(mouse_op.compare("r") == 0){
                            int index = 0;
                            BOOST_FOREACH(const pt::ptree::value_type& t, child.second.get_child("mouse_r")){
                                temp[index] = stof(t.second.data());
                                index++;
                            }
                        }
                    }
                    i++;
                }
            }
        }catch(...){
            // Might Mysterically Enter here
            std::cout << "decode exception" << std::endl;
        }
        if(mouse_op != ""){
            editPoints.push_back(glm::vec2(temp[0],temp[1]));
            editPoints.push_back(glm::vec2(temp[2],temp[3]));
        }
    }
};

#endif /* chat_message_hpp */
