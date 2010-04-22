/*
 * Robot.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Robot.h"

int Robot::counter = 1;
const int Robot::camdata[][3] = { {160, 120, 15}, {160 ,120, 15}, {0, 0, 0} };

Robot::Robot() : xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub
    pos = *(new Point(xCord,yCord));
    list = new std::map<int, float>;
    globalID = counter++;
}


Robot::Robot(boost::asio::ip::tcp::endpoint EP, int RID) : RID(RID), xCord(0), yCord(0) {
    // TODO Auto-generated constructor stub
    robotEP = EP;
    pos = *(new Point(xCord,yCord));
    list = new std::map<int, float>;
    globalID = counter++;
}

Robot::~Robot() {
    robotMutex.lock();
    robotMutex.unlock();
    delete list;
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

// returns the GLOBAL id for this robot
int Robot::getGlobalID() {
    return globalID;
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

void Robot::setDir(int d){
    dir = d;
}
int Robot::getDir(){
    return dir;
}


std::string Robot::getVideoURL() {
    return videoURL;
}
void Robot::setVideoURL(std::string url) {
    videoURL = url;
}

void Robot::setList(int* objects, float* qualities, int size){
    list->clear();

    for(int i = 0; i < size; ++i){
       (*list)[objects[i]] = qualities[i];
    }
}
void Robot::setCamera(int type){
    camera = type;
}
int Robot::getCamera(){
    return camera;
}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
