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

//const char* DbManager::db_uri = "tcp://0-22-19-13-ab-d8.dyn.utdallas.edu";
//const char* DbManager::db_user = "servergroup";
//const char* DbManager::db_pass = "agrajag";
//const char* DbManager::db_database = "zoocam";
const char* DbManager::db_uri = "tcp://localhost";
const char* DbManager::db_user = "testing";
const char* DbManager::db_pass = "testing";
const char* DbManager::db_database = "mydb";

const char* DbManager::tbl_requests = "requests";
const char* DbManager::tbl_cameras = "cameras";
const char* DbManager::tbl_objects = "animals";

const char* DbManager::col_object_id = "animal_id";

DbManager::DbManager(Vector_ts<Object*>* objs) 
    : objs_(objs)
{

    driver = sql::mysql::get_mysql_driver_instance();
    old = NULL;

}

DbManager::~DbManager() {
}

bool DbManager::normalize(demand_t*& d ) {
    demand_t::iterator it;
    double total_demand;
    
    objs_->readLock();
    std::cout << "[db] objs_->size():" << objs_->size() << "\n";
    for (Vector_ts<Object*>::iterator ito = objs_->begin(); ito < objs_->end(); ++ito) {
        (*ito)->lock();
        int OID = (*ito)->getOID();
        (*ito)->unlock();

        // make sure we have entries for all objects
        if (old)
            (*old)[OID];
        (*d)[OID];
    }
    objs_->readUnlock();

    /*
    if (old) {
        for (it = old->begin(); it != old->end(); ++it) {
            it->second /= 4;
            (*d)[it->first];
        }
    }
    */
    for (it = d->begin(); it != d->end(); ++it) {
        //if (old && (*old)[it->first] > 0.00000001)
        //    it->second += (*old)[it->first];
        total_demand += it->second;
    }

    for (it = d->begin(); it != d->end(); ++it) {
        it->second = it->second / total_demand;
        std::cout << "[db] noramlized demand for " << it->first << ":" << it->second << "\n"; 
    }

    double sum = 0;
    for (it = d->begin(); it != d->end(); ++it) {
        sum += it->second;
    }
    std::cout << "[db] demand sum: " << sum << "\n"; 

    return true;
}

