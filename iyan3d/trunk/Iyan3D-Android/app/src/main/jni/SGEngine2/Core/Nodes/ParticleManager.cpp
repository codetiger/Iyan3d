//
//  ParticleManager.cpp
//  Iyan3D
//
//  Created by Karthik on 01/02/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "ParticleManager.h"

ParticleManager::ParticleManager() {
    type = NODE_TYPE_PARTICLES;
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
    isSelected = false;
}

void ParticleManager::setDataFromJson(int count, Vector4 sColor, Vector4 mColor, Vector4 eColor, bool hasGravity, float startSpreadAngle, float startMagnitude, float magnitudeRand, int emissionSpeed, int maxLife, int maxLifeRandPercent, float startScale, float deltaScale)
{
    maxParticleCount = count;
    pool = new ParticlePool(count);
    positions = (Vector4*)malloc(sizeof(Vector4) * count);
    startColor = sColor;
    midColor = mColor;
    endColor = eColor;
    this->hasGravity = hasGravity;
    startVelocitySpreadAngle = startSpreadAngle;
    startVelocityMagnitude = startMagnitude;
    startVelocityMagnitudeRand = magnitudeRand;
    this->emissionSpeed = emissionSpeed;
    this->maxLife = maxLife;
    this->maxLifeRandPercent = maxLifeRandPercent;
    this->startScale = startScale + getScale().x;
    this->deltaScale = deltaScale;
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
            p->position = getAbsoluteTransformation().getTranslation();
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
    while(p != NULL && isSelected) {
        p->age++;
        p->position += p->velocity;
        
        if(p->age > maxLife)
            p->isLive = false;
        
        if(hasGravity)
            p->velocity.y -= 0.001f;
        
        p = pool->getNextLiveParticle();
    }
}

void ParticleManager::updateParticles(bool isSelected)
{
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* p = pool->getParticleByIndex(i);
        this->isSelected = isSelected;
        Vector3 nodePos = getAbsoluteTransformation().getTranslation();
        
        if(!isSelected) {
            positions[i] = Vector4(nodePos.x, nodePos.y, nodePos.z, p->age);
            if(i > 0)
                p->isLive = false;
        }

        if(p->isLive) {
            if(isSelected)
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