//
//  SGSceneLoader.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGSceneLoader.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene *currentScene;

SGSceneLoader::SGSceneLoader(SceneManager* sceneMngr, void* scene)
{
    this->smgr = sceneMngr;
    currentScene = (SGEditorScene*)scene;
}

SGSceneLoader::~SGSceneLoader()
{
    
}

bool SGSceneLoader::loadSceneData(std::string *filePath)
{
    if(!currentScene || !smgr)
        return false;

    ifstream inputSGBFile(*filePath,ios::in | ios::binary );
    FileHelper::resetSeekPosition();
    if(!readScene(&inputSGBFile)){
        inputSGBFile.close();
        return false;
    }
    inputSGBFile.close();
    return true;
}

bool SGSceneLoader::readScene(ifstream *filePointer)
{
    if(!currentScene || !smgr)
        return false;
    
    int nodeCount = 0;
    int sgbVersion = readSceneGlobalInfo(filePointer, nodeCount);
    currentScene->totalFrames = (currentScene->totalFrames < 24) ? 24 : currentScene->totalFrames;
    currentScene->nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;
    
    vector<SGNode*> tempNodes;
    
    if(sgbVersion == SGB_VERSION_CURRENT) {
        for(int i = 0; i < nodeCount; i++) {
            SGNode *sgNode = new SGNode(NODE_UNDEFINED);
            int origId = 0;
            Mesh* mesh = sgNode->readData(filePointer, origId);
            sgNode->materialProps.push_back(new MaterialProperty(sgNode->getType()));

            if(sgNode->getType() == NODE_CAMERA || sgNode->getType() == NODE_LIGHT) {
                loadNode(sgNode, OPEN_SAVED_FILE);
                sgNode->setPropertiesOfNode();
                
            } else {
                if(mesh->meshType == MESH_TYPE_LITE)
                    sgNode->node = smgr->createNodeFromMesh(mesh, "setUniforms");
                else
                    sgNode->node = smgr->createAnimatedNodeFromMesh((AnimatedMesh*)mesh, "setUniforms", ShaderManager::maxJoints, CHARACTER_RIG, mesh->meshType);
                
                sgNode->node->setMaterial(smgr->getMaterialByIndex((mesh->meshType == MESH_TYPE_LITE) ? SHADER_MESH : SHADER_SKIN));
                sgNode->setPropertiesOfNode();
                string textureName = sgNode->getProperty(TEXTURE).fileName;
                Texture * texture = smgr->loadTexture(textureName, FileHelper::getTexturesDirectory() + textureName + ".png", TEXTURE_RGBA8, TEXTURE_BYTE, sgNode->getProperty(TEXTURE_SMOOTH).value.x);
                sgNode->materialProps[0]->setTextureForType(texture, NODE_TEXTURE_TYPE_COLORMAP);
                currentScene->nodes.push_back(sgNode);
            }
            sgNode->setInitialKeyValues(OPEN_SAVED_FILE);
            
        }
    } else {
        for(int i = 0;i < nodeCount;i++) {
            SGNode *sgNode = new SGNode(NODE_UNDEFINED);
            int origId = 0;
            sgNode->readData(filePointer, origId);
            bool status = true;
            
            if(origId > 0) {
                sgNode->setType(NODE_TEMP_INST);
                sgNode->assetId = origId;
            } else if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ) {
                status = currentScene->downloadMissingAssetCallBack(to_string(sgNode->assetId),sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), sgNode->getProperty(TEXTURE).fileName);
            } else if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_TEXT) {
                status = currentScene->downloadMissingAssetCallBack(sgNode->optionalFilePath,sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), sgNode->getProperty(TEXTURE).fileName);
            } else if (sgNode->getType() == NODE_IMAGE) {
                status = currentScene->downloadMissingAssetCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x), sgNode->getProperty(TEXTURE).fileName);
            } else if (sgNode->getType() == NODE_PARTICLES) {
                status = currentScene->downloadMissingAssetCallBack(to_string(sgNode->assetId), sgNode->getType(), true, sgNode->getProperty(TEXTURE).fileName);
            }
            
            if(!status)
                sgNode = NULL;
            tempNodes.push_back(sgNode);
        }
        currentScene->syncSceneWithPhysicsWorld();
    }
    
    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;
        
        if(sgNode && sgNode->getType() == NODE_TEMP_INST) {
            nodeLoaded = loadInstance(sgNode, sgNode->assetId, OPEN_SAVED_FILE);
        } else if(sgNode)
            nodeLoaded = loadNode(sgNode, OPEN_SAVED_FILE);
        
        if(!nodeLoaded)
            delete sgNode;
    }
    
    currentScene->syncSceneWithPhysicsWorld();
    
    return true;
}

