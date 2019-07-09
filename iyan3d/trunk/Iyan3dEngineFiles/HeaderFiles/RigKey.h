#ifndef Iyan3D_RigKey_h
#define Iyan3D_RigKey_h

#include <iostream>
#include <string.h>
#include "../../SGEngine2/Utilities/Logger.h"
#include "Constants.h"
#include "SGNode.h"

#include <map>
class RigKey{

public:
    int parentId;
    SGNode *referenceNode;
    SGNode *sphere;
    SGNode *bone;
    float envelopeRadius;
    
    RigKey(){
        referenceNode = NULL;
        parentId = -1;
        sphere = NULL;
        bone = NULL;
        envelopeRadius = 0.5;
        referenceNode = new SGNode(NODE_SGM);
        sphere = new SGNode(NODE_SGM);
        bone = new SGNode(NODE_SGM);
    }
    
    void drop(){
//        if(referenceNode){
//            referenceNode = NULL;
//        }
//        if(sphere){
//            sphere = NULL;
//        }
//        if(bone){
//            bone = NULL;
//        }
    }
    
    ~RigKey(){
        if(referenceNode) {
            delete referenceNode;
            referenceNode = NULL;
        }
        if(sphere) {
            delete sphere;
            sphere = NULL;
        }
        if(bone) {
            delete bone;
            bone = NULL;
        }
    }
};

#endif