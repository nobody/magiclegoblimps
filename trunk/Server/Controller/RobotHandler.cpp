/* RobotHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>
#include "RobotHandler.h"

conn_map RobotHandler::connections;
thread_map RobotHandler::threads;

RobotHandler::RobotHandler(){}
RobotHandler::RobotHandler(Vector_ts<Robot*>* robots_, Vector_ts<Object*>* objects_, VideoHandler* vids_, DbManager* db_)
    : robots(robots_), objects(objects_), vidHandler(vids_), db(db_)
{
    handlers = 0;
    running = true;
}
RobotHandler::~RobotHandler(){}

void RobotHandler::setRobots(Vector_ts<Robot*>* robots_){
    robots = robots_;
}

void RobotHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection){
    if(!running) return;

    handlerMutex.lock();
    ++handlers;
    handlerMutex.unlock();
    
    //std::cout<<"[RH] in handler fucntion\n";
    //enter the tcp connection pointer into the map
    connections[tcp_connection->socket().remote_endpoint()] = tcp_connection;
    tcp_connection->releaseSocket();

    //declare stuff to pull data;
    boost::asio::streambuf inputBuffer;
    boost::system::error_code error;
    boost::asio::ip::tcp::endpoint connEP = tcp_connection->socket().remote_endpoint();
    tcp_connection->releaseSocket();

    //need to get the robots from the controller that just connected
    connections[connEP]->readLock();
    size_t count; 
    try{
        count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(5), error);
    }catch(boost::exception &e){
        std::cerr << "[RH] exception occured: \n" << diagnostic_information(e);
        connections[connEP]->readUnlock();
        cleanupConn(connEP);
        return;
    }catch(...){        
        std::cerr << "[RH] unknown exception occured in on connect\n";
        connections[connEP]->readUnlock();
        cleanupConn(connEP);
        return;
    }
    connections[connEP]->readUnlock();

    //std::cout<<"[RH] data read(" << count << "), socket released\n";

    //catch errors to tell when the connection closes;
    if(error == boost::asio::error::eof){
        std::cout<<"[RH] EOF\n";
        //die gracefully, think i can do it by calling the cleanup connection method
            cleanupConn(connEP);
        return;
        
    }
    if(error == boost::asio::error::operation_aborted){
        std::cout<<"[RH] aborted..\n";
        //die gracefully
        cleanupConn(connEP);

        return;
    }

    //std::cout<<"[RH] made it past the error traps...\n";
    
    //get the total number of bytes to read
    char* arr = new char[5];
    boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
    boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

    //int type = *iter;
    //iter++;
    if(count < 5){
        cleanupConn(connEP);
        std::cout <<"only got: " << count << " bytes; exiting...\n";
        return;
    }
    for(int i = 0; i < 5; ++i){
        arr[i] = *iter;
        //printf("[RH] hex: %02X\n", *iter);
        //std::cout<<std::hex<<arr[i]<<"\n";
        iter++;
    }
    
    //compute the total and the bytes remaing to be pulled from the socket
    int total = *((int*)(arr+1));
    int remaining = total - count;

    //if there are bytes remainging to be read read them
    if(remaining > 0){
        connections[connEP]->readLock();
        try{
            count = boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(remaining), error);
        }catch(boost::exception &e){
            std::cerr << "[RH] exception occured: \n" << diagnostic_information(e);
            connections[connEP]->readUnlock();
            cleanupConn(connEP);
            return;
        }catch(...){        
            std::cerr << "[RH] unknown exception occured in on connect\n";
            connections[connEP]->readUnlock();
            cleanupConn(connEP);
            return;
        }
        connections[connEP]->readUnlock();


        //catch errors to tell when the connection closes;
        if(error == boost::asio::error::eof){
            std::cout<<"[RH] EOF\n";
            //die gracefully;
            cleanupConn(connEP);
            return;
        }
        if(error == boost::asio::error::operation_aborted){
            std::cout<<"[RH] aborted..\n";
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
    inputBuffer.consume(total);
    count -= total;

    //convert the char array to a an array of objects
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

    if(!(robotData[0].RID < 0)){

        //get a lock on the vector
        robots->lock();
        for(int i = 0; i < message->size; ++i){
            Robot* temp = new Robot(connEP, robotData[i].RID);
                 temp->setXCord(robotData[i].x);
                 temp->setYCord(robotData[i].y);
                 temp->setCamera(robotData[i].cameraType);
             //temp->setList(robotData[i].list, robotData[i].listSize);
                 temp->setVideoURL(std::string(*robotData[i].VideoURL));
            
                robots->push_back(temp);
                //might want to clean up some stuff here if patrick doesn't fix destructor
        }
        robots->unlock();
        delete[] robotData;    

        //clean up stuff from 
        delete message;

        // let the web server know about the robots
        db->insertCameras(robots);

    }

    //now that we have processed the new robots from this controller, we need to 
    //send the controller the list of objects, first we need to construct an array of
    //object structs;
    
    objects->readLock();
    if(objects->size() > 0){
        object* objArr = new object[objects->size()];
        Vector_ts<Object*>::iterator ObjIt = objects->begin();

        for(int i = 0; i < objects->size(); ++i){
            (*ObjIt)->lock();
            objArr[i].OID = (*ObjIt)->getOID();
            objArr[i].name = new std::string((*ObjIt)->getName());
            objArr[i].color_size = (*ObjIt)->getColorsize();
            objArr[i].color = (*ObjIt)->getColor();
            objArr[i].box = (*ObjIt)->getBox();
            objArr[i].box_size = (*ObjIt)->getBoxsize();;
            (*ObjIt)->unlock();

            ++ObjIt;
        }

        //now that we have the object array we need to gett the binary stream to transmitt
        byteArray* byte_ptr = new byteArray;
        write_data(P_OBJECT, objArr, objects->size(), byte_ptr);

        //now that we have our byte array we can unlock the vector
        //and delete the array
        objects->readUnlock();
        delete[] objArr;    

        printf("[RH] Color: \n");
        ObjIt--;
        for (int i = 0; i < (*ObjIt)->getColorsize(); ++i){
            printf("%02X ", (*ObjIt)->getColor()[i]);
            fflush(stdout);
        }
        printf("\n[RH] Data being sent:\n");
        for (int i = 0; i < byte_ptr->size; ++i){
            printf("%02X ", (unsigned)byte_ptr->array[i]);
            fflush(stdout);
        }
        printf("\n");
        
        //hopefully the number of bytes is always a positive number
        connections[connEP]->writeLock();
        boost::asio::write(connections[connEP]->socket(),  boost::asio::buffer(byte_ptr->array, byte_ptr->size),
            boost::asio::transfer_at_least(byte_ptr->size), error);

        connections[connEP]->writeUnlock();
        delete byte_ptr;
    }else{
        //if there are no objects in the system we should just unclock the vector
        objects->readUnlock();
    }
    
    //spawn a thread to listen on the socket and return the function
    //std::cout<<"[RH] launching threadd...\n";
    boost::thread* connThread = new boost::thread(&RobotHandler::threaded_listen, this, connEP);
    threads[connEP] = connThread;

}

void RobotHandler::threaded_listen(const boost::asio::ip::tcp::endpoint connEP){
        
    //std::cout<<"[RH] Robot socket is being handled\n";

    //declare loop  varibles
    bool connected = true;
    boost::asio::streambuf inputBuffer;
    inputBuffer.prepare(2048);
    boost::system::error_code error;
    size_t count = 0;

    //main listening loop for a server connection
    //std::cout<<"[RH] initiating loop\n";
    while(connected){
        //std::cout<<"[RH] looping...\n";

        //count keeps track of the number of bytes in the buffer
        //it checks to see if the bytes it wants are already there
        //so it doesn't block on the socket and wait for a new message 
        //so that it can process a message we already have
        if(count < 5){

            //locks the socket, reads into the buffer, increases count by the number of
            //bytes read in and then unlocks the socket.
            connections[connEP]->readLock();
            try{
                count += boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(5), error);
            }catch(boost::exception &e){
                std::cerr << "[RH] exception occured: \n" << diagnostic_information(e);
                connections[connEP]->readUnlock();
                cleanupConn(connEP);
                return;
            }catch(...){        
                std::cerr << "[RH] unknown exception occured in listen\n";
                connections[connEP]->readUnlock();
                cleanupConn(connEP);
                return;
            }
            connections[connEP]->readUnlock();

            //std::cout<<"[RH] data read, socket released\n";

        }
        if (count < 5) {
            connected = false;
            continue;
        }

        //catch errors to tell when the connection closes;
        if(error == boost::asio::error::eof){
            std::cout<<"[RH] EOF\n";
            connected = false;
            continue;
        }
        if(error == boost::asio::error::operation_aborted){
            std::cout<<"[RH] aborted..\n";
            connected = false;
            continue;
        }
        if(error){
            cleanupConn(connEP);
            return;
        }

        //std::cout<<"[RH] made it past the error traps...\n";
        
        //get the total number of bytes to read
        //the last four bytes in this array should tell it how many it 
        //needs overall to reconstruct the message
        char* arr = new char[5];
        boost::asio::streambuf::const_buffers_type data = inputBuffer.data();
        boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iter = boost::asio::buffers_begin(data);

        //std::cout << "[RH] iterating over data\n";

        //int type = *iter;
        //iter++;
        for(int i = 0; i < 5; ++i){
            arr[i] = *iter;
            //printf("[RH] hex: %02X\n", *iter);
            if (iter != buffers_end(data))
                iter++;
        }
        
        //compute the total and the bytes that need to be pulled from the socket 
        //to get the entire message
        size_t total = (size_t)(*((int*)(arr+1)));
        size_t remaining = total - count;

        //if there are bytes remainging to be read read them
        while (remaining > 0 /*&& count < total*/){
            std::cout << "remaining: " << remaining << "  count: " << count << "\n";
            connections[connEP]->readLock();
            try{
                count += boost::asio::read(connections[connEP]->socket(), inputBuffer, boost::asio::transfer_at_least(remaining - count), error);
                remaining = total - count;
            }catch(boost::exception &e){
                std::cerr << "[RH] exception occured: \n" << diagnostic_information(e);
                connections[connEP]->readUnlock();
                delete[] arr;
                cleanupConn(connEP);
                return;
            }catch(...){        
                std::cerr << "[RH] unknown exception occured in on connect\n";
                connections[connEP]->readUnlock();
                delete[] arr;
                cleanupConn(connEP);
                return;
            }
            connections[connEP]->readUnlock();


            //catch errors to tell when the connection closes;
            if(error == boost::asio::error::eof){
                std::cout<<"[RH] EOF\n";
                connected = false;
                continue;
            }
            if(error == boost::asio::error::operation_aborted){
                std::cout<<"[RH] aborted..\n";
                connected = false;
                continue;
            }
            if(error){
                cleanupConn(connEP);
                return;
            }
        }
        std::cout << "remaining: " << remaining << "  count: " << count << "\n";


        std::cout<<"[RH] Recieved a message (" << count << " of " << total << " bytes):\n";
        std::string tmp__(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));
        std::cout << tmp__ << "\n\n";
        //reset all the varibles with the new data in the buffer
        delete[] arr;
        arr = new char[total];
        data = inputBuffer.data();
        iter = boost::asio::buffers_begin(data);
        for(size_t i = 0; i < total; ++i){
            arr[i] = *iter;
            if (iter != buffers_end(data))
                iter++;
            else
                arr[i] = 0;
        }

        //consume the stuff in the buffer, we're done with it now;
        //wasn't quite sure how to deal with the size_t issue...
        inputBuffer.consume(total);

        //remove the number of bytes consumed from count so that we 
        //keep acurate track of how much is in the buffer
        count -= total;



        //convert the char array to a 
        //std::cout<<"[RH] converting message to structs"<<std::endl;
    
        //create a readReturn struct and decode the message
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
                //std::cout<<"[RH] the message is an update"<<std::endl;
                //initilize stuff
                Vector_ts<Robot*>::iterator it;
                robotUpdate* robotData = new robotUpdate[message->size];
                robotData = (robotUpdate*)(message->array);

                //get a readlock on the vector
                objects->readLock();
                robots->readLock();
                for(int i = 0; i < message->size; ++i){
                    for(it = robots->begin(); it < robots->end(); ++it){

                        //if it is the robot we are looking for, lock and update it
                        if ((*it)->getEndpoint() == connEP  && (*it)->getRID() == robotData[i].RID){

                            //set the robot
                            (*it)->lock();
                            (*it)->setXCord(robotData[i].x);
                            (*it)->setYCord(robotData[i].y);
                            (*it)->setDir(robotData[i].dir);
                            (*it)->setList(robotData[i].objects, robotData[i].qualities, robotData[i].listSize);

                            for (int j = 0; j < robotData[i].listSize; ++j) {
                                Vector_ts<Object*>::iterator oit;
                                for (oit = objects->begin(); oit < objects->end(); ++oit) {
                                    if ((*oit)->getOID() == robotData[i].objects[j]) {
                                        (*oit)->pos.x = robotData[i].xs[j];
                                        (*oit)->pos.y = robotData[i].ys[j];
                                        break;
                                    }
                                }
                            }

                            //std::cout << "[RH] got robot with listsize: " << robotData[i].listSize << "\n";

                            //update the video handeler
                            std::stringstream msg_ss;
                            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
                            msg_ss << now << "\n\n";
                            msg_ss << (*it)->getGlobalID() << ";" << (*it)->getVideoURL() << ";";

                            //free the robot
                            (*it)->unlock();

                            for (int j = 0; j < robotData[i].listSize; ++j) {
                                //std::cout << "[RH] j: " << j << " listsize:" << robotData[i].listSize << "\n";
                                msg_ss << robotData[i].objects[j] << ";" << robotData[i].qualities[j] << ";";
                            }
                            msg_ss << "\n";
                            //std::cout << "[RH] Calling write() on message \"" << msg_ss.str() << "\"\n";
                            vidHandler->write(msg_ss.str());
                            break;
                        } else {
                            //std::cout << "[RH] These are not the droids you're looking for\n";
                            //std::cout << "[RH]  " << (*it)->getEndpoint() << " looking for: "<<connEP << std::endl;
                            //std::cout << "[RH]  " << (*it)->getRID() << " looking for: " << robotData[i].RID << std::endl;
                            if ((*it)->getEndpoint() != connEP){
                                //std::cout << "[RH] WTF? Its from a different connection?(" << (*it)->getEndpoint() << " vs. " << connEP << ")\n";
                            }
                        }
                    }
                    
                }
                robots->readUnlock();
                objects->readUnlock();
                delete[] robotData;    

            }
            break;
            case P_OBJECT:
            //i don't think this should happen, but i don't know
            //actually it should whenever a new object is added
            //i'll have to deal with that
            {
                //initialize stuff
                object* objs = new object[message->size];
                objs = (object*)(message->array);

                Vector_ts<Object*>::iterator it;

                //loop over the structs in the message
                for(int i = 0; i < message->size; ++i){
                    bool exists = false;

                    //see if the object exists with the same id or name
                    for(it = objects->begin(); it != objects->end() && !exists; ++it){
                        (*it)->lock();
                        if(objs[i].OID == (*it)->getOID() || !objs[i].name->compare((*it)->getName()))
                            exists = true;
                        (*it)->unlock();
                    }

                    //if it doesen't exist we can add it to the system
                    if(!exists){

                        //create the new robot
                        Object* temp = new Object(objs[i].OID, *(objs[i].name), objs[i].color, objs[i].color_size, objs[i].box, objs[i].box_size);
                        objects->lock();
                        objects->push_back(temp);
                        objects->unlock();

                        //add it to the web database
                        db->insertObject(temp);

                        conn_map::iterator conn_iter;

                        //notify any other handlers that a new aobject has been added
                        for(conn_iter = connections.begin(); conn_iter != connections.end(); conn_iter++){
                            if((*conn_iter).first == connEP)
                                continue;
                            byteArray bytes;
                            write_data(P_OBJECT, &objs[i], 1, &bytes);
                            boost::asio::write((*conn_iter).second->socket(), 
                                boost::asio::buffer(bytes.array, bytes.size), boost::asio::transfer_at_least(bytes.size));
                            (*conn_iter).second->releaseSocket();
                        }
                    }
                }
            }

            break;

            case P_ROBOT_INIT:
            {
                //std::cout << "[RH] the message is a robotInit\n";
                //initiales stuff
                robotInit* robot = new robotInit[message->size];
                robot = (robotInit*)(message->array);
                
                //lock the robot vector for writing
                robots->lock();
               for(int i = 0; i < message->size; ++i){

                     //create the new robot object and add it to the vector
                    Robot* temp = new Robot(connEP, robot[i].RID);
                    temp->setXCord(robot[i].x);
                    temp->setYCord(robot[i].y);
                    temp->setCamera(robot[i].cameraType);
                    //temp->setList(robotData[i].list, robotData[i].listSize);
                    temp->setVideoURL(std::string(*robot[i].VideoURL));
                     
                    //check to make sure we don't get duplicates
                    Vector_ts<Robot*>::iterator robot_it;
                    bool isnew = true;
                    for(robot_it = robots->begin(); robot_it != robots->end(); ++robot_it){
                        if (*temp == *(*robot_it)){
                            //std::cout << "[RH] robot is a duplicate\n";
                           isnew = false;
                           break;
                        }
                    }

                    //if its not a duplicate add it, if it is delete it
                    if(isnew){
                        robots->push_back(temp);
                    }else{
                        delete temp;
                    }
                    //might want to clean up some stuff here if patrick doesn't fix destructor
                }
                robots->unlock();

                // let the web server know about the robots
                db->insertCameras(robots);
            }
            break;
            
            case P_ROBOT_RM:
            {
                robotRm* robot = new robotRm[message->size];
                robot = (robotRm*)(message->array);
                for( int i = 0; i < message->size; ++i){
                    Robot* temp = new Robot(connEP, robot[i].RID);
                    robots->lock();
                    Vector_ts<Robot*>::iterator robot_it;
                    for(robot_it = robots->begin(); robot_it != robots->end(); ++robot_it){
                        if(*(*robot_it) == *temp){
                            robots->erase(robot_it);
                            break;
                        }   
                    }
                    robots->unlock();
                }
            }

            default:
            //its broken if it gets here, need to figure out what to do.
            std::cerr<<"[RH] message was not of correct type\n[RH] type was: "<<message->type<<std::endl;
            break;
        }

        //clean up stuff from iteration
        delete message;
        
    
        //sleep abit so other threads can grab a lock on the socket
        boost::this_thread::sleep(boost::posix_time::seconds(2));
        
        connected = running;

    }
    //clean up stuff from connection
    cleanupConn(connEP);
    std::cout << "[RH] thread exiting\n";
}


