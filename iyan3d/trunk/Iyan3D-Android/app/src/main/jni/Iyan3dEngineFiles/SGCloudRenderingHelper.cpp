//
//  SGCloudRenderingHelper.cpp
//  Iyan3D
//
//  Created by karthik on 18/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGCloudRenderingHelper.h"

SGCloudRenderingHelper::SGCloudRenderingHelper()
{
    
}

bool SGCloudRenderingHelper::writeFrameData(SGEditorScene *scene , SceneManager *smgr, int frameId)
{
    string outputFileName =  FileHelper::getDocumentsDirectory() + to_string(frameId) + ".sgfd";
    ofstream frameFilePtr(outputFileName , ios::binary);
    scene->renHelper->setRenderCameraOrientation();
    Vector3 camPos = scene->renderCamera->getAbsolutePosition();
    
    FileHelper::writeFloat(&frameFilePtr, camPos.x);
    FileHelper::writeFloat(&frameFilePtr, camPos.y);
    FileHelper::writeFloat(&frameFilePtr, camPos.z);
    Vector3 camTarget = scene->renderCamera->getTarget();
    
    FileHelper::writeFloat(&frameFilePtr, camTarget.x);
    FileHelper::writeFloat(&frameFilePtr, camTarget.y);
    FileHelper::writeFloat(&frameFilePtr, camTarget.z);
    
    Vector3 camRotation = scene->nodes[NODE_CAMERA]->node->getRotationInDegrees();
    camRotation += 180.0f;
    
    FileHelper::writeFloat(&frameFilePtr, camRotation.x);
    FileHelper::writeFloat(&frameFilePtr, camRotation.y);
    FileHelper::writeFloat(&frameFilePtr, camRotation.z);
    
    FileHelper::writeFloat(&frameFilePtr, (scene->renderCamera->getFOVInRadians() * RADTODEG));  // Camera FOV

    short nodesCount = (short)scene->nodes.size()-1;
    for (int i = 0; i < (int)scene->nodes.size(); i++) {
        if (!scene->nodes[i]->props.isVisible) {
            nodesCount--;
        }
    }

    FileHelper::writeShort(&frameFilePtr, nodesCount); // Nodes Count
    
    for (int nodeId = 1; nodeId < (int)scene->nodes.size(); nodeId++) {

        NODE_TYPE nodeType = scene->nodes[nodeId]->getType();
        SGNode *thisNode = scene->nodes[nodeId];

        if(thisNode->props.isVisible) {
            Vector3 vertColor = thisNode->props.vertexColor;
            
            Quaternion lightProp;
            if(nodeType == NODE_ADDITIONAL_LIGHT) {
                lightProp = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frameId, scene->nodes[nodeId]->rotationKeys, true);
                FileHelper::writeFloat(&frameFilePtr, lightProp.w/DEFAULT_FADE_DISTANCE); // Emission
            } else
                FileHelper::writeFloat(&frameFilePtr, (nodeType == NODE_LIGHT) ? 1.0 : 0.0); // Emission
            
            Vector3 lightColor;
            if(nodeType == NODE_ADDITIONAL_LIGHT) {
                lightColor = Vector3(lightProp.x,lightProp.y,lightProp.z);
            } else {
                lightColor = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frameId, scene->nodes[nodeId]->scaleKeys);
            }
            
            FileHelper::writeFloat(&frameFilePtr, lightColor.x); //Emission Color r
            FileHelper::writeFloat(&frameFilePtr, lightColor.y); //Emission Color g
            FileHelper::writeFloat(&frameFilePtr, lightColor.z); //Emission Color b
            FileHelper::writeFloat(&frameFilePtr, 0.5); //Emission Radius
            FileHelper::writeFloat(&frameFilePtr, vertColor.x); // Diffusion Color r
            FileHelper::writeFloat(&frameFilePtr, vertColor.y); // Diffusion Color g
            FileHelper::writeFloat(&frameFilePtr, vertColor.z); // Diffusion Color b
            FileHelper::writeBool(&frameFilePtr, (nodeType == NODE_LIGHT || nodeType == NODE_ADDITIONAL_LIGHT || thisNode->textureName == "") ? false : true); // Has Texture
            
            unsigned long lastSlashPos  = (thisNode->textureName).find_last_of("\\/");
            string textureFileName;
            if(string::npos != lastSlashPos)
                textureFileName = (thisNode->textureName).substr( lastSlashPos + 1);
            else
            	textureFileName = thisNode->textureName;
            
            FileHelper::writeString(&frameFilePtr, textureFileName); // Texture File Name with extension
            FileHelper::writeFloat(&frameFilePtr, scene->nodes[nodeId]->props.reflection);
            FileHelper::writeFloat(&frameFilePtr, scene->nodes[nodeId]->props.refraction);
            FileHelper::writeFloat(&frameFilePtr, scene->nodes[nodeId]->props.transparency);

            SGCloudRenderingHelper *renderHelper = new SGCloudRenderingHelper();
            vector<TriangleData> trianglesData =   renderHelper->calculateTriangleDataForNode(thisNode);
            
            FileHelper::writeInt(&frameFilePtr, (int)trianglesData.size());
            
            for (int index = 0; index < trianglesData.size(); index++) {
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos1.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos1.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos1.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos2.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos2.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos2.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos3.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos3.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Pos3.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal1.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal1.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal1.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal2.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal2.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal2.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal3.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal3.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].Normal3.z);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV1.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV1.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV2.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV2.y);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV3.x);
                FileHelper::writeFloat(&frameFilePtr, trianglesData[index].UV3.y);
            }
            
            if(renderHelper)
                delete renderHelper;
        }
    }
    
    frameFilePtr.close();
    smgr->setActiveCamera(scene->viewCamera);
}

