/*
 * VideoHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "VideoHandler.h"

//VideoHandler::conn_map VideoHandler::connections;

VideoHandler::VideoHandler(Vector_ts<Robot*>* robots, Vector_ts<Object*>* objs)
    : robots_(robots), objs_(objs)
{
}

VideoHandler::~VideoHandler(){
}

void VideoHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    boost::asio::ip::tcp::endpoint endpoint = tcp_connection->socket().remote_endpoint();
    
    tcp_connection->releaseSocket();

    //connections[endpoint] = tcp_connection;
    //boost::thread connThread(&VideoHandler::threaded_on_connect, this, endpoint);
    boost::thread connThread(&VideoHandler::threaded_on_connect, this, tcp_connection);
}

//void VideoHandler::threaded_on_connect(boost::asio::ip::tcp::endpoint conn){
void VideoHandler::threaded_on_connect(TcpServer::TcpConnection::pointer tcp_connection){

    session* sess = new session(tcp_connection, robots_, objs_);
    sess->start();
}

VideoHandler::session::session(TcpServer::TcpConnection::pointer tcp_, Vector_ts<Robot*>* r, Vector_ts<Object*>* o) 
    : conn_(tcp_), robots_(r), objs_(o)
{
}

VideoHandler::session::~session()
{
}

void VideoHandler::session::start(){
    /*
    tcp::socket &sock = conn_->socket();
    std::string msg = "You've reached the Video Handler\n";
    msg += sock.remote_endpoint().address().to_string();
    msg += ":";
    msg += boost::lexical_cast<std::string>(sock.remote_endpoint().port());
    msg += "\n\n";

    boost::asio::async_write(sock, boost::asio::buffer(msg), 
        boost::bind(&VideoHandler::session::write_handler, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&VideoHandler::session::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    conn_->releaseSocket();
    */

    tcp::socket &sock = conn_->socket();
    // send robot information to the video server
    std::stringstream msg_ss;
    
    // print the current date/time
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    msg_ss << now << "\n";

    // send object information
    {
        objs_->readLock();

        Vector_ts<Object*>::iterator it;
        Vector_ts<Object*>::iterator it_end = objs_->end();

        for (it=objs_->begin(); it < it_end; ++it) {
            msg_ss << (*it)->getOID();
            msg_ss << ":";
            msg_ss << (*it)->getName();
            msg_ss << ":";
            // replace colorsize with qos
            msg_ss << (*it)->getColorsize();
            msg_ss << ":";
        }
        objs_->readUnlock();
    }
    msg_ss << '\n';

    // send robot information
    {
        robots_->readLock();
        robots_->readUnlock();
    }
    msg_ss << '\n';

    std::string msg = msg_ss.str();

    boost::asio::async_write(sock, boost::asio::buffer(msg), 
        boost::bind(&VideoHandler::session::write_handler, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&VideoHandler::session::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    conn_->releaseSocket();
}

void VideoHandler::session::write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    std::cout << "VideoHandler wrote " << bytes_transferred << " bytes to a client\n";
    std::cout.flush();
}

void VideoHandler::session::read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    tcp::socket &sock = conn_->socket();;


    std::cout << "VideoHandler read " << bytes_transferred << " bytes from client " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
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

    std::cout << "Got string \"" << s << "\"";
    std::cout.flush();


    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&VideoHandler::session::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    
    conn_->releaseSocket();

}

void VideoHandler::session::close() {
    tcp::socket &sock = conn_->socket();;

    sock.get_io_service().post(boost::bind(&VideoHandler::session::do_close, this));
     
    conn_->releaseSocket();
}

void VideoHandler::session::do_close() {
    tcp::socket &sock = conn_->socket();;

    std::cout << "Closing socket to " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
    sock.close();

    conn_->releaseSocket();

    delete this;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
