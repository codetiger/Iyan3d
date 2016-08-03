//
//  PhysicsHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 29/03/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "HeaderFiles/PhysicsHelper.h"
#include "HeaderFiles/SGEditorScene.h"


SGEditorScene *scene;

PhysicsHelper::PhysicsHelper(void *currentScene)
{
    previousFrame = 0;
    scene = (SGEditorScene*)currentScene;
    
    btVector3 worldAabbMin(-10000,-10000,-10000);
    btVector3 worldAabbMax(10000,10000,10000);
    int maxProxies = 10240;

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

void PhysicsHelper::calculateAndSetPropsOfObject(SGNode* sgNode, int pType)
{
    double density = 0.0;
    sgNode->addOrUpdateProperty(PHYSICS_KIND, Vector4(pType), HAS_PHYSICS);
    std::map<PROP_INDEX, Property> physicsProps = sgNode->getProperty(HAS_PHYSICS).subProps;
    
    for(int pI = PHYSICS_NONE; pI < PHYSICS_NONE+7; pI++)
        sgNode->addOrUpdateProperty((PROP_INDEX)pI, Vector4((physicsProps[PHYSICS_KIND].value.x == pI) ? 1 : 0), HAS_PHYSICS);

    switch (pType) {
        case PHYSICS_STATIC:
            sgNode->addOrUpdateProperty(WEIGHT, Vector4(0.0), HAS_PHYSICS);
            return;
            break;
        case PHYSICS_LIGHT:
            density = 3.0;
            break;
        case PHYSICS_MEDIUM:
            density = 3.0;
            break;
        case PHYSICS_HEAVY:
            density = 27.0;
            break;
        case PHYSICS_CLOTH:
        case PHYSICS_JELLY:
            sgNode->addOrUpdateProperty(IS_SOFT, Vector4(1.0), HAS_PHYSICS);
            break;
        default:
            break;
    }
    sgNode->node->updateBoundingBox();
    BoundingBox b = sgNode->node->getBoundingBox();
    double volume = b.getXExtend() * b.getYExtend() * b.getZExtend();
    sgNode->addOrUpdateProperty(WEIGHT, Vector4(volume * density), HAS_PHYSICS);
}

void PhysicsHelper::syncPhysicsWorld()
{
    for(int i = 0; i < rBodies.size(); i++) {
        world->removeRigidBody(rBodies[i]);
        if(rBodies[i]->getMotionState())
            delete rBodies[i]->getMotionState();
        if(rBodies[i]->getCollisionShape())
            delete rBodies[i]->getCollisionShape();
        if(rBodies[i])
            delete rBodies[i];
    }
    for(int i = 0; i < sBodies.size(); i++) {
        ((btSoftRigidDynamicsWorld*)world)->removeSoftBody(sBodies[i]);
        if(sBodies[i]->getCollisionShape())
            delete sBodies[i]->getCollisionShape();
//        if(sBodies[i])
//            delete sBodies[i];
    }
    
    rBodies.clear();
    sBodies.clear();
    
    for (int i = 2; i < scene->nodes.size(); i++) {
        std::map<PROP_INDEX, Property> physicsProps = scene->nodes[i]->getProperty(HAS_PHYSICS).subProps;
        if(scene->nodes[i]->getProperty(HAS_PHYSICS).value.x && !(bool)physicsProps[IS_SOFT].value.x) {
            btRigidBody *body = getRigidBody(scene->nodes[i]);
            world->addRigidBody(body);
            rBodies.push_back(body);
        } else if(scene->nodes[i]->getProperty(HAS_PHYSICS).value.x && (bool)physicsProps[IS_SOFT].value.x) {
            btSoftBody *body = getSoftBody(scene->nodes[i]);
            ((btSoftRigidDynamicsWorld*)world)->addSoftBody(body);
            if(dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache) {
                delete dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache;
                dynamic_pointer_cast<MeshNode>(scene->nodes[i]->node)->meshCache = NULL;
                scene->nodes[i]->node->memtype = NODE_GPUMEM_TYPE_STATIC;
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
                sgNode->setScale(Vector3(1.0), frame);
                updateMeshCache(sgNode);
            }
        }
        previousFrame = frame;
    }
}

void PhysicsHelper::updateMeshCache(SGNode* sgNode)
{
    shared_ptr<MeshNode> n = dynamic_pointer_cast<MeshNode>(sgNode->node);

    if(!dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache) {
        Mesh *mesh = n->getMesh();
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache = new Mesh();
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->meshformat = mesh->meshformat;
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->meshType = mesh->meshType;

        for (int i = 0; i < mesh->getVerticesCount(); i++) {
            vertexData *v = mesh->getLiteVertexByIndex(i);
            vertexData vc;
            vc.vertNormal = v->vertNormal;
            vc.texCoord1 = v->texCoord1;
            int index = n->MeshMap.find(i)->second;
            if(n->m_vertices.find(index) != n->m_vertices.end()) {
                btSoftBody::Node* node = n->m_vertices.find(index)->second;
                vc.vertPosition.x = node->m_x.x();
                vc.vertPosition.y = node->m_x.y();
                vc.vertPosition.z = node->m_x.z();
            }
            dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->addVertex(&vc);
            dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->addToIndicesArray(i);
        }
        sgNode->node->memtype = NODE_GPUMEM_TYPE_DYNAMIC;
    } else {
        for (int i = 0; i < dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->getVerticesCount(); i++) {
            vertexData *v = dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->getLiteVertexByIndex(i);
            int index = n->MeshMap.find(i)->second;
            if(n->m_vertices.find(index) != n->m_vertices.end()) {
                btSoftBody::Node* node = n->m_vertices.find(index)->second;
                v->vertPosition.x = node->m_x.x();
                v->vertPosition.y = node->m_x.y();
                v->vertPosition.z = node->m_x.z();
            }
        }
    }
    
    dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->Commit();
    sgNode->node->shouldUpdateMesh = true;
}

btRigidBody* PhysicsHelper::getRigidBody(SGNode* sgNode)
{
    std::map<PROP_INDEX, Property> physicsProps = sgNode->getProperty(HAS_PHYSICS).subProps;
    
    ActionKey key = sgNode->getKeyForFrame(0);
    Vector3 nodePos = key.position;
    Quaternion nodeRot = key.rotation;
    btQuaternion rotation = btQuaternion(nodeRot.x, nodeRot.y, nodeRot.z, nodeRot.w);
    btVector3 position = btVector3(nodePos.x, nodePos.y, nodePos.z);
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));
    btScalar bodyMass = physicsProps[WEIGHT].value.x;
    btVector3 bodyInertia;
    
    btCollisionShape* shape = getShapeForNode(sgNode);
    shape->setLocalScaling(btVector3(key.scale.x, key.scale.y, key.scale.z));
    shape->calculateLocalInertia(bodyMass, bodyInertia);
    shape->setMargin(0);

    btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState, shape, bodyInertia);
    bodyCI.m_restitution = 0.6f;
    bodyCI.m_friction = 0.5f;
    bodyCI.m_rollingFriction = 0.5f;
    
    btRigidBody *body = new btRigidBody(bodyCI);
    body->setUserPointer((void*)sgNode);
    body->setLinearFactor(btVector3(1,1,1));
    
    if(physicsProps[FORCE_MAGNITUDE].value.x > 0.0) {
        Vector4 fDir = physicsProps[FORCE_DIRECTION].value;
        Vector3 v = Vector3(fDir.x, fDir.y, fDir.z) * (physicsProps[FORCE_MAGNITUDE].value.x * 100.0);
        btVector3 velocity = btVector3(v.x, v.y, v.z);
        body->setLinearVelocity(velocity);
    }
    
    return body;
}

