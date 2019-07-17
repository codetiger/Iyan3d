//
//  CameraNode.h
//  SGEngine2
//
//  Created by Harishankar on 17/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__CameraNode__
#define __SGEngine2__CameraNode__

#include "../common/common.h"
#include "../Nodes/Node.h"
#include "../common/Frustum.h"
class CameraNode : public Node {
private:
    bool     isActive, bindTargetAndRotation;
    Vector3  target;
    Vector3  UpVector;
    float    fov, aspectRatio, nearDistance, farDistance;
    Mat4     view, projection;
    Frustum* viewFrustum;

public:
    Vector2 size;

    CameraNode();
    ~CameraNode();

    void setPosition(Vector3 position);
    void setTarget(Vector3 target);
    void setRotation(Quaternion rotation);
    void setFOVInRadians(float fov);
    void setAspectRatio(float aspectRatio);
    void setNearValue(float value);
    void setFarValue(float value);
    void setActive(bool status);
    void setUpVector(Vector3 UpVector);
    void setTargetAndRotationBindStatus(bool bind);
    void update();

    bool getActiveStatus();

    Vector3 getUpVector();
    Vector3 getPosition();
    Vector3 getTarget();

    float getFOVInRadians();
    float getAspectRatio();
    float getNearValue();
    float getFarValue();

    Mat4     getViewMatrix();
    Mat4     getProjectionMatrix();
    Frustum* getViewFrustum();
};

#endif /* defined(__SGEngine2__CameraNode__) */
