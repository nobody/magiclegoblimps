/*
 * main.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include <iostream>

#include "controller.h"

int main(int argc, const char **argv) {
    std::cout << "In main()\n";

    controller c;
    c.testdb();

}


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
