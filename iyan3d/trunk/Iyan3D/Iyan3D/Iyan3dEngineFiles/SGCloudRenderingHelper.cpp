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

bool SGCloudRenderingHelper::writeFrameData(SGAnimationScene *scene , int frameId)
{
    string outputFileName =  FileHelper::getDocumentsDirectory() + to_string(frameId) + ".sgfd";
    ofstream frameFilePtr(outputFileName , ios::binary);
    
    Vector3 camPos = scene->smgr->getActiveCamera()->getAbsolutePosition();
    
    FileHelper::writeFloat(&frameFilePtr, camPos.x);
    FileHelper::writeFloat(&frameFilePtr, camPos.y);
    FileHelper::writeFloat(&frameFilePtr, camPos.z);
    Vector3 camTarget = scene->smgr->getActiveCamera()->getTarget();
    
    FileHelper::writeFloat(&frameFilePtr, camTarget.x);
    FileHelper::writeFloat(&frameFilePtr, camTarget.y);
    FileHelper::writeFloat(&frameFilePtr, camTarget.z);
    
    FileHelper::writeFloat(&frameFilePtr, (scene->smgr->getActiveCamera()->getFOVInRadians() * RADTODEG));  // Camera FOV
    
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
        Vector4 vertColor;
        if(nodeType == NODE_TEXT)
        Vector4 vertColor = dynamic_pointer_cast<AnimatedMeshNode>(thisNode->node)->getMesh()->getHeavyVertexByIndex(0)->optionalData4;
        
        FileHelper::writeFloat(&frameFilePtr, (nodeType == NODE_LIGHT) ? 1.0 : 0.0); // Emission
            FileHelper::writeFloat(&frameFilePtr, ShaderManager::lightColor[0].x); //Emission Color r
        FileHelper::writeFloat(&frameFilePtr, ShaderManager::lightColor[0].y); //Emission Color g
        FileHelper::writeFloat(&frameFilePtr, ShaderManager::lightColor[0].z); //Emission Color b
        FileHelper::writeFloat(&frameFilePtr, 20.0); //Emission Radius
        FileHelper::writeFloat(&frameFilePtr, (nodeType == NODE_TEXT) ? vertColor.x : 1.0); // Diffusion Color r
        FileHelper::writeFloat(&frameFilePtr, (nodeType == NODE_TEXT) ? vertColor.y : 1.0); // Diffusion Color g
        FileHelper::writeFloat(&frameFilePtr, (nodeType == NODE_TEXT) ? vertColor.z : 1.0); // Diffusion Color b
        FileHelper::writeBool(&frameFilePtr, (nodeType == NODE_LIGHT) ? false : true); // Has Texture
        
            unsigned long lastSlashPos  = (scene->nodes[nodeId]->node->getActiveTexture()->textureName).find_last_of("\\/");
            string textureFileName;
            if(string::npos != lastSlashPos)
            {
                textureFileName = (scene->nodes[nodeId]->node->getActiveTexture()->textureName).substr( lastSlashPos + 1);
            }
            printf(" Texture file name %s " , textureFileName.c_str());
            
        FileHelper::writeString(&frameFilePtr, textureFileName); // Texture File Name with extension
        FileHelper::writeFloat(&frameFilePtr, scene->nodes[nodeId]->props.shininess);
        FileHelper::writeFloat(&frameFilePtr, scene->nodes[nodeId]->props.transparency);
        
        SGCloudRenderingHelper *renderHelper = new SGCloudRenderingHelper();
        vector<TriangleData> trianglesData =   renderHelper->calculateTriangleDataForNode(thisNode);
        printf("\n Triangles %lu samp %f ", trianglesData.size() , trianglesData[0].Pos1.x);
        
        FileHelper::writeInt(&frameFilePtr, (int)trianglesData.size());
        
        for (int index = 0; index < trianglesData.size(); index++) {
            printf("\n triangle pos %f %f %f ",trianglesData[index].Pos2.x,trianglesData[index].Pos2.y,trianglesData[index].Pos2.z);
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
    }
    }
    
    frameFilePtr.close();
}

