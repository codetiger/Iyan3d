#ifndef __Iyan3DEngine__SGAction__
#define __Iyan3DEngine__SGAction__

#include "Constants.h"
#include "SGNode.h"
#include <vector>
#include "ActionKey.h"
#include <iostream>
#define JOINT_ROTATION_KEYS_SIZE 54

class SGAction
{
public:
    
    ActionType actionType;
    int objectIndex;
    int frameId;
    vector<std::wstring> actionSpecificStrings;
    
    //change keys
    vector<int> actionSpecificIntegers;
    vector<ActionKey> keys;
    vector<bool> actionSpecificFlags;
    vector<float> actionSpecificFloats;
    
    vector<SGPositionKey> nodePositionKeys;
    vector<SGRotationKey> nodeRotationKeys;
    vector<SGScaleKey>    nodeSCaleKeys;
    vector<SGVisibilityKey> nodeVisibilityKeys;
    //vector<SGRotationKey> jointsRotationKeys[JOINT_ROTATION_KEYS_SIZE];
    map<int,vector<SGRotationKey> > jointRotKeys;
    map<int,vector<SGPositionKey> > jointPosKeys;
    map<int,vector<SGScaleKey> > jointScaleKeys;
    void drop();
    
    SGAction();
    ~SGAction();
};

#endif