bool DbManager::getRequests( demand_t*& m ) {
    if (old != NULL) {
        delete old;
        old = NULL;
    }
    if (m != NULL) {
        old = m;
    }

    m = new demand_t;
    
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *rs;
    std::string cmd;

    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
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
        //std::cout << "[db] Failed query: " << cmd << "\n";
        
        delete stmt;
        con->rollback();
        delete con;

        return false;
    }

    while(rs->next()) {
        sql::ResultSet *rs_obj;

        int req_obj = rs->getInt("request_animal");

        std::stringstream ss;
        ss << "SELECT count(*) FROM "
           << DbManager::tbl_requests
           << " WHERE request_animal = '"
           << req_obj << "'";
        cmd = ss.str();

        try{
             rs_obj = stmt->executeQuery(cmd);
        }catch(...){
            //std::cout << "[db] Failed query: " << cmd << "\n";
            
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

    try{                    
        cmd = "TRUNCATE ";
        cmd += DbManager::tbl_requests;
        stmt->execute(cmd);
    } catch(...){
        delete rs;
        delete stmt;
        delete con;

        return false;
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

bool DbManager::truncateCameras() {
    sql::Connection *con;
    sql::Statement *stmt;
    std::string cmd;

    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
        return false;
    }

    stmt = con->createStatement();

    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    try{                    
        cmd = "TRUNCATE service";
        stmt->execute(cmd);
    } catch(...){
        delete stmt;
        delete con;

        return false;
    }

    try{                    
        cmd = "TRUNCATE ";
        cmd += DbManager::tbl_cameras;
        stmt->execute(cmd);
    } catch(...){
        delete stmt;
        delete con;

        return false;
    }

    delete stmt;
    delete con;

    return true;
}

bool DbManager::insertCameras( Vector_ts<Robot*>* robots) {
    if (!robots)
        return false;

    sql::Connection *con;
    sql::Statement *stmt;
    std::string cmd;
    
    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
        return false;
    }

    con->setAutoCommit(0);
    stmt = con->createStatement();

    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    bool ret = true;
    robots->readLock();
    if (robots->size() > 0) {
        Vector_ts<Robot*>::iterator it;
        Vector_ts<Robot*>::iterator it_end = robots->end();

        for (it = robots->begin(); it < it_end; ++it) {
            (*it)->lock();
            std::stringstream ss;
            ss << "INSERT INTO " << tbl_cameras 
               << " (`camera_id`, `camera_type`, `camera_curr_x`, `camera_curr_y`, `camera_last_x`, `camera_last_y`, `camera_resolution`, `camera_fps`) VALUES ('";
            ss << (*it)->getGlobalID()
               << "', '";
            if ((*it)->getCamera() < 3 && (*it)->getCamera() >= 0) {
                ss << (*it)->getCamera();
            } else {
                ss << "2";
            }
            ss << "', '"
               << ( (*it)->getXCord() < 0 ? 0 : (*it)->getXCord() )
               << "', '"
               << ( (*it)->getYCord() < 0 ? 0 : (*it)->getYCord() )
               << "', '0', '0', '";
            if ((*it)->getCamera() < 3 && (*it)->getCamera() >= 0) {
                ss << Robot::camdata[(*it)->getCamera()][0]
                   << " "
                   << Robot::camdata[(*it)->getCamera()][1];
            } else {
                ss << "0 0";
            }
            ss << "', '";
            if ((*it)->getCamera() < 3 && (*it)->getCamera() >= 0) {
                ss << Robot::camdata[(*it)->getCamera()][2];
            } else {
                ss << "NULL";
            }
            ss << "')";
            cmd = ss.str();
            //std::cout << "[db] generated query \"" << cmd << "\"\n";

            try {
                stmt->execute(cmd);
            } catch(...){
                //std::cout << "[db] Failed query: " << cmd << "\n";
                
                (*it)->unlock();
                ret = false;
                continue;
            }
            //std::cout << "[db] Succeeded query: " << cmd << "\n";
            (*it)->unlock();
        }
    }
    robots->readUnlock();
    con->commit();

    delete stmt;
    delete con;

    return ret;
}
bool DbManager::updateCameras( Vector_ts<Robot*>* robots) {
    if (!robots)
        return false;

    sql::Connection *con;
    sql::Statement *stmt;
    std::string cmd;
    
    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
        return false;
    }

    con->setAutoCommit(0);
    stmt = con->createStatement();

    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    bool ret = true;
    robots->readLock();
    if (robots->size() > 0) {
        Vector_ts<Robot*>::iterator it;
        Vector_ts<Robot*>::iterator it_end = robots->end();

        for (it = robots->begin(); it < it_end; ++it) {
            (*it)->lock();
            std::stringstream ss;
            std::stringstream clear_ss;

            clear_ss << "CALL delServiceRecord('" << (*it)->getGlobalID() << "')";
            cmd = clear_ss.str();
            try {
                stmt->execute(cmd);
            } catch (...) {
                //std::cout << "[db] Failed query: " << cmd << "\n";
                (*it)->unlock();
                ret = false;
                continue;
            }
            //std::cout << "[db] Succeeded query: " << cmd << "\n";

            ss << "CALL setCameraPosition('";
            ss << (*it)->getGlobalID()
               << "', '"
               << (*it)->getXCord()
               << "', '"
               << (*it)->getYCord()
               << "', ";
            float max_qual = -1;
            int max_qual_id = -1;
            std::map<int, float>* list = (*it)->list;
            std::map<int, float>::iterator ito = list->begin();
            for (; ito != list->end(); ++ito) {
                if (max_qual < ito->second) {
                    max_qual_id = ito->first;
                    max_qual = ito->second;
                }
                std::stringstream visibility_ss;
                visibility_ss << "CALL addServiceRecord('" << (*it)->getGlobalID() << "', '" << ito->first 
                    << "', '" << ito->second << "')";
                cmd = visibility_ss.str();
                try {
                    stmt->execute(cmd);
                } catch (...) {
                    //std::cout << "[db] Failed query: " << cmd << "\n";
                    continue;
                }
                //std::cout << "[db] Succeeded query: " << cmd << "\n";
            }
            if (max_qual_id > -1)
                ss << "'" << max_qual_id << "'";
            else 
                ss << "NULL";

            ss << ", '"
               << (*it)->getDir()
               << "')";
            cmd = ss.str();

            try {
                stmt->execute(cmd);
            } catch(...){
                std::cout << "[db] Failed query: " << cmd << "\n";
                
                (*it)->unlock();
                ret = false;
                continue;
            }
            std::cout << "[db] Succeeded query: " << cmd << "\n";
            (*it)->unlock();
        }
    }
    robots->readUnlock();
    con->commit();

    delete stmt;
    delete con;

    return ret;
}

