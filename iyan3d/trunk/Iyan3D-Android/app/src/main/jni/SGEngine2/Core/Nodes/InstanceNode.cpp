//
//  InstanceNode.cpp
//  SGEngine2
//
//  Created by Vivek on 21/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#include "InstanceNode.h"
InstanceNode::InstanceNode(){
    position = Vector3(0.0);
    rotation = Vector3(0.0);
    scale = Vector3(1.0);
}
InstanceNode::~InstanceNode(){
    
}
Mat4 InstanceNode::getAbsoluteTransformation(){
    AbsoluteTransformation.translate(position);
    AbsoluteTransformation.setRotationRadians(rotation);
    AbsoluteTransformation.scale(scale);
    return AbsoluteTransformation;
}
void InstanceNode::setRotationInDegrees(Vector3 rotation){
    this->rotation = Vector3(rotation.x * DEGTORAD,rotation.y * DEGTORAD,rotation.z * DEGTORAD);
}
void InstanceNode::setRotationInRadians(Vector3 rotation){
    this->rotation = rotation;
}
void InstanceNode::setPosition(Vector3 position){
    this->position = position;
}
void InstanceNode::setScale(Vector3 scale){
    this->scale = scale;
}
Vector3 InstanceNode::getRotationInRadians(){
    return rotation;
}
Vector3 InstanceNode::getRotationInDegrees(){
    return rotation * RADTODEG;
}
Vector3 InstanceNode::getPosition(){
    return position;
}
Vector3 InstanceNode::getScale(){
    return scale;
}