vector<TriangleData> SGCloudRenderingHelper::calculateTriangleDataForNode(SGNode *sgNode)
{
    vector<TriangleData> trianglesData;
    
    vector<vertexData> allverticesData;
    
    vector<unsigned int> highPolyIndices;
    
    //TODO Write according to new meshbuffer implementation
    
    if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_RIG) {
        unsigned int verticesCount = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->getMesh()->getVerticesCount();
        sgNode->node->updateAbsoluteTransformation();
        sgNode->node->updateAbsoluteTransformationOfChildren();
        dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->update();
        Mesh * currentMesh = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->getMesh();
        
        unsigned int indicesCount = currentMesh->getTotalIndicesCount();
        
        highPolyIndices = currentMesh->getTotalIndicesArray();
        
        if(currentMesh->meshType == MESH_TYPE_HEAVY) {
            for (unsigned int index = 0; index < verticesCount; index++) {
                vertexDataHeavy *currentVertex = currentMesh->getHeavyVertexByIndex(index);
                allverticesData.push_back(calculateFinalVertexData(sgNode->node, currentVertex));
            }
        } else {
            for (unsigned int index = 0; index < verticesCount; index++) {
                vertexData *currentVertex = currentMesh->getLiteVertexByIndex(index);
                allverticesData.push_back(calculateFinalVertexData(sgNode->node, currentVertex));
            }
        }
        
        for(unsigned int i = 0; i < indicesCount; i+=3) {
            TriangleData tData;
            tData.Pos1 = allverticesData[highPolyIndices[i]].vertPosition;
            tData.Pos2 = allverticesData[highPolyIndices[i+1]].vertPosition;
            tData.Pos3 = allverticesData[highPolyIndices[i+2]].vertPosition;
            tData.Normal1 = allverticesData[highPolyIndices[i]].vertNormal;
            tData.Normal2 = allverticesData[highPolyIndices[i+1]].vertNormal;
            tData.Normal3 = allverticesData[highPolyIndices[i+2]].vertNormal;
            tData.UV1 = allverticesData[highPolyIndices[i]].texCoord1;
            tData.UV2 = allverticesData[highPolyIndices[i+1]].texCoord1;
            tData.UV3 = allverticesData[highPolyIndices[i+2]].texCoord1;
            trianglesData.push_back(tData);
        }
    }
    else {
        unsigned int verticesCount = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh()->getVerticesCount();
        Mesh * currentMesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();
        
        unsigned int indicesCount = currentMesh->getTotalIndicesCount();
        
        highPolyIndices = currentMesh->getTotalIndicesArray();
        
        if(currentMesh->meshType == MESH_TYPE_HEAVY) {
            for (unsigned int index = 0; index < verticesCount; index++) {
                vertexDataHeavy *currentVertex = currentMesh->getHeavyVertexByIndex(index);
                allverticesData.push_back(calculateFinalVertexData(sgNode->node, currentVertex));
            }
        } else {
            for (unsigned int index = 0; index < verticesCount; index++) {
                vertexData *currentVertex = currentMesh->getLiteVertexByIndex(index);
                allverticesData.push_back(calculateFinalVertexData(sgNode->node, currentVertex));
            }
        }
        for(unsigned int i = 0; i < indicesCount; i+=3) {
            TriangleData tData;
            tData.Pos1 = allverticesData[highPolyIndices[i]].vertPosition;
            tData.Pos2 = allverticesData[highPolyIndices[i+1]].vertPosition;
            tData.Pos3 = allverticesData[highPolyIndices[i+2]].vertPosition;
            tData.Normal1 = allverticesData[highPolyIndices[i]].vertNormal;
            tData.Normal2 = allverticesData[highPolyIndices[i+1]].vertNormal;
            tData.Normal3 = allverticesData[highPolyIndices[i+2]].vertNormal;
            tData.UV1 = allverticesData[highPolyIndices[i]].texCoord1;
            tData.UV2 = allverticesData[highPolyIndices[i+1]].texCoord1;
            tData.UV3 = allverticesData[highPolyIndices[i+2]].texCoord1;
            trianglesData.push_back(tData);
        }
    }
    
    return trianglesData;
}

