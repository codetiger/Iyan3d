//
//  SGPreviewScene.cpp
//  Iyan3D
//
//  Created by karthik on 07/07/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include <stdio.h>
#include "HeaderFiles/SGPreviewScene.h"
#include "HeaderFiles/SGAnimationSceneHelper.h"
#include <unistd.h>

SGPreviewScene::SGPreviewScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight , VIEW_TYPE fromView):SGScene(device,smgr,screenWidth,screenHeight , fromView)
{
    this->smgr = smgr;
    nodes.clear();
    assetIdCount = 0;
    runningRotation = Vector3(0.0,0.0,0.0);
    this->viewType = fromView;
    currentFrame = 0;
    totalFrames = 0;
    cameraZ = (viewType == ALL_ANIMATION_VIEW) ? 35.0 : 18.0;
    cameraAngle.x = cameraAngle.y = 0.0;
    if(viewCamera) {
        viewCamera->setPosition(Vector3(0.0,3.0,cameraZ));
        viewCamera->setTarget(Vector3(0.0,3.0,0.0));
    }
    isSwipe = false;
}

void SGPreviewScene::renderAll()
{
    if(viewType == ASSET_SELECTION && !isSwipe) {
        for (int i = 0; i < nodes.size(); i++) {
            if(nodes[i]->getType() != NODE_CAMERA && nodes[i]->getType() != NODE_LIGHT) {
                Vector3 nodeRot = nodes[i]->node->getRotationInDegrees();
                nodeRot.y = runningRotation.y++;
                nodes[i]->node->setRotationInDegrees(nodeRot);
                if(runningRotation.y >= 360.0)
                    runningRotation.y = 0.0;
            }
        }
    }
    bool displayPrepared = smgr->PrepareDisplay(screenWidth*screenScale,screenHeight*screenScale,true,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    
    smgr->draw2DImage(bgTexture,Vector2(0,0),Vector2(screenWidth*screenScale,screenHeight*screenScale),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->Render();
    if(viewType == ALL_ANIMATION_VIEW)
    drawGrid();
    smgr->EndDisplay();
}

void SGPreviewScene::shaderCallBackForNode(int nodeID,string matName)
{
    for(int i = 0; i < nodes.size();i++){
        if(nodes[i]->node->getID() == nodeID){
            shaderMGR->setUniforms(nodes[i],matName);
            break;
        }
    }
}

bool SGPreviewScene::isNodeTransparent(int nodeId)
{
    if(nodeId == -1)
        return false;
    else{
        for(int i = 0; i < nodes.size();i++){
            if(nodes[i]->node->getID() == nodeId){
                return (nodes[i]->props.transparency < 1.0) || nodes[i]->props.isSelected || (!nodes[i]->props.isVisible);
                break;
            }
        }
    }
}

SGNode* SGPreviewScene::loadNode(NODE_TYPE type,int assetId, wstring name, int width, int height, Vector4 textColor, string filePath)
{
    if (type == NODE_CAMERA)
        return NULL;
    
    SGNode *sgNode = new SGNode(type);
    sgNode->node = sgNode->loadNode(assetId, type, smgr, name, width, height, textColor, filePath);
    
    if(!sgNode->node){
        delete sgNode;
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        return NULL;
    }
    
    if(viewType == ALL_ANIMATION_VIEW && sgNode->getType() == NODE_TEXT) {
        textJointsBasePos[(int)nodes.size()] = SGAnimationSceneHelper::storeTextInitialPositions(sgNode);
        sgNode->setInitialKeyValues(IMPORT_ASSET_ACTION);
    }

    sgNode->assetId = assetId;
    sgNode->name = name;
    
    sgNode->setInitialKeyValues(IMPORT_ASSET_ACTION);
    sgNode->node->updateAbsoluteTransformation();
    sgNode->node->updateAbsoluteTransformationOfChildren();
    sgNode->node->setID(assetIdCount++);
    
    if(type == NODE_CAMERA || type == NODE_LIGHT)
        sgNode->props.transparency = 0.0;

    if(sgNode->node->getID() != -1)
    sgNode->node->setTexture(sgNode->node->getTextureByIndex(1),2);
    
    setUpInitialKeys(sgNode);
    
    if (sgNode->getType() == NODE_RIG)
        dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->updateMeshCache(CHARACTER_RIG);
    else if(sgNode->getType() == NODE_TEXT) {
        sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_SHADOW_SKIN_L1));
        dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node)->updateMeshCache(TEXT_RIG);
    }

    nodes.push_back(sgNode);
    setDataForFrame(currentFrame);
    return sgNode;
}

