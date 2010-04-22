/*
 * Qos.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#include "Qos.h"
#include "Point.h"
#include "hungarian.h"
#include <iostream>

/* 
* Camera quality constants.  Only two camera types are currently available, and only one is likely to be used.
* In the future, more camera types and values can be added.
* Camera Types:
* [0] DLink DCS 920 Wireless Webcam
* [1] Cisco WVC 210 Wireless Webcam
*/

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
    double ret = 0;

    for(int i = 0; i < numObjects; i++){ 
        ret += demand[i]*calcQos(objects[i], objects[i]->getViewedFrom() );
    }
        std::cout <<"[QS] System Qos: " << ret <<"\n";
    return ret;
    
}

//Calculate the Qos metric between an object and a robot
double Qos::calcQos(Object* o, Robot* r) {
    if(o == NULL || r == NULL) {
        return 0;
    }

    //Manually create points until/if we use point class
    Point* rPoint = new Point(r->getXCord(), r->getYCord());
    int camVal = 0;
    std::cout << "[QS] CamQual" << r->getCamera() << "\n";
    if(r->getCamera() != 0){
        camVal = 1;
    }
    double distBasedQuality = dist(*rPoint, o->pos) * Qos::CAM_VALUES[camVal];
    double camBasedQuality = 0;

    /*
    if (r->list && r->list->find(o->getOID()) != r->list->end()) { //verify the robot can see the item
        camBasedQuality = (*r->list)[o->getOID()] * Qos::CAM_VALUES[r->getCamera()];
    }
    else {
    */
        camBasedQuality = 0; //If it is not visible, it is not contributing to QoS
    //}
        
    //std::cout <<"[QS] Robot: " <<r->getRID() <<" Object: " <<o->getOID() <<" :: QoS: " <<q <<"\n"; 
    //return (r ? dist(r->pos, o->pos) * Qos::CAM_VALUES[r->getCamera()] : 0);
    std::cout << "[QS] qos calculated as " << distBasedQuality << "\n";
    
    delete rPoint;

   // if(camBasedQuality == 0){
   //     return 0.0;
   // }else{

        return distBasedQuality;  // * .75 + camBasedQuality * .25;
   // }
}

//Returns decimal on [0,1] representing difference from optimal
double Qos::dist(Point p1, Point p2){
    //double d = sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
    int d = abs(p1.x - p2.x) + abs(p1.y - p2.y);

    std::cout <<"[QS] Manhattan Dist (" <<p1.x <<", " <<p1.y <<"), (" <<p2.x <<", " <<p2.y <<") = " <<d <<"\n";
    if(d >= Qos::OPTIMAL_DIST){
        return Qos::OPTIMAL_DIST / (double)d; //Optimal / Actual if D > optimal
    }else{
        return 1 - abs(d - Qos::OPTIMAL_DIST)/(double)Qos::OPTIMAL_DIST; //Calculates percent error from goal value if d < optimal
    }

}

/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
