//
//  SGSceneLoader.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGSceneLoader.h"
#include "HeaderFiles/SGEditorScene.h"
#include "HeaderFiles/SGAnimationSceneHelper.h"

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
    readSceneGlobalInfo(filePointer, nodeCount);
    currentScene->totalFrames = (currentScene->totalFrames < 24) ? 24 : currentScene->totalFrames;
    currentScene->nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;
    
    vector<SGNode*> tempNodes;
    for(int i = 0;i < nodeCount;i++){
        SGNode *sgNode = new SGNode(NODE_UNDEFINED);
        sgNode->readData(filePointer);
        printf("SGNode Read Data Finished");
        bool status = true;
        
        if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ)
        {
            status = currentScene->downloadMissingAssetCallBack(to_string(sgNode->assetId),sgNode->getType());
        }
        else if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_TEXT) {
            status = currentScene->downloadMissingAssetCallBack(sgNode->optionalFilePath,sgNode->getType());
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = currentScene->downloadMissingAssetCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType());
        }
        
        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }

    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;
        if(sgNode)
            nodeLoaded = loadNode(sgNode, OPEN_SAVED_FILE);
        
        if(!nodeLoaded)
            delete sgNode;
    }
 
    return true;
}

#ifdef ANDROID
bool SGSceneLoader::loadSceneData(std::string *filePath, JNIEnv *env, jclass type)
{
    ifstream inputSGBFile(*filePath,ios::in | ios::binary );
    FileHelper::resetSeekPosition();
    if(!readScene(&inputSGBFile, env, type)){
        inputSGBFile.close();
        return false;
    }
    inputSGBFile.close();
    return true;
}
bool SGSceneLoader::readScene(ifstream *filePointer, JNIEnv *env, jclass type)
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
        sgNode->readData(filePointer);
        bool status = true;
        
        if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ)
        {
            status = currentScene->downloadMissingAssetsCallBack(to_string(sgNode->assetId),sgNode->getType(), env, type);
        }
        else if (sgNode->getType() == NODE_TEXT_SKIN || sgNode->getType() == NODE_TEXT) {
            
            status = currentScene->downloadMissingAssetsCallBack(sgNode->optionalFilePath,sgNode->getType(),env,type);
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = currentScene->downloadMissingAssetsCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType(), env, type);
        }
        
        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }
    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;
        if(sgNode)
            nodeLoaded = loadNode(sgNode,OPEN_SAVED_FILE);
        
        if(!nodeLoaded)
            delete sgNode;
    }
    
    return true;
}
#endif

void SGSceneLoader::readSceneGlobalInfo(ifstream *filePointer, int& nodeCount)
{
    if(!currentScene || !smgr)
        return;

    currentScene->totalFrames = FileHelper::readInt(filePointer);
    Vector3 lightColor;
    lightColor.x = FileHelper::readFloat(filePointer);
    lightColor.y = FileHelper::readFloat(filePointer);
    lightColor.z = FileHelper::readFloat(filePointer);
    ShaderManager::lightColor.push_back(lightColor);
    ShaderManager::shadowDensity = FileHelper::readFloat(filePointer);
    currentScene->cameraFOV = FileHelper::readFloat(filePointer);
    nodeCount = FileHelper::readInt(filePointer);
    
}

SGNode* SGSceneLoader::loadNode(NODE_TYPE type,int assetId,string textureName,std::wstring name,int imgwidth,int imgheight,int actionType, Vector4 textColor, string fontFilePath ,bool isTempNode)
{
    if(!currentScene || !smgr)
        return NULL;
    
    currentScene->selectMan->unselectObject(currentScene->selectedNodeId);
    currentScene->freezeRendering = true;
    SGNode *sgnode = new SGNode(type);
    sgnode->node = sgnode->loadNode(assetId,textureName,type,smgr,name,imgwidth,imgheight,textColor,fontFilePath);
    if(!sgnode->node){
        delete sgnode;
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        return NULL;
    }

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
        sgnode->props.isLighting = false;
    } else if (type == NODE_RIG) {
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache(CHARACTER_RIG);
    } else if (type == NODE_TEXT_SKIN) {
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache(TEXT_RIG);
    } else if (type == NODE_ADDITIONAL_LIGHT) {
        addLight(sgnode);
    }
    
    //if (type >= NODE_LIGHT && type != NODE_ADDITIONAL_LIGHT)
    sgnode->node->setTexture(currentScene->shadowTexture,2);
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION && !isTempNode)
        sgnode->actionId = ++currentScene->actionObjectsSize;
    currentScene->nodes.push_back(sgnode);
    sgnode->node->setID(currentScene->assetIDCounter++);
    performUndoRedoOnNodeLoad(sgnode,actionType);
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->updater->resetMaterialTypes(false);
    currentScene->updater->updateControlsOrientaion();
    currentScene->freezeRendering = false;
    return sgnode;
}

