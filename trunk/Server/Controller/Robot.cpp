/*
 * Robot.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Robot.h"
//#include <boost/asio.hpp>

Robot::Robot() {
    // TODO Auto-generated constructor stub

}

Robot::~Robot() {
    // TODO Auto-generated destructor stub
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


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
