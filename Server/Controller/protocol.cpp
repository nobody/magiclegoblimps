/*
 * protocol.cpp 
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */
#include"protocol.h"

 void write_data(int type, void* data_, short number, byteArray* byte_ptr){
    char* array;
    unsigned short* sizes = new unsigned short[number];
    int overall_size = 0;
    char** structs = new char*[number];
    switch(type){

        case P_ROBOT_INIT:
            {
            robotInit* data = (robotInit*)data_;
            for(int i = 0; i < number; ++i){
                short str_len = data[i].VideoURL->size() + 1;
                const char* url = data[i].VideoURL->c_str();
                short size = sizeof(int)*3 + str_len + sizeof(short)*2;
                structs[i] = new char[size];

                //put the size on the top of the array
                char* ref = (char*)&size;
                for(int j = 0; j < 2; ++j){
                    structs[i][j] = ref[j];
                }

                //push rid on the array
                ref = (char*)&(data[i].RID);
                for(int j = 2; j < 6; ++j){
                    structs[i][j] = ref[j-2];
                }

                //push x
                ref = (char*)&(data[i].x);
                for(int j = 6; j <10; ++j){
                    structs[i][j] = ref[j-6];
                }

                //push y
                ref = (char*)&(data[i].y);
                for(int j = 10; j < 14; ++j){
                    structs[i][j] = ref[j-10];
                }

                //push the string size
                ref = (char*)&str_len;
                for(int j = 14; j < 16; ++j){
                    structs[i][j] = ref[j-14];
                }

                //push the string
                for(int j = 16; j < 16+str_len; ++j){
                    structs[i][j] = url[j - 16];
                }

                //add the length of this array to the total length and store it
                sizes[i] = size;
                overall_size += size;
            }
            break;
            }

        case P_ROBOT_UPDATE:
            {
            robotUpdate* data = (robotUpdate*)data_;
            for (int i = 0; i < number; ++i){
                short size = sizeof(int) * 3 + sizeof(short);
                structs[i] = new char[size];

                //push size ontop of the array
                char* ref = (char*)&size;
                for(int j = 0; j < 2; ++j){
                    structs[i][j] = ref[j];
                }
                
                //push rid 
                ref = (char*)&(data[i].RID);
                for(int j = 2; j < 6; ++j){
                    structs[i][j] = ref[j-2];
                }

                //push x
                ref = (char*)&(data[i].x);
                for(int j = 6; j <10; ++j){
                    structs[i][j] = ref[j-6];
                }

                //push y
                ref = (char*)&(data[i].y);
                for(int j = 10; j < 14; ++j){
                    structs[i][j] = ref[j-10];
                }
                sizes[i] = size;
                overall_size += size;
            }
        }

    }

    //intiallize the array to return
    array = new char[overall_size + 3 + 4];

    //set type
    array[0] = type;

    //push length in bytes
    int length = overall_size+ 3 + 4;
    char* ref = (char*)&length;
    for(int i = 1; i < 5; ++i){
        array[i] = ref[i-1];
    }

    //push number of elements;
    ref = (char*)&number;
    for(int i = 5; i < 7; ++i){
        array[i] = ref[i-5];
    }
    
    //push each array onto the array
    int position = 7;
    for(int i = 0; i < number; ++i){
             memcpy(&array[position], structs[i], sizes[i]);
             position += sizes[i];
    }
    //clean up
    delete[] sizes;
    for(int i = 0; i < number; ++i){
        delete[] structs[i];
    }
    delete[] structs;
    byte_ptr->array = array;
    byte_ptr->size = overall_size + 3;

}

int readRobotInit(void* array, robotInit* &robots) {
    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    robots = new robotInit[overall_size];

    for (int i = 0; i < overall_size; ++i) {
        short size;
        int rid;
        int x;
        int y;
        short str_len;
        char* url;

        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve x
        ref = (char*)&x;
        for (int j = 0; j < sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve y
        ref = (char*)&y;
        for (int j = 0; j < sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve str_len
        ref = (char*)&str_len;
        for (int j = 0; j < sizeof(short); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve url
        url = new char[str_len];
        for (int j = 0; j < str_len; ++j) {
            url[j] = current[0]; current++;
        }

        // build the robotInit struct
        robots[i].RID = rid;
        robots[i].x = x;
        robots[i].y = y;
        robots[i].VideoURL = new std::string(url);

        delete[] url;
    }
    return overall_size;
}

// takes a void pointer to the data as output from write_data and an 
int read_data(void* array, readReturn* ret){

    switch( (char) ((char*)array)[0] ) {
        case P_ROBOT_INIT:
            {
                robotInit* arr;
                int count = readRobotInit(array, arr);

                // Create a readReturn if we have a NULL pointer
                if (!ret)
                    ret = new readReturn;
                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_ROBOT_INIT;

                return count;
            }

        case P_ROBOT_UPDATE:

            break;

        case P_OBJECT:

            break;

        default:

            std::cerr << "Attempt to read unknown type\n";
    }
    return -1;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
