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

class RenderHelper
{
private:
    SceneManager *smgr;
    
public:
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
    
    void postRTTDrawCall();
    void rttDrawCall();
    void rttShadowMap();
    void rttControlSelectionAnim(Vector2 touchPosition);
    void drawCameraPreview();
    void rttNodeJointSelection(Vector2 touchPosition);
    void drawJointsSpheresForRTT(bool enableDepthTest);
    bool displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue = 0.7);
    bool createJointSpheres(int additionalJoints);
    void displayJointsBasedOnSelection();
    void removeJointSpheres();
};

#endif /* RenderHelper_h */
