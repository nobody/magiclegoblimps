/*
 * RobotController.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "RobotController.h"
#include <iostream>

RobotController::RobotController(boost::asio::io_service& io_service) 
    : ioService(io_service)
{
}

RobotController::~RobotController() {
    // TODO Auto-generated destructor stub
}

//this method listens on a socket for connections
void RobotController::listen(){
    boost::asio::ip::tcp::socket socket_(ioService);
    boost::asio::ip::tcp::acceptor acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5060));
    acceptor_.async_accept(socket_, boost::bind(&RobotController::onConnect, this, boost::asio::placeholders::error));
}
//this method handles intial connections to the server
void RobotController::onConnect(const boost::system::error_code& error){
    std::cout<<"connection accepted";
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
