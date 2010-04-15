/*
 * VideoHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "VideoHandler.h"

VideoHandler::VideoHandler(Vector_ts<Robot*>* robots, Vector_ts<Object*>* objs)
    : robots_(robots), objs_(objs)
{
}

VideoHandler::~VideoHandler(){
}

void VideoHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    if (conn_ != TcpServer::TcpConnection::pointer()){
        // there is another connection already
        tcp_connection->socket().close();
        tcp_connection->releaseSocket();
        std::cerr << "[VH] VideoHandler only supports one connection\n";
        return;
    }
    threaded_on_connect(tcp_connection);
}

void VideoHandler::threaded_on_connect(TcpServer::TcpConnection::pointer tcp_connection){

    conn_ = tcp_connection;

    tcp::socket &sock = conn_->socket();
    // send robot information to the video server
    std::stringstream msg_ss;
    
    // print the current date/time
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    msg_ss << now << "\n\n";

    // send robot information
    {
        robots_->readLock();

        Vector_ts<Robot*>::iterator it;
        Vector_ts<Robot*>::iterator it_end = robots_->end();

        for (it = robots_->begin(); it < it_end; ++it) {
            (*it)->lock();

            msg_ss << (*it)->getVideoURL();
            msg_ss << ";";
            std::map<int, int>* list = (*it)->list;

            std::map<int, int>::iterator i = list->begin();
            for (; i != list->end(); ++i) {
                msg_ss << (*i).first << ";" << (*i).second << ";";
            }

            (*it)->unlock();

            msg_ss << "\n";
        }
        robots_->readUnlock();
    }
    msg_ss << '\n';

    std::string msg = msg_ss.str();

    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&VideoHandler::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    conn_->releaseSocket();
    write(msg);
}

void VideoHandler::read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    tcp::socket &sock = conn_->socket();;


    std::cout << "[VH] VideoHandler read " << bytes_transferred << " bytes from client " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
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

    std::cout << "[VH] Got string \"" << s << "\"";
    std::cout.flush();


    boost::asio::async_read_until(sock, read_message_.buffer(), '\n', 
        boost::bind(&VideoHandler::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    
    conn_->releaseSocket();

}

void VideoHandler::write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred) {
    std::cout << "[VH] VideoHandler wrote " << bytes_transferred << " bytes to a client\n";
    std::cout.flush();
    if (!error) {                                                                            
        std::string msg(write_queue_.front());
        std::cout << "[VH] Successfully wrote \"" << msg << "\" to the socket.\n";

        write_queue_.pop_front();
        if (!write_queue_.empty()){
            boost::asio::async_write(conn_->socket(), boost::asio::buffer(write_queue_.front()), 
                boost::bind(&VideoHandler::write_handler, this, 
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            conn_->releaseSocket();
            std::cout << "[VH] Write queue not empty" << std::endl;
        }
    } else {
        close();
    }

}

void VideoHandler::internal_write(std::string msg) {

    bool writing = !write_queue_.empty();

    write_queue_.push_back(msg);
    if (!writing){
        boost::asio::async_write(conn_->socket(), boost::asio::buffer(msg), 
            boost::bind(&VideoHandler::write_handler, this, 
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        conn_->releaseSocket();
    }
    
}

void VideoHandler::write(std::string msg) {
    if (conn_ != TcpServer::TcpConnection::pointer()){
        conn_->socket().get_io_service().post(boost::bind(&VideoHandler::internal_write, this, msg));
        conn_->releaseSocket();
    } else {
        std::cout << "[VH] No video client to write to\n";
    }
}

void VideoHandler::close() {
    tcp::socket &sock = conn_->socket();;

    sock.get_io_service().post(boost::bind(&VideoHandler::do_close, this));
     
    conn_->releaseSocket();
}

void VideoHandler::do_close() {
    tcp::socket &sock = conn_->socket();;

    std::cout << "[VH] Closing socket to " << sock.remote_endpoint().address().to_string() << ":" << sock.remote_endpoint().port() << "\n";
    sock.close();

    conn_->releaseSocket();

    conn_ = TcpServer::TcpConnection::pointer();;
}

void VideoHandler::shutdown(){}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