bool SGSceneLoader::legacyReadScene(ifstream *filePointer)
{
    if(!currentScene || !smgr)
        return false;

    int nodeCount = 0;
    readSceneGlobalInfo(filePointer, nodeCount);
    currentScene->totalFrames = (currentScene->totalFrames < 24) ? 24 : currentScene->totalFrames;
    currentScene->nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;
    
    vector<SGNode*> tempNodes;
    for(int i = 0;i < nodeCount;i++) {
        SGNode *sgNode = new SGNode(NODE_UNDEFINED);
        int origId = 0;
        sgNode->readData(filePointer, origId);
        bool status = true;
        
        if(origId > 0) {
            sgNode->setType(NODE_TEMP_INST);
            sgNode->assetId = origId;
        } else if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ) {
            status = currentScene->downloadMissingAssetCallBack(to_string(sgNode->assetId),sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), sgNode->getProperty(TEXTURE).fileName);
        } else if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_TEXT) {
            status = currentScene->downloadMissingAssetCallBack(sgNode->optionalFilePath,sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), sgNode->getProperty(TEXTURE).fileName);
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = currentScene->downloadMissingAssetCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType(), !(sgNode->getProperty(IS_VERTEX_COLOR).value.x), sgNode->getProperty(TEXTURE).fileName);
        } else if (sgNode->getType() == NODE_PARTICLES) {
            status = currentScene->downloadMissingAssetCallBack(to_string(sgNode->assetId), sgNode->getType(), true, sgNode->getProperty(TEXTURE).fileName);
        }

        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }
    currentScene->syncSceneWithPhysicsWorld();

    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;

        if(sgNode && sgNode->getType() == NODE_TEMP_INST) {
            nodeLoaded = loadInstance(sgNode, sgNode->assetId, OPEN_SAVED_FILE);
        } else if(sgNode)
            nodeLoaded = loadNode(sgNode, OPEN_SAVED_FILE);

        if(!nodeLoaded)
            delete sgNode;
    }
    
    currentScene->syncSceneWithPhysicsWorld();

    return true;
}

#ifdef ANDROID
bool SGSceneLoader::loadSceneData(std::string *filePath, JNIEnv *env, jclass type,jobject object)
{
    ifstream inputSGBFile(*filePath,ios::in | ios::binary );
    FileHelper::resetSeekPosition();
    if(!readScene(&inputSGBFile, env, type,object)){
        inputSGBFile.close();
        return false;
    }
    inputSGBFile.close();
    return true;
}
bool SGSceneLoader::readScene(ifstream *filePointer, JNIEnv *env, jclass type,jobject object)
{
    if(!currentScene || !smgr)
        return false;
    
    int nodeCount = 0;
    readSceneGlobalInfo(filePointer, nodeCount);
    currentScene->totalFrames = (currentScene->totalFrames < 24) ? 24 : currentScene->totalFrames;
    currentScene->nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;
    
    vector<SGNode*> tempNodes;
    for(int i = 0;i < nodeCount;i++){
        SGNode *sgNode = new SGNode(NODE_UNDEFINED);
        int origId = 0;
        sgNode->readData(filePointer,origId);
        bool status = true;
        if(origId > 0) {
                    sgNode->setType(NODE_TEMP_INST);
                    sgNode->assetId = origId;
                } else if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ){
            status = currentScene->downloadMissingAssetsCallBack(object,to_string(sgNode->assetId),sgNode->getType(), !(sgNode->props.perVertexColor || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), env, type);
        } else if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_TEXT) {
            status = currentScene->downloadMissingAssetsCallBack(object,sgNode->optionalFilePath,sgNode->getType(), !(sgNode->props.perVertexColor || sgNode->getProperty(TEXTURE).fileName == "" || sgNode->getProperty(TEXTURE).fileName == "-1"), env,type);
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = currentScene->downloadMissingAssetsCallBack(object,ConversionHelper::getStringForWString(sgNode->name), sgNode->getType(), !(sgNode->props.perVertexColor), env, type);
        } else if (sgNode->getType() == NODE_PARTICLES) {
           // status = currentScene->downloadMissingAssetCallBack(object,to_string(sgNode->assetId), sgNode->getType(), true, env, type);
        }
        
        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }
    currentScene->syncSceneWithPhysicsWorld();
    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;

        if(sgNode && sgNode->getType() == NODE_TEMP_INST) {
            nodeLoaded = loadInstance(sgNode, sgNode->assetId, OPEN_SAVED_FILE);
        } else if(sgNode)
            nodeLoaded = loadNode(sgNode, OPEN_SAVED_FILE);
        
        if(!nodeLoaded)
            delete sgNode;
    }
    
    return true;
}
#endif

