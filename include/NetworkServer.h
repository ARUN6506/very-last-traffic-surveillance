#pragma once
#include <boost/asio.hpp>
#include <string>
#include <memory> // For std::shared_ptr

class NetworkServer {
public:
    NetworkServer(boost::asio::io_context& io_context, int port);
    void sendMessage(const std::string& msg);

private:
    void doAccept();
    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_; // Use shared_ptr for socket management
};