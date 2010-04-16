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

    used_robots = new Vector_ts<Robot*>();

    db = new DbManager(objs);
    db->truncateCameras();

    // let's make sure that the database server knows ablut all our objects
    objs->readLock();
    if (objs->size() > 0) {
        for (Vector_ts<Object*>::iterator it = objs->begin(); it < objs->end(); ++it) {
            db->insertObject(*it);
        }
    }
    objs->readUnlock();

    vids = new VideoHandler(robots, objs);
    vidsSrv = new TcpServer(io_service, 20000, vids);

    robo = new RobotHandler(robots, objs, vids, db);
    roboSrv = new TcpServer(io_service, 9999, robo);

    admin = new AdminHandler(robo, robots, used_robots, objs, this);
    adminSrv = new TcpServer(io_service, 10000, admin);

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

    objs->readLock();
    robots->readLock();
    Robot** r;
    if (robots->size() > 0) {
        r = new Robot*[robots->size()];
    } else {
        r = NULL;
    }

    Object** o;
    double* dem;
    if (objs->size() > 0) {
        o = new Object*[objs->size()];
        dem = new double[objs->size()];
    } else {
        o = NULL;
        dem = NULL;
    } 


    for (int i = 0; i < robots->size(); ++i) {
        r[i] = robots->at(i);
        r[i]->lock();
    }

    for (int i = 0; i < objs->size(); ++i) {
        o[i] = objs->at(i);;
        o[i]->lock();
        dem[i] =(*demand)[i];
    }

    Qos q(r, robots->size(), o, objs->size(), dem);
    Assignment ass(r, robots->size(), o, objs->size(), dem, &q);
    
    std::cout <<"Start QoS" <<'\n';
    //Display the Qos at start
    q.calcQos();
    ass.calcAssignments();
    q.calcQos();


    for (int i = 0; i < robots->size(); ++i) {
        r[i]->unlock();
    }

    for (int i = 0; i < objs->size(); ++i) {
        o[i]->unlock();
    }

    delete[] o;
    delete[] r;
    delete[] dem;


    robots->readUnlock();
    objs->readUnlock();


    return 0;
}
void controller::controllerThread(){
    std::map<int, double > *demand = NULL;
    while(running){
        std::cout << "[controller] executing the main loop\n";

        db->getRequests(demand);
        db->normalize(demand);

        db->updateCameras(robots);

        doQOS(demand);

        boost::asio::deadline_timer timer(io_, boost::posix_time::seconds(C_QOS_INTV));
        timer.wait();
    }
    std::cout << "[controller] controllerThread exiting...\n";
}
void controller::shutdown(){
    adminSrv->shutdown();
    roboSrv->shutdown();
    vidsSrv->shutdown();
    std::cout << "[controller] completed shutdown() call\n";
    running = false;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
