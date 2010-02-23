/*
 * AdminSocket.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "AdminSocket.h"

int AdminSocket::LISTEN_PORT = 10000;

AdminSocket::AdminSocket(boost::asio::io_service& io_service)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), LISTEN_PORT))
{
    listen();
}

AdminSocket::~AdminSocket() {
    // TODO Auto-generated destructor stub
}

void AdminSocket::listen() {
    tcp_connection::pointer new_connection =
      tcp_connection::create(acceptor_.io_service());

   acceptor_.async_accept(new_connection->socket(),
        boost::bind(&AdminSocket::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }

void AdminSocket::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
    if (!error)
    {
      new_connection->start();
      listen();
    }
  }




/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
