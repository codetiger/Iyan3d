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
}

SGSceneWriter::~SGSceneWriter(){
    
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
    for(unsigned long i = 0;i < writingScene->nodes.size();i++)
        writingScene->nodes[i]->writeData(filePointer);
}

void SGSceneWriter::writeGlobalInfo(ofstream *filePointer)
{
    if(!writingScene || !smgr)
        return;

    FileHelper::writeInt(filePointer, writingScene->totalFrames);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].x);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].y);
    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].z);
    FileHelper::writeFloat(filePointer,ShaderManager::shadowDensity);
    FileHelper::writeFloat(filePointer, writingScene->cameraFOV);
    FileHelper::writeInt(filePointer, (int)writingScene->nodes.size());
}
