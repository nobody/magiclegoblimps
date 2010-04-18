#ifndef GRIDLOC_H
#define GRIDLOC_H
#include <cmath>
#include <string>

class GridLoc
{
public:
	GridLoc(void);
	GridLoc(int xx, int yy);
	~GridLoc(void);

	void	setX(int xx);
	void	setY(int yy);
	int		getX(void);
	int		getY(void);
	double	calcDist(GridLoc& gl);
	bool	operator== (const GridLoc& other) const;
	bool	operator!= (const GridLoc& other) const;
private:
	int x, y;
};

#endif
