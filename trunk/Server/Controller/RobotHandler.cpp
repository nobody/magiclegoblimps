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
	size_t count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(5), error);
	connections[connEP]->releaseSocket();

	std::cout<<"data read(" << count << "), socket released\n";

	//catch errors to tell when the connection closes;
	if(error == boost::asio::error::eof){
		std::cout<<"EOF\n";
        //die gracefully, think i can do it by calling the cleanup connection method
        	cleanupConn(connEP);
        return;
		
	}
	if(error == boost::asio::error::operation_aborted){
		std::cout<<"aborted..\n";
		//die gracefully
		cleanupConn(connEP);

        return;
	}

	std::cout<<"made it past the error traps...\n";
	
	//get the total number of bytes to read
	char* arr = new char[5];
	boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
	boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

	//int type = *iter;
	//iter++;
	for(int i = 0; i < 5; ++i){
		arr[i] = *iter;
        printf("hex: %02X\n", *iter);
		//std::cout<<std::hex<<arr[i]<<"\n";
		iter++;
	}
	
	//compute the total and the bytes remaing to be pulled from the socket
	int total = *((int*)(arr+1));
	int remaining = total - count;

	//if there are bytes remainging to be read read them
	if(remaining > 0){
		count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(remaining), error);
		connections[connEP]->releaseSocket();


		//catch errors to tell when the connection closes;
		if(error == boost::asio::error::eof){
			std::cout<<"EOF\n";
			//die gracefully;
			cleanupConn(connEP);
			return;
		}
		if(error == boost::asio::error::operation_aborted){
			std::cout<<"aborted..\n";
            		//die gracefully 
			cleanupConn(connEP);
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
	inputBuffer.consume(total);

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
		cleanupConn(connEP);
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
		 //temp->setList(robotData[i].list, robotData[i].listSize);
        	 temp->setVideoURL(std::string(*robotData[i].VideoURL));
        
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
		objArr[i].name = new std::string((*ObjIt)->getName());
		objArr[i].color_size = (*ObjIt)->getColorsize();
		objArr[i].color = (*ObjIt)->getColor();

		++ObjIt;
	}

	//now that we have the object array we need to gett the binary stream to transmitt
	byteArray* byte_ptr = new byteArray;
	write_data(P_OBJECT, objArr, objects->size(), byte_ptr);
	objects->readUnlock();
	delete[] objArr;	

    printf("Color: \n");
    ObjIt--;
    for (int i = 0; i < (*ObjIt)->getColorsize(); ++i){
        printf("%02X ", (*ObjIt)->getColor()[i]);
        fflush(stdout);
    }
    printf("\nData being sent:\n");
    for (int i = 0; i < byte_ptr->size; ++i){
        printf("%02X ", (unsigned)byte_ptr->array[i]);
        fflush(stdout);
    }
    printf("\n");
	
	//hopefully the number of bytes is always a positive number
	boost::asio::write(connections[connEP]->socket(),  boost::asio::buffer(byte_ptr->array, byte_ptr->size),
		boost::asio::transfer_at_least(byte_ptr->size), error);

   	connections[connEP]->releaseSocket();
	delete byte_ptr;
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

		count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(5), error);
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
		char* arr = new char[5];
		boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
		boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

        std::cout << "iterating over data\n";

		//int type = *iter;
		iter++;
		for(int i = 0; i < 5; ++i){
			arr[i] = *iter;
			std::cout<<std::hex<<arr[i]<<"\n";
			iter++;
		}
		
		//compute the total and the bytes remaing to be pulled from the socket
		int total = *((int*)(arr+1));
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
							(*it)->setList(robotData[i].list, robotData[i].listSize);
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
			//actually it should whenever a new object is added
			//i'll have to deal with that

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
	conn_map::iterator iter;
	for(iter = connections.begin(); iter != connections.end(); ++iter){
		if((*iter).first == connEP){
			connections.erase(iter);
			break;
		}
		std::cerr<<"Robot Handler: couldn't find connection to delete\n";
	}
}

void RobotHandler::sendAssignments(std::map<Robot*, int>* assignments){
	conn_map::iterator connIter;
	std::map<Robot*, int>::iterator mapIter = assignments->begin();
	
	assignment* assigns = new assignment[assignments->size()];
	assignment* current = assigns;
	int numForConn;

	//we want to loop through all the connections we know we have
	for(connIter = connections.begin(); connIter != connections.end(); connIter++){
		
		numForConn = 0; 
		//we don't know how many robots will be on each connection
		//so we will loop as long as the connection endpoint matches 
		//the one in the outer loop
		while(((*mapIter).first->getEndpoint()) == ((*connIter).first)){
			current[numForConn].RID = (*mapIter).first->getRID();
			current[numForConn].OID = (*mapIter).second;
			++numForConn;

			mapIter++;
		}

		//now that we have an array we will transmit the assignments for this connection
		byteArray data;
			
		if(write_data(P_ASSIGNMENT, (void*)current, numForConn, &data) < 0){
			//then somthing broke and it should be dealt with
		}
		boost::system::error_code error;
		boost::asio::write(connections[(*connIter).first]->socket(), boost::asio::buffer(data->array, data->size),
			boost::asio::transfer_at_least(data->size), error);

		connections[(*connIter).first]->releaseSocket();

		//still don't know what errors i should look for on a read

		current += numForConn;


	}

	delete[] assigns;
	delete assignments;
}

void RobotHandler::sendCommand(command* comm, boost::asio::ip::tcp::endpoint conn){	
	byteArray* data = new byteArray;
	boost::system::error_code error;
	write_data(P_COMMAND, comm, 1, data);
	
	boost::asio::write(connections[conn]->socket(), boost::asio::buffer(data->array, data->size),
		boost::asio::transfer_at_least(data->size), error);

	connections[conn]->releaseSocket();

}
/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/
