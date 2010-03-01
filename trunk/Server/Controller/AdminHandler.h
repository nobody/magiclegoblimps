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

        typedef std::map<boost::asio::ip::tcp::endpoint, TcpServer::TcpConnection::pointer> conn_map;

        virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);

    private:
        //void threaded_on_connect(int connIndex);
        void threaded_on_connect(boost::asio::ip::tcp::endpoint);
        void write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);

        static conn_map connections;

};


#endif /* ADMINHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
