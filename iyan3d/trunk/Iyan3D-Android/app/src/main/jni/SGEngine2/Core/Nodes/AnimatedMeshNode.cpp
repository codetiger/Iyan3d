//
//  AnimatedMeshNode.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#include "AnimatedMeshNode.h"

AnimatedMeshNode::AnimatedMeshNode()
{
    this->type = NODE_TYPE_SKINNED;
}

AnimatedMeshNode::~AnimatedMeshNode()
{
    if (this->mesh)
        delete (SkinMesh*)this->mesh;

    this->mesh = NULL;
    if (jointNodes.size()) {
        for (u16 i = 0; i < jointNodes.size(); i++) {
            if (jointNodes[i]) {
                //TODO Have to be fixed - Memory leak
                //                jointNodes[i]->detachFromParent();
                //                jointNodes[i]->detachAllChildren();
                while(jointNodes[i].use_count() > 0)
                    jointNodes[i].reset();
                jointNodes.erase(jointNodes.begin() + i);
            }
        }
    }
    jointNodes.clear();
}

void AnimatedMeshNode::setMesh(SkinMesh* mesh, int maxJoints, rig_type rigType)
{
    this->mesh = mesh;
    SkinMesh* SMesh = (SkinMesh*)mesh;
    int jointsCount = SMesh->joints->size();

    for (int i = 0; i < jointsCount; i++) {
        shared_ptr<JointNode> node = make_shared<JointNode>();
        jointNodes.push_back(node);
    }
    
    for (int i = 0; i < jointsCount; i++) {
        jointNodes[i]->name = (*SMesh->joints)[i]->name;
        if ((*SMesh->joints)[i]->Parent) {
            unsigned short parentId = (*SMesh->joints)[i]->Parent->Index;
            jointNodes[i]->Parent = jointNodes[parentId];
        } else
            jointNodes[i]->Parent = shared_ptr<Node>();
    }
    
    if(this->mesh) {
        for (int mbi = 0; mbi < mesh->getMeshBufferCount(); mbi++) {
            for(int i = 0; i < mesh->getVerticesCountInMeshBuffer(mbi); i++) {
                vertexDataHeavy *vData = mesh->getHeavyVerticesForMeshBuffer(mbi, i);
                if(vData) {
                    int jointId = vData->optionalData1.x - 1;
                    
                    if(jointId > 0 && jointNodes[jointId])
                        jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                    
                    if(rigType == CHARACTER_RIG) {
                        jointId = vData->optionalData1.y - 1;
                        if(jointId > 0 && jointNodes[jointId])
                            jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                        jointId = vData->optionalData1.z - 1;
                        if(jointId > 0 && jointNodes[jointId])
                            jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                        jointId = vData->optionalData3.x - 1;
                        if(jointId > 0 && jointNodes[jointId])
                            jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                        jointId = vData->optionalData3.y - 1;
                        if(jointId > 0 && jointNodes[jointId])
                            jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                        jointId = vData->optionalData3.z - 1;
                        if(jointId > 0 && jointNodes[jointId])
                            jointNodes[jointId]->bBox.addPointsToCalculateBoundingBox(vData->vertPosition);
                    }
                }
            }
        }
    }

    for (int i = 0; i < jointNodes.size(); i++) {
        jointNodes[i]->bBox.calculateEdges();
        shared_ptr<JointNode> node = jointNodes[i];
        if (!jointNodes[i]->Parent)
            node->setParent(shared_from_this());
        for (int j = 0; j < (*SMesh->joints)[i]->childJoints->size(); j++) {
            unsigned short childIndex = (*(*SMesh->joints)[i]->childJoints)[j]->Index;
            node->Children->push_back(jointNodes[childIndex]);
        }
        node->updateBoundingBox();
    }
    SMesh->recoverJointsFromMesh(jointNodes);
    
    if (jointsCount > maxJoints || skinType == CPU_SKIN) {
        skinType = CPU_SKIN;
        initializeMeshCache();
    }
}

void AnimatedMeshNode::initializeMeshCache()
{
    meshCache = this->mesh->convert2Lite();
}

SkinMesh* AnimatedMeshNode::getMesh()
{
    if(skinType == CPU_SKIN)
        return (SkinMesh*)this->meshCache;
    
    return (SkinMesh*)this->mesh;
}

