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

SGEditorScene * renderingScene;

static Vector3 redColor = Vector3(1.0,0.2,0.2);
static Vector3 greenColor = Vector3(0.2,1.0,0.2);
static Vector3 blueColor = Vector3(0.2,0.2,1.0);
Vector3 constants::sgrJointDefaultColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
Vector3 constants::selectionColor = Vector3(SELECT_COLOR_R,SELECT_COLOR_G,SELECT_COLOR_B);

RenderHelper::RenderHelper(SceneManager* sceneMngr, void* scene)
{
    this->smgr = sceneMngr;
    renderingScene = (SGEditorScene*)scene;
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

void RenderHelper::drawCircle()
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->controlType == ROTATE && renderingScene->selectedControlId != NOT_SELECTED && renderingScene->selectedNode) {
        Vector3 circleRotation = Vector3(90.0,0.0,0.0);
        Vector3 circleAxis = Vector3(1.0,0.0,1.0);
        if(renderingScene->selectedControlId == Y_ROTATE){
            circleRotation = Vector3(0.0,0.0,0.0);
            circleAxis = Vector3(1.0,1.0,0.0);
        }else if(renderingScene->selectedControlId == Z_ROTATE){
            circleRotation = Vector3(0.0,90.0,0.0);
            circleAxis = Vector3(0.0,1.0,1.0);
        }
        Vector3 nodePos = Vector3(0.0);
        if(renderingScene->selectedJoint)
            nodePos = renderingScene->selectedJoint->jointNode->getAbsolutePosition();
        else if(renderingScene->selectedNode)
            nodePos = renderingScene->selectedNode->node->getAbsolutePosition();
        else
            return;
        
        float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
        float radius = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
        float longLineLength = ROTATION_CIRCLE_LONG_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS) ,shorLineLength = ROTATION_CIRCLE_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS);
        renderingScene->rotationCircle->node->setPosition(nodePos);
        renderingScene->rotationCircle->node->setScale(Vector3(radius));
        renderingScene->rotationCircle->node->setRotationInDegrees(circleRotation);
        renderingScene->rotationCircle->node->setVisible(true);
        smgr->RenderNode(smgr->getNodeIndexByID(renderingScene->rotationCircle->node->getID()),true);
        
        Vector3 lineVect = nodePos - renderingScene->circleTouchPoint;
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

void RenderHelper::drawMoveAxisLine()
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->controlType == MOVE && renderingScene->selectedControlId != NOT_SELECTED && renderingScene->selectedNodeId != NOT_SELECTED){
        Vector3 nodePos = renderingScene->nodes[renderingScene->selectedNodeId]->node->getAbsolutePosition();
        if(renderingScene->selectedJointId != NOT_SELECTED)
            nodePos = (dynamic_pointer_cast<AnimatedMeshNode>(renderingScene->nodes[renderingScene->selectedNodeId]->node))->getJointNode(renderingScene->selectedJointId)->getAbsolutePosition();
        drawMoveControlLine(nodePos);
    }
}

