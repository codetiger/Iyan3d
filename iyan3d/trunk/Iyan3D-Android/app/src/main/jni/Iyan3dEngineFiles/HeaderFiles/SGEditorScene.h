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
#ifdef ANDROID
    #define THUMBNAIL_TEXTURE_WIDTH 270
    #define THUMBNAIL_TEXTURE_HEIGHT 190
#else
    #define THUMBNAIL_TEXTURE_WIDTH 540
    #define THUMBNAIL_TEXTURE_HEIGHT 380
#endif
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
#include "SGAutoRigSceneManager.h"
#include "RenderHelper.h"
#include "SGActionManager.h"
#include "SGMovementManager.h"
#include "SGSelectionManager.h"
#include "SGSceneUpdater.h"
#include "SGSceneLoader.h"
#include "SGSceneWriter.h"
#include "SGAnimationManager.h"
#include "SGOBJManager.h"


#include "PhysicsHelper.h"


class SGEditorScene {
    
private:
    
    SceneManager *smgr;
    Vector2 previousDistance;
    void setTransparencyForObjects();
    
protected:
    
public:
    
    /* cpp class objects */
    
    bool isPreviewMode, isRigMode , shadowsOff, shouldRecalculatePhysics;
    bool freezeRendering , isPlaying;
    bool isMultipleSelection;
    bool isNodeSelected,isJointSelected,isControlSelected;
    int selectedJointId,selectedNodeId,selectedControlId,controlType;
    int riggingNodeId;
    int moveNodeId;
    int totalFrames, cameraResolutionType;
    int currentFrame, previousFrame;
    int actionObjectsSize;
    int assetIDCounter;
    
    float screenScale;
    float cameraFOV;
    float cameraRadius;
    float camPreviewScale;
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomLeft;
    Vector2 bottomRight;

    vector<int> selectedNodeIds;
    std::map<int,int> isKeySetForFrame;
    std::map<int,int>::iterator keyFramesIterator;
    
    bool isRTTCompleted;
    
    /* Iyan3D Related classs objects */
    
    SGNode *selectedNode;
    SGJoint *selectedJoint;
    SGNode* rotationCircle;
    SGNode* directionLine;
    SGNode* greenGrid;
    SGNode* blueGrid;
    SGNode* redGrid;
    SGNode* directionIndicator;
    vector<SGNode*> sceneControls;
    vector<SGNode*> nodes;
    vector<SGNode*> jointSpheres;
    vector<TPoseJoint> tPoseJoints;
    ShaderManager *shaderMGR;
    RenderHelper *renHelper;
    SGSelectionManager *selectMan;
    SGSceneUpdater *updater;
    SGSceneLoader *loader;
    SGMovementManager *moveMan;
    SGActionManager *actionMan;
    SGSceneWriter *writer;
    SGAnimationManager *animMan;
    SGAutoRigSceneManager *rigMan;
    SGOBJManager *objMan;
    PhysicsHelper * physicsHelper;

    /* SGEngine class objects */
    
    std::map<int,Texture*> renderingTextureMap;
    Texture *bgTexture,*watermarkTexture,*whiteBorderTexture,*touchTexture;
    Texture *previewTexture ,*thumbnailTexture,*shadowTexture, *alphaTexture;
    Plane3D *controlsPlane;
    Vector2 nodeJointPickerPosition;
    Vector3 circleTouchPoint,cameraAngle;
    Vector3 cameraTarget;
    Vector2 camPreviewOrigin;
    Vector2 camPreviewEnd;

    std::map<int,Vector3>             ikJointsPositionMap;
    std::map<int,Vector3>::iterator   ikJointsPositoinMapItr;
    shared_ptr<CameraNode> viewCamera;
    shared_ptr<CameraNode> lightCamera;
    shared_ptr<CameraNode> renderCamera;
    std::map<int,vector<Vector3> > textJointsBasePos;
    CollisionManager *cmgr;
    
    /* Constructor and Destructor */
    
    SGEditorScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight, int maxUniforms);
    ~SGEditorScene();
    
    void removeAllNodes();
    
    /* initializing methods */
    
    void initTextures();
    void enterOrExitAutoRigMode(bool rigMode);
    void initVariables(SceneManager *sceneMngr, DEVICE_TYPE devType, int maxUniforms);
    void initLightCamera(Vector3 position);
    
    /* Rendering Methods */
    
    void renderAll();
    
    /* Call backs */
    #ifdef ANDROID
        bool (*downloadMissingAssetsCallBack)(jobject object,std::string filePath, NODE_TYPE nodeType, bool hasTexture, JNIEnv *env, jclass type);
        unsigned char* (*getVideoFrameCallBack)(std::string fileName, int frame,int width, int height);
    #endif
    
    void (*fileWriteCallBack)();
    bool (*downloadMissingAssetCallBack)(std::string filePath, NODE_TYPE nodeType, bool hasTexture, string textureName);
    void shaderCallBackForNode(int nodeID,string matName);
    float getNodeTransparency(int nodeId);
    bool isNodeTransparent(int nodeId);
    void setJointsUniforms(int nodeID,string matName);
    void setRotationCircleUniforms(int nodeID,string matName);
    void setGridLinesUniforms(int nodeId, int rgb, string matName);
    bool isJointTransparent(int nodeID,string matName);
    void setControlsUniforms(int nodeID,string matName);
    bool isControlsTransparent(int nodeID,string matName);
    bool hasNodeSelected();
    bool hasJointSelected();
    SGNode* getSelectedNode();
    SGJoint* getSelectedJoint();
    shared_ptr<Node> getParentNode();
    
    void changeTexture(string texturePath, Vector3 vertexColor,bool isTemp, bool isUndoRedo);
    void removeTempTextureAndVertex(int selectedNode);
    bool canEditRigBones(SGNode *sgNode);
    bool isNodeInSelection(SGNode* sgNode);
    bool allObjectsScalable();
    bool allNodesRemovable();
    bool allNodesClonable();
    Vector3 getSelectedNodeScale();
    bool checkNodeSize();
    void saveThumbnail(char* imagePath);

    /* Read and Write */
    
    bool loadSceneData(std::string *filePath);
    void saveSceneData(std::string *filePath);

    /* Other Methods */
    
    int undo(int& returnValue2);
    int redo();
    
    vector<string> getUserFileNames(bool forBackup);
    void getIKJointPosition();
    void findAndInsertInIKPositionMap(int jointId);
    MIRROR_SWITCH_STATE getMirrorState();
    void clearSelections();
    Vector4 getCameraPreviewLayout();
    bool generateSGFDFile(int frame);
    void setLightingOn();
    void setLightingOff();
    void popLightProps();
    void clearLightProps();
    
    Vector3 getTransformValue();
    Vector3 getPivotPoint(bool initial);
    bool switchMirrorState();
    void setMirrorState(MIRROR_SWITCH_STATE flag);

    void updatePhysics(int frame);
    void enableDirectionIndicator();
    void updateDirectionLine();
    void updateLightMesh(int lightType, int nodeId = NOT_EXISTS);
    void setPropsOfObject(SGNode *sgNode, PHYSICS_TYPE pType);
    void syncSceneWithPhysicsWorld();

};

#endif /* SGEditorScene_h */
