/*
 * Qos.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef QOS_H_
#define QOS_H_

class Qos {
    public:
        Qos(Robot* r, POI* o, double* d);
        virtual ~Qos();
		calcQos();
		calcQos(Robot r, POI o);
		const double OPTIMAL_DIST = 10;
		

    private:
		Robot* robots;
		POI* objects;
		double* demand;
		
};


#endif /* QOS_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
