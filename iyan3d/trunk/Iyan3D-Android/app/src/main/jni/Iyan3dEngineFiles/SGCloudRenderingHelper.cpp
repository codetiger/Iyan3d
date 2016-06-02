//
//  SGCloudRenderingHelper.cpp
//  Iyan3D
//
//  Created by karthik on 18/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include "../SGEngine2/Core/Nodes/ParticleManager.h"
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
    
    FileHelper::writeFloat(&frameFilePtr, scene->cameraFOV);  // Camera FOV

    short nodesCount = (short)scene->nodes.size()-1;
    for (int i = 0; i < (int)scene->nodes.size(); i++) {
        if (!scene->nodes[i]->props.isVisible) {
            nodesCount--;
        } else if(scene->nodes[i]->props.isVisible && scene->nodes[i]->getType() == NODE_PARTICLES) {
        	shared_ptr< ParticleManager > pNode = dynamic_pointer_cast<ParticleManager>(scene->nodes[i]->node);
			nodesCount += pNode->getParticlesCount() - 1;
        }
    }

    FileHelper::writeShort(&frameFilePtr, nodesCount); // Nodes Count
    
    for (int nodeId = 1; nodeId < (int)scene->nodes.size(); nodeId++) {
        SGCloudRenderingHelper *renderHelper = new SGCloudRenderingHelper();
        if(scene->nodes[nodeId]->getType() == NODE_PARTICLES) {
        	shared_ptr< ParticleManager > pNode = dynamic_pointer_cast<ParticleManager>(scene->nodes[nodeId]->node);
            for(int i = 0; i < 48; i++) {
                pNode->update();
//                pNode->sortParticles(scene->renderCamera->getAbsolutePosition());
                pNode->updateParticles(true, scene->renderCamera->getAbsolutePosition());
            }

            Vector4* positions = pNode->getPositions();
            Vector4 props = pNode->getParticleProps();
    	    Vector4 sColor = pNode->startColor;
    	    Vector4 mColor = pNode->midColor;
    	    Vector4 eColor = pNode->endColor;

        	for(int i = 0; i < pNode->getParticlesCount(); i++) {
        	    float percent = positions[i].w/float(props.x);
        	    float phase = float(percent > 0.5);


        	    Vector4 s =  sColor * (1 - phase) + mColor * phase;
        	    Vector4 e = mColor * (1 - phase) + eColor * phase;
        	    float age = (percent * (1 - phase) + (percent - 0.5) * phase) * 2.0;

        	    Vector4 color = s * (1 - age) + e * age;

        		renderHelper->writeNodeData(scene, nodeId, frameId, &frameFilePtr, i, color);
        	}
        } else
        	renderHelper->writeNodeData(scene, nodeId, frameId, &frameFilePtr);

        if(renderHelper)
            delete renderHelper;
     }

    frameFilePtr.close();
    smgr->setActiveCamera(scene->viewCamera);

    return true;
}

