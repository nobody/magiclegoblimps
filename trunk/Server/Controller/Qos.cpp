/*
 * Qos.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Qos.h"
#include "Point.h"
#include <iostream>

const double Qos::CAM_VALUES[] = {1.0,0.7};

Qos::Qos(Robot** r, int nr, Object** o, int no,  double* d){

	robots =  r;
	numRobots = nr;
	objects =  o;
	numObjects = no;
	demand = d;
}

Qos::~Qos() {
	//Garbagecollect here
}

double Qos::calcQos(){
    
	printf("[QoS]: Calc QOS called");
	double ret = 0;
	int i = 0;
	for(i = 0; i < numObjects; i++){ 
		ret += demand[i]*calcQos(objects[i], objects[i]->viewedFrom );
        
        std::cout <<"[QS] Calc overall QoS" <<"\n";
	}
    std::cout <<"System Qos: " << ret <<"\n";
	return ret;
	
}

//Calculate the Qos metric between an object and a robot
double Qos::calcQos(Object* o, Robot* r){
    
	double q = (r ? dist(r->pos, o->pos) * Qos::CAM_VALUES[r->getCamera()] : 0);
    //std::cout <<"[QS] Robot: " <<r->getRID() <<" Object: " <<o->getOID() <<" :: QoS: " <<q <<"\n"; 
	//return (r ? dist(r->pos, o->pos) * Qos::CAM_VALUES[r->getCamera()] : 0);
	std::cout << "[QS] qos calculated as " << q;
    return q;

}

//Returns decimal on [0,1] representing difference from optimal
double Qos::dist(Point p1, Point p2){
	double d = sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
	if(d >= Qos::OPTIMAL_DIST){
		return Qos::OPTIMAL_DIST / (double)d; //Optimal / Actual if D > optimal
	}else{
		return 1 - abs(d - Qos::OPTIMAL_DIST)/(double)Qos::OPTIMAL_DIST; //Calculates percent error from goal value if d < optimal
	}

}




/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