void RenderHelper::drawMoveControlLine(Vector3 nodePos)
{
    if(!renderingScene || !smgr)
        return;
    
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    float farValue = renderingScene->viewCamera->getFarValue();
    if(renderingScene->selectedControlId == X_MOVE){
        smgr->draw3DLine(Vector3(-farValue,nodePos.y,nodePos.z),Vector3(farValue,nodePos.y,nodePos.z),redColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else if(renderingScene->selectedControlId == Y_MOVE){
        smgr->draw3DLine(Vector3(nodePos.x,-farValue,nodePos.z),Vector3(nodePos.x,farValue,nodePos.z),blueColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else{
        smgr->draw3DLine(Vector3(nodePos.x,nodePos.y,-farValue), Vector3(nodePos.x,nodePos.y,farValue),greenColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
}

void RenderHelper::renderControls()
{
    if(!renderingScene || !smgr || !renderingScene->isNodeSelected)
        return;
    
    setControlsVisibility(renderingScene);
    smgr->clearDepthBuffer();
    int controlStartIndex = (renderingScene->controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (renderingScene->controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        
        Vector3 ctrlToCam = (renderingScene->viewCamera->getPosition() - renderingScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        float ctrlTransparency = (angle > 0.9) ? (0.95 - angle) * 20.0 : 1.0;
        ctrlTransparency = (ctrlTransparency < 0.0) ? 0.0 : ctrlTransparency;
        renderingScene->sceneControls[i]->props.transparency = ctrlTransparency;
        
        if(renderingScene->selectedControlId == NOT_SELECTED){
            renderingScene->sceneControls[i]->props.isVisible = true;
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
        }else if(i == renderingScene->selectedControlId) {
            renderingScene->sceneControls[i]->props.isVisible = true;
            renderingScene->sceneControls[i]->props.vertexColor = Vector3(SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B);
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        }else {
            renderingScene->sceneControls[i]->props.isVisible = false;
        }
        
        int nodeIndex = smgr->getNodeIndexByID(renderingScene->sceneControls[i]->node->getID());
        
        if(renderingScene->sceneControls[i]->props.isVisible)
            smgr->RenderNode(nodeIndex,(i == controlStartIndex)?true:false);
    }
}

void RenderHelper::setControlsVisibility(bool isVisible)
{
    if(!renderingScene || renderingScene->sceneControls.size() == 0)
        return;
    
    if(renderingScene->isNodeSelected && renderingScene->selectedNode->getType() == NODE_LIGHT)
        renderingScene->controlType = MOVE;
    
    int controlStartToVisible = NOT_EXISTS,controlEndToVisible = NOT_EXISTS;
    if(renderingScene->isNodeSelected && isVisible){
        if(renderingScene->controlType == MOVE){
            controlStartToVisible = X_MOVE;
            controlEndToVisible = Z_MOVE;
        }else if(renderingScene->controlType == ROTATE){
            controlStartToVisible = X_ROTATE;
            controlEndToVisible = Z_ROTATE;
        } else {
            for (int i = 0; i < renderingScene->sceneControls.size(); i++) {
                renderingScene->sceneControls[i]->node->setVisible(false);
            }
            return;
        }
    }
    for(int i = X_MOVE;i <= Z_ROTATE;i++){
        isVisible = (i >= controlStartToVisible && i <= controlEndToVisible) ? true:false;
        renderingScene->sceneControls[i]->node->setVisible(isVisible);
    }
}

void RenderHelper::postRTTDrawCall()
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->previewTexture && (renderingScene->selectedNodeId == NODE_CAMERA || renderingScene->isPlaying)) {
        Vector4 previewLayout = renderingScene->getCameraPreviewLayout();
        smgr->draw2DImage(renderingScene->previewTexture,Vector2(previewLayout.x,previewLayout.y),Vector2(previewLayout.z,previewLayout.w), false, smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE),true);
    }
}

void RenderHelper::rttDrawCall()
{
    if(!renderingScene)
        return;

    if(renderingScene->selectedNodeId == NODE_CAMERA || renderingScene->isPlaying)
        drawCameraPreview();
    //rttShadowMap(renderingScene);
}

void RenderHelper::drawCameraPreview()
{
    if(!renderingScene || !smgr)
        return;
    
    setRenderCameraOrientation();
    renderingScene->rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(renderingScene->previewTexture,true,true,false,Vector4(255,255,255,255));
    Vector4 camprevlay = renderingScene->getCameraPreviewLayout();
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++){
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(false);
        if(!(renderingScene->nodes[i]->props.isVisible))
            renderingScene->nodes[i]->node->setVisible(false);
    }
    smgr->draw2DImage(renderingScene->bgTexture,Vector2(0,0),Vector2(SceneHelper::screenWidth,SceneHelper::screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    setControlsVisibility(false);
    
    
    smgr->Render();
    if(renderingScene->whiteBorderTexture)
        smgr->draw2DImage(renderingScene->whiteBorderTexture,Vector2(0,0),Vector2(SceneHelper::screenWidth, SceneHelper::screenHeight),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->setRenderTarget(NULL,false,false);
    smgr->setActiveCamera(renderingScene->viewCamera);
    
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++){
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(true);
        
        if(!(renderingScene->nodes[i]->props.isVisible))
            renderingScene->nodes[i]->node->setVisible(true);
    }
    
}

void RenderHelper::setRenderCameraOrientation()
{
    if(!renderingScene || !smgr || renderingScene->nodes.size() <= NODE_CAMERA)
        return;
    renderingScene->renderCamera->setPosition(renderingScene->nodes[NODE_CAMERA]->node->getPosition());
    renderingScene->renderCamera->updateAbsoluteTransformation();
    Vector3 rot  = renderingScene->nodes[NODE_CAMERA]->node->getRotationInDegrees();
    rot.x += 180.0f;
    renderingScene->renderCamera->setRotation(rot);
    renderingScene->renderCamera->updateAbsoluteTransformation();
    
    Vector3 upReal = Vector3(0.0,1.0,0.0);
    Mat4 rotmat;
    rotmat.setRotationRadians(Vector3(rot.x - 180.0,rot.y,rot.z) * DEGTORAD);
    rotmat.rotateVect(upReal);
    renderingScene->renderCamera->setUpVector(upReal);
    float texWidth = (float)renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->width;
    float texHeight = renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->height;
    float aspectRatio = texWidth/ texHeight;
    renderingScene->renderCamera->setAspectRatio(aspectRatio);
    smgr->setActiveCamera(renderingScene->renderCamera);
}

void RenderHelper::rttNodeJointSelection(Vector2 touchPosition)
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->shaderMGR->deviceType == METAL){
        bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return;
    }
    renderingScene->nodeJointPickerPosition = touchPosition;
    setControlsVisibility(false);
    renderingScene->rotationCircle->node->setVisible(false);
    SGNode *selectedSGNode = (renderingScene->isNodeSelected) ? renderingScene->nodes[renderingScene->selectedNodeId] : NULL;
    if(!selectedSGNode || (selectedSGNode->getType() != NODE_RIG && selectedSGNode->getType() != NODE_TEXT))
        setJointSpheresVisibility(false);
    
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    vector<Vector3> vertexColors;
    vector<float> transparency;
    vector<std::string> previousMaterialNames;
    vector<bool> nodesVisibility;
    for(int i = 0; i < renderingScene->nodes.size(); i++){
        vertexColors.push_back(renderingScene->nodes[i]->props.vertexColor);
        previousMaterialNames.push_back(renderingScene->nodes[i]->node->material->name);
        transparency.push_back(renderingScene->nodes[i]->props.transparency);
        nodesVisibility.push_back(renderingScene->nodes[i]->props.isVisible);
        renderingScene->nodes[i]->props.transparency = 1.0;
        renderingScene->nodes[i]->props.isSelected = false;
        renderingScene->nodes[i]->props.isVisible = true;
        renderingScene->nodes[i]->props.vertexColor = Vector3((i/255.0),1.0,1.0);
        if(renderingScene->nodes[i]->getType() == NODE_RIG)
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        else if (renderingScene->nodes[i]->getType() == NODE_TEXT)
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
        else
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        
    }
    smgr->Render();
    // Draw Joints
    if(selectedSGNode && (selectedSGNode->getType() == NODE_RIG || selectedSGNode->getType() == NODE_TEXT))
        drawJointsSpheresForRTT(true);
    
    for (int i = 0; i < renderingScene->nodes.size(); i++) {
        renderingScene->nodes[i]->props.vertexColor = vertexColors[i];
        renderingScene->nodes[i]->props.transparency = transparency[i];
        renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i]));
        renderingScene->nodes[i]->props.isVisible = nodesVisibility[i];
    }
    previousMaterialNames.clear(); vertexColors.clear(); transparency.clear();
    
    if(renderingScene->shaderMGR->deviceType == OPENGLES2)
        renderingScene->selectMan->getNodeColorFromTouchTexture();
    smgr->setRenderTarget(NULL,false,false);
    if(renderingScene->shaderMGR->deviceType == METAL)
        smgr->EndDisplay();
}

void RenderHelper::drawJointsSpheresForRTT(bool enableDepthTest)
{
    if(!renderingScene || !smgr)
        return;
    SGNode *sgNode = renderingScene->selectedNode;
    // Draw Joints above nodes if nodeSelected
    if(renderingScene->jointSpheres.size() <= 0)
        return;
    vector<Vector3> vertexColors;
    vector<Vector3> scaleValues;
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        vertexColors.push_back(renderingScene->jointSpheres[j]->props.vertexColor);
        scaleValues.push_back(renderingScene->jointSpheres[j]->node->getScale());
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j] * 1.3);
        renderingScene->jointSpheres[j]->props.vertexColor = Vector3(renderingScene->selectedNodeId/255.0,j/255.0,1.0);
        smgr->RenderNode(smgr->getNodeIndexByID(renderingScene->jointSpheres[j]->node->getID()), (j == 0) ? enableDepthTest:false);
    }
    // Reset joints
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        renderingScene->jointSpheres[j]->props.vertexColor = vertexColors[j];
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j]);
    }
    vertexColors.clear();
}