int SGSceneLoader::legacyReadSceneGlobalInfo(ifstream *filePointer, int sgbVersion, int& nodeCount)
{
    if(!currentScene || !smgr)
        return;

    if(sgbVersion >= SGB_VERSION_3) {
        FileHelper::readInt(filePointer); // dofNear
        FileHelper::readInt(filePointer); // dofFar
        FileHelper::readInt(filePointer); // Empty Data for future use
        FileHelper::readInt(filePointer);
        FileHelper::readInt(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        FileHelper::readFloat(filePointer);
        currentScene->totalFrames = FileHelper::readInt(filePointer);
    } else
        currentScene->totalFrames = sgbVersion;
    
    Vector3 lightColor;
    lightColor.x = FileHelper::readFloat(filePointer);
    lightColor.y = FileHelper::readFloat(filePointer);
    lightColor.z = FileHelper::readFloat(filePointer);
    ShaderManager::lightColor.push_back(lightColor);
    ShaderManager::shadowDensity = FileHelper::readFloat(filePointer);
    currentScene->cameraFOV = FileHelper::readFloat(filePointer);
    nodeCount = FileHelper::readInt(filePointer);
    
    return sgbVersion;
}

int SGSceneLoader::readSceneGlobalInfo(ifstream *filePointer, int& nodeCount)
{
    if(!currentScene || !smgr)
        return;
    
    int sgbVersion = FileHelper::readInt(filePointer);
    if(sgbVersion != SGB_VERSION_CURRENT)
        return legacyReadSceneGlobalInfo(filePointer, sgbVersion, nodeCount);
    
    FileHelper::readInt(filePointer); // dofNear
    FileHelper::readInt(filePointer); // dofFar
    currentScene->totalFrames = FileHelper::readInt(filePointer);
    Vector3 lightColor;
    lightColor.x = FileHelper::readFloat(filePointer);
    lightColor.y = FileHelper::readFloat(filePointer);
    lightColor.z = FileHelper::readFloat(filePointer);
    ShaderManager::shadowDensity = FileHelper::readFloat(filePointer);
    currentScene->cameraFOV = FileHelper::readFloat(filePointer);

    FileHelper::readFloat(filePointer); // Ambient Light
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readVector4(filePointer);
    FileHelper::readVector4(filePointer);
    FileHelper::readVector4(filePointer);
    Vector4 test = FileHelper::readVector4(filePointer);
    
    string env = FileHelper::readString(filePointer, sgbVersion); // Environment Texture Name
    FileHelper::readString(filePointer, sgbVersion);
    FileHelper::readString(filePointer, sgbVersion);
    FileHelper::readString(filePointer, sgbVersion);
    FileHelper::readString(filePointer, sgbVersion);

    ShaderManager::lightColor.push_back(lightColor);
    nodeCount = FileHelper::readInt(filePointer);
    
    return sgbVersion;
}

SGNode* SGSceneLoader::loadNode(NODE_TYPE type, int assetId, string meshPath, string textureName, std::wstring name, int imgwidth, int imgheight, int actionType, Vector4 textColor, string fontFilePath, bool isTempNode)
{
    if(!currentScene || !smgr)
        return NULL;
    
    currentScene->selectMan->unselectObject(currentScene->selectedNodeId);
    currentScene->freezeRendering = true;
    SGNode *sgnode = new SGNode(type);
    sgnode->materialProps.push_back(new MaterialProperty(type));

    sgnode->node = sgnode->loadNode(assetId, meshPath, textureName, type, smgr, name, imgwidth, imgheight, textColor, fontFilePath);
    if(!sgnode->node) {
        delete sgnode;
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        currentScene->freezeRendering = false;
        return NULL;
    }
    if(sgnode->getType() == NODE_PARTICLES && isTempNode)
        sgnode->getProperty(SELECTED).value.x = true; // Vector4(true, 0, 0, 0), UNDEFINED);
    else if(sgnode->getType() == NODE_PARTICLES && !isTempNode)
        sgnode->getProperty(SELECTED).value.x = false; // Vector4(false, 0, 0, 0), UNDEFINED);
    
    if(sgnode->getType() == NODE_TEXT_SKIN)
        currentScene->textJointsBasePos[(int)currentScene->nodes.size()] = currentScene->animMan->storeTextInitialPositions(sgnode);
    sgnode->assetId = assetId;
    sgnode->name = name;
    sgnode->setInitialKeyValues(actionType);
    sgnode->node->updateAbsoluteTransformation();
    sgnode->node->updateAbsoluteTransformationOfChildren();
    if(type == NODE_CAMERA)
        ShaderManager::camPos = sgnode->node->getAbsolutePosition();
    else if (type == NODE_LIGHT){
#ifndef UBUNTU
        currentScene->initLightCamera(sgnode->node->getPosition());
        addLight(sgnode);
#endif
    }else if(type == NODE_IMAGE || type == NODE_VIDEO){
        sgnode->getProperty(LIGHTING).value.x = false; // Vector4(false, 0, 0, 0), UNDEFINED);
    } else if (type == NODE_RIG) {
        setJointsScale(sgnode);
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache();
    } else if (type == NODE_TEXT_SKIN) {
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache();
    } else if (type == NODE_ADDITIONAL_LIGHT) {
        addLight(sgnode);
    }

    sgnode->materialProps[0]->setTextureForType(currentScene->shaderMGR->shadowTexture, NODE_TEXTURE_TYPE_SHADOWMAP);

    if(actionType != UNDO_ACTION && actionType != REDO_ACTION && !isTempNode)
        sgnode->actionId = ++currentScene->actionObjectsSize;
    currentScene->nodes.push_back(sgnode);
    sgnode->node->setID(currentScene->assetIDCounter++);
    performUndoRedoOnNodeLoad(sgnode,actionType);
    currentScene->selectMan->removeChildren(currentScene->getParentNode());
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->selectMan->updateParentPosition();
    currentScene->updater->resetMaterialTypes(false);
    currentScene->updater->updateControlsOrientaion();
    currentScene->freezeRendering = false;
    
    return sgnode;
}

void SGSceneLoader::setJointsScale(SGNode *sgNode)
{
    if(sgNode->getType() != NODE_RIG)
        return;
    
    shared_ptr<AnimatedMeshNode> animNode = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node);
    SkinMesh * sMesh = (SkinMesh*)animNode->mesh;
    if(sMesh->joints->size() != currentScene->tPoseJoints.size())
        return;
    
    for(int i = 0; i < (int)sMesh->joints->size(); i++) {
        float scale = currentScene->tPoseJoints[i].sphereRadius;
        (*sMesh->joints)[i]->sphereRadius = scale;
    }
    
}

