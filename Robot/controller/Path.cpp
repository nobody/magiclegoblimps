#include <cmath>
#include "Path.h"

Path::Path(void)
{
	metric = 0;
}

void Path::extend(GridLoc* pt)		{ path.push_back(pt); }

GridLoc* Path::advPath()
{
	GridLoc* gl;
	if(path.front())
	{
		gl = path.front();
		path.erase(path.begin());
	}
	return gl;
}

void Path::setMetric(double newMet)	{ metric = newMet; }
void Path::clearPath()				{ path.clear(); }

vector<GridLoc*> Path::getPath()	{ return path; }
GridLoc* Path::getStart()			{ return path.front(); }
GridLoc* Path::getEnd()				{ return path.back(); }
int Path::getSize()					{ return (int)path.size(); }
double Path::getMetric() const		{ return metric; }

Path* Path::copy()
{
	Path* cpy = new Path();
	vector<GridLoc*>::iterator glIter;
	for(glIter = path.begin(); glIter != path.end(); glIter++)
		cpy->extend(new GridLoc((*glIter)->getX(),
			(*glIter)->getY()));
	/*for(int i = 0; i < path.size(); i++)
		cpy->extend(new GridLoc(path[i]->getX(), 
			path[i]->getY()));*/
	return cpy;
}

bool Path::contains(GridLoc& gl)
{
	vector<GridLoc*>::iterator glIt;
	for(glIt = path.begin(); glIt != path.end(); glIt++)
	{
		if(gl == *(*glIt))
			return true;
	}
	return false;
}

void Path::calcMetric(GridLoc curr, GridLoc dest)
{
	int rLocX = curr.getX();
	int rLocY = curr.getY();

	int rDestX = dest.getX();
	int rDestY = dest.getY();

	int pEndX = path.back()->getX();
	int pEndY = path.back()->getY();

	if(curr != dest)
	{
		double distCoveredSqrd = fabs((double)(rLocX - pEndX))
			+ fabs((double)(rLocY - pEndY));
		double distRemainingSqrd = fabs((double)(rDestX - pEndX))
			+ fabs((double)(rDestY - pEndY));
		metric = (1.0/distCoveredSqrd) + distRemainingSqrd 
			+ path.size();
	}
	else
		metric = 9999;
}

void Path::print()
{
	vector<GridLoc*>::iterator pathIter;

	for(pathIter=path.begin(); pathIter < path.end(); pathIter++)
	{
		printf("(%d, %d) ", (*pathIter)->getX(), 
			(*pathIter)->getY());
	}
	printf("\n");
}

bool Path::operator()(const Path* p1, const Path* p2)
{
	return p1->getMetric() > p2->getMetric();
}

Path::~Path(void)
{
}
