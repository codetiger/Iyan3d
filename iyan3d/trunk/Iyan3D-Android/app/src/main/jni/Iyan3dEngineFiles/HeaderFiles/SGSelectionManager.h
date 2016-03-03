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
    shared_ptr<MeshNode> parentNode;
    vector<Vector3> globalPositions;
    vector<Vector3> relPositions;
    Mesh* sphereMesh;
    
public:    
    SGSelectionManager(SceneManager* smgr, void* scene);
    ~SGSelectionManager();
        
    void checkSelection(Vector2 touchPosition, bool isMultiSelectEnabled, bool isDisplayPrepared = true);
    bool checkCtrlSelection(Vector2 curTouchPos, bool isMultiSelectEnabled, bool isDisplayPrepared = false);
    void postNodeJointSelection();
    bool getCtrlColorFromTouchTextureAnim(Vector2 touchPosition);
    bool getNodeColorFromTouchTexture(bool isMultiSelected ,bool touchMove);
    bool selectNodeOrJointInPixel(Vector2 touchPixel,bool isMultiSelectEnabled, bool touchMove);
    bool updateNodeSelectionFromColor(Vector3 pixel, bool isMultipleSelectionEnabled, bool touchMove);
    bool multipleSelections(int nodeId);
    void updateParentPosition();
    void unselectObjects();
    shared_ptr<Node> getParentNode();
    void storeGlobalPositions();
    vector<Vector3> getGlobalScaleValues();
    void storeRelativePositions();
    void restoreRelativeScales(vector< Vector3 > scales);
    void addSelectedChildren(shared_ptr<Node> toParent);
    void removeChildren(shared_ptr<Node> fromParent, bool resetKeys = false);
    void highlightSelectedNode(int nodeId = -1);
    void highlightJointSpheres();
    void selectObject(int objectId ,bool isMultiSelectionEnabled);
    void unselectObject(int objectId);
    
    void checkSelectionForAutoRig(Vector2 touchPosition);
    void readSkeletonSelectionTexture();
    bool selectSkeletonJointInPixel(Vector2 touchPixel);
    void updateSkeletonSelectionColors(int prevSelectedNodeId);
    void readSGRSelectionTexture();
    void updateSGRSelection(int selectedNodeColor,int selectedJointColor, shared_ptr<AnimatedMeshNode> animNode);
};

#endif /* SGGestureManager_h */
