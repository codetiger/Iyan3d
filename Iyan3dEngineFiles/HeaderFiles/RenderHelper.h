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
    bool isFirstTimeRender;
    
    RenderHelper(SceneManager *smgr, void *scene);
    ~RenderHelper();
    bool supportsVAO();
    void drawGrid();
    void drawCircle();
    void drawMoveAxisLine();
    void drawMoveControlLine(Vector3 nodePos);
    void renderControls();
    void setControlsVisibility(bool isVisible = true);
    void setRenderCameraOrientation();
    void setJointSpheresVisibility(bool visibilityFlag);
    void renderAndSaveImage(char *imagePath, bool isDisplayPrepared, int frame, Vector4 bgColor = Vector4(0.1, 0.1, 0.1, 1.0));
    
    void movePreviewToCorner();
    void postRTTDrawCall();
    void rttDrawCall();
    void rttShadowMap();
    bool rttControlSelectionAnim(Vector2 touchPosition);
    bool isMovingCameraPreview(Vector2 currentTouchPos);
    void changeCameraPreviewCoords(Vector2 touchPos);
    void drawCameraPreview();
    void rttNodeJointSelection(Vector2 touchPosition, bool isMultiSelectenabled, bool touchMove);
    void drawJointsSpheresForRTT(bool enableDepthTest);
    void drawMeshBuffersForRTT();
    bool displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue = 0.7);
    bool createJointSpheres(int additionalJoints);
    void displayJointsBasedOnSelection();
    void removeJointSpheres();
};

#endif /* RenderHelper_h */
