/*
 * controller.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <map>

#include <boost/asio.hpp>
#include <boost/rational.hpp>

#include "DbManager.h"
#include "AdminHandler.h"
#include "RobotHandler.h"
#include "TcpServer.h"
#include "Vector_ts.h"
#include "Robot.h"

class controller {
    public:
        controller(boost::asio::io_service&);
        virtual ~controller();
        int testdb();

    private:
        boost::asio::io_service& io_;
        DbManager *db;
        AdminHandler *admin;
        RobotHandler *robo;
        TcpServer *adminSrv;
        TcpServer *roboSrv;

        Vector_ts<Robot>* robots;

        demand_t demand;

};


#endif /* CONTROLLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