bool SGPreviewScene::applyAnimations(int assetId)
{
    currentFrame = 0;
    totalFrames = 0;
    
    string extension = (nodes[nodes.size() - 1]->getType() == NODE_RIG) ? ".sgra" : ".sgta";
    string filePath;

    #ifdef IOS
    filePath = FileHelper::getCachesDirectory() + to_string(assetId) + extension;
    #endif
    #ifdef ANDROID
    filePath = constants::DocumentsStoragePath + "/animations/" + to_string(assetId) + ".sgra";
    #endif

    if(FILE *file = fopen(filePath.c_str(), "r")) {
        fclose(file);
    }
    else {
        filePath = FileHelper::getDocumentsDirectory() + "Resources/Animations/" + to_string(assetId) + extension;
        if(FILE *file = fopen(filePath.c_str(), "r")) {
            fclose(file);
        }
        else
            return false;
    }

    int animTotalFrames = 0;
    if(nodes[nodes.size() - 1]->getType() == NODE_RIG)
        SGAnimationSceneHelper::applySGRAnimations(filePath, nodes[nodes.size() - 1], totalFrames, currentFrame, animTotalFrames);
    else if (nodes[nodes.size() - 1]->getType() == NODE_TEXT)
        SGAnimationSceneHelper::applyTextAnimations(filePath, nodes[nodes.size() - 1], totalFrames, currentFrame ,textJointsBasePos[(int)nodes.size()-1], animTotalFrames);
    
    setDataForFrame(currentFrame);
    return true;
}
void SGPreviewScene::stopAnimation()
{
    if(totalFrames != 0) {
    for (int frame = 0; frame < totalFrames; frame++) {
        removeAnimationAtFrame(frame);
    }
        currentFrame = 0;
        totalFrames = 0;
    }
}
void SGPreviewScene::playAnimation()
{
        setDataForFrame(currentFrame);
}

void SGPreviewScene::setDataForFrame(int frame)
{
    if(nodes[nodes.size() - 1]->getType() == NODE_RIG || nodes[nodes.size() - 1]->getType() == NODE_TEXT) {
        Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame,nodes[nodes.size() - 1]->positionKeys);
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,nodes[nodes.size() - 1]->rotationKeys,true);
        Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame,nodes[nodes.size() - 1]->scaleKeys);
        int visibilityKeyindex = KeyHelper::getKeyIndex(nodes[nodes.size() - 1]->visibilityKeys, frame);
        if(visibilityKeyindex != -1){
            nodes[nodes.size() - 1]->props.isVisible =  nodes[nodes.size() - 1]->visibilityKeys[visibilityKeyindex].visibility;
        }
        nodes[nodes.size() - 1]->setPositionOnNode(position);
        nodes[nodes.size() - 1]->setRotationOnNode(rotation);
        nodes[nodes.size() - 1]->setScaleOnNode(scale);
        for (int j = 0; j < nodes[nodes.size() - 1]->joints.size(); j++) {
            SGJoint *joint = nodes[nodes.size() - 1]->joints[j];
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,joint->rotationKeys,true);
            joint->setRotationOnNode(rotation);
            if(nodes[nodes.size() - 1]->getType() == NODE_TEXT) {
                Vector3 jointPosition = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, joint->positionKeys);
                joint->setPositionOnNode(jointPosition);
                Vector3 jointScale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, joint->scaleKeys);
                joint->setScaleOnNode(jointScale);
            }
        }
    }
}

