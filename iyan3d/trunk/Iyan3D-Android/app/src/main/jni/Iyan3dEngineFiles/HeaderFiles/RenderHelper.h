//
//  RenderHelper.h
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef RenderHelper_h
#define RenderHelper_h

#include "Constants.h"
#include "RigKey.h"

class RenderHelper
{
private:
    SceneManager *smgr;
    
public:
    int renderingType;
    bool isExportingImages,isExporting1stTime;
    bool isMovingPreview;
    Vector2 cameraPreviewMoveDist;
    
    RenderHelper(SceneManager *smgr, void *scene);
    ~RenderHelper();
    void drawGrid();
    void drawCircle();
    void drawMoveAxisLine();
    void drawMoveControlLine(Vector3 nodePos);
    void renderControls();
    void setControlsVisibility(bool isVisible = true);
    void setRenderCameraOrientation();
    void setJointSpheresVisibility(bool visibilityFlag);
    void setJointAndBonesVisibility(std::map<int, RigKey>& rigKeys, bool isVisible);
    void setEnvelopVisibility(std::map<int, SGNode*>& envelopes, bool isVisible);
    void drawEnvelopes(std::map<int, SGNode*>& envelopes, int jointId);
    void renderAndSaveImage(char *imagePath , int shaderType,bool isDisplayPrepared, bool removeWaterMark);
    
    void movePreviewToCorner();
    void postRTTDrawCall();
    void rttDrawCall();
    void rttShadowMap();
    void rttControlSelectionAnim(Vector2 touchPosition);
    bool isMovingCameraPreview(Vector2 currentTouchPos);
    void changeCameraPreviewCoords(Vector2 touchPos);
    void drawCameraPreview();
    void rttNodeJointSelection(Vector2 touchPosition);
    void drawJointsSpheresForRTT(bool enableDepthTest);
    bool displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue = 0.7);
    bool createJointSpheres(int additionalJoints);
    void displayJointsBasedOnSelection();
    void removeJointSpheres();
    
    void AttachSkeletonModeRTTSelection(Vector2 touchPosition);
    void rttSGRNodeJointSelection(Vector2 touchPosition);
};

#endif /* RenderHelper_h */
