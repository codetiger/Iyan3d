//
//  SGPreviewScene.h
//  Iyan3D
//
//  Created by karthik on 07/07/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef Iyan3D_SGPreviewScene_h
#define Iyan3D_SGPreviewScene_h

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "../../SGEngine2/Utilities/Logger.h"
#include "Constants.h"
#include "ShaderManager.h"
#include "SGScene.h"
#include "SGNode.h"


#include <map>
#include "../../SGEngine2/Font/Font2OBJ.h"

using namespace std;

class SGPreviewScene : public SGScene
{
private:
    vector<Vector3> jointInitPositions;
    vector<Vector3> jointInitScale;
public:
    int currentFrame , totalFrames, assetIdCount;
    float scaleRatio , cameraZ;
    Vector2 cameraAngle;
    Vector3 runningRotation;
    bool isSwipe;
    std::map<int,vector<Vector3> > textJointsBasePos;
    vector<SGNode*> nodes;
    
    SGPreviewScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight , VIEW_TYPE viewType);
    ~SGPreviewScene();
    virtual void renderAll();
    void shaderCallBackForNode(int nodeID,string matName);
    bool isNodeTransparent(int nodeId);
    SGNode* loadNode(NODE_TYPE type,int assetId, wstring name, int width  = 0, int height = 0, Vector4 textColor = Vector4(0), string FilePath = "");
    void swipeToRotate(float angleX , float angleY);
    void pinchBegan(Vector2 touch1, Vector2 touch2);
    void pinchZoom(Vector2 touch1, Vector2 touch2);
    void swipeEnd();
    void removeObject(u16 nodeIndex,bool deAllocScene);
    void setUpInitialKeys(SGNode *sgnode);
    void updateLightProperties();
    void loadNode(int assetId);
    bool applyAnimations(int assetId);
    bool removeAnimationAtFrame(int selectedFrame);
    void playAnimation();
    void stopAnimation();
    void setDataForFrame(int frame);
};

#endif
