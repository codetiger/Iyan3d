//
//  SGMovementManager.cpp
//  Iyan3D
//
//  Created by Karthik on 27/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "SGMovementManager.h"
#include "SGEditorScene.h"

SGEditorScene *moveScene;

SGMovementManager::SGMovementManager(SceneManager* smgr, void * scene)
{
    this->smgr = smgr;
    moveScene = (SGEditorScene*)scene;
    swipeTiming = 0;
    xAcceleration = yAcceleration = 0.0;
}

void SGMovementManager::touchBegan(Vector2 curTouchPos)
{
    xAcceleration = yAcceleration = 0.0;
}

void SGMovementManager::swipeProgress(float angleX , float angleY)
{
    if(!moveScene || !smgr)
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

void SGMovementManager::swipeToRotate()
{
    if(!moveScene || !smgr)
        return;

    if((fabs(xAcceleration) > 0.0 || fabs(yAcceleration) > 0.0)) {
        
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
    }
}

void SGMovementManager::touchEnd(Vector2 curTouchPos)
{
    if(!moveScene || !smgr)
        return;

    // TODO add commented functions
    //setLightingOn();
    swipeTiming = 0;
//    moveScene->updater->updateControlsMaterial();
    if(moveScene->isControlSelected) {
        Logger::log(INFO , "SGScene diff ", "touch end");
//        prevRotX = prevRotY = prevRotZ = 0.0;
        moveScene->selectedControlId = NOT_SELECTED;
        moveScene->isControlSelected = false;
//        moveScene->storeMovementAction();
    }
}

void SGMovementManager::panBegan(Vector2 touch1, Vector2 touch2)
{
    if(!moveScene || !smgr)
        return;

    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    previousTarget = moveScene->cameraTarget;
    previousRadius = moveScene->cameraRadius;
}

void SGMovementManager::panProgress(Vector2 touch1, Vector2 touch2)
{
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
    Vector3 cameraRotation = moveScene->viewCamera->getRotation();
    cameraRotation.z += rotationDif;
    Quaternion quat;
    quat.fromAngleAxis(rotationDif, camForward);  //This takes rotation in rad, so it's already right
    Mat4 mat = quat.getMatrix();
    mat.rotateVect(camUpReal);
    mat.rotateVect(camRight);
    moveScene->viewCamera->setUpVector(camUpReal);
    
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    moveScene->updater->updateControlsOrientaion();
}


