#include "GridLoc.h"

GridLoc::GridLoc(void)
{
	x = 0;
	y = 0;
}

GridLoc::GridLoc(int xx, int yy)
{
	x = xx;
	y = yy;
}

void GridLoc::setX(int xx)
{
	x = xx;
}

void GridLoc::setY(int yy)
{
	y = yy;
}

int GridLoc::getX(void) { return x; }
int GridLoc::getY(void) { return y; }

bool GridLoc::operator==(const GridLoc& other) const
{
	if(x == other.x && 
		y == other.y)
		return true;
	else
		return false;
}

bool GridLoc::operator!=(const GridLoc& other) const
{
	if(*this == other)
		return false;
	else
		return true;
}

GridLoc::~GridLoc(void)
{
}
