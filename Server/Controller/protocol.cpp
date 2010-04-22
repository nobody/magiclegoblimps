 /* protocol.cpp 
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */
#include"protocol.h"
#include <stdio.h>

 int write_data(int type, void* data_, short number, byteArray* byte_ptr){
    if(!data_ | !byte_ptr)
        return P_NULL_PTR;

    char* array;
    unsigned short* sizes = new unsigned short[number];
    int overall_size = 0;
    char** structs = new char*[number];
    switch(type){

        case P_ROBOT_INIT:
            {
                robotInit* data = (robotInit*)data_;
                for(int i = 0; i < number; ++i){
                    short str_len;
                    const char* url = NULL;
                    if (data[i].VideoURL) {
                        str_len = data[i].VideoURL->size() + 1;
                        url = data[i].VideoURL->c_str();
                    } else {
                        str_len = 0;
                        url = NULL;
                    }
                    short size = sizeof(int)*4 + str_len + sizeof(short)*2;
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

                    //push cameraType
                    ref = (char*)&(data[i].cameraType);
                    for(int j = 14; j < 18; ++j){
                        structs[i][j] = ref[j-14];
                    }

                    //push the string size
                    ref = (char*)&str_len;
                    for(int j = 18; j < 20; ++j){
                        structs[i][j] = ref[j-18];
                    }

                    //push the string
                    for(int j = 20; j < 20+str_len; ++j){
                        structs[i][j] = url[j - 20];
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
                    short size = sizeof(int) * 5 + sizeof(short) + data[i].listSize * 4 * sizeof(int);
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

                    //push dir
                    ref = (char*)&(data[i].dir);
                    for(int j = 14; j < 18; ++j){
                        structs[i][j] = ref[j-14];
                    }

                    ref = (char*)&(data[i].listSize);
                    for(int j = 18; j < 22; ++j){
                        structs[i][j] = ref[j - 18];
                    }
                    int startIndex = 22;
                    ref = (char*)(data[i].objects);
                    for(int j = startIndex; j < (int) ( startIndex + data[i].listSize * sizeof(int) ); ++j){
                        structs[i][j] = ref[j-startIndex];
                    }

                    startIndex += data[i].listSize * sizeof(int);

                    ref = (char*)(data[i].qualities);
                    for(int j = startIndex; j < (int) ( startIndex + data[i].listSize * sizeof(int) ); ++j){
                        structs[i][j] = ref[j - startIndex];
                    }

                    startIndex += data[i].listSize * sizeof(int);

                    ref = (char*)(data[i].xs);
                    for(int j = startIndex; j < (int) ( startIndex + data[i].listSize * sizeof(int) ); ++j){
                        structs[i][j] = ref[j - startIndex];
                    }

                    startIndex += data[i].listSize * sizeof(int);

                    ref = (char*)(data[i].ys);
                    for(int j = startIndex; j < (int) ( startIndex + data[i].listSize * sizeof(int) ); ++j){
                        structs[i][j] = ref[j - startIndex];
                    }

                    sizes[i] = size;
                    overall_size += size;
                }
                break;
            }
            
        case P_OBJECT:
            {
                object* data = (object*)data_;
                for (int i = 0; i < number; ++i) {
                    int str_len;
                    const char* name;
                    if (data[i].name) {
                        str_len = data[i].name->size() + 1;
                        name = data[i].name->c_str();
                    } else {
                        str_len = 0;
                        name = NULL;
                    }
                    int size = sizeof(int) * 5 + str_len + data[i].color_size + data[i].box_size;

                    structs[i] = new char[size];

                    int idx = 0;
                    // push size on
                    char* ref = (char*)&size;
                    for (int j = idx; j < (int)(idx + sizeof(int)); ++j) {
                        structs[i][j] = ref[j];
                    }
                    idx += sizeof(int);

                    // push OID
                    ref = (char*)&(data[i].OID);
                    for (int j = idx; j < (int)(idx + sizeof(int)); ++j) {
                        structs[i][j] = ref[j - idx];
                    }
                    idx += sizeof(int);

                    // push strlen
                    ref = (char*)&str_len;
                    for (int j = idx; j < (int)(idx + sizeof(int)); ++j) {
                        structs[i][j] = ref[j - idx];
                    }
                    idx += sizeof(int);

                    // push string
                    for (int j = idx; j < (idx + str_len); ++j) {
                        structs[i][j] = name[j - idx];
                    }
                    idx += str_len;

                    // push color_size
                    ref = (char*)&(data[i].color_size);
                    for (int j = idx; j < (int)(idx + sizeof(int)); ++j) {
                        structs[i][j] = ref[j - idx];
                    }
                    idx += sizeof(int);

                    // push color
                    for (int j = idx; j < (idx + data[i].color_size); ++j) {
                        structs[i][j] = data[i].color[j - idx];
                    }
                    idx += data[i].color_size;

                    // push box_size
                    ref = (char*)&(data[i].box_size);
                    for (int j = idx; j < (int)(idx + sizeof(int)); ++j) {
                        structs[i][j] = ref[j - idx];
                    }
                    idx += sizeof(int);

                    // push box
                    for (int j = idx; j < (idx + data[i].box_size); ++j) {
                        structs[i][j] = data[i].box[j - idx];
                    }
                    idx += data[i].box_size;

                    // add the length of this array to the total length and store it
                    sizes[i] = size;
                    overall_size += size;
                }
                break;
            }

        case P_ASSIGNMENT:
            {
                assignment* data = (assignment*)data_;  
                short size = 4*sizeof(int) + sizeof(short);
                for(int i = 0; i < number; ++i){
                    structs[i] = new char[size];

                    //push size 
                    char* ref = (char*)(&size);
                    for(int j = 0; j < 2; ++j){
                        structs[i][j] = ref[j];
                    }

                    //push rid 
                    ref = (char*)&(data[i].RID);
                    for(int j = 2; j < 6; ++j){
                        structs[i][j] = ref[j-2];
                    }

                    //push 0ID
                    ref = (char*)&(data[i].OID);
                    for(int j = 6; j <10; ++j){
                        structs[i][j] = ref[j-6];
                    }
                    
                    //push x
                    ref = (char*)&(data[i].x);
                    for(int j = 10; j <14; ++j){
                        structs[i][j] = ref[j-10];
                    }
                    
                    //push y
                    ref = (char*)&(data[i].y);
                    for(int j = 14; j <18; ++j){
                        structs[i][j] = ref[j-14];
                    }
                    
                    sizes[i] = size;
                    overall_size += size;
                }
            }
            break;

        case P_COMMAND:
            {
                command* data = (command*)data_;
                short size = 3*sizeof(int) + sizeof(short);
                for(int i = 0; i < number; ++i){
            
                    structs[i] = new char[size];

                    //push size 
                    char* ref = (char*)(&size);
                    for(int j = 0; j < 2; ++j){
                        structs[i][j] = ref[j];
                    }

                    //push rid 
                    ref = (char*)&(data[i].RID);
                    for(int j = 2; j < 6; ++j){
                        structs[i][j] = ref[j-2];
                    }

                    //push.cmd
                    ref = (char*)&(data[i].cmd);
                    for(int j = 6; j <10; ++j){
                        structs[i][j] = ref[j-6];
                    }

                    ref = (char*)&(data[i].arg);
                    for(int j = 10; j <14; ++j){
                        structs[i][j] = ref[j-10];
                    }
                    
                    sizes[i] = size;
                    overall_size += size;
                }
            }
        break;
        case P_ROBOT_RM:
            {
                robotRm* data = (robotRm*)data_;
                short size = sizeof(int) + sizeof(short);
                for(int i = 0; i < number; ++i){
                    structs[i] = new char[size];
                    int startpos = 0;

                    //push size
                    char* ref = (char*)(&size);
                    for(int j = startpos; j < startpos + sizeof(short); ++j){
                        structs[i][j] = ref[j - startpos];
                    }
                    
                    startpos += sizeof(short);

                    //push robot id
                     ref = (char*)(data[i].RID);
                    for(int j = startpos; j < startpos + sizeof(int); ++j){
                        structs[i][j] = ref[j - startpos];
                    }

                    sizes[i] = size;
                    overall_size += size;
                    
                }
            }
        default:
            return P_INVD_TYPE;

    }

    //intiallize the array to return
    int length = overall_size + 1 + 2 + 4;
    array = new char[length];

    //set type
    array[0] = type;

    //push length in bytes
    char* ref = (char*)&length;
    for(int i = 1; i < 5; ++i){
        array[i] = ref[i-1];
    }
    
    //printf("byte#2: %02X length:%d \n", array[1], length);

    //push number of elements;
    ref = (char*)&number;
    for(int i = 5; i < 7; ++i){
        array[i] = ref[i-5];
    }

    int counter__ = 0;
    if (type == P_OBJECT) {
        std::cout << "[protocol] Writing objects:\n";
        for (int j = counter__; j < 7; ++j) {
            printf("%02X ", array[j]);
            fflush(stdout);
        }
        counter__ += 7;
    }

    //push each array onto the array
    int position = 7;
    for(int i = 0; i < number; ++i){
        memcpy(&array[position], structs[i], sizes[i]);
        position += sizes[i];
        if (type == P_OBJECT) {
            for (int j = counter__; j < counter__ + sizes[i]; ++j) {
                printf("%02X ", array[j]);
                fflush(stdout);
            }
            counter__ += sizes[i];
        }
    }
    //clean up
    delete[] sizes;
    for(int i = 0; i < number; ++i){
        delete[] structs[i];
    }
    delete[] structs;
    byte_ptr->array = array;
    byte_ptr->size = length;

    //char* ptr = array;
    //ptr += 7 + 16;
    //std::cout << ptr << "\n";

    
    return P_OK;

}

int readRobotInit(void* array, robotInit* &robots) {
    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    std::cout << "overall_size: " << *(int*)(current-4) << "\n";
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
        int cameraType;
        short str_len;
        char* url;

        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve x
        ref = (char*)&x;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve y
        ref = (char*)&y;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve cameraType
        ref = (char*)&cameraType;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve str_len
        ref = (char*)&str_len;
        for (int j = 0; j < (int)sizeof(short); ++j) {
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
        robots[i].cameraType = cameraType;
        robots[i].VideoURL = new std::string(url);

        delete[] url;
    }
    return overall_size;
}

int readRobotUpdate(void* array, robotUpdate* &robots){

    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    robots = new robotUpdate[overall_size];
    
    for (int i = 0; i < overall_size; ++i) {
        short size;
        int rid;
        int x;
        int y;
        int dir;
        int listSize;
        int* list;
        float* qos;
        int* xs;
        int* ys;
        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve x
        ref = (char*)&x;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve y
        ref = (char*)&y;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve dir
        ref = (char*)&dir;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        //retrieve listSize;
        ref = (char*)&listSize;
        for(int j = 0; j < (int)sizeof(int); ++j){
            ref[j] = current[0]; current++;
        }

        if (listSize > 0) {
            list = new int[listSize];
            qos = new float[listSize];
            xs = new int[listSize];
            ys = new int[listSize];
            ref = (char*)list;
            for(int j = 0; j < listSize*4; ++j){
                ref[j] = current[0]; current++;
            }

            ref =(char*)(qos);
            for(int j = 0; j < listSize*4; ++j){
                ref[j] = current[0]; current++;
            }

            ref = (char*)(xs);
            for(int j = 0; j < listSize * (int)sizeof(int); ++j){
                ref[j] = current[0]; ++current;
            }

            ref = (char*)(ys);
            for(int j = 0; j < listSize * (int)sizeof(int); ++j){
                ref[j] = current[0]; ++current;
            }
        } else {
            list = NULL;
            qos = NULL;
            xs = NULL;
            ys = NULL;
        }

        // build the robotInit struct
        robots[i].RID = rid;
        robots[i].x = x;
        robots[i].y = y;
        robots[i].dir = dir;
        robots[i].listSize = listSize;
        robots[i].objects = list;
        robots[i].qualities = qos;
        robots[i].xs = xs;
        robots[i].ys = ys;
    }

    return overall_size;
}

int readObject(void* array, object* &objs) {
    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    int counter__ = 0;
    std::cout << "[protocol] Reading objects:\n";
    for (int j = counter__; j < 7; ++j) {
        printf("%02X ", arr[j]);
        fflush(stdout);
    }
    counter__ += 7;

    objs = new object[overall_size];

    for (int i = 0; i < overall_size; ++i) {
        int size;
        int oid;
        int str_len;
        char* name;
        char* color;
        int color_size;
        char* box;
        int box_size;

        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;
        ref[2] = current[0]; current++;
        ref[3] = current[0]; current++;

        for (int j = counter__; j < counter__ + size; ++j) {
                printf("%02X ", arr[j]);
                fflush(stdout);
        }
        counter__ += size;

        // retrieve OID
        ref = (char*)&oid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve str_len
        ref = (char*)&str_len;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve name 
        name = new char[str_len];
        for (int j = 0; j < str_len; ++j) {
            name[j] = current[0]; current++;
        }

        // retrieve color_size 
        ref = (char*)&color_size;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve color 
        color = new char[color_size];
        for (int j = 0; j < color_size; ++j) {
            color[j] = current[0]; current++;
        }

        // retrieve box_size 
        ref = (char*)&box_size;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve box
        box = new char[box_size];
        for (int j = 0; j < box_size; ++j) {
            box[j] = current[0]; current++;
        }

        // build the object struct
        objs[i].OID = oid;
        objs[i].name = new std::string(name);
        objs[i].color_size = color_size;
        objs[i].color = color;
        objs[i].box_size = box_size;
        objs[i].box = box;

        delete[] name;
    }
    return overall_size;
}

int readAssignment(void* array, assignment* &ass){
    
    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    ass = new assignment[overall_size];

    for (int i = 0; i < overall_size; ++i) {
        short size;
        int rid;
        int oid;
        int x;
        int y;
        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve OID
        ref = (char*)&oid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve x
        ref = (char*)&x;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve y
        ref = (char*)&y;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        //initallize struct
        ass[i].RID = rid;
        ass[i].OID = oid;
        ass[i].x   = x;
        ass[i].y   = y;
    }
    return overall_size;
}

int readCommand(void* array, command* &com){

    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    com = new command[overall_size];

    
    for (int i = 0; i < overall_size; ++i) {
        short size;
        int rid;
        int cmd;
        int args;
        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        // retrieve.cmd
        ref = (char*)&cmd;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        //retrieve args
        ref = (char*)&args;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        //initallize struct
        com[i].RID = rid;
        com[i].cmd = cmd;
        com[i].arg = args;
    }
    return overall_size;
}


int readRobotRm(void* array, robotRm* &rrm){

    char* arr = (char*)array;
    char* current = arr+5;
    char* ref;

    // overall size
    short overall_size;
    ref = (char*)&overall_size;
    ref[0] = current[0]; current++;
    ref[1] = current[0]; current++;

    rrm = new robotRm[overall_size];

    
    for (int i = 0; i < overall_size; ++i) {
        short size;
        int rid;
        
        // retreive size
        ref = (char*)&size;
        ref[0] = current[0]; current++;
        ref[1] = current[0]; current++;

        // retrieve RID
        ref = (char*)&rid;
        for (int j = 0; j < (int)sizeof(int); ++j) {
            ref[j] = current[0]; current++;
        }

        rrm[i].RID = rid;

    }

    return overall_size;
}
// takes a void pointer to the data as output from write_data and a readReturn struct to put the array and its type in.
int read_data(void* array, readReturn* ret){
    
    if(!ret || !array)
        return P_NULL_PTR;
    
    switch( (char) ((char*)array)[0] ) {
        case P_ROBOT_INIT:
            {
                robotInit* arr;
                int count = readRobotInit(array, arr);

                // Create a readReturn if we have a NULL pointer
                //if (!ret)
                //    ret = new readReturn
                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_ROBOT_INIT;

                return count;
            }

        case P_ROBOT_UPDATE:
            {
                robotUpdate* arr;
                int count =readRobotUpdate(array, arr);

                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_ROBOT_UPDATE;

                return count;
            }
            break;

        case P_OBJECT:
            {
                object* arr;
                int count =readObject(array, arr);

                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_OBJECT;

                return count;
            }
            break;
        
        case P_ASSIGNMENT:
            {
                assignment* arr;
                int count =  readAssignment(array, arr);

                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_ASSIGNMENT;
                
                return count;
            }
        case P_COMMAND:
            {
                command* arr;
                int count = readCommand(array, arr);
                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_COMMAND;

                return count;
            }
        case P_ROBOT_RM:
            {
                robotRm* arr;
                int count = readRobotRm(array, arr);
                ret->array = (void*)arr;
                ret->size = count;
                ret->type = P_ROBOT_RM;

                return count;
            }

        default:

            std::cerr << "Attempt to read unknown type: " << std::hex  << (char) ((char*)array)[0] << "\n";
    }
    return P_INVD_TYPE;
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
