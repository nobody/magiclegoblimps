/*
 * DataFile.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef DATAFILE_H_
#define DATAFILE_H_

#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

#include "Vector_ts.h"
#include "Robot.h"
#include "Object.h"
#include "protocol.h"


class DataFile {
    public:
        enum type { ROBOT, OBJECT };

        DataFile(std::string filename, type t);
        ~DataFile();

        int write(void* data);
        void* read();

    private:
        std::string filename_;
        type t_;


};

#endif /* DATAFILE_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