void SGCloudRenderingHelper::writeNodeData(SGEditorScene *scene, int nodeId, int frameId, ofstream *frameFilePtr, int particleIndex, Vector4 pColor)
{
    NODE_TYPE nodeType = scene->nodes[nodeId]->getType();
     SGNode *thisNode = scene->nodes[nodeId];

     if(thisNode->props.isVisible) {
         Vector3 vertColor = thisNode->props.vertexColor;

         Vector3 lightDir = Vector3(0.0, -1.0, 0.0);
         if(nodeType == NODE_ADDITIONAL_LIGHT) {
             FileHelper::writeFloat(frameFilePtr, thisNode->props.nodeSpecificFloat/DEFAULT_FADE_DISTANCE); // Emission
         } else
             FileHelper::writeFloat(frameFilePtr, (nodeType == NODE_LIGHT) ? 1.0 : 0.0); // Emission

         Vector3 lightColor = Vector3(0.0);
         if(nodeType == NODE_ADDITIONAL_LIGHT || nodeType == NODE_LIGHT) {
             FileHelper::writeInt(frameFilePtr, thisNode->props.specificInt);
             Quaternion lightRot = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frameId, scene->nodes[nodeId]->rotationKeys);
             Mat4 rotMat;
             rotMat.setRotationRadians(MathHelper::toEuler(lightRot));
             rotMat.rotateVect(lightDir);
             lightColor = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frameId, scene->nodes[nodeId]->scaleKeys);
         }

         FileHelper::writeFloat(frameFilePtr, lightColor.x); //Emission Color r
         FileHelper::writeFloat(frameFilePtr, lightColor.y); //Emission Color g
         FileHelper::writeFloat(frameFilePtr, lightColor.z); //Emission Color b
         FileHelper::writeFloat(frameFilePtr, 0.5); //Emission Radius
         if(nodeType == NODE_PARTICLES) {
             FileHelper::writeFloat(frameFilePtr, pColor.x); // Diffusion Color r
             FileHelper::writeFloat(frameFilePtr, pColor.y); // Diffusion Color g
             FileHelper::writeFloat(frameFilePtr, pColor.z); // Diffusion Color b
         } else {
             FileHelper::writeFloat(frameFilePtr, vertColor.x); // Diffusion Color r
             FileHelper::writeFloat(frameFilePtr, vertColor.y); // Diffusion Color g
             FileHelper::writeFloat(frameFilePtr, vertColor.z); // Diffusion Color b
         }
         
         if (nodeType == NODE_LIGHT || nodeType == NODE_ADDITIONAL_LIGHT) {
             FileHelper::writeFloat(frameFilePtr, lightDir.x); // LightDir
             FileHelper::writeFloat(frameFilePtr, lightDir.y);
             FileHelper::writeFloat(frameFilePtr, lightDir.z);
         }
         
         bool hasTexture = (nodeType == NODE_LIGHT || nodeType == NODE_ADDITIONAL_LIGHT || thisNode->textureName == "-1" || thisNode->textureName == "") ? false : true;
         FileHelper::writeBool(frameFilePtr, hasTexture); // Has Texture

         unsigned long lastSlashPos  = (thisNode->textureName).find_last_of("\\/");
         string textureFileName;
         if(string::npos != lastSlashPos)
             textureFileName = (thisNode->textureName).substr( lastSlashPos + 1) + ".png";
         else
         	textureFileName = thisNode->textureName + ".png";

         FileHelper::writeString(frameFilePtr, textureFileName); // Texture File Name with extension
         FileHelper::writeFloat(frameFilePtr, scene->nodes[nodeId]->props.reflection);
         FileHelper::writeFloat(frameFilePtr, scene->nodes[nodeId]->props.refraction);
         FileHelper::writeFloat(frameFilePtr, scene->nodes[nodeId]->props.transparency);
         FileHelper::writeBool(frameFilePtr, (nodeType == NODE_PARTICLES) ? false : scene->nodes[nodeId]->props.isLighting); // node lighting

         vector<TriangleData> trianglesData;
         if(nodeType == NODE_PARTICLES)
        	 trianglesData = calculateTriangleDataForParticleNode(thisNode, particleIndex);
         else
        	 trianglesData = calculateTriangleDataForNode(thisNode);

         FileHelper::writeInt(frameFilePtr, (int)trianglesData.size());

         for (int index = 0; index < (int)trianglesData.size(); index++) {
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos1.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos1.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos1.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos2.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos2.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos2.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos3.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos3.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Pos3.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal1.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal1.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal1.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal2.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal2.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal2.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal3.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal3.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].Normal3.z);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV1.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV1.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV2.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV2.y);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV3.x);
             FileHelper::writeFloat(frameFilePtr, trianglesData[index].UV3.y);
         }
     }
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
    } else {
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

vector<TriangleData> SGCloudRenderingHelper::calculateTriangleDataForParticleNode(SGNode *sgNode, int index)
{
    vector<TriangleData> trianglesData;
    vector<unsigned int> highPolyIndices;

    unsigned int verticesCount = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh()->getVerticesCount();
    Mesh * currentMesh = dynamic_pointer_cast<MeshNode>(sgNode->node)->getMesh();
    
    unsigned int indicesCount = currentMesh->getTotalIndicesCount();
    highPolyIndices = currentMesh->getTotalIndicesArray();
    
    shared_ptr< ParticleManager > pNode = dynamic_pointer_cast<ParticleManager>(sgNode->node);
    
    highPolyIndices = currentMesh->getTotalIndicesArray();
    
    Vector4* positions = pNode->getPositions();
    Vector4* rotations = pNode->getRotations();

        vector<vertexData> allverticesData;
        for (unsigned int i = 0; i < verticesCount; i++) {
            vertexData *currentVertex = currentMesh->getLiteVertexByIndex(i);
            allverticesData.push_back(calculateFinalVertexDataForParticle(sgNode->node, currentVertex, index, positions[index], rotations[index]));
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
        printf("\n Particles %d updated ", index);
        allverticesData.clear();

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
        for(int i = 0; i < (int)sMesh->joints->size(); ++i){
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

vertexData SGCloudRenderingHelper::calculateFinalVertexDataForParticle(shared_ptr<Node> node , void * vertex, int index, Vector4 position, Vector4 rotation)
{
    vertexData finalVertData;

    finalVertData.vertPosition = ((vertexData*)vertex)->vertPosition;
    finalVertData.vertNormal = ((vertexData*)vertex)->vertNormal;
    finalVertData.texCoord1 = ((vertexData*)vertex)->texCoord1;
    
    shared_ptr<ParticleManager> pNode = dynamic_pointer_cast<ParticleManager>(node);
    Vector4 props = pNode->getParticleProps();
        Mat4 translation = Mat4();
        translation[12] = position.x;
        translation[13] = position.y;
        translation[14] = position.z;
        
        Mat4 rotationMat = Mat4();
        float cr = cos(rotation.x);
        float sr = sin(rotation.x);
        float cp = cos(rotation.y);
        float sp = sin(rotation.y);
        float cy = cos(rotation.z);
        float sy = sin(rotation.z);
        
        rotationMat[0] = (cp * cy);
        rotationMat[1] = (cp * sy);
        rotationMat[2] = (-sp);
        
        float srsp = sr * sp;
        float crsp = cr * sp;
        
        rotationMat[4] = (srsp * cy - cr * sy);
        rotationMat[5] = (srsp * sy + cr * cy);
        rotationMat[6] = (sr * cp);
        
        rotationMat[8] = (crsp * cy + sr * sy);
        rotationMat[9] = (crsp * sy - sr * cy);
        rotationMat[10] = (cr * cp);
        
        float live = float(position.w > 0.0 && position.w <= float(props.x));
        if(live <= 0.0) {
            for(int i = 0; i < 16; i ++)
                translation[i] = 0;
        }
        Mat4 pModel = translation * rotationMat;
        
        Vector4 vPos = pModel * Vector4(finalVertData.vertPosition , 1.0);
        finalVertData.vertPosition = Vector3(vPos.x, vPos.y, vPos.z);

    return finalVertData;
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

