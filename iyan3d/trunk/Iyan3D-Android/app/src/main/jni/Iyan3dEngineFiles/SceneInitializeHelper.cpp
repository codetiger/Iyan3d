//
//  SceneInitializeHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//


#include "SceneInitializeHelper.h"
#include "ShaderParamOrder.h"

float SceneHelper::screenWidth = 0.0;
float SceneHelper::screenHeight = 0.0;

shared_ptr<CameraNode> SceneHelper::initViewCamera(SceneManager *smgr, Vector3& cameraTarget, float& cameraRadius)
{
    CameraViewHelper::readData();

    shared_ptr<CameraNode> viewCamera = smgr->createCameraNode("NoUniformCallbackFunctions");
    smgr->setActiveCamera(viewCamera);
    viewCamera->setPosition(Vector3(VIEW_CAM_INIT_POS_X,VIEW_CAM_INIT_POS_Y,VIEW_CAM_INIT_POS_Z));
    viewCamera->updateAbsoluteTransformation();
    //TODO: viewCamera->setID(3);
    cameraTarget = Vector3(0.0);
    viewCamera->setTarget(Vector3(0.0,10.0,0.0));//cameraTarget);
    viewCamera->setFOVInRadians(65.0 * (PI / 180));
    float aspectRatio = screenWidth/screenHeight;
    viewCamera->setAspectRatio(aspectRatio);
    cameraRadius = 20.0;
    //testPlane.setPositionAndNormal(Vector3(0.0,0.0,100.0),Vector3(0.0,1.0,0.0));
    return viewCamera;

}

shared_ptr<CameraNode> SceneHelper::initRenderCamera(SceneManager *smgr, float fov)
{
    shared_ptr<CameraNode>renderCamera = smgr->createCameraNode("RenderingUniforms");
    renderCamera->setTargetAndRotationBindStatus(true);
    renderCamera->setFOVInRadians(fov * PI / 180.0f);
    return renderCamera;
}

Vector3 SceneHelper::planeFacingDirection(int controlType)
{
    if(controlType == X_MOVE || controlType == X_ROTATE)
        return Vector3(0,1,0);
    else if(controlType == Y_MOVE || controlType == Y_ROTATE)
        return Vector3(0,0,1);
    else
        return Vector3(0,1,0);
}

Vector3 SceneHelper::controlDirection(int controlType)
{
    if(controlType == X_MOVE || controlType == X_ROTATE)
        return Vector3(1,0,0);
    else if(controlType == Y_MOVE || controlType == Y_ROTATE)
        return Vector3(0,1,0);
    else
        return Vector3(0,0,1);
}

SGNode* SceneHelper::createCircle(SceneManager *smgr){
    SGNode* rotationCircle = new SGNode(NODE_SGM);
    rotationCircle->node = smgr->createCircleNode(100,1.0,"RotationCircle");
    rotationCircle->node->setID(CIRCLE_NODE_ID);
    rotationCircle->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    rotationCircle->props.vertexColor = Vector3(0.0,1.0,0.0);
    rotationCircle->props.transparency = 1.0;
    return rotationCircle;
}

vector<SGNode*> SceneHelper::initControls(SceneManager *smgr)
{
    vector<SGNode*> sceneControls;
    for(int i = 0;i < TOTAL_CONTROLS;i++){
        SGNode *sgNode = new SGNode(NODE_SGM);
        Mesh *ctrlMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/controls" + to_string(i+1) + ".sgm",smgr->device);
        if(ctrlMesh == NULL){
            Logger::log(ERROR,"SGScene", "SGRSpheres Mesh Not Loaded");
            return;
        }
        shared_ptr<MeshNode> ctrlNode = smgr->createNodeFromMesh(ctrlMesh,"setCtrlUniforms");
        if(!ctrlNode){
            Logger::log(ERROR,"SGScene", "Unable to create sgrSpheres");
            return;
        }
        sgNode->node = ctrlNode;
        sgNode->props.isLighting = false;
        sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        
        Texture *nodeTex = smgr->loadTexture("Dummy Texture",constants::BundlePath + "/dummy.png",TEXTURE_RGBA8,TEXTURE_BYTE);
        sgNode->node->setTexture(nodeTex,1);
        sgNode->node->setTexture(nodeTex,2);
        
        sceneControls.push_back(sgNode);
        sgNode->node->setID(CONTROLS_START_ID + (int)sceneControls.size() - 1);
        if(i == X_MOVE || i == X_ROTATE) // flipped control meshes
            sgNode->node->setRotationInDegrees(Vector3(0.0,180.0,0.0));
    }
    
    return sceneControls;
}