/*
 * AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "AdminHandler.h"

AdminHandler::conn_map AdminHandler::connections;

AdminHandler::AdminHandler(){
}

AdminHandler::~AdminHandler(){
}

void AdminHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    boost::asio::ip::tcp::endpoint endpoint = tcp_connection->socket().remote_endpoint();
    
    tcp_connection->releaseSocket();

    connections[endpoint] = tcp_connection;
    boost::thread connThread(&AdminHandler::threaded_on_connect, this, endpoint);
}

void AdminHandler::threaded_on_connect(boost::asio::ip::tcp::endpoint conn){


    TcpServer::TcpConnection::pointer tcp_connection(connections[conn]);
    //tcp::socket &sock = tcp_connection->socket();

    boost::shared_ptr<session> sess(new session(tcp_connection));
    sessions.push_back(sess);
    sess->start();


}

AdminHandler::session::session(TcpServer::TcpConnection::pointer tcp_) 
    : conn_(tcp_)
{
}

AdminHandler::session::~session()
{
}

void AdminHandler::session::start(){

    tcp::socket &sock = conn_->socket();
    std::string msg = "You've reached the Admin Handler\n";
    msg += sock.remote_endpoint().address().to_string();
    msg += ":";
    msg += boost::lexical_cast<std::string>(sock.remote_endpoint().port());
    msg += "\n\n";

    boost::asio::async_write(sock, boost::asio::buffer(msg), 
        boost::bind(&AdminHandler::session::write_handler, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&AdminHandler::session::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    conn_->releaseSocket();
    
}

void AdminHandler::session::write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    std::cout << "AdminHandler wrote " << bytes_transferred << " bytes to a client\n";
    std::cout.flush();
    
}

void AdminHandler::session::read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    tcp::socket &sock = conn_->socket();;


    std::cout << "AdminHandler read " << bytes_transferred << " bytes from client " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
    std::cout.flush();

    if (error == boost::asio::error::eof){
        conn_->releaseSocket();
        close();
        return;
    } else if (error == boost::asio::error::operation_aborted)
        return;
    else if (error)
        throw boost::system::system_error(error);

    std::string s(read_message_.data(bytes_transferred));
    read_message_.consume(bytes_transferred);

    std::cout << "Got string \"" << s << "\"\n";
    std::cout.flush();


    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&AdminHandler::session::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    
    conn_->releaseSocket();

}

void AdminHandler::session::close() {
    tcp::socket &sock = conn_->socket();;

    sock.get_io_service().post(boost::bind(&AdminHandler::session::do_close, this));
     
    conn_->releaseSocket();
}

void AdminHandler::session::do_close() {
    tcp::socket &sock = conn_->socket();;

    std::cout << "Closing socket to " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
    sock.close();

    conn_->releaseSocket();
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
