//
//  PhysicsHelper.h
//  Iyan3D
//
//  Created by Karthik on 29/03/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef PhysicsHelper_h
#define PhysicsHelper_h

#include "btSoftBody.h"
#include "btSoftRigidDynamicsWorld.h"
#include "btSoftBodyRigidBodyCollisionConfiguration.h"
#include "btSoftBodyHelpers.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "btBulletDynamicsCommon.h"
#include "SGNode.h"

class PhysicsHelper
{
    int previousFrame;
    
    btSoftBodyWorldInfo  m_softBodyWorldInfo;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;
    
    vector < btRigidBody* > rBodies;
    vector < btSoftBody* > sBodies;

public:
    
    PhysicsHelper(void *scene);
    ~PhysicsHelper();
    void syncPhysicsWorld();
    void calculateAndSetPropsOfObject(SGNode* sgNode, PHYSICS_TYPE pType);
    void updatePhysicsUpToFrame(int frame);
    void updateMeshCache(SGNode* sgNode);
    btCollisionShape* getShapeForNode(SGNode* sgNode);
    btRigidBody* getRigidBody(SGNode* sgNode);
    btSoftBody* getSoftBody(SGNode* sgNode);
};

#endif /* PhysicsHelper_h */