vertexData SGCloudRenderingHelper::calculateFinalVertexData(shared_ptr<Node> node , void * vertex)
{
    vertexData finalVertData;
    
    Mesh * mesh = dynamic_pointer_cast<MeshNode>(node)->getMesh();
    
    if (mesh->meshType == MESH_TYPE_HEAVY) {
        
        finalVertData.vertPosition = ((vertexDataHeavy*)vertex)->vertPosition;
        finalVertData.vertNormal = ((vertexDataHeavy*)vertex)->vertNormal;
        finalVertData.texCoord1 = ((vertexDataHeavy*)vertex)->texCoord1;
        
        SkinMesh *sMesh = (SkinMesh*)mesh;
        vector<Mat4> jointTransforms;
        for(int i = 0; i < sMesh->joints->size(); ++i){
            Mat4 JointVertexPull;
            JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix,(*sMesh->joints)[i]->GlobalInversedMatrix);
            jointTransforms.push_back(JointVertexPull);
        }
        calculateJointTransforms(((vertexDataHeavy*)vertex), jointTransforms, finalVertData.vertPosition, finalVertData.vertNormal);
        
        Vector4 finalPos = node->getModelMatrix() * Vector4(finalVertData.vertPosition , 1.0);
        Vector4 finalNorm = (node->getModelMatrix() * Vector4(finalVertData.vertNormal , 0.0).normalize()).normalize();
        
        finalVertData.vertPosition = Vector3(finalPos.x , finalPos.y ,finalPos.z);
        finalVertData.vertNormal = Vector3(finalNorm.x , finalNorm.y , finalNorm.z);
        
        return finalVertData;
        
    } else {
        finalVertData.vertPosition = ((vertexData*)vertex)->vertPosition;
        finalVertData.vertNormal = ((vertexData*)vertex)->vertNormal;
        finalVertData.texCoord1 = ((vertexData*)vertex)->texCoord1;
        
        Mat4 model = node->getModelMatrix();
//        printf("\n Translation %f %f %f ", model[12],model[13],model[14]);

        Vector4 finalPos = model * Vector4(finalVertData.vertPosition , 1.0);
        Vector4 finalNorm = (model * Vector4(finalVertData.vertNormal , 0.0).normalize()).normalize();

        finalVertData.vertPosition = Vector3(finalPos.x , finalPos.y ,finalPos.z);
        finalVertData.vertNormal = Vector3(finalNorm.x , finalNorm.y , finalNorm.z);

        return finalVertData;
    }
}

void SGCloudRenderingHelper::calculateJointTransforms(vertexDataHeavy *vertex , vector<Mat4> jointTransforms , Vector3 &vertPosition, Vector3 &vertNormal)
{
    Vector4 pos = Vector4(0.0);
    Vector4 nor = Vector4(0.0);
    
    int jointId = int(vertex->optionalData1.x);
    float strength = vertex->optionalData2.x ;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }else{
        pos = Vector4(vertPosition,1.0);
        nor = Vector4(vertNormal,0.0);
    }
    
    jointId = int(vertex->optionalData1.y);
    strength = vertex->optionalData2.y;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData1.z);
    strength = vertex->optionalData2.z;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData1.w);
    strength = vertex->optionalData2.w;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData3.x);
    strength = vertex->optionalData4.x;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData3.y);
    strength = vertex->optionalData4.y;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData3.z);
    strength = vertex->optionalData4.z;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( vertex->optionalData3.w);
    strength = vertex->optionalData4.w;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * Vector4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * Vector4(vertNormal,0.0)) * strength;
    }

    vertPosition = Vector3(pos.x , pos.y, pos.z);
    vertNormal = Vector3(nor.x, nor.y,nor.z);
}

void SGCloudRenderingHelper::copyMat(float* pointer,Mat4& mat){
    for(int i = 0;i < 16;++i)
        *pointer++ = mat[i];
}

