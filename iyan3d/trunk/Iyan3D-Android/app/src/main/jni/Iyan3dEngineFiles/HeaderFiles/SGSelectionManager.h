//
//  SGGestureManager.h
//  Iyan3D
//
//  Created by Karthik on 25/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGSelectionManager_h
#define SGSelectionManager_h

#include "Constants.h"

class SGSelectionManager
{
private:
    SceneManager *smgr;
public:
    SGSelectionManager(SceneManager* smgr, void* scene);
    ~SGSelectionManager();
        
    void checkSelection(Vector2 touchPosition,bool isDisplayPrepared = true);
    void checkCtrlSelection(Vector2 curTouchPos,bool isDisplayPrepared = false);
    void postNodeJointSelection();
    void getCtrlColorFromTouchTextureAnim(Vector2 touchPosition);
    void getNodeColorFromTouchTexture();
    bool selectNodeOrJointInPixel(Vector2 touchPixel);
    bool updateNodeSelectionFromColor(Vector3 pixel);
    void highlightSelectedNode();
    void highlightJointSpheres();
    void selectObject(int objectId);
    void unselectObject(int objectId);
    
    void checkSelectionForAutoRig(Vector2 touchPosition);
    void readSkeletonSelectionTexture();
    bool selectSkeletonJointInPixel(Vector2 touchPixel);
    void updateSkeletonSelectionColors(int prevSelectedNodeId);
    void readSGRSelectionTexture();
    void updateSGRSelection(int selectedNodeColor,int selectedJointColor, shared_ptr<AnimatedMeshNode> animNode);
};

#endif /* SGGestureManager_h */
