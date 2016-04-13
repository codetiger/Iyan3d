//
//  SceneInitializeHelper.h
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SceneHelper_h
#define SceneHelper_h

#define TOTAL_CONTROLS 9

#include "SGNode.h"
#include "CameraViewHelper.h"


class SceneHelper
{
    public:
    
    static float screenWidth , screenHeight;
    
    /* Creation Related Methods */
    
    static shared_ptr<CameraNode> initViewCamera(SceneManager *smgr, Vector3& cameraTarget, float& cameraRadius);
    static shared_ptr<CameraNode> initRenderCamera(SceneManager *smgr, float cameraFOV);
    static SGNode* createCircle(SceneManager *smgr);
    static vector<SGNode*> initControls(SceneManager *smgr);
    static SGNode* initIndicatorNode(SceneManager *smgr);

    static Vector3 planeFacingDirection(int controlType);
    static Vector3 controlDirection(int controlType);
    static void limitPixelCoordsWithinTextureRange(float texWidth,float texHeight,float &xCoord,float &yCoord);
    
};

#endif /* SceneInitializeHelper_h */