btSoftBody* PhysicsHelper::getSoftBody(SGNode* sgNode)
{
    shared_ptr<MeshNode> n = dynamic_pointer_cast<MeshNode>(sgNode->node);
    n->MeshMap.clear();
    n->m_vertices.clear();

    std::map<int, int> index_map;
    std::map<int, int> bullet_map;
    std::map<int, Vector3> vertex_map;
    int count = 0;

    Mesh* mesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();

    int indicesCount = mesh->getTotalIndicesCount();
    unsigned int indicesArray[indicesCount];
    vector< unsigned int > inds = mesh->getTotalIndicesArray();
    std::copy(inds.begin(), inds.end(), indicesArray);
    
    int vtxCount = mesh->getVerticesCount();
    vertexData *vertexArray = new vertexData[vtxCount];
    vector< vertexData > vtx = mesh->getTotalLiteVerticesArray();
    std::copy(vtx.begin(), vtx.end(), vertexArray);
    
    for( int i = 0; i < indicesCount; i++) {
        int iIndex = indicesArray[i];
        Vector3 v1 = vertexArray[iIndex].vertPosition;
        bool isFirst = true;
        for (int j = 0; j < i; j++) {
            int jIndex = indicesArray[j];
            Vector3 v2 = vertexArray[jIndex].vertPosition;
            
            if(v1 == v2) {
                index_map.insert(make_pair(i, j));
                isFirst = false;
                break;
            }
        }
        
        if(isFirst) {
            index_map.insert(std::make_pair(i, i));
            bullet_map.insert(std::make_pair(i, count));
            Vector3 v3 = vertexArray[iIndex].vertPosition;
            vertex_map.insert(std::make_pair(count, v3));
            count++;
        }
    }

    int *indices = new int[indicesCount];
    for(int i = 0; i < indicesCount; i++) {
        int index1 = index_map.find(i)->second;
        int index2 = bullet_map.find(index1)->second;
        indices[i]   = index2;
    }

    int vertexCount = (int)vertex_map.size();
    btScalar *vertices = new btScalar[vertexCount*3];
    
    for(int i = 0; i < vertexCount; i++) {
        vertices[3*i] =   vertex_map[i].x;
        vertices[3*i+1] = vertex_map[i].y;
        vertices[3*i+2] = vertex_map[i].z;
    }

    btSoftBody* sBody = btSoftBodyHelpers::CreateFromTriMesh(m_softBodyWorldInfo, vertices, indices, indicesCount/3);

    std::map<btSoftBody::Node*, int> node_map;

    for(int i = 0; i < sBody->m_faces.size(); i++) {
        auto face = sBody->m_faces[i];
        
        for(int j = 0; j < 3; j++)
            if(node_map.find(face.m_n[j]) == node_map.end())
                node_map.insert(std::make_pair(face.m_n[j], node_map.size()));
    }

    for(auto& node_iter : std::move(node_map))
        n->m_vertices.insert(std::make_pair(node_iter.second, node_iter.first));
    
    for(u32 i = 0; i < vtxCount; i++) {
        for(auto& it : std::move(n->m_vertices)) {
            int v_index = it.first;
            auto node = it.second;
            Vector3 v1 = vertexArray[i].vertPosition;
            if(node->m_x.x() == v1.x &&
               node->m_x.y() == v1.y &&
               node->m_x.z() == v1.z) {
                n->MeshMap.insert(std::make_pair(i, v_index));
                break;
            }
        }
    }

    ActionKey key = sgNode->getKeyForFrame(0);
    Vector3 nodePos = key.position;
    Quaternion nodeRot = key.rotation;
    btQuaternion rotation = btQuaternion(nodeRot.x, nodeRot.y, nodeRot.z, nodeRot.w);
    btVector3 position = btVector3(nodePos.x, nodePos.y, nodePos.z);
    
    sBody->m_cfg.kDP = 0.0f;  // Damping coefficient [0,1]
    sBody->m_cfg.kDF = 0.2f;  // Dynamic friction coefficient [0,1]
    sBody->m_cfg.kMT = 0.02f; // Pose matching coefficient [0,1]
    sBody->m_cfg.kCHR = 1.0f; // Rigid contacts hardness [0,1]
    sBody->m_cfg.kKHR = 0.8f; // Kinetic contacts hardness [0,1]
    sBody->m_cfg.kSHR = 1.0f; // Soft contacts hardness [0,1]
    sBody->m_cfg.piterations=2;
    sBody->m_materials[0]->m_kLST = 0.8f;
    sBody->m_materials[0]->m_kAST = 0.8f;
    sBody->m_materials[0]->m_kVST = 0.8f;
    sBody->scale(btVector3(sgNode->getNodeScale().x, sgNode->getNodeScale().y, sgNode->getNodeScale().x));
    sBody->setUserPointer((void*)sgNode);
    
    std::map<PROP_INDEX, Property> physicsProps = sgNode->getProperty(HAS_PHYSICS).subProps;
    
    if(physicsProps[PHYSICS_KIND].value.x == PHYSICS_CLOTH) {
        sBody->generateBendingConstraints(2);
        sBody->m_cfg.piterations = 2;
        sBody->randomizeConstraints();
    } else if(physicsProps[PHYSICS_KIND].value.x == PHYSICS_JELLY) {
        sBody->generateBendingConstraints(2);
        sBody->m_cfg.piterations = 2;
        sBody->randomizeConstraints();
    }
    
    btMatrix3x3 m;
    m.setIdentity();
    sBody->transform(btTransform(rotation, position));
    
    return sBody;
}

