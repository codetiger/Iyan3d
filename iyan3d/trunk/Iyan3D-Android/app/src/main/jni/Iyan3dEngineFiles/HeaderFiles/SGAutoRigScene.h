#ifndef __Iyan3D__SGAutoRigScene__
#define __Iyan3D__SGAutoRigScene__

#define OBJ_BOUNDINGBOX_MAX_LIMIT 24.0
#define RIG_BONE_SCALE_TEMP_FIX 0.85
#define RIG_MAX_BONES 57


#include "ShaderManager.h"
#include "SGNode.h"
#include "MathHelper.h"
#include "Constants.h"
#include "FileHelper.h"
#include "AutoRigJointsDataHelper.h"
#include "RigKey.h"
#include <iostream>
#include "SGScene.h"
#include <map>
#include <vector>
#include <string.h>
#include "Constants.h"
#include "ShaderManager.h"
#include "AutoRigHelper.h"
#include "exportSGR.h"

enum SKELETON_TYPE{
    SKELETON_HUMAN,
    SKELETON_OWN
};
using namespace std;

class SGAutoRigScene : public SGScene
{
private:
    Texture* shadowTexture;
    Vector2 touchPosForSkeletonSelection,touchPosForSGRSelection,touchPosForCtrlPos;
    vector<Vector3> previousPositions;
    vector<Vector3> previousRotations;

    void resetRigKeys();
    void readSkeletonSelectionTexture();
    void readSGRSelectionTexture();
    void setRigBonesScale(int jointId,int parentId);
    bool findInRigKeys(int key);
    void rttControlSelectionAutoRig(Vector2 touchPosition);
    bool selectSkeletonJointInPixel(Vector2 touchPixel);

protected:
    bool isMoved;
    string animatedSGRPath;
    
public:
    bool isOBJimported,isSkeletonJointSelected,isSkeletonSelected,isRTTCompleted;
    int sceneMode,rigBoneCount;
    SKELETON_TYPE skeletonType;
    
    SGNode *objSGNode,*sgrSGNode;
    Mesh *boneMesh,*sphereMesh;
    
    SGAutoRigScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight);
    ~SGAutoRigScene();
    void setLighting(bool status);
    void updateLightProperties();
    void (*boneLimitsCallBack)();
    void (*objLoaderCallBack)(int);
    void (*notRespondingCallBack)();
    void renderAll();
    void addObjToScene(string objPath,string texturePath);
    void objNodeCallBack(string materialName);
    void boneNodeCallBack(int id,string materialName);
    void jointNodeCallBack(int id,string materialName);
    void touchBeganRig(Vector2 curTouchPos);
    void touchEndRig(Vector2 curTouchPos);
    void touchMoveRig(Vector2 curTouchPos,Vector2 prevTouchPos,float width,float height);
    void addAction(SGAction& action);
    void clearNodeSelections();
    bool changeSkeletonPosition(Vector3 outputValue);
    bool changeSkeletonRotation(Vector3 outputValue);
    void initSkeletonJoints();
    void addNewJoint();
    void removeJoint();
    void switchSceneMode(AUTORIG_SCENE_MODE mode);
    bool switchMirrorState();
    void changeEnvelopeScale(Vector3 scale, bool isChanged);
    void setEnvelopeScale(Vector3 scale);
    void updateEnvelopes();
    bool setSceneMode(AUTORIG_SCENE_MODE mode);
    bool isOBJTransparent(string materialName);
    void setJointAndBonesVisibility(bool isVisible);
    void AttachSkeletonModeRTTSelection(Vector2 touchPosition);
    void checkSelection(Vector2 touchPosition);
    void rttSGRNodeJointSelection(Vector2 touchPosition);
    void updateSGRSelection(int selectedColor,int selectedJointColor,shared_ptr<AnimatedMeshNode> animNode);
    void setSkeletonKeys(vector<ActionKey> &keys);
    void setSGRKeys(vector<ActionKey> &keys);
    void undo();
    void redo();
    void updateSelectionOfAttachSkeletonMode();
    void updateSkeletonBone(int jointId);
    void updateSkeletonBones();
    void initEnvelope(int jointId);
    void updateEnvelope(int jointId, shared_ptr<AnimatedMeshNode> envelopeNode);
    void updateSkeletonSelectionColors(int prevSelectedNodeId);
    void setEnvelopeUniforms(int jointId,string matName);
    void drawEnvelopes(int jointId);
    void updateOBJVertexColor();
    void setEnvelopVisibility(bool isVisible);
    void exportSGR(std::string filePath);
    void setSGRUniforms(int jointId,string matName);
    void changeNodeScale(Vector3 scale, bool changeAction);
    void removeRigKeys();
    void resetSGROriginalKeys();
    bool isSGRTransparent(int jointId,string matName);
    bool changeSGRPosition(Vector3 outputValue);
    bool changeSGRRotation(Vector3 outputValue);
    float getSelectedJointScale();
    Vector3 getSelectedNodeScale();
    vector<ActionKey> getNodeKeys();
    bool isReachMaxBoneCount();
};
#endif