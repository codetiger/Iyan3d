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

void AnimatedMeshNode::setMesh(AnimatedMesh* mesh, rig_type rigType)
{
    // If Skinned Mesh
    this->mesh = mesh;
    SkinMesh* SMesh = (SkinMesh*)mesh;
    for (int i = 0; i < SMesh->joints->size(); i++) {
        shared_ptr<JointNode> node = make_shared<JointNode>();
        if ((*SMesh->joints)[i]->Parent) {
            unsigned short parentId = (*SMesh->joints)[i]->Parent->Index;
            node->Parent = jointNodes[parentId];
        }
        jointNodes.push_back(node);
    }
    
    if(this->mesh) {
        for(int i = 0; i < mesh->getVerticesCount(); i++) {
            vertexDataHeavy *vData = mesh->getHeavyVertexByIndex(i);
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

    for (int i = 0; i < jointNodes.size(); i++) {
        jointNodes[i]->bBox.calculateEdges();
        shared_ptr<JointNode> node = jointNodes[i];
        if (i == 0)
            node->setParent(shared_from_this());
        for (int j = 0; j < (*SMesh->joints)[i]->childJoints->size(); j++) {
            unsigned short childIndex = (*(*SMesh->joints)[i]->childJoints)[j]->Index;
            node->Children->push_back(jointNodes[childIndex]);
        }
        node->updateBoundingBox();
        //node.reset();
    }
    SMesh->recoverJointsFromMesh(jointNodes);
    if (rigType != NO_RIG && skinType == CPU_SKIN)
        initializeMeshCache(rigType);
}

void AnimatedMeshNode::initializeMeshCache(rig_type rigType)
{

    /*
        meshCache = new Mesh();
    
    for (int vIndex = 0; vIndex < this->mesh->getVerticesCount(); vIndex++) {
        vertexData finalVertData;
        vertexDataHeavy* vertex = this->mesh->getHeavyVertexByIndex(vIndex);

        finalVertData.vertPosition = vertex->vertPosition;
        finalVertData.vertNormal = vertex->vertNormal;
        finalVertData.texCoord1 = vertex->texCoord1;
        finalVertData.optionalData1 = vertex->optionalData4;

        meshCache->addVertex(&finalVertData);
    }

    this->meshCache->indicesType = this->mesh->indicesType;
    if (this->mesh->indicesType == LOW_POLY_INDICES) {
        for (int iIndex = 0; iIndex < this->mesh->getIndicesCount(); iIndex++) {
            this->meshCache->addToLowPolyIndicesArray(this->mesh->getLowPolyIndicesArray()[iIndex]);
        }
    }
    else {
        for (int iIndex = 0; iIndex < this->mesh->getIndicesCount(); iIndex++) {
            this->meshCache->addToHighPolyIndicesArray(this->mesh->getHighPolyIndicesArray()[iIndex]);
        }
    }
     */
}

AnimatedMesh* AnimatedMeshNode::getMesh()
{
    return (AnimatedMesh*)this->mesh;
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
    //If Skinned
    ((SkinMesh*)this->mesh)->transferJointsToMesh(jointNodes);
    ((SkinMesh*)this->mesh)->buildAllGlobalAnimatedMatrices(NULL, NULL);
}

void AnimatedMeshNode::updateMeshCache(rig_type rigType)
{
    /*
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

    for (int vIndex = 0; vIndex < this->mesh->getVerticesCount(); vIndex++) {
        vertexData* finalVertData = this->meshCache->getLiteVertexByIndex(vIndex);
        vertexDataHeavy* vertex = this->mesh->getHeavyVertexByIndex(vIndex);
        
        finalVertData->vertPosition = vertex->vertPosition;
        finalVertData->vertNormal = vertex->vertNormal;
        finalVertData->texCoord1 = vertex->texCoord1;
        finalVertData->optionalData1 = vertex->optionalData4;

        calculateJointTransforms(vertex, jointTransforms, finalVertData->vertPosition, finalVertData->vertNormal);
    }
    jointTransforms.clear();
    this->shouldUpdateMesh = true;
     */
}

void AnimatedMeshNode::updatePartOfMeshCache(rig_type rigType , int jointId)
{
    /*
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
        vertexData* finalVertData = this->meshCache->getLiteVertexByIndex(vIndex);
        vertexDataHeavy* vertex = this->mesh->getHeavyVertexByIndex(vIndex);
        
        finalVertData->vertPosition = vertex->vertPosition;
        finalVertData->vertNormal = vertex->vertNormal;
        finalVertData->texCoord1 = vertex->texCoord1;
        finalVertData->optionalData1 = vertex->optionalData4;
        
        calculateJointTransforms(vertex, jointTransforms, finalVertData->vertPosition, finalVertData->vertNormal);
    }
    
    jointTransforms.clear();
    PaintedVertexIndices.clear();
    
    this->shouldUpdateMesh = true;
     */
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
    if(jointNodes.size() > jointId)
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

void AnimatedMeshNode::calculateJointTransforms(vertexDataHeavy* vertex, vector<Mat4> jointTransforms, Vector3& vertPosition, Vector3& vertNormal , rig_type rigType)
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
