/*
 * RobotController.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ROBOTCONTROLLER_H_
#define ROBOTCONTROLLER_H_

#include<boost/asio.hpp>


class RobotController {
public:
    RobotController(boost::asio::io_service& io_service);
    virtual ~RobotController();

    //connection methods
    void listen();
    //msg methods
private:

    boost::asio::io_service *ioService;
    //connection methods
    void onConnect();
    //msg methods
};

#endif /* ROBOTCONTROLLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
