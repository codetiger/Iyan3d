//
//  SGActionManager.h
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGActionManager_h
#define SGActionManager_h

#define MAXUNDO 500

#include "SGAction.h"
#include "Constants.h"

class SGActionManager
{
private:
    SceneManager* smgr;
    SGAction changeKeysAction;
    SGAction propertyAction;
    SGAction assetAction;
    MIRROR_SWITCH_STATE mirrorSwitchState;
    
    void moveJoint(Vector3 outputValue);
    void rotateJoint(Vector3 outputValue);
    void StoreDeleteObjectKeys(int nodeIndex);
    int getObjectIndex(int actionIndex);

public:
    
    int currentAction;
    SGAction addJointAction;
    
    vector<SGAction> actions;

    SGActionManager(SceneManager* smgr, void* scene);
    ~SGActionManager();
    
    void addAction(SGAction& action);
    void finalizeAndAddAction(SGAction& action);
    void removeActions();
    
    /* Move/Rotate node/joint actions */
    
    bool changeObjectOrientation(Vector3 outputValue);
    bool isIKJoint(int jointId);
    void storeActionKeys(bool finished);
    
    /* Frames change related actions */
    
    void switchFrame(int frame);
    
    /* Property Change actions */
    
    void changeMeshProperty(float brightness, float specular, bool isLighting, bool isVisible, bool isChanged = false);
    void changeCameraProperty(float fov , int resolutionType, bool isChanged = false);
    void changeLightProperty(float red , float green, float blue, float shadow, bool isChanged = false);
    void storeLightPropertyChangeAction(float red , float green , float blue , float shadowDensity);

    /* Mirror Change action */
    
    void setMirrorState(MIRROR_SWITCH_STATE flag);
    bool switchMirrorState();
    MIRROR_SWITCH_STATE getMirrorState();
    /* Node add or remove actions */
    
    void storeAddOrRemoveAssetAction(int actionType, int assetId, string optionalFilePath);
    
    /* Undo and Redo*/
    
    int undo(int& returnValue2);
    int redo();
    
    /* AutoRig */
    
    bool changeSkeletonPosition(Vector3 outputValue);
    bool changeSkeletonRotation(Vector3 outputValue);
    bool changeSGRPosition(Vector3 outputValue);
    bool changeSGRRotation(Vector3 outputValue);
};

#endif /* SGActionManager_h */
