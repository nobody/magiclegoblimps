/*
 * AdminSocket.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ADMINHANDLER_H_
#define ADMINHANDLER_H_

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "TcpServer.h"

class AdminHandler : public TcpServer::ConnHandler {
    public:
       AdminHandler(); 

       virtual ~AdminHandler();

       virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);

    private:
       void handle_write(const boost::system::error_code&, size_t);

       void threaded_on_connect(int connIndex);

       static std::vector<TcpServer::TcpConnection::pointer> connections;

};


#endif /* ADMINHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
