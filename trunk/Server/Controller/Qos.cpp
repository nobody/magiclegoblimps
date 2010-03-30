/*
 * Qos.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Qos.h"
#include "Point.h"

const double Qos::CAM_VALUES[] = {1.0,0.7};

Qos::Qos(Robot* r,int nr, Object* o, int no,  double* d, int nd){

	robots =  r;
	numRobots = nr;
	objects =  o;
	numObjects = no;
	demand = d;
	numDemand = nd;
}

Qos::~Qos() {
	//Garbagecollect here
}

double Qos::calcQos(){

	double ret = 0;
	int i = 0;
	for(i = 0; i < numObjects; i++){
		ret += demand[i]*calcQos(&objects[i], (&objects[i])->viewedFrom );
	}
	return ret;
	
}

//Calculate the Qos metric between an object and a robot
double Qos::calcQos(Object* o, Robot* r){
	return dist(r->pos, o->pos) * Qos::CAM_VALUES[r->camType];

}

//Returns decimal on [0,1] representing difference from optimal
double Qos::dist(Point p1, Point p2){
	double d = sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
	
	return abs(d - Qos::OPTIMAL_DIST);
	

}




/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
