//
//  SGEditorScene.h
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGEditorScene_h
#define SGEditorScene_h

#define TOUCH_TEXTURE_WIDTH 128
#define TOUCH_TEXTURE_HEIGHT 128
#define RENDER_TEXTURE_WIDTH 1024
#define RENDER_TEXTURE_HEIGHT 1024
#define SHADOW_TEXTURE_WIDTH 2048
#define SHADOW_TEXTURE_HEIGHT 2048
#define PREVIEW_TEXTURE_WIDTH 512
#define PREVIEW_TEXTURE_HEIGHT 512
#define THUMBNAIL_TEXTURE_WIDTH 540
#define THUMBNAIL_TEXTURE_HEIGHT 380

#define CAM_UPVEC_UPREAL_MAX_DIF 0.80
#define CAM_PREV_PERCENT 0.18
#define CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE 1.5
#define CAM_PREV_BORDER_WIDTH 0.01


#include "Constants.h"
#include "cmath"
#include "AutoRigJointsDataHelper.h"
#include "SGNode.h"
#include "ShaderManager.h"
#include "SGAction.h"
#include "BoneLimitsHelper.h"
#include "CameraViewHelper.h"
#include "../../SGEngine2/Loaders/OBJMeshFileLoader.h"
#include "SceneHelper.h"
#include "RenderHelper.h"
#include "SGActionManager.h"
#include "SGMovementManager.h"
#include "SGSelectionManager.h"
#include "SGSceneUpdater.h"
#include "SGSceneLoader.h"
#include "SGSceneWriter.h"

class SGEditorScene {
    
private:
    
    MIRROR_SWITCH_STATE mirrorSwitchState;
    bool isPreviewMode;
    SceneManager *smgr;

    void setTransparencyForObjects();
    
protected:
    
public:
    
    /* cpp class objects */
    
    bool freezeRendering , isPlaying, isRTTCompleted;
    bool isNodeSelected,isJointSelected,isControlSelected;
    int selectedJointId,selectedNodeId,selectedControlId,controlType,currentAction;
    int totalFrames, cameraResolutionType;
    int currentFrame;
    int actionObjectsSize;
    int assetIDCounter;
    
    float screenScale;
    float cameraFOV;
    float cameraRadius;

    std::map<int,int> isKeySetForFrame;
    std::map<int,int>::iterator keyFramesIterator;
    
    /* Iyan3D Related classs objects */
    
    SGNode *selectedNode;
    SGJoint *selectedJoint;
    SGNode* rotationCircle;
    vector<SGNode*> sceneControls;
    vector<SGNode*> nodes;
    vector<SGNode*> jointSpheres;
    vector<SGAction> actions;
    vector<TPoseJoint> tPoseJoints;
    ShaderManager *shaderMGR;
    RenderHelper *renHelper;
    SGSelectionManager *selectMan;
    SGSceneUpdater *updater;
    SGSceneLoader *loader;
    SGMovementManager *moveMan;
    SGActionManager *actionMan;
    
    /* SGEngine class objects */
    
    std::map<int,Texture*> renderingTextureMap;
    OBJMeshFileLoader *objLoader;
    Texture *bgTexture,*watermarkTexture,*whiteBorderTexture,*touchTexture;
    Texture *previewTexture ,*thumbnailTexture,*shadowTexture;
    Mesh *jointSphereMesh;
    Plane3D *controlsPlane;
    Vector2 nodeJointPickerPosition;
    Vector3 circleTouchPoint,cameraAngle;
    Vector3 cameraTarget;
    std::map<int,Vector3>             ikJointsPositionMap;
    std::map<int,Vector3>::iterator   ikJointsPositoinMapItr;
    shared_ptr<CameraNode> viewCamera;
    shared_ptr<CameraNode> lightCamera;
    shared_ptr<CameraNode> renderCamera;
    std::map<int,vector<Vector3> > textJointsBasePos;
    CollisionManager *cmgr;
    
    /* Constructor and Destructor */
    
    SGEditorScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight);
    ~SGEditorScene();
    
    /* initializing methods */
    
    void initTextures();
    void initVariables(SceneManager *sceneMngr, DEVICE_TYPE devType);
    void initLightCamera(Vector3 position);
    
    /* Rendering Methods */
    
    void renderAll();
    
    /* Call backs */
    #ifdef ANDROID
        bool (*downloadMissingAssetsCallBack)(std::string filePath, NODE_TYPE nodeType, JNIEnv *env, jclass type);
    #endif
    
    void (*fileWriteCallBack)();
    bool (*downloadMissingAssetCallBack)(std::string filePath, NODE_TYPE nodeType);
    void shaderCallBackForNode(int nodeID,string matName);
    bool isNodeTransparent(int nodeId);
    void setJointsUniforms(int nodeID,string matName);
    void setRotationCircleUniforms(int nodeID,string matName);
    bool isJointTransparent(int nodeID,string matName);
    void setControlsUniforms(int nodeID,string matName);
    bool isControlsTransparent(int nodeID,string matName);
    
    /* Other Methods */
    
    
    void getIKJointPosition();
    void findAndInsertInIKPositionMap(int jointId);
    void setMirrorState(MIRROR_SWITCH_STATE flag);
    MIRROR_SWITCH_STATE getMirrorState();
    void clearSelections();
    Vector4 getCameraPreviewLayout();

};

#endif /* SGEditorScene_h */
