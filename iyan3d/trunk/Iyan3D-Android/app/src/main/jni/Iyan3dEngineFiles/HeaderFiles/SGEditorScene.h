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

#include "Constants.h"
#include "cmath"
#include "AutoRigJointsDataHelper.h"
#include "SGNode.h"
#include "ShaderManager.h"
#include "SGAction.h"
#include "BoneLimitsHelper.h"
#include "CameraViewHelper.h"
#include "../../SGEngine2/Loaders/OBJMeshFileLoader.h"
#include "SceneInitializeHelper.h"

class SGEditorScene {
    
private:
    float screenScale;
    float cameraRadius;
    
    MIRROR_SWITCH_STATE mirrorSwitchState;
    
    vector<SGNode*> jointSpheres;
    vector<TPoseJoint> tPoseJoints;
    vector<SGAction> actions;

    Vector3 cameraTarget;
    shared_ptr<CameraNode> viewCamera;
    shared_ptr<CameraNode> renderCamera;
    SceneManager *smgr;
    ShaderManager *shaderMGR;
    CollisionManager *cmgr;
    
protected:
    
public:
    
    /* cpp class objects */
    
    bool isNodeSelected,isJointSelected,isControlSelected;
    int selectedJointId,selectedNodeId,selectedControlId,controlType,currentAction;
    int totalFrames;
    
    float cameraFOV;
    /* Iyan3D Related classs objects */
    
    SGNode *selectedNode;
    SGJoint *selectedJoint;
    SGNode* rotationCircle;
    vector<SGNode*> sceneControls;
    vector<SGNode*> nodes;
    
    /* SGEngine class objects */
    
    std::map<int,Texture*> renderingTextureMap;
    OBJMeshFileLoader *objLoader;
    Texture *bgTexture,*watermarkTexture,*whiteBorderTexture,*touchTexture;
    Texture *previewTexture ,*thumbnailTexture,*shadowTexture;
    Mesh *jointSphereMesh;
    Plane3D *controlsPlane;
    
    /* Constructor and Destructor */
    
    SGEditorScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight);
    ~SGEditorScene();
    
    /* initializing methods */
    
    void initTextures();
    void initVariables(SceneManager *sceneMngr, DEVICE_TYPE devType);
    
    /* Rendering Methods */
    
    void renderAll();

};

#endif /* SGEditorScene_h */
