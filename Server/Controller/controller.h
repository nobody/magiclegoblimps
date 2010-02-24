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
#include "AdminSocket.h"
#include "RobotController.h"
#include "TcpServer.h"


class controller {
    public:
        controller(boost::asio::io_service&);
        virtual ~controller();
        int testdb();

    private:
        boost::asio::io_service& io_;
        DbManager *db;
        AdminSocket *admin;
        TcpServer *admin2;
        RobotController *robotCon;

        demand_t demand_;

};


#endif /* CONTROLLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
