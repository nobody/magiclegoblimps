/*
 * Qos.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef QOS_H_
#define QOS_H_

#include "Robot.h"
#include "Object.h"

class Qos {
public:
    Qos();
    Qos(Robot** r,int nr, Object** o, int no,  double* d);
    virtual ~Qos();
    double calcQos();
    double calcQos(Object* o, Robot* r);
    double dist(Point p1, Point p2);
    static const double OPTIMAL_DIST = 10;
    static const double CAM_VALUES [];

    private:
        Robot** robots;
        int numRobots;
        Object** objects;
        int numObjects;
        double* demand;
        int numDemand;
};


#endif /* QOS_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
