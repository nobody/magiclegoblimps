/*
 * controller.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <map>

#include <boost/rational.hpp>

#include "DbManager.h"
#include "AdminSocket.h"


class controller {
	public:
		controller();
		virtual ~controller();
		int testdb();

	private:
		DbManager *db;
		AdminSocket *admin;

		demand_t demand_;

};


#endif /* CONTROLLER_H_ */