void RobotHandler::cleanupConn(boost::asio::ip::tcp::endpoint connEP){
    std::cout << "[RH] thread "<<boost::this_thread::get_id() << " is cleaning up\n";
    //get a lock on the robot vector and declare and iterator
    robots->lock();
    Vector_ts<Robot*>::iterator it;

    //loop over the robot vector and remove the elements with the 
    //corresponding connection's endpoint
    //std::cout<<"[RH] want endpoint: "<<connEP<<std::endl;
    bool loop = true;
    while(loop){
        loop = false;
        for(it = robots->begin(); it < robots->end(); ++it){
            //std::cout<<"[RH] have endpoint: " << (*it)->getEndpoint() << std::endl;
            if ((*it)->getEndpoint() == connEP){
                
                //notify the video server its dying
                std::stringstream msg_ss;
                boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
                msg_ss << now << "\n\n";
                msg_ss << "DELETE " << (*it)->getGlobalID() << ";" << (*it)->getVideoURL() << "\n";
                vidHandler->write(msg_ss.str());
                
                // delete camera from db
                db->deleteCam((*it)->getGlobalID());

                //delet the robot and remove the pointer from the vector
                delete (*it);
                robots->erase(it);
                loop = true;
                break;
            }
        }
    }
    robots->unlock();
    //remove endpoint connection from map

    connections[connEP]->stop();
    
    conn_map::iterator iter;
    for(iter = connections.begin(); iter != connections.end(); ++iter){
        if((*iter).first == connEP){
            connections.erase(iter);
            break;
        }
        std::cerr<<"[RH] Robot Handler: couldn't find connection to delete\n";
    }


    thread_map::iterator iter2;
    for(iter2 = threads.begin(); iter2 != threads.end(); ++iter2){
        if((*iter2).first == connEP){
            threads.erase(iter2);
            break;
        }
        std::cerr<<"[RH] Robot Handler: couldn't find thread to delete\n";
    }

    //decrement handlers so we have an accurate count of how many connections we have
    handlerMutex.lock();
    --handlers;
    handlerMutex.unlock();
    std::cout << "[RH] " << boost::this_thread::get_id() << " cleanup done\n";
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
        if ((*mapIter).first != NULL && mapIter != assignments->end()) {
            while(((*mapIter).first->getEndpoint()) == ((*connIter).first)){
                current[numForConn].RID = (*mapIter).first->getRID();
                current[numForConn].OID = (*mapIter).second;

                Vector_ts<Object*>::iterator oit;
                for (oit = objects->begin(); oit < objects->end(); ++oit) {
                    if ((*oit)->getOID() == (*mapIter).second) {
                        current[numForConn].x = (*oit)->pos.x;
                        current[numForConn].y = (*oit)->pos.y;
                        break;
                    }
                }
                ++numForConn;

                mapIter++;
                if (mapIter == assignments->end())
                    break;
            }
        }

        //now that we have an array we will transmit the assignments for this connection
        byteArray data;
            
        if(write_data(P_ASSIGNMENT, (void*)current, numForConn, &data) < 0){
            //then somthing broke and it should be dealt with
        }
        boost::system::error_code error;
        connections[(*connIter).first]->writeLock();
        boost::asio::write(connections[(*connIter).first]->socket(), boost::asio::buffer(data.array, data.size),
            boost::asio::transfer_at_least(data.size), error);

        connections[(*connIter).first]->writeUnlock();

        //still don't know what errors i should look for on a write

        current += numForConn;


    }

    delete[] assigns;
    delete assignments;
}

