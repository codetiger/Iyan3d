//
//  ParticleManager.cpp
//  Iyan3D
//
//  Created by Karthik on 01/02/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "ParticleManager.h"

ParticleManager::ParticleManager()
{
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
    drawMode = DRAW_MODE_POINTS;
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
    this->shouldUpdateMesh = false;
    
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

void ParticleManager::update()
{
    
    
}

bool ParticleManager::updateParticles(bool isSelected, Vector3 camPos)
{
    for (int i = 0; i < emissionSpeed/2; ++i) {
        Particle *p = pool->reuseDeadParticle();
        if(p) {
            p->isLive = true;
            p->age = maxLife * maxLifeRandPercent * 0.01f * rand()/(float)RAND_MAX;
            p->position = getAbsoluteTransformation().getTranslation();
            Vector3 direction = Vector3(0, 1, 0);
            Mat4 rotMatix;
            Quaternion rot = getRotation() * Vector3(- 0.5 + rand()/(float)RAND_MAX, 0.0, - 0.5 + rand()/(float)RAND_MAX) * startVelocitySpreadAngle * DEGTORAD;
            rotMatix.setRotation(rot);
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

    bool meshCacheCreated = false;
    
    if(this->meshCache == NULL) {
        this->meshCache = new Mesh();
    } else
        meshCacheCreated = true;
    
    this->meshCache->clearVertices();
    //if(!meshCacheCreated)
        this->meshCache->clearIndices();
    this->meshCache->getBoundingBox()->clearPoints();
    
    vector< vertexData > mbvd;
    vector< unsigned short > mbi;
    
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* p = pool->getParticleByIndex(i);
        this->isSelected = isSelected;
        Vector3 nodePos = getAbsoluteTransformation().getTranslation();
        
        vertexData v;
        
        Vector4 finalPos;
        if(!isSelected) {
            finalPos = Vector4(nodePos.x, nodePos.y, nodePos.z, p->age);
            p->isLive = false;
        }
        
        if(p->isLive) {
            if(isSelected)
                finalPos = Vector4(p->position.x, p->position.y, p->position.z, p->age);
        } else
            finalPos = Vector4(nodePos.x, nodePos.y, nodePos.z, p->age);
        
        v.vertPosition = Vector3(finalPos.x, finalPos.y, finalPos.z);
        v.vertNormal.x = finalPos.w;
        v.vertNormal.y = finalPos.w;
        v.vertNormal.z = finalPos.w;
        
//        bool updateBB = p->isLive; //TODO updateBoundingbox check
        
        mbvd.push_back(v);
        //if(!meshCacheCreated) {
            mbi.push_back(i);
        //}
    }
    
    this->meshCache->addMeshBuffer(mbvd, mbi, 0);
    this->shouldUpdateMesh = true;
    
    return true;
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

void ParticleManager::sortParticles(Vector3 position)
{
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
    return Vector4(maxLife, getScale().x, deltaScale, 1.0);
}

int ParticleManager::getParticlesCount()
{
    return maxParticleCount;
}

void ParticleManager::setParticleRotation(Vector3 rot)
{
    particleRotation = rot;
}
