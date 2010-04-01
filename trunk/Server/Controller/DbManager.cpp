/*
 * DbManager.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <string>
#include <iostream>
#include <sstream>


#include "DbManager.h"
#include "Robot.h"

const char* DbManager::db_uri = "tcp://localhost";
const char* DbManager::db_user = "testing";
const char* DbManager::db_pass = "testing";
const char* DbManager::db_database = "mydb";
const char* DbManager::tbl_requests = "requests";

DbManager::DbManager() {

    std::cout << "In database constructor\n";
    driver = sql::mysql::get_mysql_driver_instance();

}

DbManager::~DbManager() {
}

bool DbManager::normalize(demand_t*& d, demand_t*& old ) {
    // for now, let's just put the percentage of votes
    int total_demand = 0;
    demand_t::iterator it;
    for (it = d->begin(); it != d->end(); ++it) {
        total_demand += it->second.numerator() / it->second.denominator();
    }
    for (it = d->begin(); it != d->end(); ++it) {
        it->second = it->second / total_demand;
    }
    
    return true;
}

bool DbManager::getRequests( demand_t* m ) {
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *rs;
    std::string cmd;

    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cout << "Failed to connect to database\n";
        return false;
    }

    con->setAutoCommit(0);

    stmt = con->createStatement();
    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    cmd = "SELECT DISTINCT request_animal FROM ";
    cmd += DbManager::tbl_requests;

    try{
        rs = stmt->executeQuery(cmd);
    }catch(...){
        std::cout << "Failed query: " << cmd << "\n";
        
        delete stmt;
        con->rollback();
        delete con;

        return false;
    }

    while(rs->next()) {
        sql::ResultSet *rs_obj;

        int req_obj = rs->getInt("request_animal");

        cmd = "SELECT count(*) FROM ";
        cmd += DbManager::tbl_requests;
        cmd += " WHERE request_animal = ";
        cmd.append(1, (char)(req_obj + 0x30)); // convert int to ascii 

        std::cout << "Executing query: " << cmd << "\n";
        try{
             rs_obj = stmt->executeQuery(cmd);
        }catch(...){
            std::cout << "Failed query: " << cmd << "\n";
            
            delete rs;
            delete stmt;
            con->rollback();
            delete con;

            return false;
        }

        rs_obj->first();
        m->insert(std::pair<int, int>(req_obj, rs_obj->getInt(1)));
        delete rs_obj;
    }

    delete rs;
    delete stmt;
    con->commit();
    delete con;

    return true;
}

void DbManager::printRequests() {
    sql::Connection *con;

    sql::Statement *stmt;

    con = driver->connect(db_uri, db_user, db_pass);

    stmt = con->createStatement();
    std::string cmd("USE ");
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    cmd = "SELECT * FROM ";
    cmd += DbManager::tbl_requests;
    sql::ResultSet *rs = stmt->executeQuery(cmd);
    while(rs->next()) {
        std::cout << "Time: " << rs->getString("request_time")
                << "\tZone: " << rs->getString("request_zone")
                << "\tObject: " << rs->getString("request_animal") << "\n";
    }

    delete rs;
    delete stmt;
    delete con;
}



/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
