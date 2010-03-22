/*
 * Point.cpp
 *
 * Modified on:    $Date: 2010-03-07 14:20:29 -0600 (Sun, 07 Mar 2010) $
 * Last Edited By: $Author: mealey.patrick $
 * Revision:       $Revision: 75 $
 */

#include "Point.h"



Point::Point(double a, double b) {
    std::cout << "In Point constructor\n";
	x = a;
	y = b;
}

Point::~Point{
}




/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */