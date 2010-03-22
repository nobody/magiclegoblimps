/*
 * Point.h
 *
 * Modified on:    $Date: 2010-03-07 14:20:29 -0600 (Sun, 07 Mar 2010) $
 * Last Edited By: $Author: mealey.patrick $
 * Revision:       $Revision: 75 $
 */

#ifndef POINT_H_
#define POINT_H_

class Point {
    public:
		Point(double a, double b);
		virtual ~Point();
        double x;
		double y;
		
};


#endif /* POINT_H_ */
