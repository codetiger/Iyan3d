//
//  ParticleManager.cpp
//  Iyan3D
//
//  Created by Karthik on 01/02/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "ParticleManager.h"

ParticleManager::ParticleManager(int count) {
    type = NODE_TYPE_PARTICLES;
    maxParticleCount = count;
    pool = new ParticlePool(count);
    positions = (Vector4*)malloc(sizeof(Vector4) * count);
    startColor = Vector4(1.0,1.0,1.0,1.0);
    midColor = Vector4(1.0, 1.0, 0.0, 1.0);
    endColor = Vector4(1.0, 0.0, 0.0, 1.0);
    hasGravity = false;
    startVelocitySpreadAngle = 30.0;
    startVelocityMagnitude = 0.1f;
    startVelocityMagnitudeRand = 0.001f;
    emissionSpeed = 2;
    maxLife = 300;
    maxLifeRandPercent = 10;
    startScale = 0.0 + getScale().x;
    deltaScale = 0.001;
}

ParticleManager::~ParticleManager()
{
    
}

void ParticleManager::update() {
    
    for (int i = 0; i < emissionSpeed; ++i) {
        Particle *p = pool->reuseDeadParticle();
        if(p) {
            p->isLive = true;
            p->age = maxLife * maxLifeRandPercent * 0.01f * rand()/(float)RAND_MAX;
            p->position = position;
            Vector3 direction = Vector3(0, 1, 0);
            Mat4 rotMatix;
            Vector3 rot = getRotationInRadians() + Vector3(- 0.5 + rand()/(float)RAND_MAX, 0.0, - 0.5 + rand()/(float)RAND_MAX) * startVelocitySpreadAngle * DEGTORAD;
            rotMatix.setRotationRadians(rot);
            rotMatix.rotateVect(direction);
            p->velocity = direction * (startVelocityMagnitude + rand()/RAND_MAX *  startVelocityMagnitudeRand);
        } else break;
    }
    
    pool->resetIteration();
    Particle* p = pool->getNextLiveParticle();
    while(p != NULL) {
        p->age++;
        p->position += p->velocity;

        if(p->age > maxLife)
            p->isLive = false;
        
        if(hasGravity)
            p->velocity.y -= 0.001f;
        
        p = pool->getNextLiveParticle();
    }

}

void ParticleManager::updateParticles()
{
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* p = pool->getParticleByIndex(i);
        if(p->isLive) {
            positions[i] = Vector4(p->position.x, p->position.y, p->position.z, p->age);
        } else {
            positions[i] = Vector4(0.0, 0.0, 0.0, p->age);
        }
    }
}

void ParticleManager::sortParticles(Vector3 position) {
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* p = pool->getParticleByIndex(i);
        if(p->isLive) {
            p->distance = p->position.getDistanceFrom(position);
        } else {
            p->distance = 99999.99;
        }
    }
    pool->sortByDistance();
}

Vector4* ParticleManager::getPositions()
{
    return positions;
}

Vector4 ParticleManager::getParticleProps()
{
    return Vector4(maxLife, startScale + getScale().x, deltaScale, 1.0);
}

int ParticleManager::getParticlesCount()
{
    return maxParticleCount;
}

void ParticleManager::setParticleRotation(Vector3 rot) {
    particleRotation = rot;
}