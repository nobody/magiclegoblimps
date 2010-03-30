/*
 * Robot.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Robot.h"


Robot::Robot() : xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub
    pos = *(new Point(xCord,yCord));
    list = new std::vector<int>;
}


Robot::Robot(boost::asio::ip::tcp::endpoint EP, int RID) : RID(RID), xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub
    pos = *(new Point(xCord,yCord));
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
    return yCord;
}


std::string Robot::getVideoURL() {
    return videoURL;
}
void Robot::setVideoURL(std::string url) {
    videoURL = url;
}

void Robot::setList(int* array, int size){
    delete list;
    list = new std::vector<int>;

    for(int i = 0; i < size; ++i){
        list->push_back(array[i]);
    }
}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
