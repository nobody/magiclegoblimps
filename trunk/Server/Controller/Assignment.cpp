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
#include <iostream>

Assignment::Assignment(Robot** r, int nr, Object** o,int no,  double* d, Qos* q){
	demand = d;
	robots = r;
	objects = o;
	numObjects = no;
	numRobots = nr;
	quality = q;
}
Assignment::~Assignment()
{

}

std::map<Robot*, int>* Assignment::calcAssignments() {
    

    if (numRobots == 0 || numObjects == 0) {
        std::cout <<"[AS] Empty List of Robots||Objects detected. Null Assignments returned." <<"\n";
        std::map<Robot* , int>* ret = new std::map<Robot* , int>();
        return ret;
    }

    initAssignments();

    int next = 0;
 
    double* qual = new double[numRobots]; //The quality value for robot N
    
    for(int i = 0; i < numRobots; i++){ // Initialize quality vector
        qual[i] = -1;
    }
    
    // objAss stores the mapping of which robot is assigned to each object.
    // A value of -1 indicates there is not a robot assigned to that object.
    //
    int* objAss = new int[numObjects];
    for(int i = 0; i < numObjects; i++){  //The robot N that is viewing object i
        objAss[i] = -1;    
    }
  std::cout <<"NumObjects: " <<numObjects <<"\n";
while(true){
//Find the object with highest demand that has not yet been assigned.
	int maxObject = -1;
	for(int j = 0; j < numObjects; j++){
		if((maxObject==-1|| demand[j] > demand[maxObject]) && objAss[j] == -1){
			maxObject = j;
		}
	}
//Find the robot that best services this demand that has not yet been assigned
	int maxRobot = -1;
	double tQos = -1;
	for(int j = 0; j < numRobots; j++){
		double t = quality -> calcQos(objects[maxObject],robots[j]);
		if(t>tQos && robotAssignments[j]==-1){
			tQos = t;
			maxRobot = j;
		}
	}
//Store the assignment
	robotAssignments[maxRobot] = maxObject;
	objAss[maxObject] = maxRobot;

//Continue until done
	if(isDone()==-1){
		break;
	}
}
	
/*
 while(next != -1) { //until all robots have assignments
    
    int maxIndex = -1;

    for(int i = 0; i < numObjects; i++){//Find the assignment that most greatly increases the system quality
        double t = 0;
        if(objAss[i] != -1){ //If the object is already viewed by something
            std::cout <<"2" <<"\n";
            t = quality->calcQos(objects[i],robots[next]);
            t = t - qual[objAss[i]]-.0001;
            std::cout <<"3" <<"\n";
            
        }else{ //If the object is not already viewed
            t = quality->calcQos(objects[i],robots[next]);
        }
        std::cout << t << "\n";
        if(t > qual[next] || qual[next]==-1){
            maxIndex = i;   // The current best object to view
            qual[next] = t; // The current best quality robot 'next' can contribute is t.
        }
    }
	std::cout << maxIndex << "\n";
    //
    // At this point we have a maximum assignment for a the currently considered robot
    // If there is no conflict (no robot is currently looking at this object) assign it.
    // If there is a conflict, choose the robot which best contributes to quality.
    
    if(objAss[maxIndex]!= -1){ //The object we just chose is assigned already
           
        robotAssignments[objAss[maxIndex]] = -1; // Unassign current robot assigned to object
        qual[objAss[maxIndex]] = -1; // Reset its quality contribution
    }

    objAss[maxIndex] = next; // current robot is now assigned to object of interest
    robotAssignments[next] = maxIndex; //current robot is now assigned to object of interest

    std::cout << "Robot " <<next <<" is now assigned to object " <<maxIndex <<"\n";
        
    next = isDone(); //Find the next unassigned robot
	break;
 }
*/

/*  ASSIGNMENT METHOD: Ordered Assignment
for(int i = 0; i < numRobots; i++){
	robotAssignments[i] = i % numObjects;
}
*/
 //
 // After all assignments have been completed, write assignment map and return it.
 // Set 'viewed from' field of objects for next round of QoS
 //
 std::map<Robot* , int>* ret = new std::map<Robot* , int>();
 for(int i = 0; i < numRobots; i++){
 	ret->insert(std::pair<Robot*, int>(robots[i],objects[robotAssignments[i]]->getOID()));
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
