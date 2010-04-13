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
#include <cstdlib>
#include "VideoHandler.h"
#include "Vector_ts.h"
#include "Robot.h"
#include "Object.h"
#include "DbManager.h"
#include "protocol.h"

typedef std::map<boost::asio::ip::tcp::endpoint,TcpServer::TcpConnection::pointer> conn_map;
class RobotHandler: public TcpServer::ConnHandler{
    public:
    RobotHandler();
    RobotHandler(Vector_ts<Robot*>* robots_, Vector_ts<Object*>* objects_, VideoHandler* vids_, DbManager* db_);
    ~RobotHandler();
    virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);
    virtual void shutdown();
    void setRobots(Vector_ts<Robot*>* robots_);
    void sendAssignments(std::map<Robot*, int>* assignments);
    void sendCommand(command* comm, boost::asio::ip::tcp::endpoint conn);
    private:
    void threaded_listen(boost::asio::ip::tcp::endpoint connEP);
    void cleanupConn(boost::asio::ip::tcp::endpoint connEP);

    bool running;
    int handlers;
    boost::mutex handlerMutex;

    static conn_map connections;
    Vector_ts<Robot*>* robots;
    Vector_ts<Object*>* objects;
    VideoHandler* vidHandler;
    DbManager* db;

};

#endif

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */

