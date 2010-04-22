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
#include <stdlib.h>

int main(int argc, char** argv) {
    //const char* db_uri = "tcp://0-22-19-13-ab-d8.dyn.utdallas.edu";
    //const char* db_user = "servergroup";
    //const char* db_pass = "agrajag";
    //const char* db_database = "zoocam";
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

    int total = 1000;
    int* arr = new int[argc + 1];

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

    int sum = 0;
    int idx = 1;
    while(rs->next()) {
        int req_obj = rs->getInt("animal_id");
        int count = 100;
        if (argc > idx) {
            count = atoi(argv[idx++]);
        }
        sum += count;
        //std::cout << "rnd() returned: " << count << " for Object:" << req_obj << "\n";
        std::cout << count <<  "\n";

        if (count > 0) {
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
    }

    std::cout << "Simulated " << sum << " users\n";

    delete rs;
    delete stmt;
    delete con;

    return 0;
    return true;
}
