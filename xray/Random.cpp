#include "Random.h"
#define _USE_MATH_DEFINES // for C++
#include <math.h>

int CRandom::randI()
{
	holdrand = 214013 * holdrand + 2531011;
	return (holdrand >> 16) & 0x7FFF;
}

float CRandom::randF(float a1, float a2)
{
	return randI() * (float)(a2 - a1) * 0.000030518509f + a1;
}
