#pragma once

class /*__declspec(dllimport)*/ CRandom {
	volatile int holdrand;
public:
	int randI();
	float randF(float a1, float a2);
};


__declspec(dllimport) extern CRandom Random;