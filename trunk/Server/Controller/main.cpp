/*
 * main.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "controller.h"

void work(boost::asio::io_service &io_){              
        std::cout << "[main] Worker thread calling io_.run()\n";
        io_.run();
        std::cout << "[main] worker thread exiting\n";
}


int main(int argc, const char **argv) {
    std::cout << "In main()\n";

    boost::asio::io_service io;
    controller c(io);
    boost::thread worker(work, boost::ref(io));
    boost::this_thread::sleep(boost::posix_time::seconds(5));
//    c.testdb();

    io.run();
    c.qosThread->join();
    worker.join();

    std::cout << "[main] goodbye\n";
}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
