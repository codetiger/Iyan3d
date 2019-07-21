//
//  SGSceneLoader.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGSceneLoader.h"
#include "HeaderFiles/SGEditorScene.h"
#include "SceneImporter.h"

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
    float cameraFov = 72.0;
    int cameraResolution = 0;
    int sgbVersion = readSceneGlobalInfo(filePointer, nodeCount, cameraFov, cameraResolution);
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

            if(sgNode->getType() == NODE_CAMERA || sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT || sgNode->getType() == NODE_IMAGE || sgNode->getType() == NODE_VIDEO) {
                loadNode(sgNode, OPEN_SAVED_FILE);
                if(sgNode->getType() == NODE_CAMERA) {
                    sgNode->getProperty(FOV).value.x = cameraFov;
                    sgNode->getProperty(CAM_RESOLUTION).value.x = cameraResolution;
                } else if (sgNode->getType() == NODE_LIGHT) {
                    ShaderManager::shadowDensity = sgNode->getProperty(SHADOW_DARKNESS).value.x;
                }
            } else {
                
                SceneImporter* importer = new SceneImporter();
                importer->importNodeFromMesh(currentScene, sgNode, mesh);
                delete importer;
                for(int j = 0; j < sgNode->materialProps.size(); j++) {
                    string textureName = sgNode->getProperty(TEXTURE, j).fileName;
                    Texture * texture = smgr->loadTexture(textureName, FileHelper::getTexturesDirectory() + textureName, TEXTURE_RGBA8, TEXTURE_BYTE, sgNode->getProperty(TEXTURE_SMOOTH, j).value.x);
                    sgNode->materialProps[j]->setTextureForType(texture, NODE_TEXTURE_TYPE_COLORMAP);
                    
                    string bumpMapName = sgNode->getProperty(BUMP_MAP, j).fileName;
                    string bumpPath = FileHelper::getTexturesDirectory() + bumpMapName;
                    if(sgNode->checkFileExists(bumpPath)) {
                        Texture * bumpMap = smgr->loadTexture(bumpMapName, bumpPath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
                        sgNode->materialProps[j]->setTextureForType(bumpMap, NODE_TEXTURE_TYPE_NORMALMAP);
                    }
                    
                    sgNode->materialProps[j]->setTextureForType(currentScene->shaderMGR->shadowTexture, NODE_TEXTURE_TYPE_SHADOWMAP);
                }
                
                currentScene->nodes.push_back(sgNode);
                
                if(sgNode->IsPropertyExists(HAS_PHYSICS)) {
                    if(sgNode->getProperty(HAS_PHYSICS).value.x == 1.0)
                        currentScene->setPropsOfObject(sgNode, sgNode->getProperty(PHYSICS_KIND).value.x);
                }
            }
            sgNode->setInitialKeyValues(OPEN_SAVED_FILE);
            
        }
    } else {
        for(int i = 0;i < nodeCount;i++) {
            SGNode *sgNode = new SGNode(NODE_UNDEFINED);
            int origId = 0;
            sgNode->readData(filePointer, origId);
            bool status = true;
            
            if(sgNode->getType() == NODE_CAMERA)
                sgNode->getProperty(FOV).value.x = cameraFov;
            
            if(origId > 0) {
                sgNode->setType(NODE_TEMP_INST);
                sgNode->assetId = origId;
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
    
    for( int i = 0; i < currentScene->nodes.size(); i++) {
        if(currentScene->nodes[i]->getType() == NODE_LIGHT || currentScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT) {
            ActionKey key = currentScene->nodes[i]->getKeyForFrame(0);
            float red = 1.0; float green = 1.0; float blue = 1.0;
            if(key.isScaleKey) {
                red = key.scale.x;
                green = key.scale.y;
                blue = key.scale.z;
            }
            float shadow = currentScene->nodes[i]->getProperty(SHADOW_DARKNESS).value.x;
            float distance = currentScene->nodes[i]->getProperty(SPECIFIC_FLOAT).value.x;
            int lightType = currentScene->nodes[i]->getProperty(LIGHT_TYPE).value.x;
            
            currentScene->selectMan->selectObject(i, NOT_EXISTS, false);
            currentScene->actionMan->changeLightProperty(red, green, blue, shadow, distance, lightType);
            currentScene->selectMan->unselectObject(i);
        }
    }
    
    currentScene->syncSceneWithPhysicsWorld();
    currentScene->setEnvironmentTexture(currentScene->shaderMGR->getProperty(ENVIRONMENT_TEXTURE).fileName, false);
    
    return true;
}

int SGSceneLoader::readSceneGlobalInfo(ifstream *filePointer, int& nodeCount, float& cameraFov, int& cameraResolution)
{
    if(!currentScene || !smgr)
        return;
    
    int sgbVersion = FileHelper::readInt(filePointer);
//    if(sgbVersion != SGB_VERSION_CURRENT)
//        return legacyReadSceneGlobalInfo(filePointer, sgbVersion, nodeCount);
    
    FileHelper::readInt(filePointer); // dofNear
    FileHelper::readInt(filePointer); // dofFar
    currentScene->totalFrames = FileHelper::readInt(filePointer);
    Vector3 lightColor;
    lightColor.x = FileHelper::readFloat(filePointer);
    lightColor.y = FileHelper::readFloat(filePointer);
    lightColor.z = FileHelper::readFloat(filePointer);
    ShaderManager::shadowDensity = FileHelper::readFloat(filePointer);
    cameraFov = FileHelper::readFloat(filePointer);
    
    currentScene->shaderMGR->getProperty(AMBIENT_LIGHT).value.x = FileHelper::readFloat(filePointer); // Ambient Light
    cameraResolution = FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readVector4(filePointer);
    FileHelper::readVector4(filePointer);
    FileHelper::readVector4(filePointer);
    Vector4 test = FileHelper::readVector4(filePointer);
    
    currentScene->shaderMGR->getProperty(ENVIRONMENT_TEXTURE).fileName = FileHelper::readString(filePointer, sgbVersion); // Environment Texture Name
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
        Logger::log(INFO, "SGANimationScene", "Node not loaded");
        currentScene->freezeRendering = false;
        return NULL;
    }
    
    sgnode->assetId = assetId;
    sgnode->name = name;
    sgnode->setInitialKeyValues(actionType);
    sgnode->node->updateAbsoluteTransformation();
    sgnode->node->updateAbsoluteTransformationOfChildren();

    if(type == NODE_CAMERA)
        ShaderManager::camPos = sgnode->node->getAbsolutePosition();
    else if (type == NODE_LIGHT) {
        currentScene->initLightCamera(sgnode->node->getPosition());
        addLight(sgnode);
    } else if(type == NODE_IMAGE || type == NODE_VIDEO) {
        sgnode->getProperty(LIGHTING).value.x = false; // Vector4(false, 0, 0, 0), UNDEFINED);
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
    
        if(sgnode->getType() == NODE_ADDITIONAL_LIGHT) {
            ActionKey key = sgnode->getKeyForFrame(0);
            float red = 1.0; float green = 1.0; float blue = 1.0;
            if(key.isScaleKey) {
                red = key.scale.x;
                green = key.scale.y;
                blue = key.scale.z;
            }
            float shadow = sgnode->getProperty(SHADOW_DARKNESS).value.x;
            float distance = sgnode->getProperty(SPECIFIC_FLOAT).value.x;
            int lightType = sgnode->getProperty(LIGHT_TYPE).value.x;
            
            currentScene->selectMan->selectObject(currentScene->nodes.size()-1, NOT_EXISTS, false);
            currentScene->actionMan->changeLightProperty(red, green, blue, shadow, distance, lightType);
            currentScene->selectMan->unselectObject(currentScene->nodes.size()-1);
        }

    
    return sgnode;
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
        currentScene->initLightCamera(sgNode->node->getPosition());
        addLight(sgNode);
    } else if(sgNode->getType() == NODE_ADDITIONAL_LIGHT) {
        addLight(sgNode);
    } else if((sgNode->getType() == NODE_IMAGE || sgNode->getType() == NODE_VIDEO) && actionType != OPEN_SAVED_FILE && actionType != UNDO_ACTION)
        sgNode->getProperty(LIGHTING).value.x = false; // Vector4(false, 0, 0, 0), UNDEFINED);
    
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

void SGSceneLoader::restoreTexture(SGNode* meshObject,int actionType)
{
    
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
        currentScene->selectMan->selectObject(currentScene->actionMan->getObjectIndex(meshObject->actionId), NOT_SELECTED, false);
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
        
        meshCahce->removeVerticesOfAnInstance(origMesh->getVerticesCountInMeshBuffer(0), origMesh->getIndicesCount(0));
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
    
    if(nType != NODE_TEXT_SKIN && nType != NODE_ADDITIONAL_LIGHT && instanceSize <= 0 && currentNode->node->type != NODE_TYPE_INSTANCED) {
        for (int i = 0; i < currentNode->materialProps.size(); i++) {
            for (int j = NODE_TEXTURE_TYPE_COLORMAP; j < NODE_TEXTURE_TYPE_NORMALMAP; j++) {
                Texture* nodeTex = currentNode->materialProps[i]->getTextureOfType((node_texture_type)j);
                if(nodeTex)
                    smgr->RemoveTexture(nodeTex);
            }
        }
    }
    
    smgr->RemoveNode(currentNode->node);
    delete currentNode;
    
    if(!deAllocScene) {
        currentScene->nodes.erase(currentScene->nodes.begin() + nodeIndex);
        currentScene->updater->updateLightProperties(currentScene->currentFrame);
        currentScene->selectedNodeId = currentScene->selectedJointId = currentScene->selectedMeshBufferId = NOT_EXISTS;
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
        Texture* oldTex = currentNode->materialProps[i]->getTextureOfType(NODE_TEXTURE_TYPE_COLORMAP);
        it->second->materialProps[i]->setTextureForType(oldTex, NODE_TEXTURE_TYPE_COLORMAP);
    }
    
    std::map< int, SGNode* >::iterator newIt = it->second->instanceNodes.begin();
    it->second->instanceNodes.erase(newIt);
    it->second->node->setUserPointer(it->second);
    it->second->node->type = NODE_TYPE_MESH;
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
 //TODO Remove actions of the node using nodeId
    for(int i =0; i < actionIds.size(); i++){
        int nodeId = currentScene->actionMan->getObjectIndex(actionIds[i]);
        if(nodeId < 2 || nodeId > currentScene->nodes.size())
            continue;
        removeObject(nodeId);
    }    
    
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

SGNode* SGSceneLoader::copyOfSGNode(SGNode *sgNode)
{
    NODE_TYPE nType = sgNode->getType();
    SGNode *newNode = new SGNode(nType);
    
    newNode->assetId = sgNode->assetId;
    newNode->name = sgNode->name;
    
    for(int i = 0; i < sgNode->materialProps.size(); i++) {
        MaterialProperty * m = new MaterialProperty(nType);
        memcpy(m, sgNode->materialProps[i], sizeof(sgNode->materialProps[i]));
        m->getProps() = sgNode->materialProps[i]->getPropsCopy();
        
        for( int j = 0; j < 2; j++) {
            if(sgNode->materialProps[i]->getTextureOfType((node_texture_type)j)) {
                PROP_INDEX pI = (j==0) ? TEXTURE : BUMP_MAP;
                string fileName = m->getProperty(pI).fileName;
                Texture *tex = smgr->loadTexture(fileName, FileHelper::getTexturesDirectory() + fileName, TEXTURE_RGBA8, TEXTURE_BYTE, m->getProperty(TEXTURE_SMOOTH).value.x);
                m->setTextureForType(tex, (node_texture_type)j);
            }
        }

        newNode->materialProps.push_back(m);
    }
    
    return newNode;
}

void SGSceneLoader::createInstance(SGNode* sgNode, NODE_TYPE nType, ActionType actionType)
{
    if(!currentScene || (currentScene->selectedNodeId == NOT_EXISTS && currentScene->selectedNodeIds.size() <= 0 && actionType == IMPORT_ASSET_ACTION))
        return;
    
    SGNode* iNode = new SGNode(nType);
    for(int i = 0; i < sgNode->materialProps.size(); i++) {
        MaterialProperty * m = new MaterialProperty(nType);
        memcpy(m, sgNode->materialProps[i], sizeof(sgNode->materialProps[i]));
        m->getProps() = sgNode->materialProps[i]->getPropsCopy();
        iNode->materialProps.push_back(m);
    }
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
    iNode->options = sgNode->options;
    
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


