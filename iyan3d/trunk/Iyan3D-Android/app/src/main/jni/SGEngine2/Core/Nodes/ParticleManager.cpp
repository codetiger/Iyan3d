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
    gravity = 0.0;
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

void ParticleManager::setDataFromJson(int count, Vector4 sColor, Vector4 mColor, Vector4 eColor, double gravity, float startSpreadAngle, float startMagnitude, float magnitudeRand, int emissionSpeed, int maxLife, int maxLifeRandPercent, float startScale, float deltaScale)
{
    maxParticleCount = count;
    pool = new ParticlePool(count);
    positions = (Vector4*)malloc(sizeof(Vector4) * count);
    rotations = (Vector4*)malloc(sizeof(Vector4) * count);
    startColor = sColor;
    midColor = mColor;
    endColor = eColor;
    this->gravity = gravity;
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
    if(pool) {
        delete pool;
        pool = NULL;
    }
    
    if(positions) {
        delete positions;
        positions = NULL;
    }
    
    if(rotations) {
        delete rotations;
        rotations = NULL;
    }
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
    while(p != NULL) {
        p->age++;
        p->position += p->velocity;
        
        if(p->age > maxLife)
            p->isLive = false;
        
            p->velocity.y += gravity;
        
        p = pool->getNextLiveParticle();
    }
}

void ParticleManager::updateParticles(bool isSelected, Vector3 camPos)
{
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* p = pool->getParticleByIndex(i);
        this->isSelected = isSelected;
        Vector3 nodePos = getAbsoluteTransformation().getTranslation();
        
        if(!isSelected) {
            positions[i] = Vector4(nodePos.x, nodePos.y, nodePos.z, p->age);
            p->isLive = false;
        }

        if(p->isLive) {
            if(isSelected) {
                positions[i] = Vector4(p->position.x, p->position.y, p->position.z, p->age);
                Vector3 rotDir = (camPos - Vector3(positions[i].x, positions[i].y, positions[i].z)).normalize();
                Quaternion rotQ;
                rotQ = rotationBetweenVectors(rotDir, Vector3(0.0,1.0,0.0));
                Vector3 rotEuler;rotQ.toEuler(rotEuler);
                rotations[i] = Vector4(rotEuler.x, rotEuler.y, rotEuler.z, 0.0);
            }
        } else {
            positions[i] = Vector4(nodePos.x, nodePos.y, nodePos.z, p->age);
            Vector3 rotDir = (camPos - Vector3(positions[i].x, positions[i].y, positions[i].z)).normalize();
            Quaternion rotQ;
            rotQ = rotationBetweenVectors(rotDir, Vector3(0.0,1.0,0.0));
            Vector3 rotEuler;rotQ.toEuler(rotEuler);
            rotations[i] = Vector4(rotEuler.x, rotEuler.y, rotEuler.z, 0.0);
        }
    }
}

Quaternion ParticleManager::rotationBetweenVectors(Vector3 targetDirection, Vector3 initialDirection)
{
    float dot = initialDirection.dotProduct(targetDirection);
    Vector3 cross = initialDirection.crossProduct(targetDirection);
    cross = cross.normalize();
    dot = (dot < -1 || dot > 1) ? 0.0:dot;
    float turnAngle = acos(dot);
    Quaternion delta;
    delta.fromAngleAxis(turnAngle, cross);
    delta.normalize();
    return delta;
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

Vector4* ParticleManager::getRotations()
{
    return rotations;
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