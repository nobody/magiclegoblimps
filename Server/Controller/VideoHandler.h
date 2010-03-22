/*
 * VideoHandler.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef VIDEOHANDLER_H_
#define VIDEOHANDLER_H_

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "TcpServer.h"
#include "Vector_ts.h"
#include "Robot.h"
#include "Object.h"
#include "message.h"

class VideoHandler : public TcpServer::ConnHandler {
    public:
        //VideoHandler(); 
        VideoHandler(Vector_ts<Robot*>* robots, Vector_ts<Object*>* objs);
        virtual ~VideoHandler();

        typedef std::map<boost::asio::ip::tcp::endpoint, TcpServer::TcpConnection::pointer> conn_map;

        virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);

    private:
        void threaded_on_connect(TcpServer::TcpConnection::pointer);
        //void threaded_on_connect(boost::asio::ip::tcp::endpoint);

        static conn_map connections;

        class session {
            public:
                session(TcpServer::TcpConnection::pointer, Vector_ts<Robot*>*, Vector_ts<Object*>*);
                ~session();

                void start();

            private:
                TcpServer::TcpConnection::pointer conn_;
                message read_message_;
                Vector_ts<Robot*>* robots_;
                Vector_ts<Object*>* objs_;

                void write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void close();
                void do_close();

        };

        
        Vector_ts<Robot*>* robots_;
        Vector_ts<Object*>* objs_;
        std::vector<boost::shared_ptr<session> > sessions;
};


#endif /* VIDEOHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
