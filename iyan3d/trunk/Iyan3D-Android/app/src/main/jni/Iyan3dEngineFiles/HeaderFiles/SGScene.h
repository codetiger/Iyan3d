#ifndef Iyan3D_SGScene_h
#define Iyan3D_SGScene_h

#define CAM_ZOOM_SPEED 30.0
#define CAM_PAN_SPEED 30.0
#define CAM_ZOOM_IN_LIMIT 5.0
#define CAM_SWIPE_SPEED 10.0
#define CAM_UPVEC_UPREAL_MAX_DIF 0.80
#define CAM_PREV_PERCENT 0.18
#define CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE 1.5
#define CAM_PREV_BORDER_WIDTH 0.01
#define MAXUNDO 500
#define TOTAL_CONTROLS 6
#define ROTATION_CIRCLE_LINE_MARKER_RADIUS 4.65
#define ROTATION_CIRCLE_LINE_LENGTH 0.75
#define ROTATION_CIRCLE_LONG_LINE_LENGTH 1.5

#define CONTROLS_MARKED_DISTANCE_FROM_CAMERA 6.17
#define CONTROLS_MARKED_SCALE 0.27
#define CONTROLS_MARKED_DISTANCE_FROM_NODE 1.019
#define TOUCH_TEXTURE_WIDTH 128
#define TOUCH_TEXTURE_HEIGHT 128
#define RENDER_TEXTURE_WIDTH 1024
#define RENDER_TEXTURE_HEIGHT 1024
#define SHADOW_TEXTURE_WIDTH 2048
#define SHADOW_TEXTURE_HEIGHT 2048
#define SHADOW_TEXTURE_AUTORIG_WIDTH 64
#define SHADOW_TEXTURE_AUTORIG_HEIGHT 64
#define PREVIEW_TEXTURE_WIDTH 512
#define PREVIEW_TEXTURE_HEIGHT 512
#define THUMBNAIL_TEXTURE_WIDTH 540
#define THUMBNAIL_TEXTURE_HEIGHT 380

#define SGR_JOINT_DEFAULT_COLOR_R 0.6
#define SGR_JOINT_DEFAULT_COLOR_G 0.6
#define SGR_JOINT_DEFAULT_COLOR_B 1.0

#define SELECT_COLOR_R 0.0
#define SELECT_COLOR_G 1.0
#define SELECT_COLOR_B 0.0



#include "Constants.h"
#include "cmath"
#include "AutoRigJointsDataHelper.h"
#include "SGNode.h"
#include "ShaderManager.h"
#include "SGAction.h"
#include "BoneLimitsHelper.h"
#include "CameraViewHelper.h"
#include "../../SGEngine2/Loaders/OBJMeshFileLoader.h"

class SGScene{
private:
    MIRROR_SWITCH_STATE mirrorSwitchState;
    
    
    
    Plane3D *controlsPlane;
    Vector3 circleTouchPoint,cameraAngle;
    
protected:
    Vector2 panBeganPoints[2];
    float cameraRadius,previousRadius,prevZoomDif;
    Vector2 prevTouchPoints[2];
    Texture *bgTexture,*watermarkTexture,*whiteBorderTexture;
    OBJMeshFileLoader *objLoader;
    VIEW_TYPE viewType;
    SGNode* rotationCircle;
    void getCtrlColorFromTouchTexture(Vector2 touchPosition);
    void createCircle();
    
public:
    float prevRotX , prevRotY, prevRotZ;
    bool isNodeSelected,isJointSelected,isControlSelected,shouldUpdateShadows;
    int selectedJointId,selectedNodeId,selectedControlId,controlType,currentAction, previousMaterialIndex , swipeTiming;
    float screenWidth,screenHeight,screenScale;
    Vector3	planeFacingDirection[3];
    Vector3 cameraTarget,previousTarget;
    Texture *touchTexture;
    SGNode *selectedNode;
    SGJoint *selectedJoint;
    CollisionManager *cmgr;
    Plane3D testPlane;
    shared_ptr<CameraNode> viewCamera;
    shared_ptr<CameraNode> lightCamera;
    SceneManager *smgr;
    ShaderManager *shaderMGR;
    Mesh *jointSphereMesh;
    vector<SGNode*> jointSpheres;
    vector<SGNode*> sceneControls;
    vector<TPoseJoint> tPoseJoints;
    vector<SGAction> actions;
    
    SGScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight, VIEW_TYPE fromView = ANIMATION);
    ~SGScene();
    virtual void renderAll() = 0;
    void swipeProgress(float angleX , float angleY);
    void panBegan(Vector2 touch1, Vector2 touch2);
    void panProgress(Vector2 touch1, Vector2 touch2);
    bool createJointSpheres(int additionalJoints);
    void initViewCamera();
    void initLightCamera(Vector3 position);
    void updateLightCam(Vector3 position);
    bool displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue = 0.7);
    void removeJointSpheres();
    void setJointSpheresVisibility(bool visibilityFlag);
    void highlightJointSpheres();
    void displayJointsBasedOnSelection();
    void setJointsUniforms(int nodeID,string matName);
    void setControlsUniforms(int nodeID,string matName);
    void setRotationCircleUniforms(int nodeID,string matName);
    void drawJointsSpheresForRTT(SGNode* sgNode,bool enableDepthTest);
    void initControls();
    void setControlsVariations();
    void renderControls();
    void updateControlsOrientaion(bool forRTT = false);
    void setControlsVisibility(bool visible = true);
    void updateControlsMaterial();
    void removeActions();
    void setMirrorState(MIRROR_SWITCH_STATE flag);
    void drawGrid();
    void drawMoveControlLine(Vector3 nodePos);
    MIRROR_SWITCH_STATE getMirrorState();
    void addAction(SGAction& action);
    void clearSelections();
    void changeCameraView(CAMERA_VIEW_MODE mode);
    void drawCircle();
    void limitPixelCoordsWithinTextureRange(float texWidth,float texHeight,float &x,float &y);
    
    bool isIKJoint(int jointId);
    bool isJointTransparent(int nodeID,string matName);
    bool isControlsTransparent(int nodeID,string matName);
    bool calculateControlMovements(Vector2 curPoint,Vector2 prevTouchPoint,Vector3 &outputValue,bool isSGJoint = true);
};
#endif