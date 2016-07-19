//
//  SGMovementManager.cpp
//  Iyan3D
//
//  Created by Karthik on 27/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGMovementManager.h"
#include "HeaderFiles/SGEditorScene.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

SGEditorScene *moveScene;

SGMovementManager::SGMovementManager(SceneManager* smgr, void * scene) {
    this->smgr = smgr;
    moveScene = (SGEditorScene*)scene;
    swipeTiming = 0;
    isPan = false;
    xAcceleration = yAcceleration = 0.0;
}

SGMovementManager::~SGMovementManager() {
    
}

void SGMovementManager::touchBegan(Vector2 curTouchPos) {
    prevTouchPoints[0] = curTouchPos;
    if(moveScene->renHelper->isMovingCameraPreview(curTouchPos)) {
        moveScene->renHelper->isMovingPreview = true;
    }
    xAcceleration = yAcceleration = 0.0;
}

void SGMovementManager::swipeProgress(float angleX , float angleY) {
    if(!moveScene || !smgr || moveScene->moveNodeId != NOT_EXISTS)
        return;
    if(moveScene->isControlSelected || swipeTiming < 3) {
        swipeTiming++;
        return;
    }
    xAcceleration += angleX;
    yAcceleration += angleY;
    
    xAcceleration = (xAcceleration > 300.0) ? 300.0 : (xAcceleration < -300.0 ? -300.0 : xAcceleration) ;
    yAcceleration = (yAcceleration > 200.0) ? 200.0 : (yAcceleration < -200.0 ? -200.0 : yAcceleration) ;
    
}

void SGMovementManager::swipeToRotate() {
    if(!moveScene || !smgr)
        return;
    
    if((fabs(xAcceleration) > 0.0 || fabs(yAcceleration) > 0.0)) {
        
        ShaderManager::shadowsOff = true;
        Vector3 camTarget = moveScene->viewCamera->getTarget();
        Vector3 camForward = (camTarget - moveScene->viewCamera->getPosition()).normalize();
        Vector3 camRight = (camForward.crossProduct(moveScene->viewCamera->getUpVector())).normalize();
        Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
        Vector3 cameraPos = moveScene->viewCamera->getPosition();
        Vector3 camOldPos = cameraPos;
        
        float distanceRatio = cameraPos.getDistanceFrom(camTarget);
        distanceRatio = distanceRatio/50.0;
        distanceRatio = (distanceRatio > 1.0) ? 1.0 : distanceRatio;
        
        float angleBWCamYAndUpVec = camForward.dotProduct(moveScene->viewCamera->getUpVector());
        if(angleBWCamYAndUpVec < CAM_UPVEC_UPREAL_MAX_DIF && angleBWCamYAndUpVec > -CAM_UPVEC_UPREAL_MAX_DIF)
            moveScene->viewCamera->setUpVector(camUpReal);
        cameraPos += camUpReal * (-yAcceleration * distanceRatio/ CAM_SWIPE_SPEED) + camRight * (-xAcceleration * distanceRatio/ CAM_SWIPE_SPEED);
        moveScene->viewCamera->setPosition(cameraPos);
        float swipeAdjustDis = cameraPos.getDistanceFrom(camTarget) - camOldPos.getDistanceFrom(camTarget);
        
        if(swipeAdjustDis > 0.0){
            Vector3 camNewFwd = (camTarget - moveScene->viewCamera->getPosition()).normalize();
            cameraPos -= (camNewFwd * -(swipeAdjustDis));
            moveScene->viewCamera->setPosition(cameraPos);
        }
        moveScene->updater->updateControlsOrientaion();
        xAcceleration = xAcceleration / (1.25f);
        yAcceleration = yAcceleration / (1.25f);
        
        if(fabs(xAcceleration) < 0.001f)
            xAcceleration = 0.0f;
        
        if(fabs(yAcceleration) < 0.001f)
            yAcceleration = 0.0f;
        
        moveScene->updater->updateLightCamera();
    } else
        ShaderManager::shadowsOff = false;
    
    if(!isPan && !moveScene->isPlaying && swipeTiming == 0 && (fabs(xAcceleration) == 0.0 && fabs(yAcceleration) == 0.0))
        moveScene->setLightingOn();
    
}

