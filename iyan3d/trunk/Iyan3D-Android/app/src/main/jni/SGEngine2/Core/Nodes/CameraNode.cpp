//
//  CameraNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 17/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "CameraNode.h"

CameraNode::CameraNode() {
    type = NODE_TYPE_CAMERA;
    isActive = false;
    position = Vector3(0.0);
    target = Vector3(0.0);
    UpVector = Vector3(0.0,1.0,0.0);
    fov = 65.0*(PI/180.0);
    aspectRatio = 4.0/3.0;
    nearDistance = 1.0;
    farDistance = 3000.0;
    needsVertexPosition = bindTargetAndRotation = needsIndexBuf = false;
    viewFrustum = new Frustum();
    update();
}
CameraNode::~CameraNode() {
    if(viewFrustum)
        delete viewFrustum;
}
void CameraNode::setPosition(Vector3 position){
    this->position = position;
    update();
}
void CameraNode::setTarget(Vector3 target){
    this->target = target;
    update();
}
void CameraNode::setRotation(Vector3 rotation){
    if(bindTargetAndRotation)
        this->target = position + rotation.rotationToDirection();
    this->rotation = rotation;
    update();
}
void CameraNode::setScale(Vector3 Scale){
    
}
void CameraNode::setFOVInRadians(float fov){
    this->fov = fov;
    update();
}
void CameraNode::setAspectRatio(float aspectRatio){
    this->aspectRatio = aspectRatio;
    update();
}
void CameraNode::setNearValue(float value){
    this->nearDistance = value;
    update();
}
void CameraNode::setFarValue(float value){
    this->farDistance = value;
    update();
}
void CameraNode::setActive(bool status){
    isActive = status;
}
void CameraNode::setUpVector(Vector3 UpVector){
    this->UpVector = UpVector;
    update();
}
void CameraNode::setTargetAndRotationBindStatus(bool bind){
    bindTargetAndRotation = bind;
}
bool CameraNode::getActiveStatus(){
    return  isActive;
}
Vector3 CameraNode::getPosition(){
    return position;
}
Vector3 CameraNode::getTarget(){
    return target;
}
float CameraNode::getFOVInRadians(){
    return fov;
}
float CameraNode::getAspectRatio(){
    return aspectRatio;
}
float CameraNode::getNearValue(){
    return nearDistance;
}
float CameraNode::getFarValue(){
    return farDistance;
}
Vector3 CameraNode::getUpVector(){
    return UpVector;
}
Mat4 CameraNode::getViewMatrix(){
    Vector3 zaxis = target - position;
    zaxis.normalize();
    
    Vector3 xaxis = UpVector.crossProduct(zaxis);
    xaxis.normalize();
    
    Vector3 yaxis = zaxis.crossProduct(xaxis);
    yaxis.normalize();
    
    (view)[0] = xaxis.x;
    (view)[1] = yaxis.x;
    (view)[2] = zaxis.x;
    (view)[3] = 0;
    
    (view)[4] = xaxis.y;
    (view)[5] = yaxis.y;
    (view)[6] = zaxis.y;
    (view)[7] = 0;
    
    (view)[8] = xaxis.z;
    (view)[9] = yaxis.z;
    (view)[10] = zaxis.z;
    (view)[11] = 0;
    
    (view)[12] = -xaxis.dotProduct(position);
    (view)[13] = -yaxis.dotProduct(position);
    (view)[14] = -zaxis.dotProduct(position);
    (view)[15] = 1;
    return view;
}
Mat4 CameraNode::getProjectionMatrix(){
    const float h = 1.0 / (tan(fov*0.5));
    const float w = (h / aspectRatio);
    
    (projection)[0] = w;
    (projection)[1] = 0;
    (projection)[2] = 0;
    (projection)[3] = 0;
    
    (projection)[4] = 0;
    (projection)[5] = h;
    (projection)[6] = 0;
    (projection)[7] = 0;
    
    (projection)[8] = 0;
    (projection)[9] = 0;
    (projection)[10] = (farDistance/(farDistance-nearDistance));
    (projection)[11] = 1;
    
    (projection)[12] = 0;
    (projection)[13] = 0;
    (projection)[14] = (-nearDistance*farDistance/(farDistance-nearDistance));
    (projection)[15] = 0;
    return projection;
}
Vector3 CameraNode::getRotation(){
    return rotation;
}
Vector3 CameraNode::getScale(){
    return scale;
}
void CameraNode::update(){
    getViewMatrix();
    getProjectionMatrix();
    Mat4 mat;
    mat.setbyproduct(projection,view);
    viewFrustum->constructWithProjViewMatrix(mat);
}
Frustum* CameraNode::getViewFrustum(){
    return  viewFrustum;
}