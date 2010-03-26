/* RobotHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>
#include "RobotHandler.h"

conn_map RobotHandler::connections;

RobotHandler::RobotHandler(){}
RobotHandler::RobotHandler(Vector_ts<Robot*>* robots_){
		robots = robots_;
}
RobotHandler::~RobotHandler(){}

void RobotHandler::setRobots(Vector_ts<Robot*>* robots_){
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
	size_t count = 0;

	//main listening loop for a server connection
	std::cout<<"initiating loop\n";
	while(connected){
		std::cout<<"looping...\n";
		//pull data from socket and release
		//indexOfChar = boost::asio::read_until(connections[connEP]->socket(), inputBuffer, '\n', error);		

		count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(3), error);
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

		char* size = new char[4];
		boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
		boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);
		iter++;
		for(int i = 0; i < 4; ++i){
			size[i] = *iter;
			std::cout<<std::hex<<size[i]<<"\n";
			iter++;
		}
		/*	
		//pull desired string from data
		boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
		str_ptr = new std::string(boost::asio::buffers_begin(data), boost::asio::buffers_begin(data)+indexOfChar);
		
		//do stuff with string 
		std::cout<<"robot handler:" << *str_ptr;
		parseRobots(str_ptr, connEP, ROBOT_UPDATE);

		//clean up from loop iteration 
		inputBuffer.consume(indexOfChar);
		//delete str_ptr;
		*/
		


		//sleep abit so other threads can grab a lock on the socket
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		

	}
//clean up stuff from connection
}

void RobotHandler::parseRobots(std::string* msg, boost::asio::ip::tcp::endpoint connEP, int use){

bool moreRobots = true;
size_t end = 0;
int robotID = 0;
int x = 0; 
int y = 0;
std::string current;
while(moreRobots){

//parse robot data from string here
//find the end of the id
end = msg->find('$');

//check to see if there is a dollar sign
if (end != std::string::npos){
	//pull out the id and create a temporary string that is the 
	//current string minus the id, delete the current string and 
	//reassign the pointer to the new string;
	current = msg->substr(0, end);
	std::string* temp = new std::string(msg->substr(end+1, std::string::npos));
	delete msg;
	msg = temp;
}else{
	//complain, clean up, and return the function
}
robotID = atoi(current.c_str());

//find the end of xcord
end = msg->find('$');

if (end != std::string::npos){
	//pull out the id and create a temporary string that is the 
	//current string minus the xCord, delete the current string and 
	//reassign the pointer to the new string;
	current = msg->substr(0, end);
		std::string* temp = new std::string(msg->substr(end+1, std::string::npos));
		delete msg;
		msg = temp;
	}else{
		//complain, clean up, and return the function
	}

	x = atoi(current.c_str());

	
	//find the end of Ycord
	end = msg->find('$');

	if (end != std::string::npos){
		//pull out the id and create a temporary string that is the 
		//current string minus the ycord, delete the current string and 
		//reassign the pointer to the new string;
		current = msg->substr(0, end);
			std::string* temp = new std::string(msg->substr(end+1, std::string::npos));
			delete msg;
			msg = temp;
		}else{

			//since this is the last element if there is no dollar sign, it is the last 
			//so we set the end flag for the loop and go ahead and delete the msg
			moreRobots = false;
			current = msg->substr(0, msg->size() - 1);
			delete msg;
		}

		y = atoi(current.c_str());

	
		//switch to do stuff with the robot data based on weather on not we 
		//are creating or updating robots
		Robot* robot_temp;
		switch(use){
			case ROBOT_INIT:
				// create a robot and initialize it from the string
				// then add it to the list
				robot_temp = new Robot(connEP, robotID);
				robot_temp->setXCord(x);
				robot_temp->setYCord(y);
				robots->lock();
				robots->push_back(robot_temp);
				robots->unlock();
				break;
			case ROBOT_UPDATE:
				Vector_ts<Robot*>::iterator it;

				for(it = robots->begin(); it < robots->end(); ++it){
					
					if ((*it)->getEndpoint() == connEP  && (*it)->getRID() == robotID){
						(*it)->lock();
						(*it)->setXCord(x);
						(*it)->setYCord(y);
						(*it)->lock();
					}
				
				}
				break;
		}//switch
	}//while
}//function

void RobotHandler::cleanupConn(boost::asio::ip::tcp::endpoint connEP){
	//get a lock on the robot vector and declare and iterator
	robots->lock();
	Vector_ts<Robot*>::iterator it;

	//loop over the robot vector and remove the elements with the 
	//corresponding connection's endpoint
	for(it = robots->begin(); it < robots->end(); ++it){
		if ((*it)->getEndpoint() == connEP){
			delete (*it);
			robots->erase(it);
		}
	}
	robots->unlock();
	//remove endpoint connection from map
	//not sure its really nesscary
}
/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
