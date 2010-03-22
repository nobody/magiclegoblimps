/*
 * Qos.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Qos.h"
#include Math.h
#include "Point.h"

Qos::Qos(Robot* r, POI* o, double* d) {
    std::cout << "In Qos constructor\n";
	robots =  r;
	objects =  o;
	demand = d;
}

Qos::~Qos() {
	//Garbagecollect here
}

double Qos::calcQos(){

	double ret = 0;
	int i = 0;
	while(POI[i]){
		ret += calcQos(r[i],r[i].target,d[i]);
		i++
	}
	return ret;
	
}

//Calculate the Qos metric between an object and a robot
double Qos::calcQos(Robot r, POI o, double d){
	return d * dist(r.pos, o.pos) * camValues[r.camType];

}

//Returns decimal on [0,1] representing difference from optimal
double dist(Point p1, Point p2){
	double d = sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
	
	return abs(d - OPTIMAL_DIST);
	

}




/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
