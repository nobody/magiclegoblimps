/*
 * Robot.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

class Robot {
public:
    Robot();
    Robot(boost::asio::ip::tcp::endpoint EP, int RID_);
    virtual ~Robot();

    void lock();
    void unlock();

    int getRID();
    boost::asio::ip::tcp::endpoint getEndpoint();


private:
    //mutex for thread safety
    boost::mutex robotMutex;

    //robot id that is unique to each robot controller client
    int RID;
    
    //endpoint of the controller this robot is on
    boost::asio::ip::tcp::endpoint robotEP;
    
};

#endif /* ROBOT_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