bool SGSceneLoader::loadNode(SGNode *sgNode,int actionType,bool isTempNode)
{
    if(!currentScene || !smgr)
        return false;

    Vector4 nodeSpecificColor = Vector4(sgNode->props.vertexColor.x,sgNode->props.vertexColor.y,sgNode->props.vertexColor.z,1.0);
    sgNode->node = sgNode->loadNode(sgNode->assetId,sgNode->textureName,sgNode->getType(),smgr,sgNode->name,sgNode->props.fontSize,sgNode->props.nodeSpecificFloat,nodeSpecificColor,sgNode->optionalFilePath);
    if(!sgNode->node){
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        return false;
    }
    if(sgNode->getType() == NODE_TEXT_SKIN)
        currentScene->textJointsBasePos[(int)currentScene->nodes.size()] = currentScene->animMan->storeTextInitialPositions(sgNode);

    sgNode->setInitialKeyValues(actionType);
    sgNode->node->updateAbsoluteTransformation();
    sgNode->node->updateAbsoluteTransformationOfChildren();
    //if(sgNode->getType() >= NODE_LIGHT)
    sgNode->node->setTexture(currentScene->shadowTexture,2);
    sgNode->node->setVisible(true);
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION && !isTempNode)
        sgNode->actionId = ++currentScene->actionObjectsSize;
    currentScene->nodes.push_back(sgNode);
    sgNode->node->setID(currentScene->assetIDCounter++);
    performUndoRedoOnNodeLoad(sgNode,actionType);
    if(sgNode->getType() == NODE_LIGHT) {
#ifndef UBUNTU
        currentScene->initLightCamera(sgNode->node->getPosition());
        addLight(sgNode);
#endif
    } else if(sgNode->getType() == NODE_ADDITIONAL_LIGHT)
        addLight(sgNode);
    else if(sgNode->getType() == NODE_IMAGE && actionType != OPEN_SAVED_FILE && actionType != UNDO_ACTION)
        sgNode->props.isLighting = false;
    currentScene->updater->setDataForFrame(currentScene->currentFrame);
    currentScene->updater->resetMaterialTypes(false);
    return true;
}

bool SGSceneLoader::loadNodeOnUndoORedo(SGAction action, int actionType)
{
    SGNode *sgNode = new SGNode(NODE_UNDEFINED);
    sgNode->setType((NODE_TYPE)action.actionSpecificIntegers[0]);
    sgNode->props.fontSize = action.actionSpecificIntegers[1];
    //            node->props.shaderType = recentAction.actionSpecificIntegers[1];
    sgNode->props.vertexColor.x = action.actionSpecificFloats[0];
    sgNode->props.vertexColor.y = action.actionSpecificFloats[1];
    sgNode->props.vertexColor.z = action.actionSpecificFloats[2];
    sgNode->props.nodeSpecificFloat = action.actionSpecificFloats[3];
    //sgNode->props.prevMatName = ConversionHelper::getStringForWString(action.actionSpecificStrings[0]);
    sgNode->optionalFilePath = ConversionHelper::getStringForWString(action.actionSpecificStrings[1]);
    sgNode->name = action.actionSpecificStrings[2];
    
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
    Vector3 lightColor = (light->getType() == NODE_LIGHT) ? Vector3(scale.x,scale.y,scale.z) : Vector3(rotation.x, rotation.y, rotation.z);
    float fadeDistance = (light->getType() == NODE_LIGHT) ? 999.0 : rotation.w;
    
    ShaderManager::lightPosition.push_back(light->node->getAbsolutePosition());
    ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
    ShaderManager::lightFadeDistances.push_back(fadeDistance);
}

