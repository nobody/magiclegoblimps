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
#include <cstdlib>

#include "TcpServer.h"
#include "message.h"
#include "Robot.h"
#include "protocol.h"
#include "RobotHandler.h"

class AdminHandler : public TcpServer::ConnHandler {
    public:
        AdminHandler();
        AdminHandler(RobotHandler* robotControl_, Vector_ts<Robot*>* robots_);
        virtual ~AdminHandler();

        typedef std::map<boost::asio::ip::tcp::endpoint, TcpServer::TcpConnection::pointer> conn_map;

        virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);
        virtual void shutdown();

    private:
        //void threaded_on_connect(int connIndex);
        void threaded_on_connect(boost::asio::ip::tcp::endpoint);

        static conn_map connections;

        RobotHandler* robotControl;
        Vector_ts<Robot*>* robots;

        class session {
            public:
                session(TcpServer::TcpConnection::pointer, Vector_ts<Robot*>*, RobotHandler* );
                ~session();

                void start();

            private:
                TcpServer::TcpConnection::pointer conn_;
                Vector_ts<Robot*>* robots;
                RobotHandler* robotControl;
                message read_message_;

                void write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void close();
                void do_close();

        };

        std::vector<boost::shared_ptr<session> > sessions;
};


#endif /* ADMINHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
