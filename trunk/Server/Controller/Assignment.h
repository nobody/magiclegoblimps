/*
 * Assignments.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include "Object.h"
#include "Robot.h"
#include "Qos.h"

class Assignment {
    public:
        Assignment(Robot* r, int nr, Object* o,int no,  double* d, Qos* q);
        ~Assignment();
        std::map<Robot*, int>* calcAssignments();

    private:
		Qos* quality;
		Robot* robots;
		int numRobots;
		Object* objects;
		int numObjects;
		double* demand;
		double gamma;         //Multplier for cost term
		int* robotAssignments;
		void initAssignments();
		int isDone();         //Returns -1 if done, else returns index of next robot to assign
		
		
};


#endif /* ASSIGNMENT_H */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