bool SGSceneLoader::loadNode(SGNode *sgNode,int actionType,bool isTempNode)
{
    if(!currentScene || !smgr)
        return false;

    currentScene->freezeRendering = true;
    Vector4 nodeSpecificColor = Vector4(sgNode->getProperty(VERTEX_COLOR).value.x,sgNode->getProperty(VERTEX_COLOR).value.y,sgNode->getProperty(VERTEX_COLOR).value.z,1.0);
    sgNode->node = sgNode->loadNode(sgNode->assetId, "", sgNode->getProperty(TEXTURE).fileName, sgNode->getType(), smgr, sgNode->name, sgNode->getProperty(FONT_SIZE).value.x, sgNode->getProperty(SPECIFIC_FLOAT).value.x, nodeSpecificColor,sgNode->optionalFilePath);
    if(!sgNode->node){
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        currentScene->freezeRendering = false;
        return false;
    }
    if(sgNode->getType() == NODE_TEXT_SKIN)
        currentScene->textJointsBasePos[(int)currentScene->nodes.size()] = currentScene->animMan->storeTextInitialPositions(sgNode);

    sgNode->setInitialKeyValues(actionType);
    sgNode->node->updateAbsoluteTransformation();
    sgNode->node->updateAbsoluteTransformationOfChildren();

    sgNode->materialProps[0]->setTextureForType(currentScene->shaderMGR->shadowTexture, NODE_TEXTURE_TYPE_SHADOWMAP);

    sgNode->node->setVisible(true);
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION && !isTempNode)
        sgNode->actionId = ++currentScene->actionObjectsSize;
    currentScene->nodes.push_back(sgNode);
    sgNode->node->setID(currentScene->assetIDCounter++);
    performUndoRedoOnNodeLoad(sgNode,actionType);
    if(sgNode->getType() == NODE_CAMERA)
        ShaderManager::camPos = sgNode->node->getAbsolutePosition();
    if(sgNode->getType() == NODE_LIGHT) {
#ifndef UBUNTU
        currentScene->initLightCamera(sgNode->node->getPosition());
        addLight(sgNode);
#endif
    } else if(sgNode->getType() == NODE_ADDITIONAL_LIGHT) {
        addLight(sgNode);
    } else if((sgNode->getType() == NODE_IMAGE || sgNode->getType() == NODE_VIDEO) && actionType != OPEN_SAVED_FILE && actionType != UNDO_ACTION)
        sgNode->getProperty(LIGHTING).value.x = false; // Vector4(false, 0, 0, 0), UNDEFINED);
    else if (sgNode->getType() == NODE_RIG)
        setJointsScale(sgNode);
    
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->updater->resetMaterialTypes(false);
    currentScene->updater->updateControlsOrientaion();
    currentScene->freezeRendering = false;
    return true;
}

bool SGSceneLoader::loadNodeOnUndoORedo(SGAction action, int actionType)
{
    SGNode *sgNode = new SGNode(NODE_UNDEFINED);
    sgNode->setType((NODE_TYPE)action.actionSpecificIntegers[0]);
    sgNode->setPropertiesOfNode();
    sgNode->getProperty(FONT_SIZE).value.x = action.actionSpecificIntegers[1];
    //            node->props.shaderType = recentAction.actionSpecificIntegers[1];
    
    Vector4 vertColor;
    vertColor.x = action.actionSpecificFloats[0];
    vertColor.y = action.actionSpecificFloats[1];
    vertColor.z = action.actionSpecificFloats[2];
    vertColor.w = 1.0;
    
    sgNode->getProperty(VERTEX_COLOR).value = vertColor;
    sgNode->getProperty(SPECIFIC_FLOAT).value.x = action.actionSpecificFloats[3];
    //sgNode->props.prevMatName = ConversionHelper::getStringForWString(action.actionSpecificStrings[0]);
    sgNode->optionalFilePath = ConversionHelper::getStringForWString(action.actionSpecificStrings[1]);
    sgNode->name = action.actionSpecificStrings[2];
    sgNode->oriTextureName = ConversionHelper::getStringForWString(action.actionSpecificStrings[3]);
    sgNode->getProperty(TEXTURE).fileName = ConversionHelper::getStringForWString(action.actionSpecificStrings[4]);
    sgNode->getProperty(LIGHTING).value.x = action.actionSpecificFlags[0];
    if(!loadNode(sgNode, actionType)) {
        delete sgNode;
        return false;
    }
    return true;
}

void SGSceneLoader::addLight(SGNode *light)
{
    if(!currentScene || !smgr)
        return;
    
    Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(currentScene->currentFrame,light->rotationKeys,true);
    Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(currentScene->currentFrame, light->scaleKeys);
    Vector3 lightColor = Vector3(scale.x,scale.y,scale.z);
    float fadeDistance = (light->getType() == NODE_LIGHT) ? 999.0 : light->getProperty(SPECIFIC_FLOAT).value.x;
    
    Vector3 posOrDir;
    if(light->getProperty(LIGHT_TYPE).value.x == (int)DIRECTIONAL_LIGHT) {
        posOrDir = Vector3(0.0, -1.0, 0.0);
        Mat4 rotMat;
        rotMat.setRotation(rotation);
        rotMat.rotateVect(posOrDir);
    } else
        posOrDir = light->node->getAbsolutePosition();
        
    
    ShaderManager::lightPosition.push_back(posOrDir);
    ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
    ShaderManager::lightFadeDistances.push_back(fadeDistance);
    ShaderManager::lightTypes.push_back(light->getProperty(LIGHT_TYPE).value.x);
}

