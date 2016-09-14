//
//  SGSceneUpdater.h
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SGSceneUpdater_h
#define SGSceneUpdater_h

#include "RigKey.h"
#include "SGNode.h"
#include "Constants.h"

class SGSceneUpdater
{
private:
    SceneManager* smgr;
    MATERIAL_TYPE commonType , commonSkinType , vertexColorTextType , toonType , toonSkinType , vertexColorTextToonType , colorType , colorSkinType , vertexColorType;

public:
    SGSceneUpdater(SceneManager* smgr, void* scene);
    ~SGSceneUpdater();

    void setDataForFrame(int frame, bool updateBB = true);
    void setKeysForFrame(int frame);
    void updateControlsMaterial();
    void updateControlsOrientaion(bool forRTT = false);
    void changeCameraView(CAMERA_VIEW_MODE mode);
    void updateLightCamera();
    void updateLightCam(Vector3 position);
    void updateJointSpheres();
    void updateJointSpheresPosition();
    void updateLightProperties(int frameId);
    void resetMaterialTypes(bool isToonShader);
    void reloadKeyFrameMap();
    void setCameraProperty(float fov , int resolutionType);
    
    void updateEnvelopes();
    void updateOBJVertexColor();
    void updateSkeletonBone(std::map<int, RigKey>& rigKeys, int jointId);
    void updateSkeletonBones();
};

#endif /* SGSceneUpdater_h */
