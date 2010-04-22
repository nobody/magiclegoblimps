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
#include "hungarian.h"
#include <iostream>
#include <string>

Assignment::Assignment(Robot** r, int nr, Object** o,int no,  double* d, Qos* q){
    demand = d;
    robots = r;
    objects = o;
    numObjects = no;
    numRobots = nr;
    quality = q;

    //Verify and output what we get.

    std::cout   <<"[AS] Initializing Robot Assigner." <<"\n"
                <<"[AS] Robots: " <<numRobots <<" Objects: " <<numObjects <<"\n"
                <<"[AS] Demand: [";
                
    for (int i = 0; i < numObjects; i++) {
        std::cout <<demand[i]; 
        if (i != numObjects - 1) 
            std::cout <<",";
    }

    std::cout <<"]\n";               
}

Assignment::~Assignment()
{

}

int** array_to_matrix(int* m, int rows, int cols) {
    int i,j;
    int** r;
    r = (int**)calloc(rows,sizeof(int*));
    for(i=0;i<rows;i++) {
        r[i] = (int*)calloc(cols,sizeof(int));
        for(j=0;j<cols;j++)
            r[i][j] = m[i*cols+j];
     }
     return r;
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

while(true){
//Find the object with highest demand that has not yet been assigned.  If all 
    int maxObject = -1;
    for(int j = 0; j < numObjects; ++j){
        std::cout   << "[AS] OID: "  <<objects[j]->getOID() <<" index: " <<j 
                    <<" Demand of: " <<demand[j]  <<" Cur max demand: " << maxObject 
                    << " robot assigned: " << objAss[j] << "\n";
        if(maxObject == -1 && objAss[j] == -1){
            maxObject = j;
        }
        else if(demand[j] > demand[maxObject] && objAss[j] == -1){
            maxObject = j;
        }
    }
    if(maxObject == -1){//No object is not being looked at
        std::cout << "[AS] More robots than objects.  Assigning remaing robots -1 object values\n";
        break;
    }
    
//Find the robot that best services this demand that has not yet been assigned
    int maxRobot = -1;
    double tQos = -1;

    for(int j = 0; j < numRobots; j++){
        double t = quality -> calcQos(objects[maxObject],robots[j]);
        if(t>tQos && robotAssignments[j] == -1){
            tQos = t;
            maxRobot = j;
        }
    }
    std::cout <<"[AS] Robot " <<maxRobot <<" assigned to object " <<maxObject <<"\n";
//Store the assignment
    robotAssignments[maxRobot] = maxObject;
    objAss[maxObject] = maxRobot;
    
//Continue until done
    if(isDone() == -1){
        std::cout <<"[AS] All robots assigned.\n";
        break;
    }
}

/*  ASSIGNMENT METHOD: Uninformed Assignment
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
    if(robotAssignments[i]!=-1){
        std::cout << "[AS] Assigning robot " << i << " to object " << objects[robotAssignments[i]]->getOID() << "\n";
         ret->insert(std::pair<Robot*, int>(robots[i],objects[robotAssignments[i]]->getOID()));
    }else{
        std::cout << "[AS] Assigning robot " << i << " to null object (-1)\n";
        ret->insert(std::pair<Robot*, int>(robots[i],-1));
    }
 }

//Hungarian Method for comparison


//matrix of QoS per object and robot
int* arr;
int** r;
int m, n;
bool normal; //flag for normal configuration of robots/objects or flipped
hungarian_problem_t prob;

// m must be less than or equal to n.
if (numObjects >= numRobots) {
    n = numObjects;
    m = numRobots;
    normal = true;
} else {
    n = numRobots;
    m = numObjects;
    normal = false;
}


arr = new int[m*n];

// fill the array
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        if(normal) { //m is robots
            arr[i*n + j] = quality->calcQos(objects[j], robots[i]) * demand[j];
        } else {
            arr[i*n+j] = quality->calcQos(objects[i], robots[j]) * demand[i];
        }

    }
}

r = array_to_matrix(arr, m, n);

hungarian_init(&prob,r,m,n,HUNGARIAN_MODE_MAXIMIZE_UTIL);
hungarian_print_costmatrix(&prob);
hungarian_solve(&prob);
hungarian_print_assignment(&prob);

hungarian_free(&prob);
free(r);

//Write the assignments back to the actual objects
//
 for(int i = 0; i < numObjects; i++) {
    if(objAss[i] != -1) {
        objects[i]->setViewedFrom(robots[objAss[i]]);
    }else{//If no robot is pursuing the object, look through the objects to find the best view 
        int maxIndex = -1;
        double qu = -1.0;
        for(int j = 0; j < numRobots; j++){ //Assign view of unseen object to cam with best view of it.
            if(maxIndex==-1 || quality->calcQos(objects[i],robots[j])>qu){
                maxIndex = j;
                qu = quality->calcQos(objects[i],robots[j]);
            }
        }
        if(qu<=0){
            std::cout << "[AS] No cameras can see the object, use archive video" << "\n";
            objects[i]->setViewedFrom(NULL);
        }else{
            objects[i]->setViewedFrom(robots[maxIndex]);   
        }
     }
  }
 
 delete[] qual;
 delete[] objAss;
 delete[] arr;

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
