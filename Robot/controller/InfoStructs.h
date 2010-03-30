#ifndef INFOSTRUCTS_H
#define INFOSTRUCTS_H

#include <cv.h>

struct RobotInfo
{
	int id = -1;
	int target = -1;
	float xLoc = 0;
	float yLoc = 0;
	float targetQuality = 0;
};

struct ObjectInfo
{
	int id = -1;
	CvBox2D originalBox;
	CvHistogram histogram;
};

#endif