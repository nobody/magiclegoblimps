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
RobotHandler::RobotHandler(Vector_ts<Robot*>* robots_, Vector_ts<Object*>* objects_){
	robots = robots_;
	objects = objects_;
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

    //declare stuff to pull data;
	boost::asio::streambuf inputBuffer;
	boost::system::error_code error;
    boost::asio::ip::tcp::endpoint connEP = tcp_connection->socket().remote_endpoint();
    tcp_connection->releaseSocket();

    //need to get the robots from the controller that just connected
	size_t count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(3), error);
	connections[connEP]->releaseSocket();

	std::cout<<"data read, socket released\n";

	//catch errors to tell when the connection closes;
	if(error == boost::asio::error::eof){
		std::cout<<"EOF\n";
        //die gracefully, think i can do it by calling the cleanup connection method
        
        return;
		
	}
	if(error == boost::asio::error::operation_aborted){
		std::cout<<"aborted..\n";
		//die gracefully

        return;
	}

	std::cout<<"made it past the error traps...\n";
	
	//get the total number of bytes to read
	char* arr = new char[4];
	boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
	boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

	//int type = *iter;
	iter++;
	for(int i = 0; i < 4; ++i){
		arr[i] = *iter;
		std::cout<<std::hex<<arr[i]<<"\n";
		iter++;
	}
	
	//compute the total and the bytes remaing to be pulled from the socket
	int total = *((int*)arr);
	int remaining = total - count;

	//if there are bytes remainging to be read read them
	if(remaining > 0){
		count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(remaining), error);
		connections[connEP]->releaseSocket();


		//catch errors to tell when the connection closes;
		if(error == boost::asio::error::eof){
			std::cout<<"EOF\n";
			//die gracefully;

			return;
		}
		if(error == boost::asio::error::operation_aborted){
			std::cout<<"aborted..\n";
            //die gracefully 

			return;
		}
	}

	//reset all the varibles with the new data
	delete[] arr;
	arr = new char[total];
	data = inputBuffer.data();
	iter = boost::asio::buffers_begin(data);
	for(int i = 0; i < total; ++i){
		arr[i] = *iter;
		iter++;
	}

	//consume the stuff in the buffer, we're done with it now;
	//wasn't quite sure how to deal with the size_t issue...
	inputBuffer.consume((size_t)total);

	//convert the char array to a 
	readReturn* message = new readReturn;
	if(read_data((void*)arr, message) < 0){
		// read failed clean up the mess
	}

	//delete the array, we're done with it now
	delete arr;

	//switch on the type of struct recovered
	if(message->type != P_ROBOT_INIT){
        //die gracefully
    }	
	
    //initilize stuff
	Vector_ts<Robot*>::iterator it;
	robotInit* robotData = new robotInit[message->size];
	robotData = (robotInit*)(message->array);

    //get a lock on the vector
	robots->lock();
	for(int i = 0; i < message->size; ++i){
		Robot* temp = new Robot(connEP, robotData[i].RID);
        temp->setXCord(robotData[i].x);
        temp->setYCord(robotData[i].y);
        //temp->setVideoURL(std::string(robotData[i].VideoURL));
        
        robots->push_back(temp);
        //might want to clean up some stuff here if patrick doesn't fix destructor
	}
	robots->unlock();
	delete[] robotData;	

	//clean up stuff from 
	delete message;

	//now that we have processed the new robots from this controller, we need to 
	//send the controller the list of objects, first we need to construct an array of
	//object structs;
	
	objects->readLock();
	object* objArr = new object[objects->size()];
	Vector_ts<Object*>::iterator ObjIt = objects->begin();

	for(int i = 0; i < objects->size(); ++i){
		objArr[i].OID = (*ObjIt)->getOID();
		objArr[i].name = &(*ObjIt)->getName();
		objArr[i].color_size = (*ObjIt)->getColorsize();
		objArr[i].color = (*ObjIt)->getColor();

		++it;
	}

	//now that we have the object array we need to gett the binary stream to transmitt
	byteArray* byte_ptr = new byteArray;
	write_data(P_OBJECT, objArr, objects->size(), byte_ptr);
	objects->readUnlock();

	//not sure that anything past here is 
	//need to put the array in a buffer
	boost::asio::streambuf outputBuffer;	
	/*data = outputBuffer.prepare(byte_ptr->size);
	iter = boost::asio::buffers_begin(data);

	//need some kind of for loop to put the array into the buffer;
	for(int i = 0; i < byte_ptr->size; ++i){
		*iter = byte_ptr->array[i];
		iter++;
	}*/

	//hopefully the number of bytes is always a positive number
	boost::asio::write(connections[connEP]->socket(), outputBuffer, boost::asio::transfer_at_least(byte_ptr->size), error);

    
    //spawn a thread to listen on the socket and return the function
    std::cout<<"launching threadd...\n";
    boost::thread connThread(&RobotHandler::threaded_listen, this, connEP);

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
			
		//should check the buffer size so that i know how much
		//is in there before it gets to a ridiculous size

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
		
		//get the total number of bytes to read
		char* arr = new char[4];
		boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
		boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

		//int type = *iter;
		iter++;
		for(int i = 0; i < 4; ++i){
			arr[i] = *iter;
			std::cout<<std::hex<<arr[i]<<"\n";
			iter++;
		}
		
		//compute the total and the bytes remaing to be pulled from the socket
		int total = *((int*)arr);
		int remaining = total - count;

		//if there are bytes remainging to be read read them
		if(remaining > 0){
			count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(remaining), error);
			connections[connEP]->releaseSocket();


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
		}

		//reset all the varibles with the new data
		delete[] arr;
		arr = new char[total];
		data = inputBuffer.data();
		iter = boost::asio::buffers_begin(data);
		for(int i = 0; i < total; ++i){
			arr[i] = *iter;
			iter++;
		}

		//consume the stuff in the buffer, we're done with it now;
		//wasn't quite sure how to deal with the size_t issue...
		inputBuffer.consume((size_t)total);

		//convert the char array to a 
		readReturn* message = new readReturn;
		if(read_data((void*)arr, message) < 0){
			// read failed clean up the mess
		}

		//delete the array, we're done with it now
		delete arr;

		//switch on the type of struct recovered
		switch(message->type){
			case P_ROBOT_UPDATE:
			{	
				//initilize stuff
				Vector_ts<Robot*>::iterator it;
				robotUpdate* robotData = new robotUpdate[message->size];
				robotData = (robotUpdate*)(message->array);

				//get a readlock on the vector
				robots->readLock();
				for(int i = 0; i < message->size; ++i){
					for(it = robots->begin(); it < robots->begin(); ++it){

						//if it is the robot we are looking for, lock and update it
						if ((*it)->getEndpoint() == connEP  && (*it)->getRID() == robotData[i].RID){
							(*it)->lock();
							(*it)->setXCord(robotData[i].x);
							(*it)->setYCord(robotData[i].y);
							(*it)->unlock();
						}
					}
					
				}
				robots->readUnlock();
				delete[] robotData;	

			}
			break;
			case P_OBJECT:
			//i don't think this should happen, but i don't know

			break;
			
			default:
			//its broken if it gets here, need to figure out what to do.
            break;
		}

		//clean up stuff from iteration
		delete message;
		
	
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
