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
    : io_(io_service), running(true)
{
    std::cout << "In controller constructor\n";
        

    // check if static variables are NULL and initialize them if necessary
    if (!robofile)
        robofile = new DataFile("robots.dat",  DataFile::ROBOT);
    if(!objfile)
        objfile  = new DataFile("objects.dat", DataFile::OBJECT);
    
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

    db = new DbManager(objs);

    admin = new AdminHandler;
    adminSrv = new TcpServer(io_service, 10000, admin);

    vids = new VideoHandler(robots, objs);
    vidsSrv = new TcpServer(io_service, 20000, vids);

    robo = new RobotHandler(robots, objs, vids);
    roboSrv = new TcpServer(io_service, 9999, robo);

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

    std::map<int, boost::rational<int> > *demand;
    db->getRequests(demand);
    db->normalize(demand);

    demand_t::iterator iter;
    for (iter = demand->begin(); iter != demand->end(); ++iter) {
        std::cout << "Demand for " << iter->first << ": " << iter->second << "\n";
    }

    robots->lock();
    objs->lock();
    Robot** r = new Robot*[robots->size()];
    Object** o = new Object*[objs->size()];
    double* dem = new double[objs->size()];

    for (int i = 0; i < robots->size(); ++i) {
        r[i] = robots->at(i);
        r[i]->lock();
    }

    for (int i = 0; i < objs->size(); ++i) {
        o[i] = objs->at(i);;
        //o[i]->lock();
        dem[i] = boost::rational_cast<double>((*demand->find(i)).second);
    }

    Qos q(r, robots->size(), o, objs->size(), dem);


    double qos = q.calcQos();

    std::cout << "qos = " << qos << "\n";




    for (int i = 0; i < robots->size(); ++i) {
        r[i]->unlock();
    }

    for (int i = 0; i < objs->size(); ++i) {
        //o[i]->unlock();
    }

    delete[] o;
    delete[] r;
    delete[] dem;


    delete demand;

    return 0;
}
void controller::controllerThread(){
    while(running){


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
