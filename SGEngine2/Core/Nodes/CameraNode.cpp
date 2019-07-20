//
//  CameraNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 17/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "CameraNode.h"

CameraNode::CameraNode() {
    type                  = NODE_TYPE_CAMERA;
    isActive              = false;
    position              = Vector3(0.0);
    target                = Vector3(0.0);
    UpVector              = Vector3(0.0, 1.0, 0.0);
    fov                   = 65.0 * (PI / 180.0);
    aspectRatio           = 4.0 / 3.0;
    nearDistance          = 1.0;
    farDistance           = 3000.0;
    bindTargetAndRotation = false;
    viewFrustum           = new Frustum();
    update();
}

CameraNode::~CameraNode() {
    if (viewFrustum)
        delete viewFrustum;
}

void CameraNode::setPosition(Vector3 position) {
    this->position = position;
    update();
}

void CameraNode::setTarget(Vector3 target) {
    this->target = target;
    update();
}

void CameraNode::setRotation(Quaternion rotation) {
    if (bindTargetAndRotation) {
        Mat4 T = Mat4();
        T.translate(position);

        Mat4 R = rotation.getMatrix();

        Mat4 transform = (T * R);

        Vector3 direction = Vector3(0.0, 0.0, -1.0);
        transform.rotateVect(direction);

        this->target = transform.getTranslation() + direction;
    }

    this->rotation = rotation;
    update();
}

void CameraNode::setFOVInRadians(float fov) {
    this->fov = fov;
    update();
}

void CameraNode::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
    update();
}

void CameraNode::setNearValue(float value) {
    this->nearDistance = value;
    update();
}

void CameraNode::setFarValue(float value) {
    this->farDistance = value;
    update();
}

void CameraNode::setActive(bool status) {
    isActive = status;
}

void CameraNode::setUpVector(Vector3 UpVector) {
    this->UpVector = UpVector;
    update();
}

void CameraNode::setTargetAndRotationBindStatus(bool bind) {
    bindTargetAndRotation = bind;
}

bool CameraNode::getActiveStatus() {
    return isActive;
}

Vector3 CameraNode::getPosition() {
    return position;
}

Vector3 CameraNode::getTarget() {
    return target;
}

float CameraNode::getFOVInRadians() {
    return fov;
}

float CameraNode::getAspectRatio() {
    return aspectRatio;
}

float CameraNode::getNearValue() {
    return nearDistance;
}

float CameraNode::getFarValue() {
    return farDistance;
}

Vector3 CameraNode::getUpVector() {
    return UpVector;
}

Mat4 CameraNode::getViewMatrix() {
    Vector3 zaxis = target - position;
    zaxis.normalize();

    Vector3 xaxis = UpVector.crossProduct(zaxis);
    xaxis.normalize();

    Vector3 yaxis = zaxis.crossProduct(xaxis);
    yaxis.normalize();

    view.setElement(0, xaxis.x);
    view.setElement(1, yaxis.x);
    view.setElement(2, zaxis.x);
    view.setElement(3, 0);

    view.setElement(4, xaxis.y);
    view.setElement(5, yaxis.y);
    view.setElement(6, zaxis.y);
    view.setElement(7, 0);

    view.setElement(8, xaxis.z);
    view.setElement(9, yaxis.z);
    view.setElement(10, zaxis.z);
    view.setElement(11, 0);

    view.setElement(12, -xaxis.dotProduct(position));
    view.setElement(13, -yaxis.dotProduct(position));
    view.setElement(14, -zaxis.dotProduct(position));
    view.setElement(15, 1);
    return view;
}

Mat4 CameraNode::getProjectionMatrix() {
    const float h = 1.0 / (tan(fov * 0.5));
    const float w = (h / aspectRatio);

    projection.setElement(0, w);
    projection.setElement(1, 0);
    projection.setElement(2, 0);
    projection.setElement(3, 0);

    projection.setElement(4, 0);
    projection.setElement(5, h);
    projection.setElement(6, 0);
    projection.setElement(7, 0);

    projection.setElement(8, 0);
    projection.setElement(9, 0);
    projection.setElement(10, (farDistance / (farDistance - nearDistance)));
    projection.setElement(11, 1);

    projection.setElement(12, 0);
    projection.setElement(13, 0);
    projection.setElement(14, (-nearDistance * farDistance / (farDistance - nearDistance)));
    projection.setElement(15, 0);
    return projection;
}

void CameraNode::update() {
    getViewMatrix();
    getProjectionMatrix();
    Mat4 mat;
    mat.setbyproduct(projection, view);
    viewFrustum->constructWithProjViewMatrix(mat);
}

Frustum* CameraNode::getViewFrustum() {
    return viewFrustum;
}
