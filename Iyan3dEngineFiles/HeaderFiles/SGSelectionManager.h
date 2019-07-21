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
    bool getNodeColorFromTouchTexture(bool isMultiSelected ,bool touchMove, bool drawMeshBufferRTT = false);
    bool selectNodeOrJointInPixel(Vector2 touchPixel,bool isMultiSelectEnabled, bool touchMove, bool drawMeshBufferRTT = false);
    bool updateNodeSelectionFromColor(Vector3 pixel, bool isMultipleSelectionEnabled, bool touchMove, bool drawMeshBufferRTT = false);
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
    void unHightlightSelectedNode();
    void highlightMeshBufferAndJointSpheres();
    void selectObject(int objectId ,int selectedMeshBufferId, bool isMultiSelectionEnabled);
    void unselectObject(int objectId);
    
    void checkSelectionForAutoRig(Vector2 touchPosition);
};

#endif /* SGGestureManager_h */
