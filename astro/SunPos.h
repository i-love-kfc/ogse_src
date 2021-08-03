// code is based on that taken from http://www.psa.es/sdg/sunpos.htm

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// Declaration of some constants 
#define pi    M_PI
#define twopi (2*M_PI)
#define rad   (pi/180)
#define dEarthMeanRadius     6371.01	// In km
#define dAstronomicalUnit    149597890	// In km

struct cTime
{
	int iYear;
	int iMonth;
	int iDay;
	double dHours;
	double dMinutes;
	double dSeconds;
};

struct cLocation
{
	double dLongitude;
	double dLatitude;
};

struct cSunCoordinates
{
	double dZenithAngle;
	double dAzimuth;
};

void sunpos(const cTime &udtTime, const cLocation &udtLocation, cSunCoordinates &udtSunCoordinates);
