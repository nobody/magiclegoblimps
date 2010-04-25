/*
 * Robot.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ROBOT_H_
#define ROBOT_H_

//camera types
#define R_DLINK 1
#define R_CISCO 2
#define R_BROKEM 3


#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include "Point.h"

class Robot {
public:
    static const int camdata[][3];
    Robot();
    Robot(boost::asio::ip::tcp::endpoint EP, int RID_);
    virtual ~Robot();

    void lock();
    void unlock();

    int getRID();
    int getGlobalID();
    boost::asio::ip::tcp::endpoint getEndpoint();

    Point pos;
    //int camType __attribute__ ((deprecated)); //0 = Cisco, 1 = d-link
    int getXCord();
    int getYCord();
    int getCamera();
    int getDir();

    std::string getVideoURL();

    std::map<int, float>* getViewable_objs();

    void setCamera(int type);
    void setXCord(int x);
    void setYCord(int y);
    void setDir(int d);
    void setVideoURL(std::string);
    void setViewable_objs(int* objects, float* qualities, int size);

    //some useful operators
    inline bool operator<(Robot& aRobot) const{
        if(robotEP < aRobot.getEndpoint())
            return true;
        else if(robotEP == aRobot.getEndpoint()){
            if(RID < aRobot.getRID())
                return true;
            else
                return false;
        }
        else
            return false;
    }
    inline bool operator ==(Robot& aRobot) const{
        if(robotEP ==  aRobot.getEndpoint()  && RID == aRobot.getRID())
            return true;
        else
            return false;
            
    }
    

private:
    //mutex for thread safety
    boost::mutex robotMutex;

    //robot id that is unique to each robot controller client
    int RID;

    // global ident
    int globalID;
    static int counter;
    
    //endpoint of the controller this robot is on
    boost::asio::ip::tcp::endpoint robotEP;

    //coordinate position of the robot
    int xCord;
    int yCord;
    int camera;
    int dir;
    std::string videoURL;
    //the map that holds the object ids of objects in view and their qualities
    std::map<int, float>* viewable_objs;

    
};

#endif /* ROBOT_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