void SGPreviewScene::updateLightProperties()
{
    int index = 0;
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->getType() == NODE_LIGHT) {
            Vector3 position = viewCamera->getPosition();
            Vector3 scale = Vector3(1.0);
            
            Vector3 lightColor = Vector3(1.0);
            float fadeDistance = 999.0;
            
            if(index < ShaderManager::lightPosition.size())
                ShaderManager::lightPosition[index] = position;
            else
                ShaderManager::lightPosition.push_back(position);
            
            if(index < ShaderManager::lightColor.size())
                ShaderManager::lightColor[index] = Vector3(lightColor.x,lightColor.y,lightColor.z);
            else
                ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
            
            if(index < ShaderManager::lightFadeDistances.size())
                ShaderManager::lightFadeDistances[index] = fadeDistance;
            else
                ShaderManager::lightFadeDistances.push_back(fadeDistance);
            
            nodes[i]->props.vertexColor = Vector3(lightColor.x,lightColor.y,lightColor.z);
            index++;
        }
    }
    
}

void SGPreviewScene::setUpInitialKeys(SGNode *sgnode)
{
    if(sgnode->getType() == NODE_LIGHT) {
        sgnode->node->setPosition(Vector3(viewCamera->getPosition().x , viewCamera->getPosition().y + 20.0, viewCamera->getPosition().z + 30.0));
        initLightCamera(sgnode->node->getPosition());
        updateLightProperties();
    } else {
        
        sgnode->props.shininess = (sgnode->getType() == NODE_TEXT) ? 0.5 : 0.3;
        
        float meshHeight = (dynamic_pointer_cast<MeshNode>(sgnode->node))->getMesh()->getBoundingBox()->getYExtend();
        float meshWidth = (sgnode->getType() == NODE_TEXT) ? (dynamic_pointer_cast<MeshNode>(sgnode->node))->getMesh()->getBoundingBox()->getXExtend() : (dynamic_pointer_cast<MeshNode>(sgnode->node))->getMesh()->getBoundingBox()->getZExtend();
        scaleRatio = meshHeight/9.0;
        
        if(viewType == ALL_ANIMATION_VIEW && (sgnode->getType() == NODE_TEXT) && meshWidth > 150.0)
            scaleRatio *= meshWidth/150.0;
        else if ((sgnode->getType() == NODE_SGM) && meshWidth > 30.0)
            scaleRatio = meshWidth/30.0;
        else if (viewType == ASSET_SELECTION && (sgnode->getType() == NODE_TEXT)) {
            Vector3 textScale = Vector3(sgnode->node->getScale().x/5.0,sgnode->node->getScale().y/5.0,sgnode->node->getScale().z/5.0);
            sgnode->node->setScale(textScale * scaleRatio);
        }
        
        float topRatio = (dynamic_pointer_cast<MeshNode>(sgnode->node))->getMesh()->getBoundingBox()->getMaxEdge().y / 9.75;
        float bottomRatio = abs((dynamic_pointer_cast<MeshNode>(sgnode->node))->getMesh()->getBoundingBox()->getMinEdge().y) / 0.75;
        viewCamera->setPosition(Vector3(0.0,5 * scaleRatio ,cameraZ * scaleRatio));
        viewCamera->setTarget(Vector3(0.0,(3.0  * topRatio)/bottomRatio,0.0));
        if(nodes.size() >= 1)
            updateLightCam(viewCamera->getPosition());
        
        
        swipeToRotate(270, 0);
        swipeEnd();
        
        sgnode->nodeInitialRotation = Quaternion(sgnode->node->getRotationInRadians());
        if(sgnode->getType() == NODE_RIG || sgnode->getType() == NODE_TEXT) {
            for (int j = 0; j < sgnode->joints.size(); j++) {
                sgnode->jointsInitialRotations.push_back(Quaternion(sgnode->joints[j]->jointNode->getRotationInRadians()));
                if(sgnode->getType() == NODE_TEXT) {
                    jointInitPositions.push_back(sgnode->joints[j]->jointNode->getPosition());
                    jointInitScale.push_back(sgnode->joints[j]->jointNode->getScale());
                }
            }
        }
    }
}

