/*
 * TcpServer.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/exception/all.hpp>


using boost::asio::ip::tcp;

class TcpServer {
    public:
        class TcpConnection 
            : public boost::enable_shared_from_this<TcpConnection>
        {
            public:
                typedef boost::shared_ptr<TcpConnection> pointer;

                static pointer create(boost::asio::io_service& io_service);

                tcp::socket& socket();

                void start();
                void stop();

                void releaseSocket();

                void readLock();
                void readUnlock();
                void writeLock();
                void writeUnlock();

                ~TcpConnection();
            private:
                boost::mutex socketMutex;
                boost::mutex readMutex;
                boost::mutex writeMutex;
                TcpConnection(boost::asio::io_service& io_service);

                void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

                tcp::socket socket_;
                std::string message_;
                bool closed;
        };
        
        class ConnHandler {
            public:
                virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection) = 0;
                virtual void shutdown() = 0;
        };

        TcpServer(boost::asio::io_service& io_service, int ListenPort, ConnHandler *connHandler);
        virtual ~TcpServer();
        void shutdown();

    private:

        void listen();
        void handle_accept(TcpConnection::pointer, const boost::system::error_code&);
        

        tcp::acceptor acceptor_;
        int ListenPort_;
        ConnHandler *connHandler_;
        bool running;

};


#endif /* TCPSERVER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
