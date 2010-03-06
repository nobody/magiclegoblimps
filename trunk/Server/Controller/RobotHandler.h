 /* AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */ 
#ifndef ROBOTHANDLER_H_
#define ROBOTHANDLER_H_

#include "TcpServer.h"
#include <boost/thread.hpp>
#include <map>
#include "Vector_ts.h"
#include "Robot.h"

typedef std::map<boost::asio::ip::tcp::endpoint,TcpServer::TcpConnection::pointer> conn_map;

class RobotHandler :public TcpServer::ConnHandler
{   
    public:
    RobotHandler();
    RobotHandler(Vector_ts<Robot>* robots_);
    ~RobotHandler();
    virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);
    void setRobots(Vector_ts<Robot>* robots_);
    
    private:
    void threaded_listen(boost::asio::ip::tcp::endpoint connEP);
    //void parseString(std::string msg, boost::asio::ip::tcp::endpoint connEP);
    const static int fields = 1;


    static conn_map connections;
    Vector_ts<Robot>* robots;

};

#endif

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
