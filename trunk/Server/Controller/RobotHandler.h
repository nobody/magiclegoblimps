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

typedef std::map<boost::asio::ip::tcp::endpoint,TcpServer::TcpConnection::pointer> conn_map;

class RobotHandler :public TcpServer::ConnHandler
{   
    public:
    RobotHandler();
    ~RobotHandler();
    virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);
    
    private:
    void threaded_listen(boost::asio::ip::tcp::endpoint connEP);

    static conn_map connections;

};

#endif

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
