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
    SGSelectionManager(SceneManager* smgr);
    ~SGSelectionManager();
        
    void checkSelection(void *scene, Vector2 touchPosition,bool isDisplayPrepared);
    void postNodeJointSelection(void *scene);
    void getNodeColorFromTouchTexture(void *scene);
    bool selectNodeOrJointInPixel(void *scene, Vector2 touchPixel);
    bool updateNodeSelectionFromColor(void *scene, Vector3 pixel);
    void highlightSelectedNode(void *scene);
    void highlightJointSpheres(void *scene);
    void selectObject(void *scene, int objectId);
    void unselectObject(void *scene, int objectId);
};

#endif /* SGGestureManager_h */
