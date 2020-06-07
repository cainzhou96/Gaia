#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
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
#include <queue>

#include "Terrain.hpp"
#include "GameManager.hpp"
#include "PhysicsConstant.h"

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

namespace pt = boost::property_tree;

class Server : public boost::enable_shared_from_this<Server>
{
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    vector <std::shared_ptr<tcp::socket>> sockets;
    int i = 0;
    std::queue<string> messages;
    GameManager gm;

    void send_info(int id, std::shared_ptr<tcp::socket> socket){
        std::chrono::high_resolution_clock::time_point prevTime = std::chrono::high_resolution_clock::now(); 
        while(1){
            if(sockets[id-1] == nullptr){
                return;
            }
            if(gm.UpdateTime()){
                std::string msg = gm.encode(id);
                boost::asio::write( *socket, boost::asio::buffer(msg) );
            }
            else{
                pt::ptree root;
                root.put("Header", "end");
                stringstream ss;
                write_json(ss, root, false);
                boost::asio::write( *socket, boost::asio::buffer(ss.str() + '\n') );
                gm.game_start = false;
            }
            std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - prevTime; 
            float elapsedTime = diff.count(); 
            //std::cout << elapsedTime << std::endl; 
            if (elapsedTime < ELAPSED_TIME) {
                std::this_thread::sleep_for(std::chrono::milliseconds((int)((ELAPSED_TIME - elapsedTime) * 1000)));
            }
            prevTime = std::chrono::high_resolution_clock::now(); 
        }
    }

    void read_info(int id, std::shared_ptr<tcp::socket> socket)
    {
        while(1){
            boost::asio::streambuf buf;
            boost::system::error_code ec;
            boost::asio::read_until( *socket, buf, "\n" , ec);
            std::string data = boost::asio::buffer_cast<const char*>(buf.data());
            messages.push(to_string(id).append(data));
        }
    }

    void start_accept()
    {
        // socket
        cout << "begin accepting" << endl;
        boost::asio::io_service io_service1;
        boost::asio::io_service io_service2;
        boost::asio::io_service io_service3;
        boost::asio::io_service io_service4;
        std::shared_ptr<tcp::socket> socket_1(new tcp::socket(io_service1));
        std::shared_ptr<tcp::socket> socket_2(new tcp::socket(io_service2));
        std::shared_ptr<tcp::socket> socket_3(new tcp::socket(io_service3));
        std::shared_ptr<tcp::socket> socket_4(new tcp::socket(io_service4));
        sockets.push_back(socket_1);
        sockets.push_back(socket_2);
        sockets.push_back(socket_3);
        sockets.push_back(socket_4);
        while (i < 4){
            acceptor_.accept(*sockets[i]);
            
            i++;
            cout << "accepted: " << i << endl;
            boost::asio::write( *sockets[i - 1], boost::asio::buffer(std::to_string(i)+'\n') );
            cout << "sent to: " << i << endl;
            notifyPlayers(i );
           
        }
        cout << "4 players ready" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        gm.setStartTime();
         for(int j=0;j<4;j++){
             boost::thread send_thread(&Server::send_info, this, j+1, sockets[j]);
             boost::thread read_thread(&Server::read_info, this, j+1, sockets[j]);
         }
        std::chrono::high_resolution_clock::time_point prevTime = std::chrono::high_resolution_clock::now();
        while(1){
            while (!messages.empty()) {
                processUpdates();
            }
            gm.updatePhysics();
        }

    }
    void processUpdates() {
        try {
            std::string update = messages.front();
            messages.pop();
            int id = update[0] - '0';
            std:string data = update.substr(1);
            gm.handle_input(data, id);
        }catch(...){}
    }
    void notifyPlayers(int i){
        int player = sockets.size();
        for(int j = 0; j < i; j++){
            pt::ptree root;
            root.put("players", i);
            root.put("Header", "wait");
            stringstream ss;
            write_json(ss, root, false);
            boost::asio::write( *sockets[j], boost::asio::buffer(ss.str() + '\n') );
        }
    }

public:
    //constructor for accepting connection from client
    Server(boost::asio::io_service& io_service) : io_service_(io_service),
            acceptor_(io_service_, tcp::endpoint(tcp::v4(), 8888))
    {
        start_accept();
    }
};

int main(int argc, char *argv[])
{

    try
    {
        boost::asio::io_service io_service;
        Server server(io_service);
        io_service.run();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << endl;
    }
    while(1){
    }
    return 0;

}
