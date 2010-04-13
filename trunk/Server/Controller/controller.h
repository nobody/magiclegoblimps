/*
 * controller.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define C_QOS_INTV 30

#include <map>

#include <boost/asio.hpp>
#include <boost/rational.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "DbManager.h"
#include "AdminHandler.h"
#include "RobotHandler.h"
#include "VideoHandler.h"
#include "TcpServer.h"
#include "Vector_ts.h"
#include "Robot.h"
#include "Object.h"
#include "DataFile.h"
#include "Qos.h"
#include "Assignment.h"

class controller {
    public:
        controller(boost::asio::io_service&);
        virtual ~controller();
        int testdb();
        void shutdown();

        static int writeRobots(Vector_ts<Robot*>*);
        static int writeObjects(Vector_ts<Object*>*);

    private:
        
        void controllerThread();
        
        bool running;
        boost::asio::io_service& io_;
        DbManager *db;
        AdminHandler *admin;
        RobotHandler *robo;
        VideoHandler *vids;
        TcpServer *adminSrv;
        TcpServer *roboSrv;
        TcpServer *vidsSrv;

        Vector_ts<Robot*>* robots;
        Vector_ts<Object*>* objs;
        Vector_ts<Robot*>* used_robots;

        static DataFile *objfile;

        demand_t demand;

};


#endif /* CONTROLLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
