
#ifndef __SGEngine2__Particle_h__
#define __SGEngine2__Particle_h__

#include "common.h"

class Particle {
public:
	bool isLive;
	int age;
    int initialAge;
    int startedFrame;

	Vector3 position;
    Vector3 initVelocity;
	Vector3 velocity;

    double distance;
};

#endif