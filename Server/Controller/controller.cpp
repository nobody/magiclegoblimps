/*
 * controller.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>

#include "controller.h"

DataFile* controller::robofile;
DataFile* controller::objfile;

controller::controller(boost::asio::io_service& io_service) 
    : io_(io_service)
{
    std::cout << "In controller constructor\n";

    // check if static variables are NULL and initialize them if necessary
    if (!robofile)
        robofile = new DataFile("robots.dat",  DataFile::ROBOT);
    if(!objfile)
        objfile  = new DataFile("objects.dat", DataFile::OBJECT);
    
    db = new DbManager();

    //robots = new Vector_ts<Robot*>();
    robots = (Vector_ts<Robot* >*) robofile->read();
    objs   = (Vector_ts<Object*>*)  objfile->read();
    //objs = new Vector_ts<Object*>();
    if (!robots) {
        std::cerr << "!!WARNING!! Robot data file unreadable. Creating new vector\n";
        robots = new Vector_ts<Robot*>();
    }
    if (!objs) {
        std::cerr << "!!WARNING!! Object data file unreadable. Creating new vector\n";
        objs = new Vector_ts<Object*>();
    }

    admin = new AdminHandler;
    adminSrv = new TcpServer(io_service, 10000, admin);

    robo = new RobotHandler(robots, objs);
    roboSrv = new TcpServer(io_service, 9999, robo);

    vids = new VideoHandler(robots, objs);
    vidsSrv = new TcpServer(io_service, 20000, vids);

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

    delete robofile;
    delete objfile;
}


int controller::writeRobots(Vector_ts<Robot*>* robots) {
    if (robofile == NULL)
        return -1;

    robots->lock();
    int ret = robofile->write(robots);
    robots->unlock();

    return ret;
}
int controller::writeObjects(Vector_ts<Object*>* objects) {
    if (objfile == NULL)
        return -1;

    objects->lock();
    int ret = objfile->write(objects);
    objects->unlock();

    return ret;
}

int controller::testdb() {
    db->printRequests();
    std::cout << "\n-----------------------------\n";

    std::map<int, boost::rational<int> > *demand = new demand_t();
    db->getRequests(demand);

    demand_t::iterator iter;
    for (iter = demand->begin(); iter != demand->end(); ++iter) {
        std::cout << "Demand for " << iter->first << ": " << iter->second << "\n";
    }

    delete demand;

    return 0;
}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
