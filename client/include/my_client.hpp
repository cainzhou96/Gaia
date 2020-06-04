#include <cstdlib>
#include <deque>
#include <iostream>
#include <queue>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::asio;
using namespace std;

using ip::tcp;

typedef std::deque<std::string> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_service& io_service,
      tcp::endpoint endpoint)
    : io_service_(io_service),
      socket_(io_service)
  {
    socket_.async_connect(endpoint,
        boost::bind(&chat_client::handle_connect, this,
          boost::asio::placeholders::error, endpoint));
  }

    string getMsg()
    {
        // For testing only
        //        istream buffer(&read_msg_);
        //        string msg;
        //        buffer >> msg;
        string update = "";
        if(!msg.empty()){
            update = msg.front();
            msg.pop();
        }
        return update;
    }

    void write(const std::string msg)
    {
        io_service_.post(boost::bind(&chat_client::do_write, this, msg));
    }

    void close()
    {
        io_service_.post(boost::bind(&chat_client::do_close, this));
    }

    int get_id() {
        return id;
    }


private:

    void handle_connect(const boost::system::error_code& error,
                                            tcp::endpoint endpoint)
    {
        if (!error)
        {
            boost::asio::async_read_until(socket_, read_msg_, '\n',
                        boost::bind(&chat_client::handle_id, this,
                        boost::asio::placeholders::error));
        }else{
            std::cout << "Connection refused!" << std::endl;
        }
    }

    void handle_id(const boost::system::error_code& error)
    {
        if (!error)
        {
            std::string id_str{buffers_begin(read_msg_.data()),         buffers_end(read_msg_.data())};
            read_msg_.consume(read_msg_.size());
            id = std::stoi(id_str);
            std::cout << "id : " << id << std::endl;
            boost::asio::async_read_until(socket_, read_msg_, '\n',
                boost::bind(&::chat_client::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout << "ID not received!" << std::endl;
        }
    }
  void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
  {
      if (!error)
      {
          int n = read_msg_.size();
          // std::cout << n << std::endl;
          std::string target{buffers_begin(read_msg_.data()), buffers_end(read_msg_.data())};
          // std::cout << target << std::endl;
          // Add debug msgs

          msg.push(target);
          read_msg_.consume(bytes_transferred + 1);

          boost::asio::async_read_until(socket_,
          read_msg_, '\n',
          boost::bind(&::chat_client::handle_read, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
      }
      else
      {
          do_close();
      }
  }

    void do_write(std::string msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg+'\n');
        if (!write_in_progress)
        {
            boost::asio::async_write(socket_,
                boost::asio::buffer(write_msgs_.front().data(),
                write_msgs_.front().length()),
                boost::bind(&chat_client::handle_write, this,
                boost::asio::placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error) {
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
                boost::asio::async_write(socket_,
                    boost::asio::buffer(write_msgs_.front().data(),
                    write_msgs_.front().length()),
                    boost::bind(&chat_client::handle_write, this,
                        boost::asio::placeholders::error));
            }
        }
        else
        {
          do_close();
        }
    }

    void do_close()
    {
        socket_.close();
    }




private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    boost::asio::streambuf read_msg_;
    chat_message_queue write_msgs_;
    int id;
    std::queue<string> msg;
};