void RenderHelper::setJointSpheresVisibility(bool visibilityFlag)
{
    if(!renderingScene || !smgr)
        return;

    visibilityFlag = (renderingScene->selectedNodeId == NOT_EXISTS)?false:visibilityFlag;
    
    if (renderingScene->selectedNodeId != NOT_EXISTS && renderingScene->selectedNode)
        visibilityFlag = (renderingScene->selectedNode->getType() == NODE_RIG || renderingScene->selectedNode->getType() == NODE_TEXT) ? visibilityFlag : false;
    
    if(renderingScene->jointSpheres.size())
        renderingScene->jointSpheres[0]->node->setVisible(false);
    for(int i=1;i<renderingScene->jointSpheres.size();i++){
        if(renderingScene->selectedNodeId != NOT_EXISTS && renderingScene->selectedNode && (renderingScene->selectedNode->getType() == NODE_RIG || renderingScene->selectedNode->getType() == NODE_TEXT) && i < renderingScene->selectedNode->joints.size())
                renderingScene->jointSpheres[i]->node->setVisible(visibilityFlag);
        else
            renderingScene->jointSpheres[i]->node->setVisible(false);
    }
}

bool RenderHelper::displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue)
{
    if(!renderingScene || !smgr)
        return;

    bool status;
    removeJointSpheres();
    int bonesCount = (int)animNode->getJointCount();
    if(bonesCount > renderingScene->jointSpheres.size())
        status = createJointSpheres(bonesCount - (int)renderingScene->jointSpheres.size());
    
    for(int i = 0;i < bonesCount;i++){
        shared_ptr<JointNode> jointNode = animNode->getJointNode(i);
        renderingScene->jointSpheres[i]->node->setParent(jointNode);
        if(bonesCount != renderingScene->tPoseJoints.size())
            renderingScene->jointSpheres[i]->node->setScale((animNode->getScale() * scaleValue)/animNode->getScale());
        else
            renderingScene->jointSpheres[i]->node->setScale(Vector3(renderingScene->tPoseJoints[i].sphereRadius/jointNode->getAbsoluteTransformation().getScale().x) * animNode->getScale());
    }
    setJointSpheresVisibility(true);
    displayJointsBasedOnSelection();
    return status;
    //animNode.reset();
}

