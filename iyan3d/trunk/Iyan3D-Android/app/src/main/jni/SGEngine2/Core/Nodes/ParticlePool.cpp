//
//  ParticlePool.cpp
//  Iyan3D
//
//  Created by Karthik on 01/02/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "ParticlePool.h"

ParticlePool::ParticlePool(int count) {
    maxParticleCount = count;
    for (int i = 0; i < count; i++) {
        Particle* p = new Particle();
        p->isLive = false;
        particles.push_back(p);
    }
}

Particle* ParticlePool::reuseDeadParticle() {
    int stopIteration = deadIterator;
    
    while (particles[deadIterator]->isLive) {
        deadIterator++;
        if(deadIterator >= maxParticleCount)
            deadIterator = 0;
        if(deadIterator == stopIteration)
            return NULL;
    }
    
    return particles[deadIterator];
}

Particle* ParticlePool::getNextLiveParticle() {
    if(iterator >= maxParticleCount)
        return NULL;
    else {
        while (!particles[iterator]->isLive) {
            iterator++;
            if(iterator >= maxParticleCount)
                return NULL;
        }
        return particles[iterator++];
    }
}

Particle* ParticlePool::getParticleByIndex(int index) {
    return particles[index];
}

void ParticlePool::resetIteration() {
    iterator = 0;
}

bool checkOrder(Particle *A, Particle *B) {
    return A->distance > B->distance;
}

void ParticlePool::sortByDistance() {
    std::sort(particles.begin(), particles.end(), checkOrder);
}

