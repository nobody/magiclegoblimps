/*
 * AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "AdminHandler.h"

std::vector<TcpServer::TcpConnection::pointer> AdminHandler::connections;

AdminHandler::AdminHandler(){
}

AdminHandler::~AdminHandler(){
}

void AdminHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    // need to lock for multithreading so that .size() will always be the new item
    connections.push_back(tcp_connection);
    boost::thread connThread(&AdminHandler::threaded_on_connect, this, connections.size() - 1);

}

void AdminHandler::handle_write(const boost::system::error_code& error, size_t bytes) {
}


void AdminHandler::threaded_on_connect(int connIndex){
    boost::this_thread::sleep(boost::posix_time::seconds(2));

    TcpServer::TcpConnection::pointer tcp_connection(connections.at(connIndex));
    std::string msg = "You've reached the Admin Handler\n";
    msg += tcp_connection->socket().remote_endpoint().address().to_string();
    msg += ":";
    msg += boost::lexical_cast<std::string>(tcp_connection->socket().remote_endpoint().port());
    msg += "\n";

    boost::asio::write(tcp_connection->socket(), boost::asio::buffer(msg));

    tcp_connection->socket().close();
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