bool DbManager::deleteCam( int id ) {
    sql::Connection *con;
    sql::Statement *stmt;
    std::string cmd;
    
    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
        return false;
    }

    con->setAutoCommit(0);
    stmt = con->createStatement();

    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    bool ret = true;
    try{
        std::stringstream ss;
        ss << "DELETE FROM service WHERE `service_camera_id`='" << id << "'";
        cmd = ss.str();
        stmt->execute(cmd);
    } catch(...){
        std::cout << "[db] Failed query: " << cmd << "\n";
        ret = false;
    }
    try{
        std::stringstream ss;
        ss << "DELETE FROM cameras WHERE `camera_id`='" << id << "'";
        cmd = ss.str();
        stmt->execute(cmd);
    } catch(...){
        std::cout << "[db] Failed query: " << cmd << "\n";

        ret = false;
    }
    delete stmt;
    delete con;
    return ret;
}

bool DbManager::insertObject( Object* obj ) {
    if (!obj)
        return false;

    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *rs;
    std::string cmd;

    try {
        con = driver->connect(db_uri, db_user, db_pass);
    } catch (...) {
        std::cerr << "[db] Failed to connect to database\n";
        return false;
    }

    con->setAutoCommit(0);
    stmt = con->createStatement();

    cmd = "USE ";
    cmd += DbManager::db_database;
    stmt->execute(cmd);

    // lock the object
    obj->lock();

    std::stringstream ss;
    ss << "SELECT * FROM "<< tbl_objects << " WHERE " << col_object_id << "='" << obj->getOID() << "'";
    cmd = ss.str();

    try{
        rs = stmt->executeQuery(cmd);
    }catch(...){
        //std::cout << "[db] Failed query: " << cmd << "\n";
        
        delete stmt;
        con->rollback();
        delete con;

        obj->unlock();
        return false;
    }

    if (rs->rowsCount() == 0) {
        // no such object, we should add it
        std::stringstream ins_ss;
        ins_ss << "INSERT INTO " << tbl_objects 
               << " (`animal_id`, `animal_name`, `animal_num_views`,"
               << " `animal_curr_x`, `animal_curr_y`, `animal_last_x`, `animal_last_y`) VALUES ('"
               << obj->getOID()
               << "', '"
               << obj->getName()
               << "', '0', '"
               << "0', '0', '0', '0')";
        cmd = ins_ss.str();

        try{
            stmt->execute(cmd);
        }catch(...){
            std::cout << "[db] Failed query: " << cmd << "\n";
            
            delete rs;
            delete stmt;
            con->rollback();
            delete con;

            obj->unlock();
            return false;
        }
        std::cout << "[db] Succeeded query: " << cmd << "\n";

    }

    con->commit();

    obj->unlock();


    return true;

}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
