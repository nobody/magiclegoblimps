/*
 * AdminHandler.cpp
 *
 * Modified on:    $Date: 2010-02-23 13:14:01 -0600 (Tue, 23 Feb 2010) $
 * Last Edited By: $Author: mealey.patrick $
 * Revision:       $Revision: 58 $
 */


#include "AdminHandler.h"

AdminHandler::AdminHandler(){
}

AdminHandler::~AdminHandler(){
}

void AdminHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    std::string msg = "You've reached the Admin Handler\n";
    msg += tcp_connection->socket().remote_endpoint().address().to_string();
    msg += ":";
    msg += boost::lexical_cast<std::string>(tcp_connection->socket().remote_endpoint().port());
    msg += "\n";

    boost::asio::async_write(tcp_connection->socket(), boost::asio::buffer(msg), 
        boost::bind(&AdminHandler::handle_write, /*shared_from_this()*/ this, 
        boost::asio::placeholders::error, 
        boost::asio::placeholders::bytes_transferred));
}

void AdminHandler::handle_write(const boost::system::error_code& error, size_t bytes) {
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
