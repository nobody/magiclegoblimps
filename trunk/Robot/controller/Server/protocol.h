/*
 * protocol.h 
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <cstring>
#include <string>
#include <iostream>

//the types 
#define P_ROBOT_INIT 1
#define P_ROBOT_UPDATE 2
#define P_OBJECT 3
#define P_ASSIGNMENT 4
#define P_COMMAND 5

//cameras
#define P_DLINK 1
#define P_CISCO 2
#define P_BROKEN 3

//commands
#define P_CMD_FWD 1
#define P_CMD_LFT  2
#define P_CMD_RGHT 3
#define P_CMD_CAMROT 4
#define P_CMD_DEL_OBJ 5
#define P_CMD_RLS_RBT 6
#define P_CMD_SHUTDOWN 7

//deprecated commands
#define P_CMD_WEST 100
#define P_CMD_MVTO 200

//macros
#define P_HIGH(x) (x >> 16) & 0x0000ffff
#define P_LOW(x) x & 0x0000ffff
#define P_COORD(x, y) ((x << 16) & 0xffff0000) | ( y & 0x0000ffff)

//some error codes
#define P_OK 0
#define P_NULL_PTR -1
#define P_INVD_TYPE -2
/* the protocol converts arrays of the defined structs into byte arrays 
 * from transmission over the socket the protocol is pretty much as 
 * follows, the first byte if the type of struct in the message,
 * the second and third bytes are short representing the total
 * number of structs transmitted. Each struct is prefixed by a two byte
 * short listing the size of that particular struct instance followed by
 * the struct itself. after that each type of struct is unique.
 */

struct robotInit{
    int RID;
    int cameraType;
    int x;
    int y;
    std::string* VideoURL;

    ~robotInit() {
        delete VideoURL;
    }
};
struct robotUpdate{
    int RID;
    int x;
    int y;
    int listSize;
    int* objects;
    float* qualities;
    int* xs;
    int* ys;
    ~robotUpdate(){
        delete[] objects;
    delete[] qualities;
    delete[] xs;
    delete[] ys;
    }
};
struct object{
    int OID;
    std::string* name;
    int color_size;
    char* color;
    int box_size;
    char* box;

    ~object() {
        delete name;
    }
};

struct byteArray{
    char* array;
    int size;
    ~byteArray(){
        delete[] array;
    }
};
struct readReturn{
    void* array;
    int size;
    int type;

};
struct assignment{
    int RID;
    int OID;
    int x;
    int y;
};

struct command{
    int RID;
    int cmd;
    int arg;
};

int write_data(int type, void* data_, short number, byteArray* byte_ptr);


// If you get a robotInit array from this function,
// 
// You MUST delete object.color when you are done with it!!!!
//
int read_data(void* array, readReturn* ret);

int readRobotInit(void* array, robotInit* &robots);

int readRobotUpdate(void* array, robotUpdate* &robots);

#endif /* PROTOCOL_H_ */

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */