//
//  PhysicsHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 29/03/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "PhysicsHelper.h"
#include "SGEditorScene.h"


SGEditorScene *scene;

PhysicsHelper::PhysicsHelper(void *currentScene)
{
    previousFrame = 0;
    scene = (SGEditorScene*)currentScene;
//    broadphase = new btDbvtBroadphase();
//    collisionConfiguration = new btDefaultCollisionConfiguration();
//    dispatcher = new btCollisionDispatcher(collisionConfiguration);
//    solver = new btSequentialImpulseConstraintSolver();
//    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

       
    btVector3 worldAabbMin(-10000,-10000,-10000);
    btVector3 worldAabbMax(10000,10000,10000);
    int maxProxies = 1024;

    broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
    m_softBodyWorldInfo.m_broadphase = broadphase;
    collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    m_softBodyWorldInfo.m_dispatcher = dispatcher;
    solver = new btSequentialImpulseConstraintSolver;
    world = new btSoftRigidDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    
    
    m_softBodyWorldInfo.m_sparsesdf.Initialize();
    m_softBodyWorldInfo.m_gravity.setValue(0,-9.8,0);
    m_softBodyWorldInfo.air_density    =   (btScalar)1.2;
    m_softBodyWorldInfo.water_density  =   0;
    m_softBodyWorldInfo.water_offset   =   0;
    m_softBodyWorldInfo.water_normal   =   btVector3(0,0,0);
    
    world->setGravity(btVector3(0, -9.8, 0));
    rBodies.clear();
    sBodies.clear();
}

PhysicsHelper::~PhysicsHelper()
{
    delete world;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}

void PhysicsHelper::calculateAndSetPropsOfObject(SGNode* sgNode, PHYSICS_TYPE pType)
{
    double density = 0.0;
    sgNode->props.physicsType = (PHYSICS_TYPE)pType;
    
    switch (pType) {
        case STATIC:
            sgNode->props.weight = 0.0;
            return;
            break;
        case LIGHT:
            density = 3.0;
            break;
        case MEDIUM:
            density = 3.0;
            break;
        case HEAVY:
            density = 27.0;
            break;
        case CLOTH:
        case BALLOON:
        case JELLY:
            sgNode->props.isSoft = true;
            break;
        default:
            break;
    }
    sgNode->node->updateBoundingBox();
    BoundingBox b = sgNode->node->getBoundingBox();
    double volume = b.getXExtend() * b.getYExtend() * b.getZExtend();
    sgNode->props.weight = volume * density;
}

void PhysicsHelper::syncPhysicsWorld()
{
    for(int i = 0; i < rBodies.size(); i++)
        world->removeRigidBody(rBodies[i]);
    for(int i = 0; i < sBodies.size(); i++)
        ((btSoftRigidDynamicsWorld*)world)->removeSoftBody(sBodies[i]);
    
    rBodies.clear();
    sBodies.clear();
    vertices.clear();
    testMesh_map.clear();
    
    for (int i = 0; i < scene->nodes.size(); i++) {
        if(scene->nodes[i]->props.isPhysicsEnabled && !scene->nodes[i]->props.isSoft) {
            btRigidBody *body = getRigidBody(scene->nodes[i]);
            world->addRigidBody(body);
            rBodies.push_back(body);
        } else if(scene->nodes[i]->props.isPhysicsEnabled && scene->nodes[i]->props.isSoft) {
            btSoftBody *body = getSoftBody(scene->nodes[i]);
            ((btSoftRigidDynamicsWorld*)world)->addSoftBody(body);
            if(dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache) {
                delete dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache;
                dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache = NULL;
            }
            scene->nodes[i]->node->shouldUpdateMesh = true;
            sBodies.push_back(body);
        }
    }
}

void PhysicsHelper::updatePhysicsUpToFrame(int frame)
{
    if(rBodies.size() > 0 || sBodies.size() > 0) {
        if(frame < previousFrame) {
            previousFrame = 0;
            syncPhysicsWorld();
        }
        
        for(int i = previousFrame+1; i <= frame; i++) {
            world->stepSimulation(1.0/24.0, 10.0, 1.0/60.0);
        }
        
        if(frame > 0) {
            for(int j = 0; j < rBodies.size(); j++) {
                btTransform transform = rBodies[j]->getWorldTransform();
                btVector3 p = transform.getOrigin();
                btQuaternion r = transform.getRotation();
                Vector3 nodePos = Vector3(p.x(), p.y(), p.z());
                Quaternion nodeRot = Quaternion(r.x(), r.y(), r.z(), r.w());
                
                SGNode* sgNode = (SGNode*)rBodies[j]->getUserPointer();
                sgNode->setPosition(nodePos, frame);
                sgNode->setRotation(nodeRot, frame);
                sgNode->setScale(sgNode->scaleKeys[0].scale, frame);
            }
            for(int k = 0; k < sBodies.size(); k++) {
                btTransform transform = sBodies[k]->getWorldTransform();
                btVector3 p = transform.getOrigin();
                btQuaternion r = transform.getRotation();
                Vector3 nodePos = Vector3(p.x(), p.y(), p.z());
                Quaternion nodeRot = Quaternion(r.x(), r.y(), r.z(), r.w());
                
                SGNode *sgNode = (SGNode*)sBodies[k]->getUserPointer();
                sgNode->setPosition(nodePos, frame);
                sgNode->setRotation(nodeRot, frame);
                sgNode->setScale(sgNode->scaleKeys[0].scale, frame);
                updateMeshCache(sgNode);
            }
        }
        previousFrame = frame;
    }
}

void PhysicsHelper::updateMeshCache(SGNode* sgNode)
{
    Mesh *mesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();
    if(!dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache) {
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache = new Mesh();
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->meshformat = mesh->meshformat;
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->meshType = mesh->meshType;
        for (int i = 0; i < mesh->getVerticesCount(); i++){
            vertexData *v = mesh->getLiteVertexByIndex(i);
            vertexData vc;
            vc.vertNormal = v->vertNormal;
            vc.texCoord1 = v->texCoord1;
            int index = testMesh_map.find(i)->second;
            if(vertices.find(index) != vertices.end()) {
                btSoftBody::Node* node = vertices.find(index)->second;
                vc.vertPosition.x = node->m_x.x();
                vc.vertPosition.y = node->m_x.y();
                vc.vertPosition.z = node->m_x.z();
            }
            dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->addVertex(&vc);
            dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->addToIndicesArray(i);
        }
    } else {
        
        for (int i = 0; i < dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->getVerticesCount(); i++)
        {
            vertexData *v = dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->getLiteVertexByIndex(i);
            int index = testMesh_map.find(i)->second;
            if(vertices.find(index) != vertices.end()) {
                btSoftBody::Node* node = vertices.find(index)->second;
                v->vertPosition.x = node->m_x.x();
                v->vertPosition.y = node->m_x.y();
                v->vertPosition.z = node->m_x.z();
            }
        }
    }
    dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->Commit();
    sgNode->node->shouldUpdateMesh = true;
}

btRigidBody* PhysicsHelper::getRigidBody(SGNode* sgNode) {
    ActionKey key = sgNode->getKeyForFrame(0);
    Vector3 nodePos = key.position;
    Quaternion nodeRot = key.rotation;
    btQuaternion rotation = btQuaternion(nodeRot.x, nodeRot.y, nodeRot.z, nodeRot.w);
    btVector3 position = btVector3(nodePos.x, nodePos.y, nodePos.z);
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));
    btScalar bodyMass = sgNode->props.weight;
    btVector3 bodyInertia;
    
    shared_ptr<MeshNode> node = dynamic_pointer_cast<MeshNode>(sgNode->node);
    btConvexHullShape* shape = getShapeForNode(node);
    shape->setLocalScaling(btVector3(key.scale.x, key.scale.y, key.scale.z));
    shape->calculateLocalInertia(bodyMass, bodyInertia);
    shape->setMargin(0);

    btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState, shape, bodyInertia);
    bodyCI.m_restitution = 0.6f;
    bodyCI.m_friction = 0.5f;
    
    btRigidBody *body = new btRigidBody(bodyCI);
    body->setUserPointer((void*)sgNode);
    body->setLinearFactor(btVector3(1,1,1));
    if(sgNode->props.forceMagnitude > 0.0) {
        Vector3 v = sgNode->props.forceDirection * sgNode->props.forceMagnitude;
        btVector3 velocity = btVector3(v.x, v.y, v.z);
        body->setLinearVelocity(velocity);
    }
    return body;
}

