//
//  CollisionManager.h
//  Iyan3D
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef __Iyan3D__CollisionManager__
#define __Iyan3D__CollisionManager__

#include <iostream>
#include "../../Core/Nodes/CameraNode.h"
#include "common.h"
#include "Frustum.h"
#include "Plane3D.h"
#include "../../Core/Nodes/CameraNode.h"
#include "Line3D.h"
class CollisionManager {
public:
    CollisionManager();
    ~CollisionManager();
    Line3D getRayFromScreenCoordinates(Vector2 screenCoords, shared_ptr<CameraNode> camera, double screenWidth, double screenHeight);
};
#endif /* defined(__Iyan3D__CollisionManager__) */
