/*
 * Assignment.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Assignment.h"
#include "Point.h"
#include "Robot.h"
#include "Object.h"

Assignment::Assignment(Robot* r, int nr, Object* o,int no,  double* d, Qos* q){
	demand = d;
	robots = r;
	objects = o;
	numObjects = no;
	numRobots = nr;
	quality = q;
    
    
    
}

std::map<Robot*, int>* Assignment::calcAssignments() {
 
 initAssignments();
 
 int next = 0;
 
    double* qual = new double[numRobots]; //The quality value for robot N
    for(int i = 0; i < numRobots; i++){
        qual[i] = 0;
    }
    int* objAss = new int[numObjects];
    for(int i = 0; i < numObjects; i++){  //The robot N that is viewing object i
        objAss[i] = -1;    
    }
 
 while(next != -1) { //until all robots have assignments
    
    int maxIndex = -1;

    for(int i = 0; i < numObjects; i++){//Find the assignment that most greatly increases the system quality
        double t;
        if(objAss[i] != -1){ //If the object is already viewed by something
            t = quality->calcQos(objects[i],robots[next]);
            t = t - qual[objAss[i]];
            
        }else{ //If the object is not already viewed
            t = quality->calcQos(objects[i],robots[next]);
        }
        
        if(t > qual[next]){
            maxIndex = i;
            qual[next] = t;
        }
    }
    
    if(objAss[maxIndex]!= -1){
        
        robotAssignments[objAss[maxIndex]] = -1;
        qual[objAss[maxIndex]] = 0;
        objAss[robotAssignments[next]] = -1;
        
        robotAssignments[next] = maxIndex;
        objAss[maxIndex] = next;
        
    } else{
        robotAssignments[next] = maxIndex;
        objAss[maxIndex] = next;
    }
    
     next = isDone();
 }
 std::map<Robot* , int>* ret = new std::map<Robot* , int>();
 for(int i = 0; i < numRobots; i++){
 	ret->insert(std::pair<Robot*, int>(&robots[i],objects[robotAssignments[i]].getOID()));
 }
 delete[] qual;
 delete[] objAss;
 
 return ret;
    
}
// Check for any remaining unassignmed robots, return index
// of first unassigned robot if there is one, -1 if there are none.
int Assignment::isDone(){
    for(int i = 0; i < numRobots; i++){
        if(robotAssignments[i] == -1)
            return i;
    }   
    return -1;
}

//Set assignment vector to -1s
void Assignment::initAssignments() {
    robotAssignments = new int[numRobots];
    
    for(int i = 0; i < numRobots; i++){
        robotAssignments[i] = -1;
    }
    
}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