void SGSceneLoader::performUndoRedoOnNodeLoad(SGNode* meshObject,int actionType)
{
    if(!currentScene || !smgr)
        return;

    if(actionType == UNDO_ACTION || actionType == REDO_ACTION) {
        int jointsCnt = (int)meshObject->joints.size();
        int actionIndex = (actionType == UNDO_ACTION) ? currentScene->actionMan->currentAction-1 : currentScene->actionMan->currentAction;
        SGAction &deleteAction = currentScene->actionMan->actions[actionIndex];

        if(deleteAction.nodePositionKeys.size())
            meshObject->positionKeys = deleteAction.nodePositionKeys;
        if(deleteAction.nodeRotationKeys.size())
            meshObject->rotationKeys = deleteAction.nodeRotationKeys;
        if(deleteAction.nodeSCaleKeys.size())
            meshObject->scaleKeys = deleteAction.nodeSCaleKeys;
        if(deleteAction.nodeVisibilityKeys.size())
            meshObject->visibilityKeys = deleteAction.nodeVisibilityKeys;
        for(int i = 0; i < jointsCnt; i++){
            if(deleteAction.jointRotKeys.find(i) != deleteAction.jointRotKeys.end() && deleteAction.jointRotKeys[i].size()) {
                //meshObject->joints[i]->rotationKeys = deleteAction.jointsRotationKeys[i];
                meshObject->joints[i]->rotationKeys = deleteAction.jointRotKeys[i];
            }
            if(meshObject->getType() == NODE_TEXT_SKIN) {
                if(deleteAction.jointPosKeys.find(i) != deleteAction.jointPosKeys.end())
                    meshObject->joints[i]->positionKeys = deleteAction.jointPosKeys[i];
                if(deleteAction.jointScaleKeys.find(i) != deleteAction.jointScaleKeys.end())
                    meshObject->joints[i]->scaleKeys = deleteAction.jointScaleKeys[i];
            }
        }
       // meshObject->props.prevMatName = ConversionHelper::getStringForWString(deleteAction.actionSpecificStrings[0]);
        meshObject->options = deleteAction.options;
        meshObject->actionId = currentScene->actionMan->actions[actionIndex].objectIndex;
    }
    
    if(actionType == REDO_ACTION) {
        SGAction &deleteAction = currentScene->actionMan->actions[currentScene->actionMan->currentAction];
        //meshObject->props.prevMatName = ConversionHelper::getStringForWString(deleteAction.actionSpecificStrings[0]);
        meshObject->actionId = deleteAction.objectIndex;
        meshObject->options = deleteAction.options;
    }
    if(meshObject->getType() == NODE_SGM || meshObject->getType() == NODE_RIG)
        restoreTexture(meshObject, actionType);
    else if(actionType == UNDO_ACTION)
        currentScene->actionMan->currentAction--;
    else if(actionType == REDO_ACTION)
        currentScene->actionMan->currentAction++;
}

void SGSceneLoader::restoreTexture(SGNode* meshObject,int actionType){
    
    if(actionType == UNDO_ACTION){
        meshObject->oriTextureName = ConversionHelper::getStringForWString(currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificStrings[1]);
        meshObject->getProperty(TEXTURE).fileName = ConversionHelper::getStringForWString(currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificStrings[2]);
        
        Vector4 oriVertColor;
        oriVertColor.x = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[0];
        oriVertColor.y =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[1];
        oriVertColor.z =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[2];
        oriVertColor.w = 0.0;
        
        Vector4 vertColor;
        vertColor.x =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[3];
        vertColor.y =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[4];
        vertColor.z =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[5];
        vertColor.w = 0.0;
        
        meshObject->getProperty(ORIG_VERTEX_COLOR).value = oriVertColor;
        meshObject->getProperty(VERTEX_COLOR).value = vertColor;
        meshObject->getProperty(IS_VERTEX_COLOR).value.x = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFlags[0];

        currentScene->actionMan->currentAction--;
    }
    if(actionType == REDO_ACTION) {
        meshObject->oriTextureName = ConversionHelper::getStringForWString(currentScene->actionMan->actions[currentScene->actionMan->currentAction].actionSpecificStrings[0]);
        meshObject->getProperty(TEXTURE).fileName = ConversionHelper::getStringForWString(currentScene->actionMan->actions[currentScene->actionMan->currentAction].actionSpecificStrings[1]);
        Vector4 oriVertColor;
        oriVertColor.x = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[0];
        oriVertColor.y =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[1];
        oriVertColor.z =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[2];
        oriVertColor.w = 0.0;
        
        Vector4 vertColor;
        vertColor.x =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[3];
        vertColor.y =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[4];
        vertColor.z =currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFloats[5];
        vertColor.w = 0.0;
        
        meshObject->getProperty(ORIG_VERTEX_COLOR).value = oriVertColor;
        meshObject->getProperty(VERTEX_COLOR).value = vertColor;
        meshObject->getProperty(IS_VERTEX_COLOR).value.x = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].actionSpecificFlags[0];

        currentScene->actionMan->currentAction++;
    }
    if((actionType == UNDO_ACTION || actionType == REDO_ACTION) && !meshObject->getProperty(IS_VERTEX_COLOR).value.x && meshObject->node->type != NODE_TYPE_INSTANCED){
        Vector4 vertColor = meshObject->getProperty(VERTEX_COLOR).value;
        currentScene->selectMan->selectObject(currentScene->actionMan->getObjectIndex(meshObject->actionId), false);
        currentScene->changeTexture(meshObject->getProperty(TEXTURE).fileName, Vector3(vertColor.x, vertColor.y, vertColor.z), true, true);
        currentScene->selectMan->unselectObject(currentScene->actionMan->getObjectIndex(meshObject->actionId));
    }
}