void SGMovementManager::touchEnd(Vector2 curTouchPos) {
    if(!moveScene || !smgr)
        return;
    
    moveScene->shadowsOff = false;
    isPan = false;
    moveScene->renHelper->cameraPreviewMoveDist = Vector2(0.0, 0.0);
    moveScene->renHelper->isMovingPreview = false;
    if((fabs(xAcceleration) == 0.0 && fabs(yAcceleration) == 0.0))
        moveScene->setLightingOn();
    swipeTiming = 0;
    
    moveScene->updater->updateControlsMaterial();
    if(moveScene->isControlSelected) {
        moveScene->selectMan->removeChildren(moveScene->getParentNode(), true);
        moveScene->selectMan->updateParentPosition();
        
        if(moveScene->controlType == SCALE && moveScene->selectedNodeIds.size() > 0)
            moveScene->actionMan->changeObjectScale(moveScene->getParentNode()->getScale(), true);
        else if(moveScene->controlType == SCALE && moveScene->selectedNodeId != NOT_SELECTED)
            moveScene->actionMan->changeObjectScale(moveScene->nodes[moveScene->selectedNodeId]->getNodeScale(), true);
        
        Logger::log(INFO , "SGScene diff ", "touch end");
        
        if(moveScene->selectedNodeIds.size() > 0 && moveScene->controlType != SCALE){
            moveScene->actionMan->storeActionKeysForMulti(true);
        } else if (moveScene->controlType == ROTATE && moveScene->directionIndicator->node->getVisible()) {
            Vector3 direction = Vector3(0.0, 1.0, 0.0);
            Mat4 rotMatrix;
            rotMatrix.setRotation(moveScene->directionIndicator->node->getRotation());
            rotMatrix.rotateVect(direction);
            moveScene->nodes[moveScene->selectedNodeId]->props.forceDirection = direction;
            moveScene->syncSceneWithPhysicsWorld();
        } else if(moveScene->controlType != SCALE)
            moveScene->actionMan->storeActionKeys(true);
        moveScene->selectedControlId = NOT_SELECTED;
        moveScene->isControlSelected = false;
    } else if (moveScene->moveNodeId != NOT_EXISTS) {
        moveScene->actionMan->storeActionKeys(true);
        moveScene->moveNodeId = NOT_EXISTS;
    }
    
    if(moveScene->selectedNodeId != NOT_SELECTED && moveScene->nodes[moveScene->selectedNodeId] && moveScene->nodes[moveScene->selectedNodeId]->node->type == NODE_TYPE_SKINNED && moveScene->nodes[moveScene->selectedNodeId]->node->skinType == CPU_SKIN) {
        dynamic_pointer_cast<AnimatedMeshNode>(moveScene->nodes[moveScene->selectedNodeId]->node)->updateMeshCache();
    }
}

void SGMovementManager::panBegan(Vector2 touch1, Vector2 touch2) {
    if(!moveScene || !smgr)
        return;
    
    isPan = true;
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    previousTarget = moveScene->cameraTarget;
    previousRadius = moveScene->cameraRadius;
}