bool RenderHelper::createJointSpheres(int additionalJoints)
{
    if(!renderingScene || !smgr)
        return;

    if(additionalJoints <= 0)
        return false;
    int maxCount = (int)renderingScene->jointSpheres.size() + additionalJoints;
    for(int i = (int)renderingScene->jointSpheres.size();i < maxCount;i++){
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
        renderingScene->jointSpheres.push_back(sgNode);
        sgNode->node->setID(JOINT_SPHERES_START_ID + (int)renderingScene->jointSpheres.size() - 1);
    }
    return true;
}

void RenderHelper::displayJointsBasedOnSelection()
{
    if(!renderingScene || !smgr)
        return;

    for(int i = 0;i < renderingScene->jointSpheres.size();i++){
        //jointSpheres[i]->props.vertexColor = (i == selectedJointId) ? selectionColor :sgrJointDefaultColor;
        if(i == renderingScene->selectedJointId){
            renderingScene->selectedJoint = renderingScene->selectedNode->joints[i];
        }
    }
}

void RenderHelper::removeJointSpheres()
{
    if(!renderingScene || !smgr)
        return;

    for(int i = 0; i < renderingScene->jointSpheres.size();i++){
        if(renderingScene->jointSpheres[i])
            delete renderingScene->jointSpheres[i];
    }
    renderingScene->jointSpheres.clear();
}