bool SGSceneLoader::removeObject(u16 nodeIndex, bool deAllocScene)
{
    if(!currentScene || !smgr || nodeIndex >= currentScene->nodes.size())
        return false;
    
    currentScene->freezeRendering = true;
    
    SGNode * currentNode = currentScene->nodes[nodeIndex];
    NODE_TYPE nType = currentNode->getType();
    
    
    int instanceSize = (int)currentNode->instanceNodes.size();
    
    if(!smgr->renderMan->supportsInstancing && (instanceSize > 0 || currentNode->node->type == NODE_TYPE_INSTANCED)) {
        
        Mesh *meshCahce;
        Mesh *origMesh;
        if(currentNode->node->type == NODE_TYPE_INSTANCED) {
            meshCahce = dynamic_pointer_cast<MeshNode>(currentNode->node->original)->meshCache;
            origMesh = dynamic_pointer_cast<MeshNode>(currentNode->node->original)->mesh;
        } else {
            meshCahce = dynamic_pointer_cast<MeshNode>(currentNode->node)->meshCache;
            origMesh = dynamic_pointer_cast<MeshNode>(currentNode->node)->mesh;
        }
        
        meshCahce->removeVerticesOfAnInstance(origMesh->getVerticesCount(), origMesh->getTotalIndicesCount());
    }
    
    if(instanceSize > 0 && !deAllocScene) {
        setFirstInstanceAsMainNode(currentNode);
    }
    
    if(!deAllocScene)
        removeNodeFromInstances(currentNode);
    

    currentScene->renHelper->setControlsVisibility(false);
    currentScene->renHelper->setJointSpheresVisibility(false);
    if(nType == NODE_ADDITIONAL_LIGHT) {
        currentScene->popLightProps();
        if(!deAllocScene)
            currentScene->updater->resetMaterialTypes(false);
    }
    
    if(nType != NODE_TEXT_SKIN && nType != NODE_ADDITIONAL_LIGHT && instanceSize <= 0 && currentNode->node->type != NODE_TYPE_INSTANCED)
        smgr->RemoveTexture(currentNode->node->getTextureByIndex(NODE_TEXTURE_TYPE_COLORMAP));
    
    smgr->RemoveNode(currentNode->node);
    delete currentNode;
    
    if(!deAllocScene) {
        currentScene->nodes.erase(currentScene->nodes.begin() + nodeIndex);
        currentScene->updater->updateLightProperties(currentScene->currentFrame);
        currentScene->selectedNodeId = NOT_EXISTS;
        currentScene->selectedNode = NULL;
        currentScene->selectedJoint = NULL;
        currentScene->isNodeSelected = currentScene->isJointSelected = false;
        currentScene->updater->reloadKeyFrameMap();
    }
    
    if(nType == NODE_LIGHT || nType == NODE_ADDITIONAL_LIGHT) {
        currentScene->updateDirectionLine();
    }
    
    currentScene->freezeRendering = false;
    return true;
}

void SGSceneLoader::setFirstInstanceAsMainNode(SGNode* currentNode)
{
    std::map< int, SGNode* >::iterator it = currentNode->instanceNodes.begin();
    it->second->instanceNodes.insert(currentNode->instanceNodes.begin(), currentNode->instanceNodes.end());
    
    currentNode->node->instancedNodes.erase(currentNode->node->instancedNodes.begin() + 0);
    it->second->node->instancedNodes = currentNode->node->instancedNodes;
    
    copyMeshFromOriginalNode(it->second);
    
    for(int i = 0; i < currentNode->materialProps.size(); i++) {
        Texture* oldTex = currentNode->materialProps[0]->getTextureOfType(NODE_TEXTURE_TYPE_COLORMAP);
        it->second->materialProps[0]->setTextureForType(oldTex, NODE_TEXTURE_TYPE_COLORMAP);
    }
    
    std::map< int, SGNode* >::iterator newIt = it->second->instanceNodes.begin();
    it->second->instanceNodes.erase(newIt);
    it->second->node->setUserPointer(it->second);
    it->second->node->type = NODE_TYPE_MESH;
    if(smgr->device == METAL)
        it->second->node->shouldUpdateMesh = true;
    
    for(newIt = it->second->instanceNodes.begin(); newIt != it->second->instanceNodes.end(); newIt++) {
        newIt->second->node->original = it->second->node;
    }
    
    currentNode->instanceNodes.clear();
    currentNode->node->instancedNodes.clear();
}

