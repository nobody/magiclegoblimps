/*
 * config.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef DBMANAGER_H_
#define DBMANAGER_H_

#include <map>

#include <boost/rational.hpp>

// MySQL Connector C++
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>
#include <cppconn/metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
// End MySQL Connector

#include "Robot.h"


enum result_t { SUCCESS, FAILURE };
typedef std::map<int, boost::rational<int> > demand_t;


class DbManager {
    public:
        DbManager();
        virtual ~DbManager();

        void printRequests();

        bool getRequests( demand_t* );
        bool updateCameras( std::map<int, Robot>* );


    private:
        static const char* db_uri;
        static const char* db_user;
        static const char* db_pass;
        static const char* db_database;
        static const char* tbl_requests;

        sql::mysql::MySQL_Driver *driver;

};


#endif /* DBMANAGER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
