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
    void postNodeJointSelection();
    void getNodeColorFromTouchTexture();
    bool selectNodeOrJointInPixel(Vector2 touchPixel);
    bool updateNodeSelectionFromColor(Vector3 pixel);
    void highlightSelectedNode();
    void highlightJointSpheres();
    void selectObject(int objectId);
    void unselectObject(int objectId);
};

#endif /* SGGestureManager_h */
