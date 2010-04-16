/*
 * TcpServer.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "TcpServer.h"
#include <iostream>

//constructs the tcp server and sets it listening
TcpServer::TcpServer(boost::asio::io_service& io_service, int ListenPort, ConnHandler *connHandler)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), ListenPort)),
      ListenPort_(ListenPort),
      connHandler_(connHandler),
      running(true)
{
    listen();
}

TcpServer::~TcpServer() {
}

//shuts down the server by stopping new connections and telling the 
//handler to close any current connections
void TcpServer::shutdown(){
    running = false;
    if (connHandler_)
        connHandler_->shutdown();
    //acceptor_.cancel();
    acceptor_.close();

}

//creates a connection and starts an async listen
void TcpServer::listen() {
    std::cout << "In Listen()\n";
    TcpConnection::pointer new_con = TcpConnection::create(acceptor_.io_service());

    if(!running)
        return;

    acceptor_.async_accept(new_con->socket(), 
        boost::bind(&TcpServer::handle_accept, this, new_con, boost::asio::placeholders::error));
    new_con->releaseSocket();
}

//the handler method to accept new asynchronous connections
void TcpServer::handle_accept(TcpServer::TcpConnection::pointer conn, const boost::system::error_code &error) {
    std::cout << "Handling a new connection\n";
    if(!running){
        //destroy the connection object
        conn->stop();
        std::cout << "[TcpServer] not running, so immediately returning on handle_accept()\n";
        return;
    }
    if (!error){
        //check if we have a valid connection handler
        if (connHandler_ != 0){
            connHandler_->onConnect(conn);
        } else {
            conn->start();
        }

        // Begin listening for a new connection
        listen();
    }
}

//returns a boost shared pointer to this object
TcpServer::TcpConnection::pointer TcpServer::TcpConnection::create(boost::asio::io_service& io_service)
{
    std::cout << "Created new TcpConnection\n";
    return pointer(new TcpConnection(io_service));
}

//locks the socket and returns the pointer
tcp::socket& TcpServer::TcpConnection::socket()
{
    socketMutex.lock();
    return socket_;
}
//releases the socket mutex
void TcpServer::TcpConnection::releaseSocket(){
    socketMutex.unlock();
}

//stops the connection and closes the socket
void TcpServer::TcpConnection::stop(){
    socketMutex.lock();
    socket_.close();
    socketMutex.unlock();

    //other clean up stuff here

}

void TcpServer::TcpConnection::start()
{
    message_ = "Testing, 1, 2, 3...\n";
    message_ += socket_.remote_endpoint().address().to_string();
    message_ += ":";
    message_ += boost::lexical_cast<std::string>(socket_.remote_endpoint().port());
    message_ += "\n";

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&TcpServer::TcpConnection::handle_write, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

TcpServer::TcpConnection::TcpConnection(boost::asio::io_service& io_service)
: socket_(io_service)
{
}

TcpServer::TcpConnection::~TcpConnection()
{
    socketMutex.lock();
    socketMutex.unlock();
}

void TcpServer::TcpConnection::handle_write(const boost::system::error_code& /*error*/,
  size_t /*bytes_transferred*/)
{
}
/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
