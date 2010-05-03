#ifndef PATH_H
#define PATH_H

#include <vector>
#include "GridLoc.h"
//#include "Robot.h"

using namespace std;

class Path
{
public:
	Path(void);

	void extend(GridLoc* pt);
	GridLoc* advPath(void);
	void setMetric(double newMet);
	void clearPath(void);

	vector<GridLoc*> getPath(void);
	GridLoc* getStart(void);
	GridLoc* getEnd(void);
	int getSize(void);
	double getMetric(void) const;
	Path* copy(void);
	bool contains(GridLoc& gl);

	void calcMetric(GridLoc curr, GridLoc dest);

	bool operator()(const Path* p1, const Path* p2);

	void print();

	~Path(void);
private:
	vector<GridLoc*> path;
	double metric;
};

#endif
