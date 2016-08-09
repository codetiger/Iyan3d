//
//  SGSceneWriter.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGSceneWriter.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene *writingScene;

SGSceneWriter::SGSceneWriter(SceneManager* smgr, void* scene)
{
    this->smgr = smgr;
    writingScene = (SGEditorScene*)scene;
    exporter = new Assimp::Exporter();
}

SGSceneWriter::~SGSceneWriter()
{
    delete exporter;
}

void SGSceneWriter::saveSceneData(std::string *filePath)
{
    if(!writingScene || !smgr)
        return;
    
    ofstream outputFile(*filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    writeScene(&outputFile);
    outputFile.close();
}
void SGSceneWriter::writeScene(ofstream *filePointer)
{
    if(!writingScene || !smgr)
        return;
    
    writeGlobalInfo(filePointer);
    for(unsigned long i = 0;i < writingScene->nodes.size();i++) {
        writingScene->nodes[i]->writeData(filePointer, writingScene->nodes);
    }
}

void SGSceneWriter::writeGlobalInfo(ofstream *filePointer)
{
    if(!writingScene || !smgr)
        return;
    
    FileHelper::writeInt(filePointer, SGB_VERSION_CURRENT);
    FileHelper::writeInt(filePointer, 0); // dofNear
    FileHelper::writeInt(filePointer, 5000); // dofFar
    FileHelper::writeInt(filePointer, writingScene->totalFrames);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].x);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].y);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].z);
    FileHelper::writeFloat(filePointer,ShaderManager::shadowDensity);
    FileHelper::writeFloat(filePointer, writingScene->cameraFOV);
    
    //SCENE PROPERTIES
    
    FileHelper::writeFloat(filePointer, writingScene->shaderMGR->getProperty(AMBIENT_LIGHT).value.x);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeFloat(filePointer, -1.0);
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeVector4(filePointer, Vector4(-1.0));
    FileHelper::writeString(filePointer, writingScene->shaderMGR->getProperty(ENVIRONMENT_TEXTURE).fileName);
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
    FileHelper::writeString(filePointer, " ");
    
    FileHelper::writeInt(filePointer, (int)writingScene->nodes.size());
}

void SGSceneWriter::saveSceneDataAssImp(std::string *filePath)
{
    if(!writingScene || !smgr)
        return;
    
    printf("Saving File: %s\n", filePath->c_str());
    printf("\nExport File Format Support:\n");
    for(int i = 0; i < exporter->GetExportFormatCount(); i++) {
        printf("%s - %s - %s\n", exporter->GetExportFormatDescription(i)->id, exporter->GetExportFormatDescription(i)->fileExtension, exporter->GetExportFormatDescription(i)->description);
    }

    vector< aiNode* > nodes;
    vector< aiMesh* > meshes;
    vector< aiMaterial* > materials;
    vector< aiTexture* > textures;
    vector< aiLight* > lights;

    aiScene scene;

    for(int i = 0; i < writingScene->nodes.size(); i++) {
        writingScene->nodes[i]->addAINodeToSave(scene, nodes, meshes, materials, textures, lights);
    }

    scene.mRootNode = new aiNode();
    scene.mRootNode->mNumChildren = nodes.size();
    scene.mRootNode->mChildren = new aiNode*[nodes.size()];
    
    scene.mNumLights = lights.size();
    scene.mLights = new aiLight*[lights.size()];

    for(int i = 0; i < lights.size(); i++) {
        scene.mLights[i] = lights[i];
    }
    
    for(int i = 0; i < nodes.size(); i++) {
        aiNode *child = nodes[i];
        child->mParent = scene.mRootNode;
        scene.mRootNode->mChildren[i] = nodes[i];
    }
    
    scene.mNumMaterials = materials.size();
    scene.mMaterials = new aiMaterial*[materials.size()];

    for(int i = 0; i < materials.size(); i++) {
        scene.mMaterials[i] = new aiMaterial();
    }

    scene.mNumMeshes = meshes.size();
    scene.mMeshes = new aiMesh*[meshes.size()];
    
    for(int i = 0; i < meshes.size(); i++) {
        scene.mMeshes[i] = meshes[i];
        scene.mMeshes[i]->mMaterialIndex = 0;
    }
    
    aiReturn r = exporter->Export(&scene, exporter->GetExportFormatDescription(1)->id, *filePath);
    if(r != AI_SUCCESS)
        printf("Error in Loading: %s\n", exporter->GetErrorString());
}

