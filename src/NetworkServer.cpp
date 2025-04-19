#include "NetworkServer.h"
#include <iostream>

NetworkServer::NetworkServer(boost::asio::io_context& io_context, int port)
    : acceptor_(io_context, { boost::asio::ip::tcp::v4(), static_cast<unsigned short>(port) }) {
    doAccept();
}

void NetworkServer::doAccept() {
    socket_ = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor()); // Create a new socket
    acceptor_.async_accept(*socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "Client connected.\n";
            // Handle client communication here (e.g., read data, send responses)
        }
        else {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
        doAccept(); // Re-initiate accept to listen for new connections
        });
}

void NetworkServer::sendMessage(const std::string& msg) {
    if (socket_ && socket_->is_open()) {
        auto data = msg + "\n";
        boost::asio::async_write(*socket_, boost::asio::buffer(data),
            [this](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "Send error: " << ec.message() << std::endl;
                    // Handle send error (e.g., close socket, notify client)
                }
            });
    }
    else {
        std::cerr << "Socket is not open. Message not sent.\n";
    }
}