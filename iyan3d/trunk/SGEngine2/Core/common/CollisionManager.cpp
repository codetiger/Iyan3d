//
//  CollisionManager.cpp
//  Iyan3D
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include "CollisionManager.h"
CollisionManager::CollisionManager()
{

}

CollisionManager::~CollisionManager()
{

}

Line3D CollisionManager::getRayFromScreenCoordinates(Vector2 screenCoords, shared_ptr<CameraNode> camera, double screenWidth, double screenHeight)
{
    Frustum* f = camera->getViewFrustum();
    Vector3 farLeftUp = f->getFarLeftUp();
    Vector3 lefttoright = f->getFarRightUp() - farLeftUp;
    Vector3 uptodown = f->getFarLeftDown() - farLeftUp;
    float dx = screenCoords.x / screenWidth;
    float dy = screenCoords.y / screenHeight;
    Line3D ln;
    ln.start = camera->getPosition();
    ln.end = farLeftUp + (lefttoright * dx) + (uptodown * dy);
    //camera.reset();

    return ln;
}