void SGSceneLoader::copyMeshFromOriginalNode(SGNode* sgNode)
{
    Mesh* sourceMesh = dynamic_pointer_cast<MeshNode>(sgNode->node->original)->mesh;
    
    dynamic_pointer_cast<MeshNode>(sgNode->node)->mesh->copyDataFromMesh(sourceMesh);
    

    if(!smgr->renderMan->supportsInstancing) {
        Mesh *meshCache = dynamic_pointer_cast<MeshNode>(sgNode->node->original)->meshCache;
        if(!dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache)
            dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache = new Mesh();
        dynamic_pointer_cast<MeshNode>(sgNode->node)->meshCache->copyDataFromMesh(meshCache);

    }

    if(smgr->device == METAL || !smgr->renderMan->supportsVAO)
        sgNode->node->shouldUpdateMesh = true;

}

void SGSceneLoader::removeNodeFromInstances(SGNode *currentNode)
{
    if(currentNode->node->type == NODE_TYPE_INSTANCED) {
        SGNode* origSgNode = (SGNode*)currentNode->node->original->getUserPointer();
        if(origSgNode->instanceNodes.find(currentNode->actionId) != origSgNode->instanceNodes.end())
            origSgNode->instanceNodes.erase(currentNode->actionId);
        
        for(int i = 0; i < origSgNode->node->instancedNodes.size(); i ++) {
            if(currentNode->node->getID() == origSgNode->node->instancedNodes[i]->getID())
                origSgNode->node->instancedNodes.erase(origSgNode->node->instancedNodes.begin() + i);
        }
    }
}

bool SGSceneLoader::removeSelectedObjects()
{
    if(!currentScene || !smgr || !currentScene->allNodesRemovable())
        return false;
    vector<int> actionIds;
    currentScene->selectMan->removeChildren(currentScene->getParentNode());
    for(int i = 0; i < currentScene->selectedNodeIds.size(); i++){
        currentScene->selectMan->unselectObject(currentScene->selectedNodeIds[i]);
        actionIds.push_back(currentScene->nodes[currentScene->selectedNodeIds[i]]->actionId);
    }
    currentScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_DELETED_BEFORE, 0);
    for(int i =0; i < actionIds.size(); i++){
        int nodeId = currentScene->actionMan->getObjectIndex(actionIds[i]);
        if(nodeId < 2 || nodeId > currentScene->nodes.size())
            continue;
        if(currentScene->nodes[nodeId]->getType() == NODE_TEXT_SKIN || currentScene->nodes[nodeId]->getType() == NODE_TEXT || currentScene->nodes[nodeId]->getType() == NODE_IMAGE || currentScene->nodes[nodeId]->getType() == NODE_VIDEO)
            currentScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, nodeId);
        else if (currentScene->nodes[nodeId]->getType() == NODE_OBJ || currentScene->nodes[nodeId]->getType() == NODE_SGM || currentScene->nodes[nodeId]->getType() == NODE_RIG || currentScene->nodes[nodeId]->getType() == NODE_ADDITIONAL_LIGHT || currentScene->nodes[nodeId]->getType() == NODE_PARTICLES)
            currentScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, nodeId);
        removeObject(nodeId);
    }    
    currentScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_DELETED_AFTER, 0);
    
    if(currentScene->getParentNode()) {
        smgr->RemoveNode(currentScene->getParentNode());
        currentScene->getParentNode().reset();
    }
    currentScene->isMultipleSelection = false;
    currentScene->selectedNodeIds.clear();
    actionIds.clear();
    currentScene->updater->resetMaterialTypes(false);

    return true;
}

bool SGSceneLoader::removeTempNodeIfExists()
{
    if(!currentScene || !smgr)
        return false;

    if(currentScene->nodes.size() < 3)
        return false;
    
    for(int index = (int)currentScene->nodes.size()-1; index > 0; index--)
    {
        if(currentScene->nodes[index]->isTempNode) {
            removeObject(index);
        }
    }
    return true;
}

void SGSceneLoader::initEnvelope(std::map<int, SGNode*>& envelopes, int jointId)
{
    std::map<int, RigKey>& rigKeys = currentScene->rigMan->rigKeys;

    SGNode *envelopeSgNod = (envelopes.find(jointId) == envelopes.end()) ? NULL:envelopes[jointId];
    if(jointId<=1 || !rigKeys[jointId].referenceNode || !rigKeys[jointId].referenceNode->node) return;  //skipping envelope between hip and it's parent.
    int parentId = rigKeys[jointId].parentId;
    
    if(envelopeSgNod == NULL){
        envelopeSgNod = new SGNode(NODE_RIG);
        AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(constants::BundlePath + "/Envelop.sgr");
        envelopeSgNod->setSkinningData((SkinMesh*)mesh);
        shared_ptr<AnimatedMeshNode> envelopeNode = smgr->createAnimatedNodeFromMesh(mesh, "envelopeUniforms", ShaderManager::maxJoints, CHARACTER_RIG, MESH_TYPE_HEAVY);
        envelopeNode->setID(ENVELOPE_START_ID + jointId);
        envelopeNode->setParent(rigKeys[parentId].referenceNode->node);
        envelopeNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        envelopeSgNod->node = envelopeNode;
        envelopeSgNod->getProperty(VERTEX_COLOR).value = Vector4(1.0);
        envelopeSgNod->getProperty(TRANSPARENCY).value.x = 0.4; // Vector4(0.4, 0.0, 0.0, 0.0), UNDEFINED);
    }
    if(envelopeSgNod->node) {
        envelopeSgNod->node->updateAbsoluteTransformation();
        
        Vector3 currentDir = BONE_BASE_DIRECTION;
        Vector3 targetDir = Vector3(rigKeys[jointId].referenceNode->node->getPosition()).normalize();
        envelopeSgNod->node->setRotation(MathHelper::rotationBetweenVectors(targetDir, currentDir));
        envelopeSgNod->node->updateAbsoluteTransformation();
        shared_ptr<JointNode> topJoint = (dynamic_pointer_cast<AnimatedMeshNode>(envelopeSgNod->node))->getJointNode(ENVELOPE_TOP_JOINT_ID);
        shared_ptr<JointNode> bottomJoint = (dynamic_pointer_cast<AnimatedMeshNode>(envelopeSgNod->node))->getJointNode(ENVELOPE_BOTTOM_JOINT_ID);
        
        float height = rigKeys[parentId].referenceNode->node->getAbsolutePosition().getDistanceFrom(rigKeys[jointId].referenceNode->node->getAbsolutePosition());
        topJoint->setPosition(Vector3(0,height,0));
        topJoint->updateAbsoluteTransformation();
        
        bottomJoint->setScale(Vector3(rigKeys[parentId].envelopeRadius));
        bottomJoint->updateAbsoluteTransformation();
        topJoint->setScale(Vector3(rigKeys[jointId].envelopeRadius));
        topJoint->updateAbsoluteTransformation();
        envelopes[jointId] = envelopeSgNod;
        envelopes[jointId]->node->setVisible(true);
    }
}