void SGMovementManager::panProgress(Vector2 touch1, Vector2 touch2) {
    if(!moveScene || !smgr)
        return;
    
    Vector3 camPos = moveScene->viewCamera->getPosition();
    Vector3 targetToCam = (camPos - moveScene->viewCamera->getTarget()).normalize();
    Vector3 camForward = (moveScene->viewCamera->getTarget() - camPos).normalize();
    Vector3 camRight = (camForward.crossProduct(moveScene->viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    
    //ZOOM
    float startDist = prevTouchPoints[0].getDistanceFrom(prevTouchPoints[1]);
    float endDist = touch1.getDistanceFrom(touch2);
    float zoomDif = ((startDist - endDist) / CAM_ZOOM_SPEED);
    if((camPos + (targetToCam * zoomDif)).getDistanceFrom(moveScene->viewCamera->getTarget()) > CAM_ZOOM_IN_LIMIT)
        camPos += (targetToCam * zoomDif);
    
    //MOVE
    Vector2 centerStart = (prevTouchPoints[0] + prevTouchPoints[1]) / 2.0;
    Vector2 centerEnd = (touch1 + touch2) / 2.0;
    Vector2 moveDif = (centerEnd - centerStart) / CAM_PAN_SPEED;
    moveScene->cameraTarget = moveScene->viewCamera->getTarget() + camRight*moveDif.x + camUpReal*moveDif.y;
    moveScene->viewCamera->setTarget(moveScene->cameraTarget);
    camPos += (camRight * moveDif.x) + (camUpReal * moveDif.y);
    
    moveScene->viewCamera->setPosition(camPos);
    
    previousRadius = moveScene->cameraRadius;
    previousTarget = moveScene->cameraTarget;
    
    //ROTATE
    float startRotation = atan2(prevTouchPoints[1].y - prevTouchPoints[0].y, prevTouchPoints[1].x - prevTouchPoints[0].x);
    float endRotation = atan2(touch2.y - touch1.y, touch2.x - touch1.x);
    float rotationDif = endRotation - startRotation;
    
    Quaternion quat;
    quat.fromAngleAxis(rotationDif, camForward);  //This takes rotation in rad, so it's already right
    Mat4 mat = quat.getMatrix();
    mat.rotateVect(camUpReal);
    mat.rotateVect(camRight);
    moveScene->viewCamera->setUpVector(camUpReal);
    
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    moveScene->updater->updateControlsOrientaion();
}

void SGMovementManager::touchMove(Vector2 curTouchPos, Vector2 prevTouchPos, float width, float height) {
    if(!moveScene || !smgr || (!moveScene->isNodeSelected && moveScene->selectedNodeIds.size() <= 0))
        return;
    
    Vector4 prevLay = moveScene->getCameraPreviewLayout();
    if(moveScene->renHelper->isMovingPreview) {
        moveScene->renHelper->changeCameraPreviewCoords(curTouchPos);
        return;
    }
    
    moveScene->updater->updateLightCam(moveScene->nodes[NODE_LIGHT]->node->getPosition());
    
    if(moveScene->isControlSelected) {
        Vector3 outputValue;
        Quaternion outputQuatValue;
        bool isMoved = calculateControlMovements(curTouchPos, prevTouchPoints[0], outputValue, outputQuatValue);
        prevTouchPoints[0] = curTouchPos;
        
        if(moveScene->controlType == SCALE) {
            Vector3 currentScale = moveScene->getSelectedNodeScale();
            if(currentScale.x <= 0.01 && moveScene->selectedControlId == X_SCALE)
                outputValue.x = 0.01;
            if(currentScale.y <= 0.01 && moveScene->selectedControlId == Y_SCALE)
                outputValue.y = 0.01;
            if(currentScale.z <= 0.01 && moveScene->selectedControlId == Z_SCALE)
                outputValue.z = 0.01;
            
            if(moveScene->getSelectedNode() && moveScene->getSelectedNode()->getType() == NODE_PARTICLES)
                outputValue = (moveScene->selectedControlId == X_SCALE) ? Vector3(outputValue.x) : (moveScene->selectedControlId == Y_SCALE) ? Vector3(outputValue.y) : Vector3(outputValue.z);
            
            moveScene->actionMan->changeObjectScale(moveScene->getSelectedNodeScale() + outputValue, false);
        } else
            isMoved |= moveScene->actionMan->changeObjectOrientation(outputValue, outputQuatValue);
        
    } else if (moveScene->moveNodeId != NOT_EXISTS && moveScene->controlType == MOVE) {
        moveObjectInPlane(curTouchPos, prevTouchPoints[0], true);
    }
}

bool SGMovementManager::moveObjectInPlane(Vector2 curPoint, Vector2 prevTouchPoint, bool isSGJoint) {
    if(!moveScene || !smgr)
        return false;
    
    Vector3 center;
    if(moveScene->moveNodeId != NOT_EXISTS && moveScene->moveNodeId == moveScene->selectedNodeId)
        center = moveScene->selectedNode->node->getAbsoluteTransformation().getTranslation();
    else
        return false;
    
    Vector3 cameraDir;
    cameraDir = moveScene->viewCamera->getTarget() - moveScene->viewCamera->getPosition();
    cameraDir.normalize();
    moveScene->controlsPlane->setPositionAndNormal(center, cameraDir);
    
    Vector2 p1;
    p1.x = prevTouchPoint.x; p1.y = prevTouchPoint.y;
    Vector2 p2;
    p2.x = curPoint.x;	p2.y = curPoint.y;
    
    Vector3 oldPos;
    Vector3 newPos;
    getOldAndNewPosInWorld(prevTouchPoint, curPoint, oldPos, newPos);
    
    if(moveScene->controlType == SCALE) {
        Vector3 diff = oldPos - newPos;
        if(diff.getLength() > 1.0f) {
            diff = diff.normalize() * 1.0f;
        }
        
        float maxDiff = MAX(fabs(diff.x), MAX(fabs(diff.y), fabs(diff.z)));
        float finalValue = 0.0;
        if(fabs(diff.x) == maxDiff)
            finalValue = diff.x;
        else if (fabs(diff.y) == maxDiff)
            finalValue = diff.y;
        else if(fabs(diff.z) == maxDiff)
            finalValue = diff.z;
        if(finalValue >= 0.1)
            moveScene->actionMan->changeObjectScale(finalValue, false);
        return true;
    }
    
    SGNode* selectedNode;
    if(moveScene->moveNodeId != NOT_EXISTS && moveScene->selectedNodeId != NOT_EXISTS)
        selectedNode = moveScene->nodes[moveScene->moveNodeId];
    
    if(moveScene->moveNodeId != NOT_EXISTS) {
        selectedNode->setPosition(newPos , moveScene->currentFrame);
        selectedNode->setPositionOnNode(newPos);
    }
    
}

bool SGMovementManager::calculateControlMovements(Vector2 curPoint, Vector2 prevTouchPoint, Vector3 &outputValue, Quaternion &outputQuatValue, bool isSGJoint) {
    if(!moveScene || !smgr)
        return false;
    
    Vector3 center;
    if(moveScene->selectedNodeIds.size() > 0)
        center = moveScene->getPivotPoint(false);
    else if(moveScene->hasJointSelected())
        center = moveScene->getSelectedJoint()->jointNode->getAbsoluteTransformation().getTranslation();
    else if(moveScene->hasNodeSelected())
        center = moveScene->getSelectedNode()->node->getAbsoluteTransformation().getTranslation();
    else
        return false;
    
    moveScene->controlsPlane->setPositionAndNormal(center, (moveScene->selectedControlId == Z_ROTATE) ? Vector3(1, 0, 0) :  SceneHelper::planeFacingDirection(moveScene->selectedControlId % 3));
    
    if(moveScene->controlType == MOVE || moveScene->controlType == SCALE) { // position and scale calculations are same
        
        Vector2 p1;
        p1.x = prevTouchPoint.x; p1.y = prevTouchPoint.y;
        Vector2 p2;
        p2.x = curPoint.x;	p2.y = curPoint.y;
        
        Vector3 oldPos;
        Vector3 newPos;
        getOldAndNewPosInWorld(prevTouchPoint, curPoint, oldPos, newPos);
        Vector3 delta = newPos - oldPos;
        
        if(delta.getLength() > 1.0f) {
            delta = delta.normalize() * 1.0f;
        }
        outputValue.x = ((moveScene->selectedControlId == X_MOVE || moveScene->selectedControlId == X_SCALE) ? 1.0 : 0.0) * delta.x;
        outputValue.y = ((moveScene->selectedControlId == Y_MOVE || moveScene->selectedControlId == Y_SCALE) ? 1.0 : 0.0) * delta.y;
        outputValue.z = ((moveScene->selectedControlId == Z_MOVE || moveScene->selectedControlId == Z_SCALE) ? 1.0 : 0.0) * delta.z;
        
        return true;
    } else if(moveScene->controlType == ROTATE) {
        Vector2 p1;
        p1.x = prevTouchPoint.x;
        p1.y = prevTouchPoint.y;
        
        Vector2 p2;
        p2.x = curPoint.x;
        p2.y = curPoint.y;
        
        Vector3 oldPos;
        Vector3 newPos;
        getOldAndNewPosInWorld(prevTouchPoint, curPoint, oldPos, newPos);
        Vector3 parentToNewPos = (newPos - center).normalize();
        Vector3 parentToOldPos = (oldPos - center).normalize();
        
        Quaternion delta = MathHelper::rotationBetweenVectors(parentToNewPos, parentToOldPos);
        Quaternion nodeRot;
        
        if(moveScene->selectedNodeIds.size() > 0) {
            
            moveScene->getParentNode()->updateAbsoluteTransformation();
            nodeRot = moveScene->getParentNode()->getRotation();
            delta = MathHelper::RotateNodeInWorld(nodeRot, delta);
            
        } else if(moveScene->hasJointSelected() || !isSGJoint) {
            
            shared_ptr<Node> jointNode;
            if(!isSGJoint)
                jointNode = moveScene->getSelectedNode()->node;
            else
                jointNode = moveScene->getSelectedJoint()->jointNode;
            
            nodeRot = MathHelper::getGlobalQuaternion(jointNode);
            Quaternion jointGlobalRot = MathHelper::RotateNodeInWorld(nodeRot, delta);
            if(!isSGJoint)
                delta = MathHelper::irrGetLocalQuaternion((moveScene->getSelectedNode()->node), jointGlobalRot);
            else
                delta = MathHelper::irrGetLocalQuaternion(moveScene->getSelectedJoint()->jointNode, jointGlobalRot);
            
        } else if(moveScene->getSelectedNode()) {
            
            moveScene->getSelectedNode()->node->updateAbsoluteTransformation();
            nodeRot = (moveScene->directionIndicator->node->getVisible()) ? moveScene->directionIndicator->node->getRotation() :moveScene->getSelectedNode()->node->getRotation();
            if(moveScene->directionLine->node->getVisible())
                nodeRot = moveScene->directionLine->node->getRotation();
            delta =  MathHelper::RotateNodeInWorld(nodeRot, delta);
        }
        
        outputQuatValue = delta;
        return true;
    }
    return false;
}

void SGMovementManager::getOldAndNewPosInWorld(Vector2 prevTouchPoint, Vector2 curPoint, Vector3& oldPos, Vector3& newPos) {
    Line3D oldRay = moveScene->cmgr->getRayFromScreenCoordinates(prevTouchPoint, smgr->getActiveCamera(), SceneHelper::screenWidth, SceneHelper::screenHeight);
    Line3D newRay = moveScene->cmgr->getRayFromScreenCoordinates(curPoint, smgr->getActiveCamera(), SceneHelper::screenWidth, SceneHelper::screenHeight);
    
    Vector3 oldRayDir = (oldRay.end - oldRay.start).normalize();
    Vector3 newRayDir = (newRay.end - newRay.start).normalize();
    moveScene->controlsPlane->getIntersectionWithLine(oldRay.start,oldRayDir,oldPos);
    moveScene->controlsPlane->getIntersectionWithLine(newRay.start,newRayDir,newPos);
    moveScene->circleTouchPoint = newPos;
}

void SGMovementManager::touchBeganRig(Vector2 curTouchPos) {
    if(!moveScene || !smgr || !moveScene->isRigMode)
        return;
    
    prevTouchPoints[0] = curTouchPos;
    moveScene->updater->updateControlsOrientaion();
    moveScene->renHelper->rttControlSelectionAnim(curTouchPos);
    if(moveScene->shaderMGR->deviceType == METAL){
        moveScene->selectMan->getCtrlColorFromTouchTextureAnim(curTouchPos);
    }
    xAcceleration = yAcceleration = 0.0;
}

void SGMovementManager::touchMoveRig(Vector2 curTouchPos,Vector2 prevTouchPos,float width,float height) {
    if(!moveScene || !smgr || !moveScene->isRigMode || !moveScene->rigMan->isNodeSelected)
        return;
    
    bool isNodeSelected = moveScene->hasNodeSelected();
    bool isJointSelected = moveScene->hasJointSelected();
    
    if(moveScene->isControlSelected) {
        Vector3 outputValue;
        Quaternion outputQuatValue;
        calculateControlMovements(curTouchPos, prevTouchPoints[0], outputValue, outputQuatValue, isJointSelected ? true : false);
        prevTouchPoints[0] = curTouchPos;
        switch(moveScene->controlType){
            case MOVE:
                if(isNodeSelected) {
                    if(moveScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS)
                        moveScene->actionMan->changeSkeletonPosition(outputValue);
                    else
                        moveScene->actionMan->changeSGRPosition(outputValue);
                }
                break;
            case ROTATE:
                if(isNodeSelected) {
                    moveScene->actionMan->changeSkeletonRotation(outputQuatValue);
                    moveScene->actionMan->changeSGRRotation(outputQuatValue);
                }
                break;
            case SCALE:
                break;
            default:
                break;
        }
        moveScene->updater->updateControlsOrientaion();
    }
    return;
}

void SGMovementManager::touchEndRig(Vector2 curTouchPos) {
    if(!moveScene || !smgr || !moveScene->isRigMode)
        return;
    
    swipeTiming = 0;
    moveScene->updater->updateControlsMaterial();
    if(moveScene->isControlSelected) {
        moveScene->selectedControlId = NOT_SELECTED;
        moveScene->isControlSelected = false;
    }
}

