
#ifndef __SGEngine2__ParticlePool_h__
#define __SGEngine2__ParticlePool_h__

#include "Particle.h"

class ParticlePool {
	vector< Particle* > particles;
	int iterator;
	int maxParticleCount;
	int deadIterator = 0;
    
public:

    ParticlePool(int count);
    Particle* reuseDeadParticle();
    Particle* getNextLiveParticle();
    Particle* getParticleByIndex(int index);
    void sortByDistance();
    void resetIteration();
};

#endif