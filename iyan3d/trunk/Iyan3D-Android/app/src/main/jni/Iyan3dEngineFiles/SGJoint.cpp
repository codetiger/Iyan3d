//
//  SGJoint.cpp
//  Iyan3D
//
//  Created by Vivek on 24/03/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGJoint.h"

SGJoint::SGJoint()
{
    positionKeys.clear();
    rotationKeys.clear();
    scaleKeys.clear();
    visibilityKeys.clear();
}
void SGJoint::readData(ifstream * filePointer)
{
    // Some values read but not stored are not neccasary, but it should be done according to the file format.
    
    // Some values read but not stored are not neccasary, but it should be done according to the file format.
    
    FileHelper::readInt(filePointer);
    int sgbVersion = FileHelper::readInt(filePointer);
    FileHelper::readInt(filePointer);
    FileHelper::readBool(filePointer);
    FileHelper::readBool(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readString(filePointer,sgbVersion);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    FileHelper::readFloat(filePointer);
    
    int keysCount = FileHelper::readInt(filePointer);
    KeyHelper::readData(filePointer, keysCount, positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    FileHelper::readInt(filePointer);
}
void SGJoint::writeData(ofstream * filePointer)
{
    
    // Junk values written are not neccassary, but they are provided because files should be written in the same old format.
    
    FileHelper::writeInt(filePointer, -1);
    FileHelper::writeInt(filePointer, SGB_VERSION);
    FileHelper::writeInt(filePointer, 0);
    FileHelper::writeBool(filePointer , false);
    FileHelper::writeBool(filePointer, false);
    FileHelper::writeFloat(filePointer, 0);
    FileHelper::writeFloat(filePointer, 0);
    FileHelper::writeString(filePointer, "");
    FileHelper::writeFloat(filePointer, 0);
    FileHelper::writeFloat(filePointer, 0);
    FileHelper::writeFloat(filePointer, 0);
    //To use it after KeyHelper created
    KeyHelper::writeData(filePointer, positionKeys, rotationKeys, scaleKeys, visibilityKeys);
    FileHelper::writeInt(filePointer, 0);
}
void SGJoint::setRotation(Quaternion rotation, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(rotationKeys,frameId);
    if(keyIndex == -1 || rotationKeys[keyIndex].id != frameId){
        SGRotationKey rotationKey;
        rotationKey.id = frameId;
        rotationKey.rotation = rotation;
        KeyHelper::addKey(rotationKeys, rotationKey);
    }
    else
        rotationKeys[keyIndex].rotation = rotation;
}
bool SGJoint::setRotationOnNode(Quaternion rotation)
{
    Vector3 eulerRotation;
    rotation.toEuler(eulerRotation);
    eulerRotation = eulerRotation * RADTODEG;
    if(jointNode->getRotationInDegrees() == eulerRotation)
        return false;
    
    jointNode->setRotationInDegrees(eulerRotation);
    jointNode->updateAbsoluteTransformationOfChildren();
    return true;
}
void SGJoint::setPosition(Vector3 position , int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(positionKeys,frameId);
    if(keyIndex == -1 || positionKeys[keyIndex].id != frameId){
        SGPositionKey positionKey;
        positionKey.id = frameId;
        positionKey.position = position;
        KeyHelper::addKey(positionKeys, positionKey);
    }
    else
        positionKeys[keyIndex].position = position;
}
bool SGJoint::setPositionOnNode(Vector3 position)
{
    if(jointNode->getPosition() == position)
        return false;
    jointNode->setPosition(position);
    jointNode->updateAbsoluteTransformation();
    return true;
}
void SGJoint::setScale(Vector3 scale, int frameId)
{
    int keyIndex = KeyHelper::getKeyIndex(scaleKeys,frameId);
    if(keyIndex == -1 || scaleKeys[keyIndex].id != frameId){
        SGScaleKey scaleKey;
        scaleKey.id = frameId;
        scaleKey.scale = scale;
        KeyHelper::addKey(scaleKeys, scaleKey);
    }
    else
        scaleKeys[keyIndex].scale = scale;
}
bool SGJoint::setScaleOnNode(Vector3 scale)
{
    if(jointNode->getScale() == scale)
        return false;
    jointNode->setScale(scale);
    jointNode->updateAbsoluteTransformation();
    return true;
}
ActionKey SGJoint::getKeyForFrame(int frameId)
{
    ActionKey key;
    int index = KeyHelper::getKeyIndex(rotationKeys, frameId);
    if(index!=-1 && rotationKeys[index].id == frameId){
        key.rotation = rotationKeys[index].rotation;
        key.isRotationKey = true;
    }
    
    index = KeyHelper::getKeyIndex(positionKeys, frameId);
    if(index!=-1 &&  positionKeys[index].id == frameId){
        key.position = positionKeys[index].position;
        key.isPositionKey = true;
    }
    
    index = KeyHelper::getKeyIndex(scaleKeys, frameId);
    if(index!=-1 &&  scaleKeys[index].id == frameId){
        key.scale = scaleKeys[index].scale;
        key.isScaleKey = true;
    }

    return key;
}
void SGJoint::setKeyForFrame(int frameId, ActionKey& key)
{
    int index = KeyHelper::getKeyIndex(rotationKeys, frameId);
    if(index!=-1  && rotationKeys[index].id == frameId){
        rotationKeys.erase(rotationKeys.begin()+index);
    }
    if(key.isRotationKey){
        setRotation(key.rotation, frameId);
    }
    
    if(positionKeys.size()) {
        index = KeyHelper::getKeyIndex(positionKeys, frameId);
        if(index!=-1 && positionKeys[index].id == frameId){
            positionKeys.erase(positionKeys.begin()+index);
        }
    }
    if(key.isPositionKey)
        setPosition(key.position, frameId);
    
    if(scaleKeys.size()) {
        index = KeyHelper::getKeyIndex(scaleKeys, frameId);
        if(index!=-1 && scaleKeys[index].id == frameId){
            scaleKeys.erase(scaleKeys.begin()+index);
        }
    }
    if(key.isScaleKey)
        setScale(key.scale, frameId);
}
void SGJoint::removeAnimationInCurrentFrame(int selectedFrame)
{
    int keyIndex = KeyHelper::getKeyIndex(positionKeys, selectedFrame);
    if(keyIndex != -1 && positionKeys[keyIndex].id == selectedFrame)
        positionKeys.erase(positionKeys.begin()+keyIndex);
    
    keyIndex = KeyHelper::getKeyIndex(rotationKeys, selectedFrame);
    if(keyIndex != -1 && rotationKeys[keyIndex].id == selectedFrame)
        rotationKeys.erase(rotationKeys.begin()+keyIndex);
    
    keyIndex = KeyHelper::getKeyIndex(scaleKeys, selectedFrame);
    if(keyIndex != -1 && scaleKeys[keyIndex].id == selectedFrame)
        scaleKeys.erase(scaleKeys.begin()+keyIndex);
}
SGJoint::~SGJoint()
{
    positionKeys.clear();
    rotationKeys.clear();
    scaleKeys.clear();
    visibilityKeys.clear();
    jointNode.reset();
}