void SGPreviewScene::swipeToRotate(float angleX , float angleY)
{
    isSwipe = true;
    cameraAngle.x += angleX/5.0;
    Vector3 camOldPos = viewCamera->getPosition();
    Vector3 newCameraPosition;
    Vector3 cameraTarget = viewCamera->getTarget();
    float cameraRadius = viewCamera->getPosition().getDistanceFrom(viewCamera->getTarget());
    
    newCameraPosition.y = viewCamera->getPosition().y;
    newCameraPosition.x = cameraTarget.x + cameraRadius * cos(-cameraAngle.x * 3.1415 / 180.0f);
    newCameraPosition.z = cameraTarget.z + cameraRadius * sin(cameraAngle.x * 3.1415 / 180.0f);
    
     
    
    /*
    Vector3 camForward = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    Vector3 cameraPos = viewCamera->getPosition();
    Vector3 camOldPos = cameraPos;
    
    float angleBWCamYAndUpVec = camForward.dotProduct(viewCamera->getUpVector());
    if(angleBWCamYAndUpVec < CAM_UPVEC_UPREAL_MAX_DIF && angleBWCamYAndUpVec > -CAM_UPVEC_UPREAL_MAX_DIF)
        viewCamera->setUpVector(camUpReal);
    cameraPos += camRight * (-angleX / CAM_SWIPE_SPEED);
    viewCamera->setPosition(cameraPos);
    */
    
    float swipeAdjustDis = newCameraPosition.getDistanceFrom(viewCamera->getTarget()) - camOldPos.getDistanceFrom(viewCamera->getTarget());
    if(swipeAdjustDis > 0.0){
        Vector3 camNewFwd = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
        newCameraPosition -= (camNewFwd * -(swipeAdjustDis));
        viewCamera->setPosition(newCameraPosition);
    }
    if(nodes.size() >= 1) {
        updateLightCam(viewCamera->getPosition());
        updateLightProperties();
    }
}

void SGPreviewScene::pinchBegan(Vector2 touch1, Vector2 touch2)
{
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    panBeganPoints[0] = touch1; panBeganPoints[1] = touch2;
    previousTarget = cameraTarget;
    previousRadius = cameraRadius;
    prevZoomDif = 0.0;
}
void SGPreviewScene::pinchZoom(Vector2 touch1, Vector2 touch2)
{
    
    Vector3 camPos = viewCamera->getPosition();
    Vector3 targetToCam = (viewCamera->getPosition() - viewCamera->getTarget()).normalize();
    Vector3 camForward = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    
    //ZOOM
    float startDist = prevTouchPoints[0].getDistanceFrom(prevTouchPoints[1]);
    float endDist = touch1.getDistanceFrom(touch2);
    float targetDistance = camPos.getDistanceFrom(viewCamera->getTarget());
    float zoomDif = ((startDist - endDist) / CAM_ZOOM_SPEED);
    if((camPos + (targetToCam * zoomDif)).getDistanceFrom(viewCamera->getTarget()) > CAM_ZOOM_IN_LIMIT)
        camPos += (targetToCam * zoomDif * targetDistance/5.0);
    
    viewCamera->setPosition(camPos);
    
    previousRadius = cameraRadius;
    previousTarget = cameraTarget;
    prevZoomDif = zoomDif;
    
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    
    if(nodes.size() >= 1)
        updateLightCam(viewCamera->getPosition());
}

