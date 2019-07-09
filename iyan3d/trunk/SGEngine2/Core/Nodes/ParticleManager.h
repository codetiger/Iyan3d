
#ifndef __SGEngine2__ParticleManager_h__
#define __SGEngine2__ParticleManager_h__

#include "../Nodes/ParticlePool.h"
#include "../Nodes/MeshNode.h"

enum EmitterType
{
    EmitterTypeSphere,
    EmitterTypeSurface,
};

class ParticleManager : public MeshNode {
    
private:
    
    int maxParticleCount;
    ParticlePool *pool;
    double gravity;
    
    int startVelocitySpreadAngle; //[ 0 - 360 ]
    float startVelocityMagnitude;
    float startVelocityMagnitudeRand;
    
    int emissionSpeed;
    
    int maxLife;
    int maxLifeRandPercent;
    
    double startScale;
    bool isSelected;
    
    Vector4* positions;
    Vector4* rotations;
public:
    
    double deltaScale;
    
    Vector3 particleRotation;
    
    Vector4 startColor;
    Vector4 midColor;
    Vector4 endColor;
    
    ParticleManager();
    ~ParticleManager();
    
    void setDataFromJson(int count, Vector4 sColor, Vector4 mColor, Vector4 eColor, double gravity, float startSpreadAngle, float startMagnitude, float magnitudeRand, int emissionSpeed, int maxLife, int maxLifeRandPercent, float startScale, float deltaScale);
    virtual void update();
    bool updateParticles(bool isSelected, Vector3 camPos);
    Quaternion rotationBetweenVectors(Vector3 targetDirection, Vector3 initialDirection);
    Vector4* getPositions();
    Vector4* getRotations();
    Vector4 getParticleProps();
    int getParticlesCount();
    void sortParticles(Vector3 position);
    void setParticleRotation(Vector3 rot);
};

#endif
