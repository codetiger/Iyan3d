
#ifndef __SGEngine2__ParticleManager_h__
#define __SGEngine2__ParticleManager_h__

#include "ParticlePool.h"
#include "MeshNode.h"

enum EmitterType
{
	EmitterTypeSphere,
	EmitterTypeSurface,
};

class ParticleManager : public MeshNode {

private:
    
	int maxParticleCount;
	ParticlePool *pool;
	bool hasGravity;

	int startVelocitySpreadAngle; //[ 0 - 360 ]
	float startVelocityMagnitude;
	float startVelocityMagnitudeRand;

	int emissionSpeed;

	int maxLife;
	int maxLifeRandPercent;

	double startScale;

    Vector4* positions;
public:
    
    double deltaScale;

    Vector3 particleRotation;
    
    Vector4 startColor;
    Vector4 midColor;
    Vector4 endColor;

    ParticleManager(int count);
    ~ParticleManager();

    virtual void update();
    void updateParticles();
    Vector4* getPositions();
    Vector4 getParticleProps();
    int getParticlesCount();
    void sortParticles(Vector3 position);
    void setParticleRotation(Vector3 rot);
};

#endif