void SGSceneLoader::createInstance(SGNode* sgNode, NODE_TYPE nType, ActionType actionType)
{
    if(!currentScene || (currentScene->selectedNodeId == NOT_EXISTS && currentScene->selectedNodeIds.size() <= 0 && actionType == IMPORT_ASSET_ACTION))
        return;
    
    SGNode* iNode = new SGNode(nType);
    iNode->materialProps.push_back(new MaterialProperty(nType)); //TODO
    shared_ptr<Node> original = (sgNode->node->type == NODE_TYPE_INSTANCED) ? sgNode->node->original : sgNode->node;
    if(sgNode->node->type != NODE_TYPE_INSTANCED)
        original->setUserPointer(sgNode);
    
    shared_ptr<Node> node = smgr->createInstancedNode(original, "setUniforms");
    iNode->node = node;
    
    iNode->assetId = sgNode->assetId;
    iNode->name = sgNode->name;
    iNode->setInitialKeyValues(actionType);
    iNode->node->updateAbsoluteTransformation();
    iNode->node->updateAbsoluteTransformationOfChildren();
    
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
        iNode->actionId = ++currentScene->actionObjectsSize;
    
    iNode->node->setID(currentScene->assetIDCounter++);
    iNode->node->setMaterial(original->material);
    performUndoRedoOnNodeLoad(iNode,actionType);
    currentScene->updater->resetMaterialTypes(false);
    currentScene->updater->updateControlsOrientaion();
    currentScene->freezeRendering = false;

    ((SGNode*)original->getUserPointer())->instanceNodes.insert(std::pair< int, SGNode* >(iNode->actionId, iNode));
    currentScene->nodes.push_back(iNode);
    
    currentScene->selectMan->removeChildren(currentScene->getParentNode());
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->selectMan->updateParentPosition();
}

bool SGSceneLoader::loadInstance(SGNode* iNode, int origId, ActionType actionType)
{
    
    if(!currentScene)
        return false;
    
    SGNode* sgNode = NULL;
    if(origId > 1 && origId < currentScene->nodes.size())
            sgNode = currentScene->nodes[origId];
    
    if(!sgNode)
        return false;
    
    iNode->setType(sgNode->getType());
    
    shared_ptr<Node> original = (sgNode->node->type == NODE_TYPE_INSTANCED) ? sgNode->node->original : sgNode->node;
    if(sgNode->node->type != NODE_TYPE_INSTANCED)
        original->setUserPointer(sgNode);
    
    shared_ptr<Node> node = smgr->createInstancedNode(original, "setUniforms");
    iNode->node = node;
    
    iNode->assetId = sgNode->assetId;
    iNode->name = sgNode->name;
    iNode->setInitialKeyValues(actionType);
    iNode->node->updateAbsoluteTransformation();
    iNode->node->updateAbsoluteTransformationOfChildren();
    
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
        iNode->actionId = ++currentScene->actionObjectsSize;
    
    iNode->node->setID(currentScene->assetIDCounter++);
    performUndoRedoOnNodeLoad(iNode,actionType);
    currentScene->updater->updateControlsOrientaion();
    currentScene->freezeRendering = false;
    
    ((SGNode*)original->getUserPointer())->instanceNodes.insert(std::pair< int, SGNode* >(iNode->actionId, iNode));
    currentScene->nodes.push_back(iNode);
    currentScene->updater->resetMaterialTypes(false);

    currentScene->selectMan->removeChildren(currentScene->getParentNode());
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->selectMan->updateParentPosition();

    if(iNode->getProperty(TEXTURE).fileName == "" || iNode->getProperty(TEXTURE).fileName == "-1")
        iNode->getProperty(IS_VERTEX_COLOR).value.x = true; // Vector4(true, 0, 0, 0), MATERIAL_PROPS);

    return true;
}