void RobotHandler::sendCommand(command* comm) {
    conn_map::iterator it;
    for (it = connections.begin(); it != connections.end(); ++it) {
        sendCommand(comm, (*it).first);
    }
}
void RobotHandler::sendCommand(command* comm, boost::asio::ip::tcp::endpoint conn){    
    
    std::cout << "[RH] sending command :" << comm->cmd << " with arg: " << comm->arg << " to " << conn << std::endl;

    byteArray* data = new byteArray;
    boost::system::error_code error;

    //get the command in bytes
    write_data(P_COMMAND, comm, 1, data);
    
    //write the command to the socket
    connections[conn]->writeLock();
    boost::asio::write(connections[conn]->socket(), boost::asio::buffer(data->array, data->size),
        boost::asio::transfer_at_least(data->size), error);

    connections[conn]->writeUnlock();

}

void RobotHandler::shutdown(){
        running = false;
        
        int counter = 0;

        handlerMutex.lock();
        //std::cout << "[RH] got handlerMutex 1\n";

        //wait for ten seconds or until the handlers are gone
        while(handlers && counter < 10){
            handlerMutex.unlock();
            ++counter;
            //std::cout << "[RH] unlocked handlerMutex 2\n";
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            //std::cout << "[RH] still waiting for handlers to go away\n";
            handlerMutex.lock();
            //std::cout << "[RH] got handlerMutex 3\n";
        }

        //if there are still handlers after tenseconds 
        //it needs to close their socket connections to force them to end
        if(counter >= 10){
            handlerMutex.unlock();
            conn_map::iterator mapIter;
            for(mapIter = connections.begin(); mapIter != connections.end(); ++mapIter){
                (*mapIter).second->stop();
            }

            thread_map::iterator thread_iter;
            for(thread_iter = threads.begin(); thread_iter != threads.end(); ++thread_iter){
                (*thread_iter).second->interrupt();
            }
            
            counter = 0;
            handlerMutex.lock();

            //wait again to see if they close now
            while(handlers && counter < 10){
                //std::cout<<"[RH] the damn handlers aren't dead yet...\n";
                handlerMutex.unlock();
                ++counter;
                boost::this_thread::sleep(boost::posix_time::seconds(3));
                handlerMutex.lock();
            }
            
            //if there any handlers left, they had there chance
            if(handlers){
                
                std::cout<<"[RH] number of threads remaining: "<<threads.size()<<std::endl;
                std::cout<<"[RH] let them die then >:( \n";
            }

        }

        handlerMutex.unlock();
        //std::cout << "[RH] unlocked handlerMutex 4\n";

        //do any thing else;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