vector<TriangleData> SGCloudRenderingHelper::calculateTriangleDataForNode(SGNode *sgNode)
{
    vector<TriangleData> trianglesData;
    
    vector<vertexData> allverticesData;
    
    vector<unsigned int> highPolyIndices;
    vector<unsigned short> lowPolyIndices;
    
    //TODO Write according to new meshbuffer implementation
    
    /*
    if (sgNode->getType() == NODE_TEXT || sgNode->getType() == NODE_RIG) {
        unsigned int verticesCount = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->getMesh()->getVerticesCount();
        Mesh * currentMesh = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->getMesh();
        
        unsigned int indicesCount = currentMesh->getIndicesCount();
        
        if(currentMesh->indicesType == LOW_POLY_INDICES) lowPolyIndices = currentMesh->lowPolyIndices;
        else highPolyIndices = currentMesh->highPolyIndices;
        
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
        
        if(currentMesh->indicesType == LOW_POLY_INDICES) {
            for(unsigned int i = 0; i < indicesCount; i+=3) {
                TriangleData tData;
                tData.Pos1 = allverticesData[lowPolyIndices[i]].vertPosition;
                tData.Pos2 = allverticesData[lowPolyIndices[i+1]].vertPosition;
                tData.Pos3 = allverticesData[lowPolyIndices[i+2]].vertPosition;
                tData.Normal1 = allverticesData[lowPolyIndices[i]].vertNormal;
                tData.Normal2 = allverticesData[lowPolyIndices[i+1]].vertNormal;
                tData.Normal3 = allverticesData[lowPolyIndices[i+2]].vertNormal;
                tData.UV1 = allverticesData[lowPolyIndices[i]].texCoord1;
                tData.UV2 = allverticesData[lowPolyIndices[i+1]].texCoord1;
                tData.UV3 = allverticesData[lowPolyIndices[i+2]].texCoord1;
                trianglesData.push_back(tData);
            }
        } else {
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
    }
    else {
    unsigned int verticesCount = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh()->getVerticesCount();
    Mesh * currentMesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();
    
    unsigned int indicesCount = currentMesh->getIndicesCount();
    
    if(currentMesh->indicesType == LOW_POLY_INDICES) lowPolyIndices = currentMesh->lowPolyIndices;
    else highPolyIndices = currentMesh->highPolyIndices;
    
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
    
    if(currentMesh->indicesType == LOW_POLY_INDICES) {
        for(unsigned int i = 0; i < indicesCount; i+=3) {
            TriangleData tData;
            tData.Pos1 = allverticesData[lowPolyIndices[i]].vertPosition;
            tData.Pos2 = allverticesData[lowPolyIndices[i+1]].vertPosition;
            tData.Pos3 = allverticesData[lowPolyIndices[i+2]].vertPosition;
            tData.Normal1 = allverticesData[lowPolyIndices[i]].vertNormal;
            tData.Normal2 = allverticesData[lowPolyIndices[i+1]].vertNormal;
            tData.Normal3 = allverticesData[lowPolyIndices[i+2]].vertNormal;
            tData.UV1 = allverticesData[lowPolyIndices[i]].texCoord1;
            tData.UV2 = allverticesData[lowPolyIndices[i+1]].texCoord1;
            tData.UV3 = allverticesData[lowPolyIndices[i+2]].texCoord1;
            trianglesData.push_back(tData);
        }
    } else {
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
    }
    */
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
        float* JointArray = new float[sMesh->joints->size() * 16];
        int copyIncrement = 0;
        for(int i = 0; i < sMesh->joints->size(); ++i){
            Mat4 JointVertexPull;
            JointVertexPull.setbyproduct((*sMesh->joints)[i]->GlobalAnimatedMatrix,(*sMesh->joints)[i]->GlobalInversedMatrix);
            copyMat(JointArray + copyIncrement,JointVertexPull);
            copyIncrement += 16;
        }
        calculateJointTransforms(((vertexDataHeavy*)vertex), JointArray, finalVertData.vertPosition, finalVertData.vertNormal);
        
        Vector4 finalPos = node->getModelMatrix() * Vector4(finalVertData.vertPosition , 1.0);
        Vector4 finalNorm = (node->getModelMatrix() * Vector4(finalVertData.vertNormal , 0.0).normalize()).normalize();
        
        finalVertData.vertPosition = Vector3(finalPos.x , finalPos.y ,finalPos.z);
        finalVertData.vertNormal = Vector3(finalNorm.x , finalNorm.y , finalNorm.z);
        
        return finalVertData;
        
    } else {
        finalVertData.vertPosition = ((vertexData*)vertex)->vertPosition;
        finalVertData.vertNormal = ((vertexData*)vertex)->vertNormal;
        finalVertData.texCoord1 = ((vertexData*)vertex)->texCoord1;
        
        node->updateAbsoluteTransformation();
        Mat4 model = node->getModelMatrix();
//        printf("\n Translation %f %f %f ", model[12],model[13],model[14]);

        Vector4 finalPos = model * Vector4(finalVertData.vertPosition , 1.0);
        Vector4 finalNorm = (model * Vector4(finalVertData.vertNormal , 0.0).normalize()).normalize();

        finalVertData.vertPosition = Vector3(finalPos.x , finalPos.y ,finalPos.z);
        finalVertData.vertNormal = Vector3(finalNorm.x , finalNorm.y , finalNorm.z);

        return finalVertData;
    }
}

void SGCloudRenderingHelper::calculateJointTransforms(vertexDataHeavy *vertex , float * jointTransforms , Vector3 &vertPosition, Vector3 &vertNormal)
{
    Vector4 pos = Vector4(0.0);
    Vector4 nor = Vector4(0.0);
    
    int jointId = int(vertex->optionalData1.x);
    float strength = vertex->optionalData2.x ;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }else{
        pos = Vector4(vertPosition,1.0);
        nor = Vector4(vertNormal,0.0);
    }
    
    jointId = int(vertex->optionalData1.y);
    strength = vertex->optionalData2.y;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData1.z);
    strength = vertex->optionalData2.z;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData1.w);
    strength = vertex->optionalData2.w;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData3.x);
    strength = vertex->optionalData4.x;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData3.y);
    strength = vertex->optionalData4.y;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData3.z);
    strength = vertex->optionalData4.z;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }
    
    jointId = int( vertex->optionalData3.w);
    strength = vertex->optionalData4.w;
    if(jointId > 0){
        pos = pos + (Vector4(vertPosition,1.0) * jointTransforms[jointId - 1]) * strength;
        nor = nor + (Vector4(vertNormal,0.0) * jointTransforms[jointId - 1]) * strength;
    }

    vertPosition = Vector3(pos.x , pos.y, pos.z);
    vertNormal = Vector3(nor.x, nor.y,nor.z);
}

void SGCloudRenderingHelper::copyMat(float* pointer,Mat4& mat){
    for(int i = 0;i < 16;++i)
        *pointer++ = mat[i];
}