Mesh* AnimatedMeshNode::getMeshCache()
{
    return this->meshCache;
}

void AnimatedMeshNode::updateBoundingBox()
{
    if(!mesh)
        return;
    
    BoundingBox bb;
    if(jointNodes.size()) {
        for(unsigned short i = 0; i < jointNodes.size();i++){
            if(jointNodes[i] && jointNodes[i]->getBoundingBox().isValid()) {
                for (int j = 0; j < 8; j++) {
                    Vector3 edge = jointNodes[i]->getBoundingBox().getEdgeByIndex(j);
                    Mat4 JointVertexPull;
                    SkinMesh *sMesh = (SkinMesh*)mesh;
                    Mat4 abs = jointNodes[i]->getAbsoluteTransformation();
                    JointVertexPull.setbyproduct(abs, (*sMesh->joints)[i]->GlobalInversedMatrix);
                    Vector4 newEdge = JointVertexPull *  Vector4(edge.x, edge.y, edge.z, 1.0);
                    bb.addPointsToCalculateBoundingBox(Vector3(newEdge.x, newEdge.y, newEdge.z));
                }
            }
        }
    }
    bb.calculateEdges();
    bBox = bb;
}

void AnimatedMeshNode::update()
{
    ((SkinMesh*)this->mesh)->transferJointsToMesh(jointNodes);
    ((SkinMesh*)this->mesh)->buildAllGlobalAnimatedMatrices(NULL, NULL);
}

void AnimatedMeshNode::updateMeshCache()
{
    if(skinType != CPU_SKIN)
        return;
    
    update();
    SkinMesh* sMesh = (SkinMesh*)mesh;
    vector<Mat4> jointTransforms;
    for (int i = 0; i < sMesh->joints->size(); ++i) {
        Mat4 JointVertexPull;
        JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix, (*sMesh->joints)[i]->GlobalInversedMatrix);
        jointTransforms.push_back(JointVertexPull);
    }

    for (int mbi = 0; mbi < this->mesh->getMeshBufferCount(); mbi++) {
        for (int vIndex = 0; vIndex < this->mesh->getVerticesCountInMeshBuffer(mbi); vIndex++) {
            vertexData* finalVertData = this->meshCache->getLiteVerticesForMeshBuffer(mbi, vIndex);
            vertexDataHeavy* vertex = this->mesh->getHeavyVerticesForMeshBuffer(mbi, vIndex);
            
            finalVertData->vertPosition = vertex->vertPosition;
            finalVertData->vertNormal = vertex->vertNormal;
            finalVertData->texCoord1 = vertex->texCoord1;
            finalVertData->vertColor = vertex->optionalData4;
            finalVertData->vertTangent = vertex->vertTangent;
            finalVertData->vertBitangent = vertex->vertBitangent;
            
            calculateJointTransforms(vertex, jointTransforms, finalVertData->vertPosition, finalVertData->vertNormal);
        }
    }
    jointTransforms.clear();
    
    this->shouldUpdateMesh = true;
}

void AnimatedMeshNode::updatePartOfMeshCache(int jointId)
{
    if(jointId <= 0 || skinType != CPU_SKIN)
        return;
    
    update();
    SkinMesh* sMesh = (SkinMesh*)mesh;
    vector<Mat4> jointTransforms;
    for (int i = 0; i < sMesh->joints->size(); ++i) {
        Mat4 JointVertexPull;
        JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix, (*sMesh->joints)[i]->GlobalInversedMatrix);
        jointTransforms.push_back(JointVertexPull);
    }
    vector<int> PaintedVertexIndices;
    getAllPaintedVertices(sMesh, PaintedVertexIndices, jointId);
    
    for(int index = 0; index < PaintedVertexIndices.size(); index++) {
        
        int vIndex = PaintedVertexIndices[index];
        vertexData* finalVertData = this->meshCache->getLiteVerticesForMeshBuffer(0, vIndex);
        vertexDataHeavy* vertex = this->mesh->getHeavyVerticesForMeshBuffer(0, vIndex);
        
        finalVertData->vertPosition = vertex->vertPosition;
        finalVertData->vertNormal = vertex->vertNormal;
        finalVertData->texCoord1 = vertex->texCoord1;
        finalVertData->vertColor = vertex->optionalData4;
        
        calculateJointTransforms(vertex, jointTransforms, finalVertData->vertPosition, finalVertData->vertNormal);
    }
    
    jointTransforms.clear();
    PaintedVertexIndices.clear();
    
    this->shouldUpdateMesh = true;
}

