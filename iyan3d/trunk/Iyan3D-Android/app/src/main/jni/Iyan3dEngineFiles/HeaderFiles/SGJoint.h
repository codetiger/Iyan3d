#ifndef __Iyan3DEngine__SGJoint__
#define __Iyan3DEngine__SGJoint__

#include <iostream>
#include "Constants.h"
#include "SGPositionKey.h"
#include "SGRotationKey.h"
#include "SGScaleKey.h"
#include "SGVisibilityKey.h"
#include "FileHelper.h"
#include "ActionKey.h"
#include "KeyHelper.h"

using namespace std;

class SGJoint{
    
public:
    
    SGJoint();
    shared_ptr<JointNode> jointNode;
    vector<SGPositionKey> positionKeys;
    vector<SGRotationKey> rotationKeys;
    vector<SGScaleKey> scaleKeys;
    vector<SGVisibilityKey> visibilityKeys;
    
    void  setRotation(Quaternion rotation , int frameId);
    void  setPosition(Vector3 position , int frameId);
    void  setScale(Vector3 scale , int frameId);
    bool  setRotationOnNode(Quaternion rotation);
    bool  setPositionOnNode(Vector3 position);
    bool  setScaleOnNode(Vector3 scale);
    
    void  readData(ifstream * filePointer);
    void  writeData(ofstream * filePointer);
    ActionKey getKeyForFrame(int frameId);
    void setKeyForFrame(int frameId, ActionKey& key);
    void removeAnimationInCurrentFrame(int selectedFrame);
    
    ~SGJoint();
};
#endif