//
//  SGActionManager.h
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGActionManager_h
#define SGActionManager_h

#include "Constants.h"

class SGActionManager
{
private:
    SceneManager* smgr;
public:
    
    SGActionManager(SceneManager* smgr, void* scene);
    
    bool changeObjectOrientation(Vector3 outputValue);
    void moveJoint(Vector3 outputValue);
    void rotateJoint(Vector3 outputValue);
    bool isIKJoint(int jointId);
};

#endif /* SGActionManager_h */