void SGPreviewScene::swipeEnd()
{
    isSwipe = false;
}
void SGPreviewScene::removeObject(u16 nodeIndex,bool deAllocScene)
{
    if(nodes.size() <= 1 || nodeIndex >= nodes.size() || nodes[nodeIndex] == NULL || nodes[nodeIndex]->getType() == NODE_LIGHT || nodes[nodeIndex]->getType() == NODE_CAMERA)
        return;
    
    if(nodes[nodeIndex]->getType() != NODE_TEXT)
        smgr->RemoveTexture(nodes[nodeIndex]->node->getActiveTexture());
    //smgr->RemoveAllTextures();
    smgr->RemoveNode(nodes[nodeIndex]->node);
    delete nodes[nodeIndex];
    if(!deAllocScene) {
        nodes.erase(nodes.begin() + nodeIndex);
    }
//    if(!deAllocScene)
//        reloadKeyFrameMap();
}

bool SGPreviewScene::removeAnimationAtFrame(int selectedFrame)
{
    if(nodes.size() && nodes.size() < 4) {
        int nodeIndex = (int)nodes.size() - 1;
            if(nodes[nodeIndex] == NULL)
                return false;
    
            if(nodes[nodeIndex]->getType() != NODE_CAMERA && nodes[nodeIndex]->getType() != NODE_LIGHT) {
            Vector3 initPos = Vector3(0.0);
            if(nodes[nodeIndex]->positionKeys.size() > 0)
                nodes[nodeIndex]->positionKeys.erase(nodes[nodeIndex]->positionKeys.begin() + (nodes[nodeIndex]->positionKeys.size()-1));
            if(nodes[nodeIndex]->rotationKeys.size() > 0)
                    nodes[nodeIndex]->rotationKeys.erase(nodes[nodeIndex]->rotationKeys.begin() + (nodes[nodeIndex]->rotationKeys.size()-1));
            if(nodes[nodeIndex]->scaleKeys.size() > 0)
                    nodes[nodeIndex]->scaleKeys.erase(nodes[nodeIndex]->scaleKeys.begin() + (nodes[nodeIndex]->scaleKeys.size()-1));

                nodes[nodeIndex]->setPosition(initPos, 0);
                nodes[nodeIndex]->setRotation(nodes[nodeIndex]->nodeInitialRotation, 0);
                nodes[nodeIndex]->setScale(Vector3(1.0), 0);
            for(unsigned long i = 0; i < nodes[nodeIndex]->joints.size(); i++){
                SGJoint *joint = nodes[nodeIndex]->joints[i];
                if(joint->positionKeys.size() > 0)
                    joint->positionKeys.erase(joint->positionKeys.begin() + (joint->positionKeys.size()-1));
                if(joint->rotationKeys.size() > 0)
                    joint->rotationKeys.erase(joint->rotationKeys.begin() + (joint->rotationKeys.size()-1));
                if(joint->scaleKeys.size() > 0)
                    joint->scaleKeys.erase(joint->scaleKeys.begin() + (joint->scaleKeys.size()-1));

                joint->setRotation(nodes[nodeIndex]->jointsInitialRotations[i], 0);
                if(nodes[nodeIndex]->getType() == NODE_TEXT) {
                    joint->setPosition(jointInitPositions[i], 0);
                    joint->setScale(jointInitScale[i], 0);
                }
                //nodes[nodeIndex]->joints[i]->rotationKeys[0].rotation = nodes[nodeIndex]->jointsInitialRotations[i];
            }
            setDataForFrame(selectedFrame);
            }
    }
}



SGPreviewScene::~SGPreviewScene()
{
    jointInitPositions.clear();
    jointInitScale.clear();
    textJointsBasePos.clear();
    for(int i = 0;i < nodes.size();i++){
        if(nodes[i]) {
            delete nodes[i];
            nodes[i] = NULL;
        }
    }
    nodes.clear();
    if(smgr) {
        delete smgr;
        smgr = NULL;
    }
}