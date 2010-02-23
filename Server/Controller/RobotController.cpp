/*
 * RobotController.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "RobotController.h"

RobotController::RobotController(boost::asio::io_service& io_service) {
    ioService = io_service;

}

RobotController::~RobotController() {
    // TODO Auto-generated destructor stub
}

//this method listens on a socket for connections
RobotController::void listen(){

}
//this method handles intial connections to the server
RobotController::void onconnect(){

}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
