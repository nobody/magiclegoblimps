/*
 * AdminHandler.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */


#include "AdminHandler.h"
#include "controller.h"

//AdminHandler::conn_map AdminHandler::connections;

AdminHandler::AdminHandler(){
}

AdminHandler::AdminHandler(RobotHandler* robotControl_, Vector_ts<Robot*>* robots_, Vector_ts<Robot*>* inUse_, Vector_ts<Object*>* objects_, controller* cont_)
    : robotControl(robotControl_), robots(robots_), inUse(inUse_), objects(objects_), cont(cont_)
{
}

AdminHandler::~AdminHandler(){
}

void AdminHandler::onConnect(TcpServer::TcpConnection::pointer tcp_connection) {
    boost::asio::ip::tcp::endpoint endpoint = tcp_connection->socket().remote_endpoint();
    
    tcp_connection->releaseSocket();

    //connections[endpoint] = tcp_connection;
    boost::thread connThread(&AdminHandler::threaded_on_connect, this, tcp_connection);
}

void AdminHandler::threaded_on_connect(TcpServer::TcpConnection::pointer tcp_connection){


    //TcpServer::TcpConnection::pointer tcp_connection(connections[conn]);
    //tcp::socket &sock = tcp_connection->socket();

    boost::shared_ptr<session> sess(new session(tcp_connection, robots, inUse,  robotControl, objects, cont));
    sessions.push_back(sess);
    sess->start();


    std::cout << "[AH] thread exiting\n";
}

AdminHandler::session::session(TcpServer::TcpConnection::pointer tcp_, Vector_ts<Robot*>* robots_, Vector_ts<Robot*>* inUse_,RobotHandler* robotHandler, Vector_ts<Object*>* objects_, controller* cont_) 
    : conn_(tcp_), robots(robots_), inUse(inUse_), objects(objects_), robotControl(robotHandler), cont(cont_)
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
    } else if (error == boost::asio::error::operation_aborted) {
        conn_->releaseSocket();
        close();
        return;
    } else if (error) {
        conn_->releaseSocket();
        close();
        throw boost::system::system_error(error);
    }

    std::string s(read_message_.data(bytes_transferred));
    read_message_.consume(bytes_transferred);

    std::cout << "Got string \"" << s << "\"\n";
    std::cout.flush();

    if(!s.compare(std::string("shutdown\n"))){
        // we need to shutdown the system.
        conn_->releaseSocket();
        cont->shutdown();
        
        return;
    }

    command cmd;
    Robot* subject;

    //check to make sure that the input is what we want
    if(s.find('$', 0) != std::string::npos){
        
        //parse the command id
        std::string token = s.substr(0, s.find('$', 0));
        s =  s.substr( s.find('$', 0), std::string::npos);

        robots->readLock();
        int switchvar = atoi(token.c_str());
        bool isUsed = false;
        //switch on the command
        switch(switchvar){
            case P_CMD_FWD:
            {
                
                cmd.cmd = P_CMD_FWD;
                
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int robotid = atoi(token.c_str());

                Vector_ts<Robot*>::iterator it;
                Vector_ts<Robot*>::iterator used_it;

                for(it = robots->begin(); it != robots->end(); ++it){
                    (*it)->lock();
                    if((*it)->getGlobalID() == robotid){
                        subject = (*it);
                        break;
                    }
                    (*it)->unlock();
                }


                inUse->lock();
                    
                for(used_it = inUse->begin(); used_it != inUse->end(); ++used_it){
                    if((*used_it) == subject)
                         isUsed = true;
                }


                if(!isUsed)
                    inUse->push_back(subject);
                inUse->unlock();

                cmd.RID = subject->getRID();
                robotControl->sendCommand(&cmd, subject->getEndpoint());
                (*it)->unlock();
            }
            break;

            case P_CMD_LFT:
            {
                cmd.cmd = P_CMD_LFT;
                
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int robotid = atoi(token.c_str());

                Vector_ts<Robot*>::iterator it;
                Vector_ts<Robot*>::iterator used_it;

                for(it = robots->begin(); it != robots->end(); ++it){
                    (*it)->lock();
                    if((*it)->getGlobalID() == robotid){
                        subject = (*it);
                        break;
                    }
                    (*it)->unlock();
                }


                inUse->lock();
                    
                for(used_it = inUse->begin(); used_it != inUse->end(); ++used_it){
                    if((*used_it) == subject)
                         isUsed = true;
                }


                if(!isUsed)
                    inUse->push_back(subject);
                inUse->unlock();
                
                cmd.RID = subject->getRID();
                robotControl->sendCommand(&cmd, subject->getEndpoint());
                (*it)->unlock();
            }
            break;

            case P_CMD_RGHT:
            {
                cmd.cmd = P_CMD_LFT;
                
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int robotid = atoi(token.c_str());

                Vector_ts<Robot*>::iterator it;
                Vector_ts<Robot*>::iterator used_it;


                for(it = robots->begin(); it != robots->end(); ++it){
                    (*it)->lock();
                    if((*it)->getGlobalID() == robotid){
                        subject = (*it);
                        break;
                    }
                    (*it)->unlock();
                }

                inUse->lock();
                    
                for(used_it = inUse->begin(); used_it != inUse->end(); ++used_it){
                    if((*used_it) == subject)
                         isUsed = true;
                }


                if(!isUsed)
                    inUse->push_back(subject);
                inUse->unlock();
                
                cmd.RID = subject->getRID();
                robotControl->sendCommand(&cmd, subject->getEndpoint());
                (*it)->unlock();
            }
            break;

            case P_CMD_WEST:

            break;
            case P_CMD_MVTO:

            break;
            case P_CMD_CAMROT:
            {
                cmd.cmd = P_CMD_LFT;
                
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int robotid = atoi(token.c_str());

                Vector_ts<Robot*>::iterator it;
                Vector_ts<Robot*>::iterator used_it;

                for(it = robots->begin(); it != robots->end(); ++it){
                    (*it)->lock();
                    if((*it)->getGlobalID() == robotid){
                        subject = (*it);
                        break;
                    }
                    (*it)->unlock();
                }
                

                inUse->lock();
                    
                for(used_it = inUse->begin(); used_it != inUse->end(); ++used_it){
                    if((*used_it) == subject)
                         isUsed = true;
                }


                if(!isUsed)
                    inUse->push_back(subject);
                inUse->unlock();


                cmd.RID = subject->getRID();

                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                cmd.arg = atoi(token.c_str());


                robotControl->sendCommand(&cmd, subject->getEndpoint());
                (*it)->unlock();
            }
            break;
            case P_CMD_RLS_RBT:
            {
            
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int robotid = atoi(token.c_str());

                Vector_ts<Robot*>::iterator it;

                inUse->lock();
                for(it = inUse->begin(); it != inUse->end(); ++it){
                    if((*it)->getGlobalID() == robotid){    
                        inUse->erase(it);
                        break;
                    }
                }
                inUse->unlock();

            }
            break;

            case P_CMD_DEL_OBJ:
            {
                token = s.substr(0, s.find('$', 0));
                s = s.substr(s.find('$', 0), std::string::npos);

                int objID = atoi(token.c_str());

                Vector_ts<Object*>::iterator it;

                objects->lock();

                for(it = objects->begin(); it != objects->end(); ++it){
                    if((*it)->getOID() == objID){
                        objects->erase(it);
                        break;
                    }
                }

                objects->unlock();

            }
            break;
            
            default:
            break;
        }

        robots->readUnlock();
    }
    
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

void AdminHandler::shutdown(){}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
