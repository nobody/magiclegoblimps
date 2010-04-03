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
        std::cout << "Worker thread calling io_.run()\n";
            io_.run();
}


int main(int argc, const char **argv) {
    std::cout << "In main()\n";

    boost::asio::io_service io;
    controller c(io);
    boost::thread worker(work, boost::ref(io));
    boost::this_thread::sleep(boost::posix_time::seconds(5));
    c.testdb();

    io.run();
    worker.join();

}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
