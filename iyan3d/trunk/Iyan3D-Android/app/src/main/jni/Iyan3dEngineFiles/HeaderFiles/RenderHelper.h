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
    RenderHelper(SceneManager *smgr);
    ~RenderHelper();
    void drawGrid();
    void drawCircle(void* currentScene);
    void drawMoveAxisLine(void *scene);
    void drawMoveControlLine(Vector3 nodePos, void * scene);
    void renderControls(void *scene);
    void setControlsVisibility(void *scene, bool isVisible = true);
    void setRenderCameraOrientation(void *scene);
    void setJointSpheresVisibility(void * scene, bool visibilityFlag);
    
    void postRTTDrawCall(void *scene);
    void rttDrawCall(void *scene);
    void drawCameraPreview(void *scene);
    void rttNodeJointSelection(void * scene, Vector2 touchPosition);
    void drawJointsSpheresForRTT(void *scene, bool enableDepthTest);
    bool displayJointSpheresForNode(void *scene, shared_ptr<AnimatedMeshNode> animNode , float scaleValue = 0.7);
    bool createJointSpheres(void *scene, int additionalJoints);
    void removeJointSpheres(void *scene);
};

#endif /* RenderHelper_h */