void RenderHelper::rttShadowMap()
{
    if(!renderingScene || !smgr)
        return;
    
    bool isRotationCircleVisible = renderingScene->rotationCircle->node->getVisible();
    renderingScene->rotationCircle->node->setVisible(false);
    setControlsVisibility(false);
    ShaderManager::isRenderingDepthPass = true;
    smgr->setActiveCamera(renderingScene->lightCamera);
    smgr->setRenderTarget(renderingScene->shadowTexture,true,true,true,Vector4(255,255,255,255));
    setJointSpheresVisibility(false); //hide joints
    renderingScene->nodes[NODE_CAMERA]->node->setVisible(false);//hide camera
    renderingScene->nodes[NODE_LIGHT]->node->setVisible(false);//hide light
    vector<std::string> previousMaterialNames;
    for(unsigned long i = 2;i < renderingScene->nodes.size();i++){ // set shadow 1st pass shaders
        SGNode* sgNode = renderingScene->nodes[i];
        if(!sgNode->props.isVisible || !sgNode->props.isLighting)
            sgNode->node->setVisible(false);
        
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT)
            sgNode->node->setVisible(false);
        previousMaterialNames.push_back(sgNode->node->material->name);
        
        if(sgNode->node->skinType == CPU_SKIN) {
            sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
        } else {
            if(sgNode->getType() == NODE_RIG)
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_SKIN));
            else if (sgNode->getType() == NODE_TEXT)
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_TEXT));
            else
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
            
        }
    }
    smgr->Render();
    setJointSpheresVisibility(true); // Unhide joints
    renderingScene->nodes[NODE_CAMERA]->node->setVisible(true);// Unhide camera
    renderingScene->nodes[NODE_LIGHT]->node->setVisible(true);// Unhide light
    for(unsigned long i = 2;i < renderingScene->nodes.size();i++){// set previous shaders
        SGNode* sgNode = renderingScene->nodes[i];
        if(!sgNode->props.isVisible || !sgNode->props.isLighting)
            sgNode->node->setVisible(true);
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT)
            sgNode->node->setVisible(true);
        sgNode->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i - 2]));
    }
    
    smgr->setActiveCamera(renderingScene->viewCamera);
    smgr->setRenderTarget(NULL,true,true,true,Vector4(255,255,255,255));
    renderingScene->rotationCircle->node->setVisible(isRotationCircleVisible);
    ShaderManager::isRenderingDepthPass = false;
}
