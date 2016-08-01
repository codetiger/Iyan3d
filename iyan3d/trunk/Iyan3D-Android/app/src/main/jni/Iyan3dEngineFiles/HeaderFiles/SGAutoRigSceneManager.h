//
//  SGAutoRigSceneManager.h
//  Iyan3D
//
//  Created by Karthik on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGAutoRigSceneManager_h
#define SGAutoRigSceneManager_h

#include "exportSGR.h"
#include "AutoRigHelper.h"
#include "AutoRigJointsDataHelper.h"
#include "Constants.h"
#include "SGNode.h"

#define OBJ_BOUNDINGBOX_MAX_LIMIT 24.0
#define RIG_BONE_SCALE_TEMP_FIX 0.85

enum SKELETON_TYPE{
    SKELETON_HUMAN,
    SKELETON_OWN
};

class SGAutoRigSceneManager
{
private:
    
    int actualNodeId;
    std::string animatedSGRPath;
    shared_ptr<AnimatedMeshNode> animNode;
    SkinMesh* sMesh;
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
    bool isVertexColoredNode;

    int selectedNodeId, selectedJointId, rigBoneCount;
    float scaleRatio;
    AUTORIG_SCENE_MODE sceneMode;
    SKELETON_TYPE skeletonType;
    NODE_TYPE rigNodeType;
    
    SGNode* nodeToRig;
    SGNode* selectedNode;
    SGJoint* selectedJoint;
    std::map<int, SGNode*> envelopes;
    std::map<int, RigKey> rigKeys;
    Vector2 touchPosForSkeletonSelection;
    shared_ptr<MeshNode> sgmNode;

    SGAutoRigSceneManager(SceneManager* smgr, void *scene);
    
    void clearNodeSelections();
    SGNode* getSGMNodeForRig(SGNode *rigNode);
    void sgmForRig(SGNode* sgNode); // Pass the node to be rigged to this method
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
    void (*objLoaderCallBack)(int);
    
    bool setSceneMode(AUTORIG_SCENE_MODE mode);
    void switchSceneMode(AUTORIG_SCENE_MODE mode);
    void initSkeletonJoints();
    bool findInRigKeys(int key);
    void removeRigKeys();
    void resetRigKeys();
    ~SGAutoRigSceneManager();
    void changeEnvelopeScale(Vector3 scale, bool isChanged);
    void updateEnvelopes();
    void initEnvelope(int jointId);
    bool deallocAutoRig(bool isCompleted);
    float getSelectedJointScale();
    Vector3 getSelectedNodeScale();
    void changeNodeScale(Vector3 scale);
    bool switchMirrorState();



};

#endif /* SGAutoRigSceneManager_h */
