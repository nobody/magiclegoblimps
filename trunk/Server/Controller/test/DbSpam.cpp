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
 
#include <boost/nondet_random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/variate_generator.hpp>

#include <sstream>

int main() {
    const char* db_uri = "tcp://localhost";
    const char* db_user = "testing";
    const char* db_pass = "testing";
    const char* db_database = "mydb";
    const char* tbl_requests = "requests";
    const char* tbl_objects = "animals";

#ifdef __linux__
    boost::random_device dev;
    //timing<unsigned int>(dev, iter, "random_device");
#else
#error The non-deterministic random device is currently available on Linux only.
#endif
    boost::uniform_int<> range(1, 100);
    boost::variate_generator<boost::random_device&, boost::uniform_int<> > rnd(dev, range);


    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
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

    stmt = con->createStatement();
    cmd = "USE ";
    cmd += db_database;
    stmt->execute(cmd);

    cmd = "SELECT DISTINCT animal_id FROM ";
    cmd += tbl_objects;

    try{
        rs = stmt->executeQuery(cmd);
    }catch(...){
        std::cout << "Failed query: " << cmd << "\n";
        
        delete stmt;
        delete con;

        return false;
    }

    /*
     // this will be taken care of in the controller
    try{
        cmd = "TRUNCATE ";
        cmd += tbl_requests;
        stmt->execute(cmd);
    } catch(...){
        delete rs;
        delete stmt;
        delete con;

        return false;
    }
    */

    int sum = 0;
    while(rs->next()) {
        int req_obj = rs->getInt("animal_id");
        int count = rnd();
        sum += count;
        std::cout << "rnd() returned: " << count << " for Object:" << req_obj << "\n";

        std::stringstream ss;
        ss << "INSERT INTO "
           << tbl_requests
           << " (`request_ip`, `request_time`, `request_zone`, `request_animal`) ";
        ss << "SELECT "
           << "'1.1.1.1', CURRENT_TIMESTAMP, '1', '"
           << req_obj
           << "' ";
        while (--count > 0) {
            ss << "UNION ALL SELECT "
               << "'1.1.1.1', CURRENT_TIMESTAMP, '1', '"
               << req_obj
               << "' ";
        }

        cmd = ss.str();

        //std::cout << "Executing query: " << cmd << "\n";
        try{
             stmt->execute(cmd);
        }catch(...){
            std::cout << "Failed query: " << cmd << "\n";
            
            delete rs;
            delete stmt;
            delete con;

            sum -= count;
            break;
        }
    }

    std::cout << "Simulated " << sum << " users\n";

    delete rs;
    delete stmt;
    delete con;

    return 0;
    return true;
}
