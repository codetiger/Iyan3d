//
//  SGAutoRigSceneManager.h
//  Iyan3D
//
//  Created by Karthik on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGAutoRigSceneManager_h
#define SGAutoRigSceneManager_h

#include "SGNode.h"
#include "Constants.h"
#include "RigKey.h"
#include "AutoRigHelper.h"
#include "AutoRigJointsDataHelper.h"

#define OBJ_BOUNDINGBOX_MAX_LIMIT 24.0
#define RIG_BONE_SCALE_TEMP_FIX 0.85
#define RIG_MAX_BONES 57

enum SKELETON_TYPE{
    SKELETON_HUMAN,
    SKELETON_OWN
};

class SGAutoRigSceneManager
{
private:
    std::string animatedSGRPath;
    SceneManager *smgr;
    SGNode* sgrSGNode;
    Mesh *boneMesh,*sphereMesh;
    std::map<int, RigKey>::iterator rigKeyIterator;
    std::map<int, vector<InfluencedObject> > influencedVertices;    //For each joint
    std::map<int, vector<InfluencedObject> > influencedJoints;      //For each vertex
    // TODO SGAction moveAction,_scaleAction,addJointAction;


public:
    
    bool isSkeletonJointSelected;
    bool isSGRJointSelected, isNodeSelected;

    int selectedNodeId, selectedJointId, rigBoneCount;
    AUTORIG_SCENE_MODE sceneMode;
    SKELETON_TYPE skeletonType;
    
    SGNode* objSGNode;
    SGNode* selectedNode;
    SGJoint* selectedJoint;
    std::map<int, SGNode*> envelopes;
    std::map<int, RigKey> rigKeys;
    Vector2 touchPosForSkeletonSelection;

    SGAutoRigSceneManager(SceneManager* smgr, void *scene);
    
    void clearNodeSelections();
    void objForRig(SGNode* sgNode);
    void addNewJoint();
    void removeJoint();
    void exportSGR(std::string filePath);
    
    void setEnvelopeUniforms(int nodeID,string matName);
    void objNodeCallBack(string materialName);
    void boneNodeCallBack(int id,string materialName);
    void jointNodeCallBack(int id,string materialName);
    void setSGRUniforms(int jointId,string matName);
    bool isSGRTransparent(int jointId,string matName);
    bool isOBJTransparent(string materialName);
    
    SGNode* getRiggedNode();
    
    /* Call Backs */
    void (*boneLimitsCallBack)();
    void (*objLoaderCallBack)(int);
    
    bool setSceneMode(AUTORIG_SCENE_MODE mode);
    void initSkeletonJoints();
    bool findInRigKeys(int key);
    void removeRigKeys();
    void resetRigKeys();
    ~SGAutoRigSceneManager();

};

#endif /* SGAutoRigSceneManager_h */
