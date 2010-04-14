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

    admin = new AdminHandler(robo, robots, used_robots, objs);
    adminSrv = new TcpServer(io_service, 10000, admin);

    boost::thread qosThread(&controller::controllerThread, this);
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
}

int controller::writeObjects(Vector_ts<Object*>* objects) {
    if (objfile == NULL)
        return -1;

    int ret = objfile->write(objects);

    return ret;
}

int controller::testdb() {
    //db->printRequests();
    std::cout << "\n-----------------------------\n";

    std::map<int, boost::rational<int> > *demand;
    db->getRequests(demand);
    db->normalize(demand);

    demand_t::iterator iter;
    for (iter = demand->begin(); iter != demand->end(); ++iter) {
        std::cout << "Demand for " << iter->first << ": " << iter->second << "\n";
    }

    objs->readLock();
    robots->readLock();
    Robot** r = new Robot*[robots->size()];
    Object** o = new Object*[objs->size()];
    double* dem = new double[objs->size()];

    for (int i = 0; i < robots->size(); ++i) {
        r[i] = robots->at(i);
        r[i]->lock();
    }

    for (int i = 0; i < objs->size(); ++i) {
        o[i] = objs->at(i);;
        o[i]->lock();
        dem[i] = boost::rational_cast<double>((*demand->find(i)).second);
    }

    Qos q(r, robots->size(), o, objs->size(), dem);
    Assignment ass(r, robots->size(), o, objs->size(), dem, &q);

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


    delete demand;

    robots->readUnlock();
    objs->readUnlock();


    return 0;
}
void controller::controllerThread(){
    //std::map<int, boost::rational<int> > *demand = new std::map<int, boost::rational<int> >;
    std::map<int, boost::rational<int> > *demand = NULL;
    while(running){
        std::cout << "[controller] executing the main loop\n";

        db->getRequests(demand);
        db->normalize(demand);

        db->updateCameras(robots);

        boost::asio::deadline_timer timer(io_, boost::posix_time::seconds(C_QOS_INTV));
        timer.wait();
    }
}
void controller::shutdown(){
    adminSrv->shutdown();
    roboSrv->shutdown();
    vidsSrv->shutdown();
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
