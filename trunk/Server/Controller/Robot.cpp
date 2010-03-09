/*
 * Robot.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Robot.h"
//#include <boost/asio.hpp>

Robot::Robot() : xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub

}


Robot::Robot(boost::asio::ip::tcp::endpoint EP, int RID) : xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub

}

Robot::~Robot() {
    robotMutex.lock();
    robotMutex.unlock();
}

//locks and unlocks the robot's private mutex
void Robot::lock(){
    robotMutex.lock();
}

void Robot::unlock(){
    robotMutex.unlock();
}

//returns the controller specific
//identifer for this robot
int Robot::getRID(){
    return RID;
}

//returns the boost library tcp endpoint for the controller
//that this robot is on
boost::asio::ip::tcp::endpoint Robot::getEndpoint(){
    return robotEP;
}

void Robot::setXCord(int x){
    xCord = x;
}
void Robot::setYCord(int y){
    yCord = y;
}
int Robot::getXCord(){
    return xCord;
}
int Robot::getYCord(){
    return xCord;
}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
