#include "HeaderFiles/SGAction.h"


SGAction::SGAction()
{
    actionType = ACTION_EMPTY;
    objectIndex = -1;
    frameId = -1;
    
    actionSpecificIntegers.clear();
    keys.clear();
    actionSpecificFlags.clear();
    actionSpecificFloats.clear();
}

void SGAction::drop(){
    objectIndex = -1;
    frameId = -1;
    actionType = ACTION_EMPTY;
    actionSpecificStrings.clear();
    actionSpecificIntegers.clear();
    keys.clear();
    actionSpecificFlags.clear();
    actionSpecificFloats.clear();
    
    nodePositionKeys.clear();
    nodeRotationKeys.clear();
    nodeSCaleKeys.clear();
    nodeVisibilityKeys.clear();
    for(int i = 0;i < jointRotKeys.size();i++) {
        if(jointRotKeys.find(i) != jointRotKeys.end())
            jointRotKeys[i].clear();
    }
}

SGAction::~SGAction(){
    actionSpecificStrings.clear();
    actionSpecificIntegers.clear();
    keys.clear();
    actionSpecificFlags.clear();
    actionSpecificFloats.clear();
    nodePositionKeys.clear();
    nodeRotationKeys.clear();
    nodeSCaleKeys.clear();
    nodeVisibilityKeys.clear();
    for(int i = 0;i < jointRotKeys.size();i++) {
        if(jointRotKeys.find(i) != jointRotKeys.end())
            jointRotKeys[i].clear();
    }
}


