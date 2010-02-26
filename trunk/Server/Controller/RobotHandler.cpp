/* AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "RobotHandler.h"

conn_map RobotHandler::connections;

RobotHandler::RobotHandler(){}
RobotHandler::~RobotHandler(){}

void RobotHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection){

    //enter the tcp connection pointer into the map
    connections[tcp_connection->socket().remote_endpoint()] = tcp_connection;

    //insert initialization stuff here
    
    //spawn a thread to listen on the socket and return the function
    boost::thread connThread(&RobotHandler::threaded_listen, this, tcp_connection->socket().remote_endpoint());
}

void RobotHandler::threaded_listen(const boost::asio::ip::tcp::endpoint connEP){
        /*std::string msg = "your robot socket is being handled";
        boost::asio::write(connections[connEP]->socket(), boost::asio::buffer(msg));
        connections[connEP]->socket().close();*/

	bool connected = true;

	while(connected){
		connections[connEP]->socket();
		connections[connEP]->releaseSocket();

	}
}

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
