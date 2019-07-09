
#ifndef __SGEngine2__Particle_h__
#define __SGEngine2__Particle_h__

#include "../common/common.h"

class Particle {
public:
    bool isLive;
    int age;
    
    Vector3 position;
    Vector3 initialPos;
    Vector3 velocity;
    
    double distance;
};

#endif