void AnimatedMeshNode::getAllPaintedVertices(SkinMesh *skinMesh , vector<int> &paintedVertices , int jointId)
{
    for(int index = 0; index < (*skinMesh->joints)[jointId]->PaintedVertices->size(); index++) {
        paintedVertices.push_back((*(*skinMesh->joints)[jointId]->PaintedVertices)[index]->vertexId);
    }
    
    for(int childId = 0; childId < (*skinMesh->joints)[jointId]->childJoints->size(); childId++) {
        if((*(*skinMesh->joints)[jointId]->childJoints)[childId]) {
            getAllPaintedVertices(skinMesh, paintedVertices, (*(*skinMesh->joints)[jointId]->childJoints)[childId]->Index);
        }
    }
}

short AnimatedMeshNode::getActiveMeshIndex(int index)
{
    return index;
}

Mesh* AnimatedMeshNode::getMeshByIndex(int index)
{
    return this->mesh;
}

shared_ptr<JointNode> AnimatedMeshNode::getJointNode(int jointId)
{
    if(jointNodes.size() > jointId && jointId >= 0)
        return jointNodes[jointId];
    else
        return shared_ptr<JointNode>();
}

int AnimatedMeshNode::getJointCount()
{
    return (int)jointNodes.size();
}

void AnimatedMeshNode::calculateSingleJointTransforms(vertexDataHeavy* vertex, Mat4 jointTransform, int jointId, Vector3& vertPosition, Vector3& vertNormal , rig_type rigType)
{
    Vector4 pos = Vector4(0.0);
    Vector4 nor = Vector4(0.0);
    
    if(jointId > 0) {
        
        if (jointId == vertex->optionalData1.x) {
            
            float strength = vertex->optionalData2.x;
                pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
                nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData1.y) {
            
            float strength = vertex->optionalData2.y;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData1.z) {
            
            float strength = vertex->optionalData2.z;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData1.w) {
            
            float strength = vertex->optionalData2.w;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData3.x) {
            
            float strength = vertex->optionalData4.x;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData3.y) {
            
            float strength = vertex->optionalData4.y;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData3.z) {
            
            float strength = vertex->optionalData4.z;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        } else if (jointId == vertex->optionalData3.w) {
            
            float strength = vertex->optionalData4.w;
            pos = pos + (jointTransform * Vector4(vertPosition, 1.0)) * strength;
            nor = nor + (jointTransform * Vector4(vertNormal, 0.0)) * strength;
            
        }
    } else {
        
        pos = Vector4(vertPosition, 1.0);
        nor = Vector4(vertNormal, 0.0);
        
    }
    
    vertPosition = Vector3(pos.x, pos.y, pos.z);
    vertNormal = Vector3(nor.x, nor.y, nor.z);
    
}

void AnimatedMeshNode::calculateJointTransforms(vertexDataHeavy* vertex, vector<Mat4> jointTransforms, Vector3& vertPosition, Vector3& vertNormal, rig_type rigType)
{
    Vector4 pos = Vector4(0.0);
    Vector4 nor = Vector4(0.0);

    int jointId = int(vertex->optionalData1.x);
    float strength = vertex->optionalData2.x;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }
    else {
        pos = Vector4(vertPosition, 1.0);
        nor = Vector4(vertNormal, 0.0);
    }

    if(rigType == CHARACTER_RIG) {
    jointId = int(vertex->optionalData1.y);
    strength = vertex->optionalData2.y;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData1.z);
    strength = vertex->optionalData2.z;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData1.w);
    strength = vertex->optionalData2.w;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData3.x);
    strength = vertex->optionalData4.x;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData3.y);
    strength = vertex->optionalData4.y;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData3.z);
    strength = vertex->optionalData4.z;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }

    jointId = int(vertex->optionalData3.w);
    strength = vertex->optionalData4.w;
    if (jointId > 0) {
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition, 1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal, 0.0)) * strength;
    }
    }
    
    vertPosition = Vector3(pos.x, pos.y, pos.z);
    vertNormal = Vector3(nor.x, nor.y, nor.z);
}
