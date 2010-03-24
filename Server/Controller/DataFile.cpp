/*
 * DataFile.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "DataFile.h"

DataFile::DataFile(std::string filename, type t)
    : filename_(filename), t_(t)
{
}

DataFile::~DataFile()
{
}

// Returns -1 on failure and 1 on success
int DataFile::write(void* data)
{
    struct stat status; 
    if (stat("data/", &status)) {
        // data does not exist or is unreadable
        if (mkdir("data/", O_CREAT | 0755))
            return -1;
    }
    std::string fn("data/");
    fn.append(filename_);
    std::fstream file(fn.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
    switch (t_) {
        case ROBOT:
            {
                std::cout << "Writing ROBOT data\n";
                Vector_ts<Robot*>* robots = (Vector_ts<Robot*>*)data;
                
            }
            break;

        case OBJECT:
            std::cout << "Writing OBJECT data\n";
            break;
    }
    return -1;
}

void* DataFile::read()
{
    struct stat status; 
    if (stat("data/", &status)) {
        // data does not exist or is unreadable
        if (mkdir("data/", O_CREAT | 0755))
            return NULL;
    }
    std::string fn("data/");
    fn.append(filename_);
    std::fstream file(fn.c_str(), std::fstream::in | std::fstream::binary);
    switch (t_) {
        case ROBOT:
            std::cout << "Reading ROBOT data\n";
            if (file.good()){
                // read the data here.
                std::cout << "ROBOT Datafile good.\n";
            }
            break;

        case OBJECT:
            std::cout << "Reading OBJECT data\n";
            if (file.good()){
                // read the data here.
                std::cout << "OBJECT Datafile good.\n";
            }
            break;
    }

    return NULL;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
