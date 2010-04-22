/*
 * controller.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>

#include "controller.h"

DataFile* controller::objfile;

controller::controller(boost::asio::io_service& io_service) 
    : running(true), io_(io_service)
{
    std::cout << "In controller constructor\n";
        

    // check if static variables are NULL and initialize them if necessary
    if(!objfile)
        objfile  = new DataFile("objects.dat", DataFile::OBJECT);
    
    robots = new Vector_ts<Robot*>();
    objs   = (Vector_ts<Object*>*)  objfile->read();
    if (!objs) {
        std::cerr << "!!WARNING!! Object data file unreadable. Creating new vector\n";
        objs = new Vector_ts<Object*>();
    }

    //this wiil keep track of the robots commendered by admins
    used_robots = new Vector_ts<Robot*>();

    //create a new database object and clear out the tables
    db = new DbManager(objs);
    db->truncateCameras();

    // let's make sure that the database server knows about all our objects
    objs->readLock();
    if (objs->size() > 0) {
        for (Vector_ts<Object*>::iterator it = objs->begin(); it < objs->end(); ++it) {
            db->insertObject(*it);
        }
    }
    objs->readUnlock();

    //video handler and tcp server
    vids = new VideoHandler(robots, objs);
    vidsSrv = new TcpServer(io_service, 20000, vids);

    //new robot handler and tcp server
    robo = new RobotHandler(robots, objs, vids, db);
    roboSrv = new TcpServer(io_service, 9999, robo);

    //new adminhandler and tcp server
    admin = new AdminHandler(robo, robots, used_robots, objs, this);
    adminSrv = new TcpServer(io_service, 10000, admin);

    //launch the controller's worker thread
    qosThread = new boost::thread(&controller::controllerThread, this);
}

controller::~controller() {
    delete db;

    delete admin;
    delete adminSrv;

    delete robo;
    delete roboSrv;
    delete robots;

    delete vids;
    delete vidsSrv;

    delete objfile;

    delete objs;
    delete used_robots;

    delete qosThread;
}

int controller::writeObjects(Vector_ts<Object*>* objects) {
    if (objfile == NULL)
        return -1;

    int ret = objfile->write(objects);

    return ret;
}

int controller::doQOS(demand_t* demand) {

    //lock the vectors so they don't change size
    objs->readLock();
    robots->readLock();

    //create an array of robot pointers
    Robot** r;
    if (robots->size() - used_robots->size() > 0) {
        r = new Robot*[robots->size()];
    } else {
        r = NULL;
    }

    //create an array of object pointers
    Object** o;
    double* dem;
    if (objs->size() > 0) {
        o = new Object*[objs->size()];
        dem = new double[objs->size()];
    } else {
        o = NULL;
        dem = NULL;
    } 

    //add unused robots to the array
    for (int i = 0; i < robots->size(); ++i) {
        bool used = false;

        //make sure the robot[i] has not been commendeered by an admin
        for(int j = 0; j < used_robots->size(); ++j){
            if(robots->at(i) == robots->at(j)){
                used = true;
                break;
            }
        }

        //if it hasn't, add it to the list to give to the qos and lock it
        if(!used){
            r[i] = robots->at(i);
            r[i]->lock();
        }
    }

    //add all the objects to the array
    for (int i = 0; i < objs->size(); ++i) {
        o[i] = objs->at(i);;
        o[i]->lock();
        dem[i] =(*demand)[o[i]->getOID()];
    }

    std::map<Robot*, int>* assign = NULL;
    //set up the qos stuff and wait for it to comput
    if ( !r || !o || !dem ) {
        Qos q(r, robots->size(), o, objs->size(), dem);
        Assignment ass(r, robots->size(), o, objs->size(), dem, &q);
        
        std::cout <<"Start QoS" <<'\n';
        //Display the Qos at start
        q.calcQos();
        assign = ass.calcAssignments();
        q.calcQos();

        // send the assignemnts to the robots
        robo->sendAssignments(assign);
    } else {
        std::cerr << "[controller] skipping QOS this round\n";
    }


    // unlock of the robots
    /*
    for (int i = 0; i < robots->size(); ++i) {
        r[i]->unlock();
    }
    */
    for (int i = 0; i < robots->size(); ++i) {
        bool used = false;

        //make sure the robot[i] has not been commendeered by an admin
        for(int j = 0; j < used_robots->size(); ++j){
            if(robots->at(i) == robots->at(j)){
                used = true;
                break;
            }
        }

        //if it hasn't, unlock it
        if(!used){
            robots->at(i)->unlock();
        }
    }

    //unlock all of the objects
    for (int i = 0; i < objs->size(); ++i) {
        o[i]->unlock();
    }

    //delete all of the arrays
    if (o)
        delete[] o;
    if (r)
        delete[] r;
    if (dem)
        delete[] dem;

    //unlock the vectors
    robots->readUnlock();
    objs->readUnlock();


    return 0;
}
void controller::controllerThread(){
    std::map<int, double > *demand = NULL;
    int dbupdate = 0;
    while(running){
        std::cout << "[controller] executing the main loop\n";

        std::cout << "[controller] updating camera locations\n";
        db->updateCameras(robots);

        if (dbupdate == 0) {
            objfile->write(objs);

            db->getRequests(demand);
            db->normalize(demand);

            doQOS(demand);
        }
        dbupdate = (dbupdate + 1) % 2;

        boost::asio::deadline_timer timer(io_, boost::posix_time::seconds(C_QOS_INTV));
        timer.wait();
    }
    std::cout << "[controller] controllerThread exiting...\n";
}
void controller::shutdown(){
    running = false;
    adminSrv->shutdown();
    vidsSrv->shutdown();
    roboSrv->shutdown();
    std::cout << "[controller] completed shutdown() call\n";
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
