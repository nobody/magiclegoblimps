#ifndef GRIDLOC_H
#define GRIDLOC_H

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
	bool	operator== (const GridLoc& other) const;
	bool	operator!= (const GridLoc& other) const;
private:
	int x, y;
};

#endif
