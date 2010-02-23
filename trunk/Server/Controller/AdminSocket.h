/*
 * AdminSocket.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ADMINSOCKET_H_
#define ADMINSOCKET_H_

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class AdminSocket {
public:
    AdminSocket(boost::asio::io_service& io_service);
    virtual ~AdminSocket();

private:
    tcp::acceptor acceptor_;
    static int LISTEN_PORT;
        
        class tcp_connection
          : public boost::enable_shared_from_this<tcp_connection>
        {
        public:
          typedef boost::shared_ptr<tcp_connection> pointer;

          static pointer create(boost::asio::io_service& io_service)
          {
            return pointer(new tcp_connection(io_service));
          }

          tcp::socket& socket()
          {
            return socket_;
          }

          void start()
          {
            message_ = "Testing, 1, 2, 3...\n";

            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
          }

        private:
          tcp_connection(boost::asio::io_service& io_service)
            : socket_(io_service)
          {
          }

          void handle_write(const boost::system::error_code& /*error*/,
              size_t /*bytes_transferred*/)
          {
          }

          tcp::socket socket_;
          std::string message_;
        };

    void listen();
    void handle_accept(tcp_connection::pointer, const boost::system::error_code&);

};


#endif /* ADMINSOCKET_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
