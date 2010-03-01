/*
 * AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "AdminHandler.h"

AdminHandler::conn_map AdminHandler::connections;

AdminHandler::AdminHandler(){
}

AdminHandler::~AdminHandler(){
}

void AdminHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    // need to lock for multithreading so that .size() will always be the new item
    //connections.push_back(tcp_connection);
    boost::asio::ip::tcp::endpoint endpoint = tcp_connection->socket().remote_endpoint();
    connections[endpoint] = tcp_connection;
    boost::thread connThread(&AdminHandler::threaded_on_connect, this, endpoint);

}

void AdminHandler::threaded_on_connect(boost::asio::ip::tcp::endpoint conn){
    //boost::this_thread::sleep(boost::posix_time::seconds(2));

    TcpServer::TcpConnection::pointer tcp_connection(connections[conn]);
    std::string msg = "You've reached the Admin Handler\n";
    msg += tcp_connection->socket().remote_endpoint().address().to_string();
    msg += ":";
    msg += boost::lexical_cast<std::string>(tcp_connection->socket().remote_endpoint().port());
    msg += "\n\n";

    boost::asio::async_write(tcp_connection->socket(), boost::asio::buffer(msg), 
        boost::bind(&AdminHandler::write_handler, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    
    //tcp_connection->socket().close();
}

void AdminHandler::write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    std::cout << "AdminHandler wrote " << bytes_transferred << " bytes to a client\n";
    std::cout.flush();
    
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
