//
//  RenderHelper.cpp
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#define ROTATION_CIRCLE_LINE_MARKER_RADIUS 4.65
#define ROTATION_CIRCLE_LINE_LENGTH 0.75
#define ROTATION_CIRCLE_LONG_LINE_LENGTH 1.5
#define CONTROLS_MARKED_DISTANCE_FROM_CAMERA 6.17
#define CONTROLS_MARKED_SCALE 0.27
#define CONTROLS_MARKED_DISTANCE_FROM_NODE 1.019

#define SGR_JOINT_DEFAULT_COLOR_R 0.6
#define SGR_JOINT_DEFAULT_COLOR_G 0.6
#define SGR_JOINT_DEFAULT_COLOR_B 1.0

#define SELECT_COLOR_R 0.0
#define SELECT_COLOR_G 1.0
#define SELECT_COLOR_B 0.0


#include "RenderHelper.h"
#include "SGEditorScene.h"


static Vector3 redColor = Vector3(1.0,0.2,0.2);
static Vector3 greenColor = Vector3(0.2,1.0,0.2);
static Vector3 blueColor = Vector3(0.2,0.2,1.0);
Vector3 constants::sgrJointDefaultColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
Vector3 constants::selectionColor = Vector3(SELECT_COLOR_R,SELECT_COLOR_G,SELECT_COLOR_B);

RenderHelper::RenderHelper(SceneManager* sceneMngr)
{
    this->smgr = sceneMngr;
}

