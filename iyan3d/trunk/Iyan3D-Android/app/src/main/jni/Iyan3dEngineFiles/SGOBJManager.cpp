//
//  SGOBJManager.cpp
//  Iyan3D
//
//  Created by Karthik on 14/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "../../SGEngine2/Loaders/CSGRMeshFileLoader.h"
#include "HeaderFiles/SGOBJManager.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene* objScene;

SGOBJManager::SGOBJManager(SceneManager* smgr, void *scene)
{
    this->smgr = smgr;
    objScene = (SGEditorScene*) scene;
    objLoader = new OBJMeshFileLoader();
}

SGOBJManager::~SGOBJManager()
{
    delete objLoader;
    objLoader = NULL;
}

bool SGOBJManager::loadAndSaveAsSGM(string objPath,string textureName, int assetId, bool isVertexColor, Vector3 vColor)
{
    SGNode* objSGNode = new SGNode(NODE_OBJ);
    int objLoadStatus = 0;
    string finalPath = FileHelper::getDocumentsDirectory() + objPath + ".obj";
    Mesh *objMes = objLoader->createMesh(finalPath, objLoadStatus, objScene->shaderMGR->deviceType);
    if(objMes == NULL) {
        printf(" Obj path %s ", finalPath.c_str());
        Logger::log(ERROR, "SGOBJMan ", "Mesh is null");
        return false;
    }
    
    if(isVertexColor) {
        unsigned int vertCount = objMes->getVerticesCount();
        for (unsigned int i = 0; i < vertCount; i++) {
            Vector4 *optionalData1 = &(objMes->getLiteVertexByIndex(i)->optionalData1);
            (*optionalData1) = Vector4(vColor.x, vColor.y, vColor.z,1.0);
        }
        objMes->Commit();
    }

    shared_ptr<MeshNode> objNode = smgr->createNodeFromMesh(objMes,"ObjUniforms");
    objSGNode->node = objNode;
    
    if(!isVertexColor) {
        string texturePath = FileHelper::getDocumentsDirectory() + textureName + ".png";
        Texture *nodeTex = smgr->loadTexture(texturePath,texturePath,TEXTURE_RGBA8,TEXTURE_BYTE);
        objNode->setTexture(nodeTex,1);
        objNode->setTexture(objScene->shadowTexture,2);
    }
    // scale to fit all obj in same proportion-----
    float extendX = objNode->getMesh()->getBoundingBox()->getXExtend();
    float extendY = objNode->getMesh()->getBoundingBox()->getYExtend();
    float extendZ = objNode->getMesh()->getBoundingBox()->getZExtend();
    float max = ((extendX >= extendY) ? extendX:extendY);
    max = ((max >= extendZ) ? max:extendZ);
    float scaleRatio = (max / OBJ_BOUNDINGBOX_MAX_LIMIT);
    objNode->setScale(Vector3(1.0/scaleRatio));
    //-----------
    
    objNode->setMaterial(smgr->getMaterialByIndex((isVertexColor) ? SHADER_VERTEX_COLOR_L1 : SHADER_COMMON_L1));
    objSGNode->props.isLighting = true;
    
    objNode->setID(OBJ_ID);
    objSGNode->node->updateAbsoluteTransformation();
    if(objMes == NULL){
        Logger::log(ERROR, "Loaded OBJ is NULL", "SGAutoRigScene");
        return false;
    }
    
     return writeSGM(to_string(assetId) , objSGNode, !isVertexColor);
}

bool SGOBJManager::writeSGM(string filePath, SGNode *objNode, bool hasUV)
{
    string outputPath = FileHelper::getDocumentsDirectory()+"/Resources/Sgm/"+filePath + ".sgm";
    ofstream f(outputPath , ios::binary);
    
    unsigned char versionIdentifier = (hasUV) ? UV_MAPPED : VERTEX_COLORED;
    f.write((char*)&versionIdentifier, sizeof(unsigned char));
    SSGMCountHeaderLowPoly counts;
    Mesh* objMesh = dynamic_pointer_cast<MeshNode>(objNode->node)->mesh;
    counts.vertCount = objMesh->getVerticesCount();
    counts.indCount = objMesh->getTotalIndicesCount();
    counts.colCount = objMesh->getVerticesCount();
    
    f.write((char*)&counts, sizeof(SSGMCountHeaderLowPoly));
    
    SSGMVectHeader* verts = new SSGMVectHeader[counts.vertCount];
    for (int i = 0; i < counts.vertCount; i++) {
        vertexData * vData = objMesh->getLiteVertexByIndex(i);
        verts[i].vx = vData->vertPosition.x;
        verts[i].vy = vData->vertPosition.y;
        verts[i].vz = vData->vertPosition.z;
        verts[i].nx = vData->vertNormal.x;
        verts[i].ny = vData->vertNormal.y;
        verts[i].nz = vData->vertNormal.z;
        f.write((char*)&verts[i], sizeof(SSGMVectHeader));
    }
    
    SSGMUVHeader* texs = new SSGMUVHeader[counts.colCount];
    SSGMColHeader* col = new SSGMColHeader[counts.colCount];
    if (hasUV) {
        for (int i = 0; i < counts.colCount; i++) {
            vertexData * vData = objMesh->getLiteVertexByIndex(i);
            texs[i].s = vData->texCoord1.x;
            texs[i].t = -vData->texCoord1.y;
            f.write((char*)&texs[i], sizeof(SSGMUVHeader));
        }
    }
    else {
        for (int i = 0; i < counts.colCount; i++) {
            vertexData * vData = objMesh->getLiteVertexByIndex(i);
            col[i].r = vData->optionalData1.x*255;
            col[i].g = vData->optionalData1.y*255;
            col[i].b = vData->optionalData1.z*255;
            f.write((char*)&col[i], sizeof(SSGMColHeader));
        }
    }
    
    for (int i = 0; i < counts.indCount; i++) {
        SSGMIndexHeaderLowPoly ind;
        vector<unsigned int> allInd = objMesh->getTotalIndicesArray();
        ind.vtInd = allInd[i];
        ind.colInd = allInd[i];

        f.write((char*)&ind, sizeof(SSGMIndexHeaderLowPoly));
    }
    delete verts;
    delete texs;
    delete col;
    
    f.close();

    if(objNode->node) {
        smgr->RemoveNode(objNode->node);
    }
    
    if(objNode)
        delete objNode;
    objNode = NULL;
    
    return true;
}