btCollisionShape* PhysicsHelper::getShapeForNode(SGNode* sgNode)
{
    shared_ptr<MeshNode> node = dynamic_pointer_cast<MeshNode>(sgNode->node);
    Mesh* mesh = node->getMesh();
    
    std::map<PROP_INDEX, Property> physicsProps = sgNode->getProperty(HAS_PHYSICS).subProps;

    if(physicsProps[PHYSICS_KIND].value.x == PHYSICS_STATIC) {
        btTriangleMesh *m = new btTriangleMesh();
        
        vector< unsigned int > indices = mesh->getTotalIndicesArray();
        for (int i = 0; i < indices.size(); i += 3) {
            vertexData *v1 = mesh->getLiteVertexByIndex(indices[i+0]);
            btVector3 btv1 = btVector3(v1->vertPosition.x, v1->vertPosition.y, v1->vertPosition.z);
            vertexData *v2 = mesh->getLiteVertexByIndex(indices[i+1]);
            btVector3 btv2 = btVector3(v2->vertPosition.x, v2->vertPosition.y, v2->vertPosition.z);
            vertexData *v3 = mesh->getLiteVertexByIndex(indices[i+2]);
            btVector3 btv3 = btVector3(v3->vertPosition.x, v3->vertPosition.y, v3->vertPosition.z);
            
            m->addTriangle(btv1, btv2, btv3);
        }
        
        btBvhTriangleMeshShape *_shape = new btBvhTriangleMeshShape(m, false);
        _shape->setMargin(0.5);
        
        return _shape;
    } else {
        btConvexHullShape* _shape = new btConvexHullShape();
        for (int j = 0; j < mesh->getVerticesCount(); j++) {
            vertexData *v = mesh->getLiteVertexByIndex(j);
            btVector3 btv = btVector3(v->vertPosition.x, v->vertPosition.y, v->vertPosition.z);
            _shape->addPoint(btv);
        }
        
        btShapeHull *hull = new btShapeHull(_shape);
        hull->buildHull(0.0f);
        
        btConvexShape *simlifiedShape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());
        
        delete _shape;
        delete hull;
        return simlifiedShape;
    }
}