void RenderHelper::drawGrid()
{
    if(!smgr)
        return;
    
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    for (int i = -20; i <= 20; i+= 5) {
        smgr->draw3DLine(Vector3(i, 0, -20), Vector3(i, 0, 20), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
        smgr->draw3DLine(Vector3(-20, 0, i), Vector3(20, 0, i), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    smgr->draw3DLine(Vector3(-20, 0, 0), Vector3(20, 0, 0), Vector3(1.0,0.2,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    smgr->draw3DLine(Vector3(0, 0, -20), Vector3(0, 0, 20), Vector3(0.2,1.0,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
}

void RenderHelper::drawCircle(void* scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    
    if(!currentScene || !smgr)
        return;
    
    if(currentScene->controlType == ROTATE && currentScene->selectedControlId != NOT_SELECTED && currentScene->selectedNode) {
        Vector3 circleRotation = Vector3(90.0,0.0,0.0);
        Vector3 circleAxis = Vector3(1.0,0.0,1.0);
        if(currentScene->selectedControlId == Y_ROTATE){
            circleRotation = Vector3(0.0,0.0,0.0);
            circleAxis = Vector3(1.0,1.0,0.0);
        }else if(currentScene->selectedControlId == Z_ROTATE){
            circleRotation = Vector3(0.0,90.0,0.0);
            circleAxis = Vector3(0.0,1.0,1.0);
        }
        Vector3 nodePos = Vector3(0.0);
        if(currentScene->selectedJoint)
            nodePos = currentScene->selectedJoint->jointNode->getAbsolutePosition();
        else if(currentScene->selectedNode)
            nodePos = currentScene->selectedNode->node->getAbsolutePosition();
        else
            return;
        
        float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
        float radius = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
        float longLineLength = ROTATION_CIRCLE_LONG_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS) ,shorLineLength = ROTATION_CIRCLE_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS);
        currentScene->rotationCircle->node->setPosition(nodePos);
        currentScene->rotationCircle->node->setScale(Vector3(radius));
        currentScene->rotationCircle->node->setRotationInDegrees(circleRotation);
        currentScene->rotationCircle->node->setVisible(true);
        smgr->RenderNode(smgr->getNodeIndexByID(currentScene->rotationCircle->node->getID()),true);
        
        Vector3 lineVect = nodePos - currentScene->circleTouchPoint;
        lineVect.normalize();
        lineVect = lineVect * Vector3(radius);
        Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
        smgr->draw3DLine(nodePos,nodePos - lineVect,Vector3(0.0,1.0,0.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
        
        double theta = 0.0;
        Vector3 rotVect = Vector3(0.0);
        // draw lines inside Circle.
        theta = 0.0;
        while(theta <= 360.0){
            rotVect.x = (1.0 - circleAxis.x) * theta - (((circleAxis.x + circleAxis.z) == 2.0) ? 0.0 : 90.0);
            rotVect.y = (1.0 - circleAxis.y) * theta;
            rotVect.z = (1.0 - circleAxis.z) * theta;
            Vector3 dir = rotVect.rotationToDirection();
            dir.normalize();
            // long lines for every 45 deg gap.
            float lineLenght = (int(theta) % 45 == 0) ? longLineLength:shorLineLength;
            Vector3 vertexPos1 = nodePos + (dir * Vector3((circleAxis.x * radius),(circleAxis.y * radius),(circleAxis.z * radius)));
            Vector3 vertexPos2 = nodePos + (dir * Vector3((circleAxis.x * radius - lineLenght),(circleAxis.y * radius - lineLenght),(circleAxis.z * radius - lineLenght)));
            smgr->draw3DLine(vertexPos1,vertexPos2,Vector3(0.0,1.0,0.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
            theta += 5.0;
        }
        
    }
}

void RenderHelper::drawMoveAxisLine(void *scene)
{
    SGEditorScene * currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    
    if(currentScene->controlType == MOVE && currentScene->selectedControlId != NOT_SELECTED && currentScene->selectedNodeId != NOT_SELECTED){
        Vector3 nodePos = currentScene->nodes[currentScene->selectedNodeId]->node->getAbsolutePosition();
        if(currentScene->selectedJointId != NOT_SELECTED)
            nodePos = (dynamic_pointer_cast<AnimatedMeshNode>(currentScene->nodes[currentScene->selectedNodeId]->node))->getJointNode(currentScene->selectedJointId)->getAbsolutePosition();
        drawMoveControlLine(nodePos, currentScene);
    }
}

void RenderHelper::drawMoveControlLine(Vector3 nodePos, void * scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    float farValue = currentScene->viewCamera->getFarValue();
    if(currentScene->selectedControlId == X_MOVE){
        smgr->draw3DLine(Vector3(-farValue,nodePos.y,nodePos.z),Vector3(farValue,nodePos.y,nodePos.z),redColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else if(currentScene->selectedControlId == Y_MOVE){
        smgr->draw3DLine(Vector3(nodePos.x,-farValue,nodePos.z),Vector3(nodePos.x,farValue,nodePos.z),blueColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else{
        smgr->draw3DLine(Vector3(nodePos.x,nodePos.y,-farValue), Vector3(nodePos.x,nodePos.y,farValue),greenColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
}

void RenderHelper::renderControls(void *scene)
{
    SGEditorScene* currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr || !currentScene->isNodeSelected)
        return;
    
    RenderHelper::setControlsVisibility(currentScene);
    smgr->clearDepthBuffer();
    int controlStartIndex = (currentScene->controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (currentScene->controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        
        Vector3 ctrlToCam = (currentScene->viewCamera->getPosition() - currentScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        float ctrlTransparency = (angle > 0.9) ? (0.95 - angle) * 20.0 : 1.0;
        ctrlTransparency = (ctrlTransparency < 0.0) ? 0.0 : ctrlTransparency;
        currentScene->sceneControls[i]->props.transparency = ctrlTransparency;
        
        if(currentScene->selectedControlId == NOT_SELECTED){
            currentScene->sceneControls[i]->props.isVisible = true;
            currentScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
        }else if(i == currentScene->selectedControlId) {
            currentScene->sceneControls[i]->props.isVisible = true;
            currentScene->sceneControls[i]->props.vertexColor = Vector3(SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B);
            currentScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        }else {
            currentScene->sceneControls[i]->props.isVisible = false;
        }
        
        int nodeIndex = smgr->getNodeIndexByID(currentScene->sceneControls[i]->node->getID());
        
        if(currentScene->sceneControls[i]->props.isVisible)
            smgr->RenderNode(nodeIndex,(i == controlStartIndex)?true:false);
    }
}

void RenderHelper::setControlsVisibility(void *scene, bool isVisible)
{
    SGEditorScene* currentScene = (SGEditorScene*)scene;
    if(!currentScene || currentScene->sceneControls.size() == 0)
        return;
    
    if(currentScene->isNodeSelected && currentScene->selectedNode->getType() == NODE_LIGHT)
        currentScene->controlType = MOVE;
    
    int controlStartToVisible = NOT_EXISTS,controlEndToVisible = NOT_EXISTS;
    if(currentScene->isNodeSelected && isVisible){
        if(currentScene->controlType == MOVE){
            controlStartToVisible = X_MOVE;
            controlEndToVisible = Z_MOVE;
        }else if(currentScene->controlType == ROTATE){
            controlStartToVisible = X_ROTATE;
            controlEndToVisible = Z_ROTATE;
        } else {
            for (int i = 0; i < currentScene->sceneControls.size(); i++) {
                currentScene->sceneControls[i]->node->setVisible(false);
            }
            return;
        }
    }
    for(int i = X_MOVE;i <= Z_ROTATE;i++){
        isVisible = (i >= controlStartToVisible && i <= controlEndToVisible) ? true:false;
        currentScene->sceneControls[i]->node->setVisible(isVisible);
    }
}

void RenderHelper::postRTTDrawCall(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    
    if(currentScene->previewTexture && (currentScene->selectedNodeId == NODE_CAMERA || currentScene->isPlaying)) {
        Vector4 previewLayout = currentScene->getCameraPreviewLayout();
        smgr->draw2DImage(currentScene->previewTexture,Vector2(previewLayout.x,previewLayout.y),Vector2(previewLayout.z,previewLayout.w), false, smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE),true);
    }
}

void RenderHelper::rttDrawCall(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene)
        return;

    if(currentScene->selectedNodeId == NODE_CAMERA || currentScene->isPlaying)
        drawCameraPreview(currentScene);
    //rttShadowMap();
}

void RenderHelper::drawCameraPreview(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    
    setRenderCameraOrientation(currentScene);
    currentScene->rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(currentScene->previewTexture,true,true,false,Vector4(255,255,255,255));
    Vector4 camprevlay = currentScene->getCameraPreviewLayout();
    for(unsigned long i = 1; i < currentScene->nodes.size(); i++){
        if(currentScene->nodes[i]->getType() == NODE_LIGHT || currentScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            currentScene->nodes[i]->node->setVisible(false);
        if(!(currentScene->nodes[i]->props.isVisible))
            currentScene->nodes[i]->node->setVisible(false);
    }
    smgr->draw2DImage(currentScene->bgTexture,Vector2(0,0),Vector2(SceneHelper::screenWidth,SceneHelper::screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    setControlsVisibility(currentScene, false);
    
    
    smgr->Render();
    if(currentScene->whiteBorderTexture)
        smgr->draw2DImage(currentScene->whiteBorderTexture,Vector2(0,0),Vector2(SceneHelper::screenWidth, SceneHelper::screenHeight),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->setRenderTarget(NULL,false,false);
    smgr->setActiveCamera(currentScene->viewCamera);
    
    for(unsigned long i = 1; i < currentScene->nodes.size(); i++){
        if(currentScene->nodes[i]->getType() == NODE_LIGHT || currentScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            currentScene->nodes[i]->node->setVisible(true);
        
        if(!(currentScene->nodes[i]->props.isVisible))
            currentScene->nodes[i]->node->setVisible(true);
    }
    
}

void RenderHelper::setRenderCameraOrientation(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr || currentScene->nodes.size() <= NODE_CAMERA)
        return;
    currentScene->renderCamera->setPosition(currentScene->nodes[NODE_CAMERA]->node->getPosition());
    currentScene->renderCamera->updateAbsoluteTransformation();
    Vector3 rot  = currentScene->nodes[NODE_CAMERA]->node->getRotationInDegrees();
    rot.x += 180.0f;
    currentScene->renderCamera->setRotation(rot);
    currentScene->renderCamera->updateAbsoluteTransformation();
    
    Vector3 upReal = Vector3(0.0,1.0,0.0);
    Mat4 rotmat;
    rotmat.setRotationRadians(Vector3(rot.x - 180.0,rot.y,rot.z) * DEGTORAD);
    rotmat.rotateVect(upReal);
    currentScene->renderCamera->setUpVector(upReal);
    float texWidth = (float)currentScene->renderingTextureMap[RESOLUTION[currentScene->cameraResolutionType][0]]->width;
    float texHeight = currentScene->renderingTextureMap[RESOLUTION[currentScene->cameraResolutionType][0]]->height;
    float aspectRatio = texWidth/ texHeight;
    currentScene->renderCamera->setAspectRatio(aspectRatio);
    smgr->setActiveCamera(currentScene->renderCamera);
}

void RenderHelper::rttNodeJointSelection(void * scene, Vector2 touchPosition)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    
    if(currentScene->shaderMGR->deviceType == METAL){
        bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return;
    }
    currentScene->nodeJointPickerPosition = touchPosition;
    setControlsVisibility(currentScene, false);
    currentScene->rotationCircle->node->setVisible(false);
    SGNode *selectedSGNode = (currentScene->isNodeSelected) ? currentScene->nodes[currentScene->selectedNodeId] : NULL;
    if(!selectedSGNode || (selectedSGNode->getType() != NODE_RIG && selectedSGNode->getType() != NODE_TEXT))
        setJointSpheresVisibility(currentScene, false);
    
    smgr->setRenderTarget(currentScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    vector<Vector3> vertexColors;
    vector<float> transparency;
    vector<std::string> previousMaterialNames;
    vector<bool> nodesVisibility;
    for(int i = 0; i < currentScene->nodes.size(); i++){
        vertexColors.push_back(currentScene->nodes[i]->props.vertexColor);
        previousMaterialNames.push_back(currentScene->nodes[i]->node->material->name);
        transparency.push_back(currentScene->nodes[i]->props.transparency);
        nodesVisibility.push_back(currentScene->nodes[i]->props.isVisible);
        currentScene->nodes[i]->props.transparency = 1.0;
        currentScene->nodes[i]->props.isSelected = false;
        currentScene->nodes[i]->props.isVisible = true;
        currentScene->nodes[i]->props.vertexColor = Vector3((i/255.0),1.0,1.0);
        if(currentScene->nodes[i]->getType() == NODE_RIG)
            currentScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        else if (currentScene->nodes[i]->getType() == NODE_TEXT)
            currentScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
        else
            currentScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        
    }
    smgr->Render();
    // Draw Joints
    if(selectedSGNode && (selectedSGNode->getType() == NODE_RIG || selectedSGNode->getType() == NODE_TEXT))
        drawJointsSpheresForRTT(currentScene, true);
    
    for (int i = 0; i < currentScene->nodes.size(); i++) {
        currentScene->nodes[i]->props.vertexColor = vertexColors[i];
        currentScene->nodes[i]->props.transparency = transparency[i];
        currentScene->nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i]));
        currentScene->nodes[i]->props.isVisible = nodesVisibility[i];
    }
    previousMaterialNames.clear(); vertexColors.clear(); transparency.clear();
    
    if(currentScene->shaderMGR->deviceType == OPENGLES2)
        currentScene->selectMan->getNodeColorFromTouchTexture(currentScene);
    smgr->setRenderTarget(NULL,false,false);
    if(currentScene->shaderMGR->deviceType == METAL)
        smgr->EndDisplay();
}

void RenderHelper::drawJointsSpheresForRTT(void *scene,bool enableDepthTest)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    SGNode *sgNode = currentScene->selectedNode;
    // Draw Joints above nodes if nodeSelected
    if(currentScene->jointSpheres.size() <= 0)
        return;
    vector<Vector3> vertexColors;
    vector<Vector3> scaleValues;
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        vertexColors.push_back(currentScene->jointSpheres[j]->props.vertexColor);
        scaleValues.push_back(currentScene->jointSpheres[j]->node->getScale());
        currentScene->jointSpheres[j]->setScaleOnNode(scaleValues[j] * 1.3);
        currentScene->jointSpheres[j]->props.vertexColor = Vector3(currentScene->selectedNodeId/255.0,j/255.0,1.0);
        smgr->RenderNode(smgr->getNodeIndexByID(currentScene->jointSpheres[j]->node->getID()), (j == 0) ? enableDepthTest:false);
    }
    // Reset joints
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        currentScene->jointSpheres[j]->props.vertexColor = vertexColors[j];
        currentScene->jointSpheres[j]->setScaleOnNode(scaleValues[j]);
    }
    vertexColors.clear();
}


void RenderHelper::setJointSpheresVisibility(void * scene, bool visibilityFlag)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    visibilityFlag = (currentScene->selectedNodeId == NOT_EXISTS)?false:visibilityFlag;
    
    if (currentScene->selectedNodeId != NOT_EXISTS && currentScene->selectedNode)
        visibilityFlag = (currentScene->selectedNode->getType() == NODE_RIG || currentScene->selectedNode->getType() == NODE_TEXT) ? visibilityFlag : false;
    
    if(currentScene->jointSpheres.size())
        currentScene->jointSpheres[0]->node->setVisible(false);
    for(int i=1;i<currentScene->jointSpheres.size();i++){
        if(currentScene->selectedNodeId != NOT_EXISTS && currentScene->selectedNode && (currentScene->selectedNode->getType() == NODE_RIG || currentScene->selectedNode->getType() == NODE_TEXT) && i < currentScene->selectedNode->joints.size())
                currentScene->jointSpheres[i]->node->setVisible(visibilityFlag);
        else
            currentScene->jointSpheres[i]->node->setVisible(false);
    }
}

bool RenderHelper::displayJointSpheresForNode(void *scene, shared_ptr<AnimatedMeshNode> animNode , float scaleValue)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    bool status;
    removeJointSpheres(currentScene);
    int bonesCount = (int)animNode->getJointCount();
    if(bonesCount > currentScene->jointSpheres.size())
        status = createJointSpheres(currentScene, bonesCount - (int)currentScene->jointSpheres.size());
    
    for(int i = 0;i < bonesCount;i++){
        shared_ptr<JointNode> jointNode = animNode->getJointNode(i);
        currentScene->jointSpheres[i]->node->setParent(jointNode);
        if(bonesCount != currentScene->tPoseJoints.size())
            currentScene->jointSpheres[i]->node->setScale((animNode->getScale() * scaleValue)/animNode->getScale());
        else
            currentScene->jointSpheres[i]->node->setScale(Vector3(currentScene->tPoseJoints[i].sphereRadius/jointNode->getAbsoluteTransformation().getScale().x) * animNode->getScale());
    }
    setJointSpheresVisibility(currentScene, true);
    // TODO displayJointsBasedOnSelection(currentScene);
    return status;
    //animNode.reset();
}

bool RenderHelper::createJointSpheres(void *scene, int additionalJoints)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    if(additionalJoints <= 0)
        return false;
    int maxCount = (int)currentScene->jointSpheres.size() + additionalJoints;
    for(int i = (int)currentScene->jointSpheres.size();i < maxCount;i++){
        SGNode *sgNode = new SGNode(NODE_SGM);
        Mesh *jointSphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",smgr->device);
        if(jointSphereMesh == NULL)
            return false;
        
        shared_ptr<MeshNode> sphereNode = smgr->createNodeFromMesh(jointSphereMesh,"setJointSpheresUniforms");
        if(!sphereNode){
            Logger::log(ERROR,"SGScene", "Unable to create sgrSpheres");
            break;
        }
        sphereNode->updateAbsoluteTransformation();
        sphereNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        sgNode->node = sphereNode;
        sgNode->props.vertexColor = constants::sgrJointDefaultColor;
        currentScene->jointSpheres.push_back(sgNode);
        sgNode->node->setID(JOINT_SPHERES_START_ID + (int)currentScene->jointSpheres.size() - 1);
    }
    return true;
}

void RenderHelper::removeJointSpheres(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    for(int i = 0; i < currentScene->jointSpheres.size();i++){
        if(currentScene->jointSpheres[i])
            delete currentScene->jointSpheres[i];
    }
    currentScene->jointSpheres.clear();
}


