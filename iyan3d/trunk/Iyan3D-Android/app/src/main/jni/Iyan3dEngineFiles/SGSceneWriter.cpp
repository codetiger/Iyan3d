////
////  SGSceneWriter.cpp
////  Iyan3D
////
////  Created by Karthik on 26/12/15.
////  Copyright © 2015 Smackall Games. All rights reserved.
////
//
//#include "SGSceneWriter.h"
//#include "SGEditorScene.cpp"
//
//SGSceneWriter::SGSceneWriter(SceneManager* smgr)
//{
//    this->smgr = smgr;
//}
//
//void SGSceneWriter::saveSceneData(void* scene, std::string *filePath)
//{
//    ofstream outputFile(*filePath,ios::out | ios::binary);
//    FileHelper::resetSeekPosition();
//    writeScene(scene, &outputFile);
//    outputFile.close();
//}
//void SGSceneWriter::writeScene(void* scene, ofstream *filePointer)
//{
//    SGEditorScene *currentScene = (SGEditorScene*)scene;
//    if(!currentScene || !smgr)
//        return;
//
//    writeGlobalInfo(currentScene, filePointer);
//    for(unsigned long i = 0;i < currentScene->nodes.size();i++)
//        currentScene->nodes[i]->writeData(filePointer);
//}
//
//void SGSceneWriter::writeGlobalInfo(void *scene, ofstream *filePointer)
//{
//    SGEditorScene *currentScene = (SGEditorScene*)scene;
//    if(!currentScene || !smgr)
//        return;
//
//    FileHelper::writeInt(filePointer, currentScene->totalFrames);
//    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].x);
//    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].y);
//    FileHelper::writeFloat(filePointer,ShaderManager::lightColor[0].z);
//    FileHelper::writeFloat(filePointer,ShaderManager::shadowDensity);
//    FileHelper::writeFloat(filePointer, currentScene->cameraFOV);
//    FileHelper::writeInt(filePointer, (int)currentScene->nodes.size());
//}
