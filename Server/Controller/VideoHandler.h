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
#include <deque>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "TcpServer.h"
#include "Vector_ts.h"
#include "Robot.h"
#include "Object.h"
#include "message.h"

class VideoHandler : public TcpServer::ConnHandler {
    public:
        VideoHandler(Vector_ts<Robot*>* robots, Vector_ts<Object*>* objs);
        virtual ~VideoHandler();

        virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);
        void write(std::string msg);

    private:
        void threaded_on_connect(TcpServer::TcpConnection::pointer);

        TcpServer::TcpConnection::pointer conn_;
        message read_message_;
        void internal_write(std::string);
        void write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
        void read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
        void close();
        void do_close();

        
        Vector_ts<Robot*>* robots_;
        Vector_ts<Object*>* objs_;
        std::deque<std::string> write_queue_;
};


#endif /* VIDEOHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
