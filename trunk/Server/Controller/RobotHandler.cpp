/* AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>
#include "RobotHandler.h"

conn_map RobotHandler::connections;

RobotHandler::RobotHandler(){}
RobotHandler::RobotHandler(Vector_ts<Robot>* robots_){
		robots = robots_;
}
RobotHandler::~RobotHandler(){}

void RobotHandler::setRobots(Vector_ts<Robot>* robots_){
	robots = robots_;
}

void RobotHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection){
    std::cout<<"in handler fucntion\n";
    //enter the tcp connection pointer into the map
    connections[tcp_connection->socket().remote_endpoint()] = tcp_connection;
    tcp_connection->releaseSocket();

    //insert initialization stuff here
    
    //spawn a thread to listen on the socket and return the function
    std::cout<<"launching threadd...\n";
    boost::asio::ip::tcp::endpoint endpoint = tcp_connection->socket().remote_endpoint();
    tcp_connection->releaseSocket();
    boost::thread connThread(&RobotHandler::threaded_listen, this, endpoint);

}

void RobotHandler::threaded_listen(const boost::asio::ip::tcp::endpoint connEP){
        
	std::cout<<"Robot socket is being handled\n";

	//declare loop  varibles
	bool connected = true;
	boost::asio::streambuf inputBuffer;
	std::string* str_ptr;
	boost::system::error_code error;
	int indexOfChar = 0;

	//main listening loop for a server connection
	std::cout<<"initiating loop\n";
	while(connected){
		std::cout<<"looping...\n";
		//pull data from socket and release
		indexOfChar = boost::asio::read_until(connections[connEP]->socket(), inputBuffer, '\n', error);		
		connections[connEP]->releaseSocket();

		std::cout<<"data read, socket released\n";

		//catch errors to tell when the connection closes;
		if(error == boost::asio::error::eof){
			std::cout<<"EOF\n";
			connected = false;
			continue;
		}
		if(error == boost::asio::error::operation_aborted){
			std::cout<<"aborted..\n";
			connected = false;
			continue;
		}

		std::cout<<"made it past the error traps...\n";
		
		//pull desired string from data
		boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
		str_ptr = new std::string(boost::asio::buffers_begin(data), boost::asio::buffers_begin(data)+indexOfChar);
		
		//do stuff with string 
		std::cout<<"robot handler:" << *str_ptr;

		//clean up from loop iteration 
		inputBuffer.consume(indexOfChar);
		delete str_ptr;

		//sleep abit so other threads can grab a lock on the socket
		boost::this_thread::sleep(boost::posix_time::seconds(2));

	}
	//clean up stuff from connection
}

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
