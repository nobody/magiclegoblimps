/*
 * AdminSocket.h
 *
 * Modified on:    $Date: 2010-02-23 19:48:07 -0600 (Tue, 23 Feb 2010) $
 * Last Edited By: $Author: mealey.patrick $
 * Revision:       $Revision: 59 $
 */

#ifndef ADMINHANDLER_H_
#define ADMINHANDLER_H_

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TcpServer.h"

class AdminHandler : public TcpServer::ConnHandler {
    public:
       AdminHandler(); 

       virtual ~AdminHandler();

       virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);

    private:
       void handle_write(const boost::system::error_code&, size_t);

};


#endif /* ADMINHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