void SGSceneLoader::performUndoRedoOnNodeLoad(SGNode* meshObject,int actionType)
{
    if(!currentScene || !smgr)
        return;

    if(actionType == UNDO_ACTION) {
        int jointsCnt = (int)meshObject->joints.size();
        SGAction &deleteAction = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1];
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
        meshObject->actionId = currentScene->actionMan->actions[currentScene->actionMan->currentAction-1].objectIndex;
        currentScene->actionMan->currentAction--;
    }
    
    if(actionType == REDO_ACTION) {
        SGAction &deleteAction = currentScene->actionMan->actions[currentScene->actionMan->currentAction];
        //meshObject->props.prevMatName = ConversionHelper::getStringForWString(deleteAction.actionSpecificStrings[0]);
        meshObject->actionId = deleteAction.objectIndex;
        currentScene->actionMan->currentAction++;
    }
}

bool SGSceneLoader::removeObject(u16 nodeIndex, bool deAllocScene)
{
    if(!currentScene || !smgr || nodeIndex >= currentScene->nodes.size())
        return false;

    currentScene->renHelper->setControlsVisibility(false);
    currentScene->renHelper->setJointSpheresVisibility(false);
    
    SGNode * currentNode = currentScene->nodes[nodeIndex];
    if(currentNode->getType() == NODE_ADDITIONAL_LIGHT) {
        currentScene->popLightProps();
        currentScene->updater->resetMaterialTypes(false);
    }
    
    if(currentNode->getType() != NODE_TEXT_SKIN && currentNode->getType() != NODE_ADDITIONAL_LIGHT)
        smgr->RemoveTexture(currentNode->node->getActiveTexture());
    
    smgr->RemoveNode(currentNode->node);
    delete currentNode;
    if(!deAllocScene) {
        currentScene->nodes.erase(currentScene->nodes.begin() + nodeIndex);
    }
    currentScene->updater->updateLightProperties(currentScene->currentFrame);
    currentScene->selectedNodeId = NOT_EXISTS;
    currentScene->selectedNode = NULL;
    currentScene->selectedJoint = NULL;
    currentScene->isNodeSelected = currentScene->isJointSelected = false;
    if(!deAllocScene)
        currentScene->updater->reloadKeyFrameMap();
    return true;
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
        if(currentScene->nodes[nodeId]->getType() == NODE_TEXT_SKIN || currentScene->nodes[nodeId]->getType() == NODE_TEXT || currentScene->nodes[nodeId]->getType() == NODE_IMAGE)
            currentScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, nodeId);
        else if (currentScene->nodes[nodeId]->getType() == NODE_OBJ || currentScene->nodes[nodeId]->getType() == NODE_SGM || currentScene->nodes[nodeId]->getType() == NODE_RIG || currentScene->nodes[nodeId]->getType() == NODE_ADDITIONAL_LIGHT)
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
    if(!currentScene || !smgr || currentScene->nodes.size() < 3)
        return false;
    
    for(int index = currentScene->nodes.size()-1; index > 0; index--)
    {
        if(currentScene->nodes[index]->isTempNode)
            removeObject(index);
    }
    return true;
}

void SGSceneLoader::initEnvelope(std::map<int, SGNode*>& envelopes, int jointId)
{
    std::map<int, RigKey>& rigKeys = currentScene->rigMan->rigKeys;

    SGNode *envelopeSgNod = (envelopes.find(jointId) == envelopes.end()) ? NULL:envelopes[jointId];
    if(jointId<=1) return;  //skipping envelope between hip and it's parent.
    int parentId = rigKeys[jointId].parentId;
    
    if(envelopeSgNod == NULL){
        envelopeSgNod = new SGNode(NODE_RIG);
        AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(constants::BundlePath + "/Envelop.sgr", currentScene->shaderMGR->deviceType);
        envelopeSgNod->setSkinningData((SkinMesh*)mesh);
        shared_ptr<AnimatedMeshNode> envelopeNode = smgr->createAnimatedNodeFromMesh(mesh,"envelopeUniforms",CHARACTER_RIG, MESH_TYPE_HEAVY);
        envelopeNode->setID(ENVELOPE_START_ID + jointId);
        envelopeNode->setParent(rigKeys[parentId].referenceNode->node);
        envelopeNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        envelopeSgNod->node = envelopeNode;
        envelopeSgNod->props.vertexColor = Vector3(1.0);
        envelopeSgNod->props.transparency = 0.4;
    }
    if(envelopeSgNod->node) {
        envelopeSgNod->node->updateAbsoluteTransformation();
        
        Vector3 currentDir = BONE_BASE_DIRECTION;
        Vector3 targetDir = Vector3(rigKeys[jointId].referenceNode->node->getPosition()).normalize();
        envelopeSgNod->node->setRotationInDegrees(MathHelper::toEuler(MathHelper::rotationBetweenVectors(targetDir,currentDir))*RADTODEG);
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