btSoftBody* PhysicsHelper::getSoftBody(SGNode* sgNode)
{
    Mesh* mesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();
    btScalar* mesh_vertices = new btScalar[mesh->getVerticesCount() * 3];
    int* mesh_indices = new int[mesh->getTotalIndicesCount()];
    
    for(int i = 0; i < mesh->getVerticesCount(); i++) {
        vertexData *v = mesh->getLiteVertexByIndex(i);
        mesh_vertices[i * 3] = v->vertPosition.x;
        mesh_vertices[(i * 3) + 1] = v->vertPosition.y;
        mesh_vertices[(i * 3) + 2] = v->vertPosition.z;
    }
    
    for( int i = 0; i < mesh->getTotalIndicesCount(); i++)
    {
        mesh_indices[i] = mesh->getTotalIndicesArray()[i];
    }
    
    std::map<btSoftBody::Node*, int> node_map;
    std::vector<int> indices;
    
    btSoftBody* sBody = btSoftBodyHelpers::CreateFromTriMesh(m_softBodyWorldInfo, mesh_vertices, mesh_indices, mesh->getTotalIndicesCount()/3);
    btScalar bodyMass = sgNode->props.weight;
    sBody->setMass(1, bodyMass);
    
    for (int i = 0; i< sBody->m_faces.size(); i++)
    {
        btSoftBody::Face face = sBody->m_faces[i];
        
        for (int j = 0; j < 3; j++)
        {
            if (node_map.find(face.m_n[j]) == node_map.end())
            {
                node_map.insert(std::make_pair(face.m_n[j], node_map.size()));
            }
        }
        for (int j = 0; j < 3; j++)
        {
            indices.push_back(node_map.find(face.m_n[j])->second);
        }
    }

    std::map<btSoftBody::Node*, int>::const_iterator node_iter;
    for (node_iter = node_map.begin(); node_iter != node_map.end(); ++node_iter)
    {
        vertices.insert(std::make_pair(node_iter->second, node_iter->first));
    }
    
    std::map<int, btSoftBody::Node*>::const_iterator it;
    
    for (int i = 0; i < mesh->getVerticesCount(); i++)
    {
        for (it=vertices.begin(); it != vertices.end(); ++it)
        {
            int v_index = it->first;
            btSoftBody::Node* node = it->second;
            Vector3 vPos = mesh->getLiteVertexByIndex(i)->vertPosition;
            if (node->m_x.x() ==  vPos.x &&
                node->m_x.y() ==  vPos.y &&
                node->m_x.z() ==  vPos.z)
            {
                testMesh_map.insert(std::make_pair(i, v_index));
                break;
            }
        }
    }
    
    ActionKey key = sgNode->getKeyForFrame(0);
    Vector3 nodePos = key.position;
    Quaternion nodeRot = key.rotation;
    btQuaternion rotation = btQuaternion(nodeRot.x, nodeRot.y, nodeRot.z, nodeRot.w);
    btVector3 position = btVector3(nodePos.x, nodePos.y, nodePos.z);
    
    sBody->m_cfg.kDP = 0.3;// Damping coefficient [0,1]
    sBody->m_cfg.kDF = 0.2;// Dynamic friction coefficient [0,1]
    sBody->m_cfg.kMT = 0.02;// Pose matching coefficient [0,1]
    sBody->m_cfg.kCHR = 1.0;// Rigid contacts hardness [0,1]
    sBody->m_cfg.kKHR = 0.8;// Kinetic contacts hardness [0,1]
    sBody->m_cfg.kSHR = 1.0;// Soft contacts hardness [0,1]
    sBody->m_cfg.piterations = 2;
    sBody->m_materials[0]->m_kLST = 0.8;
    sBody->m_materials[0]->m_kAST = 0.8;
    sBody->m_materials[0]->m_kVST = 0.8;
//    sBody->scale(btVector3(key.scale.x, key.scale.y, key.scale.z));
    sBody->setPose(false, false);
    sBody->generateBendingConstraints(1000);
    sBody->randomizeConstraints();
//    sBody->generateClusters(300);
    sBody->setUserPointer((void*)sgNode);
    
    btMatrix3x3 m;
    m.setIdentity();
    sBody->transform(btTransform(rotation, position));
    
    return sBody;
}

btConvexHullShape* PhysicsHelper::getShapeForNode(shared_ptr<MeshNode> node)
{
    Mesh* mesh = node->getMesh();
    btConvexHullShape* _shape = new btConvexHullShape();
    for (int i = 0; i < mesh->getVerticesCount(); i++)
    {
        vertexData *v = mesh->getLiteVertexByIndex(i);
        btVector3 btv = btVector3(v->vertPosition.x, v->vertPosition.y, v->vertPosition.z);
        _shape->addPoint(btv);
    }
    return _shape;
}


