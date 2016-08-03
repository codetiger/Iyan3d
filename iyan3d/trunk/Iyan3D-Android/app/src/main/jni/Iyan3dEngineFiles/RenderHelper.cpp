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

#define SELECT_COLOR_R 0.0
#define SELECT_COLOR_G 1.0
#define SELECT_COLOR_B 0.0

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "HeaderFiles/RenderHelper.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene * renderingScene;

static Vector3 redColor = Vector3(1.0, 0.2, 0.2);
static Vector3 greenColor = Vector3(0.2, 1.0, 0.2);
static Vector3 blueColor = Vector3(0.2, 0.2, 1.0);

Vector3 constants::sgrJointDefaultColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R, SGR_JOINT_DEFAULT_COLOR_G, SGR_JOINT_DEFAULT_COLOR_B);
Vector3 constants::selectionColor = Vector3(SELECT_COLOR_R, SELECT_COLOR_G, SELECT_COLOR_B);

RenderHelper::RenderHelper(SceneManager* sceneMngr, void* scene)
{
    this->smgr = sceneMngr;
    renderingScene = (SGEditorScene*)scene;
    isExportingImages = false;
    isExporting1stTime = true;
    renderingType = SHADER_MESH;
    isMovingPreview = false;
    cameraPreviewMoveDist = Vector2(0.0, 0.0);
    isFirstTimeRender = true;
}

RenderHelper::~RenderHelper()
{
    
}

bool RenderHelper::supportsVAO()
{
    #ifdef  ANDROID
     return renderingScene->addVAOSupport;
    #endif
    return true;
}

void RenderHelper::drawGrid()
{
    if(!smgr)
        return;
    
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    if(renderingScene->selectedNodeIds.size() > 0 && renderingScene->getParentNode()) {
        vector<Vector3> vPosBB;
        BoundingBox bb = renderingScene->getParentNode()->getBoundingBox();
        for (int j = 0; j < 3; j++) {
            vPosBB.push_back(bb.getEdgeByIndex(j));
            vPosBB.push_back(bb.getEdgeByIndex(j + 1));
            vPosBB.push_back(bb.getEdgeByIndex(j + 4));
            vPosBB.push_back(bb.getEdgeByIndex(j + 5));
            vPosBB.push_back(bb.getEdgeByIndex(j));
            vPosBB.push_back(bb.getEdgeByIndex(j + 5));
        }
        vPosBB.push_back(bb.getEdgeByIndex(3));
        vPosBB.push_back(bb.getEdgeByIndex(0));
        vPosBB.push_back(bb.getEdgeByIndex(7));
        vPosBB.push_back(bb.getEdgeByIndex(4));
        vPosBB.push_back(bb.getEdgeByIndex(3));
        vPosBB.push_back(bb.getEdgeByIndex(4));
        
        smgr->draw3DLines(vPosBB, Vector3(1.0,1.0,0.0), mat, SHADER_COLOR_mvp, SHADER_COLOR_vertexColor, SHADER_COLOR_transparency);
    }
}

void RenderHelper::drawCircle()
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->controlType == ROTATE && renderingScene->selectedControlId != NOT_SELECTED && (renderingScene->selectedNode || renderingScene->getParentNode())) {
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
        if(renderingScene->selectedNodeIds.size() > 0)
            nodePos = renderingScene->getPivotPoint(false);
        else if(renderingScene->selectedJoint)
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
        renderingScene->rotationCircle->node->setRotation(Quaternion(circleRotation * DEGTORAD));
        renderingScene->rotationCircle->node->setVisible(true);
        smgr->RenderNode(false, smgr->getNodeIndexByID(renderingScene->rotationCircle->node->getID()),true);
        
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
        if(renderingScene->selectedJointId != NOT_SELECTED) {
            if(dynamic_pointer_cast<AnimatedMeshNode>(renderingScene->nodes[renderingScene->selectedNodeId]->node) &&(dynamic_pointer_cast<AnimatedMeshNode>(renderingScene->nodes[renderingScene->selectedNodeId]->node))->getJointNode(renderingScene->selectedJointId))
                nodePos = (dynamic_pointer_cast<AnimatedMeshNode>(renderingScene->nodes[renderingScene->selectedNodeId]->node))->getJointNode(renderingScene->selectedJointId)->getAbsolutePosition();
        }
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
    if(!renderingScene || !smgr || (!renderingScene->hasNodeSelected() && renderingScene->selectedNodeIds.size() <=0))
        return;
    
    setControlsVisibility(true);
    smgr->clearDepthBuffer();
    int controlStartIndex = (renderingScene->controlType == MOVE) ? X_MOVE : (renderingScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndIndex = (renderingScene->controlType == MOVE) ? Z_MOVE : (renderingScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        Vector3 ctrlToCam = (renderingScene->viewCamera->getPosition() - renderingScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        float ctrlTransparency = (angle > 0.9) ? (0.95 - angle) * 20.0 : 1.0;
        ctrlTransparency = (ctrlTransparency < 0.0) ? 0.0 : ctrlTransparency;
        renderingScene->sceneControls[i]->addOrUpdateProperty(TRANSPARENCY, Vector4(ctrlTransparency), UNDEFINED);
        
        if(renderingScene->selectedControlId == NOT_SELECTED) {
            renderingScene->sceneControls[i]->getProperty(VERTEX_COLOR).value = Vector4(Vector3(-1.0), 0.0);
            renderingScene->sceneControls[i]->addOrUpdateProperty(VISIBILITY, Vector4(true), UNDEFINED);
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
        } else if(i == renderingScene->selectedControlId) {
            renderingScene->sceneControls[i]->addOrUpdateProperty(VISIBILITY, Vector4(true), UNDEFINED);
            renderingScene->sceneControls[i]->addOrUpdateProperty(VERTEX_COLOR, Vector4(SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B, 1.0), MATERIAL_PROPS);
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        } else {
            renderingScene->sceneControls[i]->addOrUpdateProperty(VISIBILITY, Vector4(false), UNDEFINED);
        }
        
        int nodeIndex = smgr->getNodeIndexByID(renderingScene->sceneControls[i]->node->getID());
        
        if(renderingScene->sceneControls[i]->getProperty(VISIBILITY).value.x && renderingScene->sceneControls[i]->getProperty(TRANSPARENCY).value.x > 0.0) {
            if((renderingScene->isRigMode && renderingScene->rigMan->sceneMode != RIG_MODE_EDIT_ENVELOPES) || !renderingScene->isRigMode)
                smgr->RenderNode(false, nodeIndex,(i == controlStartIndex)?true:false);
        }
    }
}

void RenderHelper::setControlsVisibility(bool isVisible)
{
    if(!renderingScene || renderingScene->sceneControls.size() == 0)
        return;
    
    bool isNodeSelected = renderingScene->hasNodeSelected();
    SGNode* selectedNode = renderingScene->getSelectedNode();

    if(renderingScene->selectedNodeIds.size() <= 0 && isNodeSelected && selectedNode->getType() == NODE_LIGHT && selectedNode->getProperty(LIGHT_TYPE).value.x != (int)DIRECTIONAL_LIGHT)
        renderingScene->controlType = MOVE;
    
    int controlStartToVisible = NOT_EXISTS,controlEndToVisible = NOT_EXISTS;
    if((isNodeSelected || renderingScene->selectedNodeIds.size() > 0) && isVisible){
        if(renderingScene->controlType == MOVE) {
            controlStartToVisible = X_MOVE;
            controlEndToVisible = Z_MOVE;
        } else if(renderingScene->controlType == ROTATE) {
            controlStartToVisible = X_ROTATE;
            controlEndToVisible = Z_ROTATE;
        } else if(renderingScene->controlType == SCALE) {
            controlStartToVisible = X_SCALE;
            controlEndToVisible = Z_SCALE;
        } else {
            for (int i = 0; i < renderingScene->sceneControls.size(); i++)
                renderingScene->sceneControls[i]->node->setVisible(false);

            return;
        }
    }
    
    for(int i = X_MOVE;i <= Z_SCALE;i++) {
        isVisible = (i >= controlStartToVisible && i <= controlEndToVisible) ? true:false;
        if(renderingScene->isControlSelected && isVisible)
            isVisible = (i == renderingScene->selectedControlId) ? true : false;
        
        renderingScene->sceneControls[i]->node->setVisible(isVisible);
    }
}

void RenderHelper::movePreviewToCorner()
{
    if(cameraPreviewMoveDist.x != 0.0 || cameraPreviewMoveDist.y != 0.0)
        return;
    
    Vector2 leftTop = renderingScene->topLeft;
    Vector2 leftBottom = renderingScene->bottomLeft;
    Vector2 rightTop = renderingScene->topRight;
    Vector2 rightBotton = renderingScene->bottomRight;
    
    Vector2 origin = renderingScene->camPreviewOrigin;
    Vector2 end = renderingScene->camPreviewEnd;
    
    Vector2 center = (origin+end)/2.0;
    float distance1 = leftTop.getDistanceFrom(center);
    float distance2 = leftBottom.getDistanceFrom(center);
    float distance3 = rightTop.getDistanceFrom(center);
    float distance4 = rightBotton.getDistanceFrom(center);
    float minimumDistance = MIN(MIN(distance1, distance2) , MIN(distance3, distance4));
    
    if(distance1 == minimumDistance && distance1 > 0.0) {
        if(renderingScene->camPreviewOrigin.x > leftTop.x)
            renderingScene->camPreviewOrigin.x -= 20.0;
        if(renderingScene->camPreviewOrigin.y > leftTop.y)
            renderingScene->camPreviewOrigin.y -= 20.0;
    } else if(distance2 == minimumDistance && distance2 > 0.0) {
        if(renderingScene->camPreviewOrigin.x > leftBottom.x)
            renderingScene->camPreviewOrigin.x -= 20.0;
        if(renderingScene->camPreviewEnd.y < (leftBottom.y - 1.0))
            renderingScene->camPreviewOrigin.y += 20.0;
    } else if(distance3 == minimumDistance && distance3 > 0.0) {
        if(renderingScene->camPreviewEnd.x < (rightTop.x  - 1.0))
            renderingScene->camPreviewOrigin.x += 20.0;
        if(renderingScene->camPreviewOrigin.y > rightTop.y)
            renderingScene->camPreviewOrigin.y -= 20.0;
    } else if(distance4 == minimumDistance && distance4 > 0.0) {
        if(renderingScene->camPreviewEnd.x < (rightBotton.x  - 1.0))
            renderingScene->camPreviewOrigin.x += 20.0;
        if(renderingScene->camPreviewEnd.y < (rightBotton.y  - 1.0))
            renderingScene->camPreviewOrigin.y += 20.0;
    }
    
}

void RenderHelper::postRTTDrawCall()
{
    if(!renderingScene || !smgr)
        return;
    
    if(renderingScene->previewTexture && (renderingScene->selectedNodeId == NODE_CAMERA || renderingScene->isPlaying)) {
        movePreviewToCorner();
        Vector4 previewLayout = renderingScene->getCameraPreviewLayout();
        smgr->draw2DImage(renderingScene->previewTexture,Vector2(previewLayout.x, previewLayout.y),Vector2(previewLayout.z, previewLayout.w), false, smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE),true);
    }
}

void RenderHelper::rttDrawCall()
{
    if (!renderingScene)
        return;

    if (renderingScene->selectedNodeId == NODE_CAMERA || renderingScene->isPlaying) {
        drawCameraPreview();
    }

    if ((!renderingScene->isRigMode && !renderingScene->shadowsOff && ShaderManager::shadowDensity > 0.0) || isFirstTimeRender) {
        rttShadowMap();
        renderingScene->shadowsOff = true;
        isFirstTimeRender = false;
    } else
        renderingScene->shadowsOff = true;
}

bool RenderHelper::isMovingCameraPreview(Vector2 curTouchPos)
{
    Vector4 prevLay = renderingScene->getCameraPreviewLayout();
    if(curTouchPos.x > prevLay.x && curTouchPos.x < prevLay.z && curTouchPos.y > prevLay.y && curTouchPos.y < prevLay.w) {
        return true;
    }
    return false;
}

void RenderHelper::changeCameraPreviewCoords(Vector2 touchPos)
{
    if(!renderingScene || !smgr)
        return;
    Vector2 previousTouchPos = renderingScene->moveMan->prevTouchPoints[0];
    cameraPreviewMoveDist = previousTouchPos - touchPos;
    renderingScene->moveMan->prevTouchPoints[0] = touchPos;
}

void RenderHelper::drawCameraPreview()
{
    if(!renderingScene || !smgr || renderingScene->selectedNodeIds.size() > 0)
        return;
    
    ShaderManager::renderingPreview = true;
    bool isLightOn = ShaderManager::sceneLighting;
    if(!isLightOn)
        renderingScene->setLightingOn();
    
    setRenderCameraOrientation();
    renderingScene->rotationCircle->node->setVisible(false);
    renderingScene->greenGrid->node->setVisible(false);
    renderingScene->blueGrid->node->setVisible(false);
    renderingScene->redGrid->node->setVisible(false);
    
    smgr->setRenderTarget(renderingScene->previewTexture,true,true,false,Vector4(0.1,0.1,0.1,1.0));
    Vector4 camprevlay = renderingScene->getCameraPreviewLayout();
    std::map< int, Vector3 > nPositions;
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++){
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(false);

        if(!(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x)) {
            renderingScene->nodes[i]->node->setVisible(false);
            if(renderingScene->nodes[i]->node->type == NODE_TYPE_INSTANCED) {
                nPositions.insert(std::pair<int, Vector3>(i, renderingScene->nodes[i]->node->getAbsolutePosition()));
                renderingScene->nodes[i]->node->setPosition(Vector3(5000.0));
            }
        }
    }
    setControlsVisibility(false);
    
    
    smgr->Render(false);
    if(renderingScene->whiteBorderTexture)
        smgr->draw2DImage(renderingScene->whiteBorderTexture,Vector2(0,0),Vector2(SceneHelper::screenWidth, SceneHelper::screenHeight ),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->setRenderTarget(NULL,false,false);
    smgr->setActiveCamera(renderingScene->viewCamera);
    
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++){
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(true);
        
        if(!(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x)) {
            renderingScene->nodes[i]->node->setVisible(true);
            if(renderingScene->nodes[i]->node->type == NODE_TYPE_INSTANCED) {
                renderingScene->nodes[i]->node->setPosition(nPositions[i]);
            }
        }
        
        if(!isLightOn)
            renderingScene->setLightingOff();
    }
    ShaderManager::renderingPreview = false;
    renderingScene->greenGrid->node->setVisible(true);
    renderingScene->blueGrid->node->setVisible(true);
    renderingScene->redGrid->node->setVisible(true);

    
}

void RenderHelper::setRenderCameraOrientation()
{
    if(!renderingScene || !smgr || renderingScene->nodes.size() <= NODE_CAMERA)
        return;
    renderingScene->renderCamera->setPosition(renderingScene->nodes[NODE_CAMERA]->node->getPosition());
    renderingScene->renderCamera->updateAbsoluteTransformation();
    Quaternion rot  = renderingScene->nodes[NODE_CAMERA]->node->getRotation();
    renderingScene->renderCamera->setRotation(rot * Quaternion(Vector3(180, 0, 0) * DEGTORAD));
    renderingScene->renderCamera->updateAbsoluteTransformation();
    
    Vector3 upReal = Vector3(0.0, 1.0, 0.0);
    Mat4 rotmat;
    rotmat.setRotation(Quaternion(Vector3(rot.x - 180.0, rot.y, rot.z) * DEGTORAD));
    rotmat.rotateVect(upReal);
    renderingScene->renderCamera->setUpVector(upReal);
#ifndef UBUNTU
    float texWidth = (float)renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->width;
    float texHeight = renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->height;
    float aspectRatio = texWidth/ texHeight;
    renderingScene->renderCamera->setAspectRatio(aspectRatio);
    smgr->setActiveCamera(renderingScene->renderCamera);
#endif
}

void RenderHelper::rttNodeJointSelection(Vector2 touchPosition, bool isMultiSelectenabled, bool touchMove)
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
    if(!selectedSGNode || (selectedSGNode->getType() != NODE_RIG && selectedSGNode->getType() != NODE_TEXT_SKIN))
        setJointSpheresVisibility(false);
    
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    vector<Vector4> vertexColors;
    vector<float> transparency;
    vector<std::string> previousMaterialNames;
    vector<bool> nodesVisibility;
    vector<bool> nodeSelection;
    vector<bool> isVcolored;
    vector<bool> isLighting;
    
    for(int i = 0; i < renderingScene->nodes.size(); i++){
        vertexColors.push_back(renderingScene->nodes[i]->getProperty(VERTEX_COLOR).value);
        previousMaterialNames.push_back(renderingScene->nodes[i]->node->material->name);
        transparency.push_back(renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x);
        nodesVisibility.push_back(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x);
        nodeSelection.push_back(renderingScene->nodes[i]->getProperty(SELECTED).value.x);
        isVcolored.push_back(renderingScene->nodes[i]->getProperty(IS_VERTEX_COLOR).value.x);
        isLighting.push_back(renderingScene->nodes[i]->getProperty(LIGHTING).value.x);
        
        renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x = 1.0;
        
        if(renderingScene->nodes[i]->getType() == NODE_PARTICLES)
            renderingScene->nodes[i]->getProperty(SELECTED).value.x = true;
        else
            renderingScene->nodes[i]->getProperty(SELECTED).value.x = false;
        
        if(renderingScene->nodes[i]->getType() == NODE_PARTICLES)
                renderingScene->nodes[i]->addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), MATERIAL_PROPS);
        renderingScene->nodes[i]->getProperty(VISIBILITY).value.x = renderingScene->nodes[i]->isTempNode ? false : true;
        Vector3 packed = MathHelper::packInterger(i);
        renderingScene->nodes[i]->addOrUpdateProperty(VERTEX_COLOR, Vector4(packed/255.0 ,1.0), MATERIAL_PROPS);
        
        if(renderingScene->nodes[i]->getType() == NODE_RIG || renderingScene->nodes[i]->getType() == NODE_TEXT_SKIN)
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(renderingScene->nodes[i]->node->skinType == CPU_SKIN ? SHADER_COLOR : SHADER_COLOR_SKIN));
        else if(renderingScene->nodes[i]->getType() == NODE_PARTICLES)
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_PARTICLES_RTT));
        else {
            if(renderingScene->nodes[i]->getType() == NODE_CAMERA)
                renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
            renderingScene->nodes[i]->addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(true, 0, 0, 0), MATERIAL_PROPS);
            renderingScene->nodes[i]->getProperty(LIGHTING).value.x = false;
        }
    }
    
    smgr->Render(true);

    if((selectedSGNode && renderingScene->selectedNodeIds.size() <= 0 && (selectedSGNode->getType() == NODE_RIG || selectedSGNode->getType() == NODE_TEXT_SKIN) && !touchMove) || renderingScene->isJointSelected)
        drawJointsSpheresForRTT(true);
    
    for (int i = 0; i < renderingScene->nodes.size(); i++) {
        if(renderingScene->nodes[i]->getType() == NODE_PARTICLES)
            renderingScene->nodes[i]->addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(false, 0, 0, 0), MATERIAL_PROPS);

        renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i]));
        renderingScene->nodes[i]->addOrUpdateProperty(VERTEX_COLOR, vertexColors[i], MATERIAL_PROPS);
        renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x = transparency[i];
        renderingScene->nodes[i]->getProperty(VISIBILITY).value.x = nodesVisibility[i];
        renderingScene->nodes[i]->getProperty(SELECTED).value.x = nodeSelection[i];
        renderingScene->nodes[i]->addOrUpdateProperty(IS_VERTEX_COLOR, Vector4(isVcolored[i], 0, 0, 0), MATERIAL_PROPS);
        renderingScene->nodes[i]->getProperty(LIGHTING).value.x = isLighting[i];
    }
    previousMaterialNames.clear(); vertexColors.clear(); transparency.clear();
    
    if(renderingScene->shaderMGR->deviceType == OPENGLES2)
        renderingScene->selectMan->getNodeColorFromTouchTexture(isMultiSelectenabled,touchMove);
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
    vector<Vector4> vertexColors;
    vector<Vector3> scaleValues;
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        vertexColors.push_back(renderingScene->jointSpheres[j]->getProperty(VERTEX_COLOR).value);
        scaleValues.push_back(renderingScene->jointSpheres[j]->node->getScale());
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j] * 1.3);
        Vector3 packed = MathHelper::packInterger(renderingScene->selectedNodeId);
        packed.z = j;
        renderingScene->jointSpheres[j]->addOrUpdateProperty(VERTEX_COLOR, Vector4( packed/255.0, 1.0 ), MATERIAL_PROPS);
        smgr->RenderNode(false, smgr->getNodeIndexByID(renderingScene->jointSpheres[j]->node->getID()), (j == 0) ? enableDepthTest:false);
    }
    // Reset joints
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        renderingScene->jointSpheres[j]->addOrUpdateProperty(VERTEX_COLOR, vertexColors[j], MATERIAL_PROPS);
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j]);
    }
    vertexColors.clear();
}

void RenderHelper::setJointSpheresVisibility(bool visibilityFlag)
{
    if(!renderingScene || !smgr)
        return;

    bool isNodeSelected = renderingScene->hasNodeSelected();
    SGNode *selectedNode = renderingScene->getSelectedNode();
    visibilityFlag = (!isNodeSelected || renderingScene->selectedNodeIds.size() > 0) ?false:visibilityFlag;
    
    if (isNodeSelected && selectedNode)
        visibilityFlag = (selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT_SKIN) ? visibilityFlag : false;
    
    if(renderingScene->jointSpheres.size())
        renderingScene->jointSpheres[0]->node->setVisible(false);
    for(int i=1;i<renderingScene->jointSpheres.size();i++){
        if(isNodeSelected && selectedNode && (selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT_SKIN) && i < selectedNode->joints.size())
                renderingScene->jointSpheres[i]->node->setVisible(visibilityFlag);
        else if (renderingScene->selectedNodeIds.size() > 0)
            renderingScene->jointSpheres[i]->node->setVisible(false);
        else
            renderingScene->jointSpheres[i]->node->setVisible(false);
    }
}

void RenderHelper::setJointAndBonesVisibility(std::map<int, RigKey>& rigKeys, bool isVisible)
{
    if(!renderingScene || !smgr || !renderingScene->isRigMode)
        return;
    vector<TPoseJoint> tPoseJoints(renderingScene->tPoseJoints);
    for(int i = 1;i < rigKeys.size();i++){
        if(rigKeys[tPoseJoints[i].id].referenceNode && rigKeys[tPoseJoints[i].id].referenceNode->node)
            rigKeys[tPoseJoints[i].id].referenceNode->node->setVisible(isVisible);
        if(rigKeys[tPoseJoints[i].id].sphere && rigKeys[tPoseJoints[i].id].sphere->node)
            rigKeys[tPoseJoints[i].id].sphere->node->setVisible(isVisible);
        if(rigKeys[tPoseJoints[i].id].parentId > 0){
            if(rigKeys[tPoseJoints[i].id].bone && rigKeys[tPoseJoints[i].id].bone->node)
                rigKeys[tPoseJoints[i].id].bone->node->setVisible(isVisible);
        }
    }
}

void RenderHelper::setEnvelopVisibility(std::map<int, SGNode*>& envelopes, bool isVisible)
{
    if(!renderingScene || !smgr || !renderingScene->isRigMode)
        return;
    for(std::map<int,SGNode *> :: iterator it = envelopes.begin(); it!=envelopes.end(); it++){
        if(it->second != NULL){
            it->second->node->setVisible(isVisible);
        }
    }
}

void RenderHelper::drawEnvelopes(std::map<int, SGNode*>& envelopes, int jointId)
{
    setEnvelopVisibility(envelopes, false);
    if(jointId<=0)  return;
    if(jointId != 1)   //skipping the envelop between hip and pivot
        renderingScene->loader->initEnvelope(envelopes, jointId);
    
    std::map<int, RigKey>& rigKeys = renderingScene->rigMan->rigKeys;
    
    if(rigKeys[jointId].referenceNode && rigKeys[jointId].referenceNode->node) {
        shared_ptr< vector< shared_ptr<Node> > > childs = rigKeys[jointId].referenceNode->node->Children;
        int childCount = (int)childs->size();
        for(int i = 0; i < childCount;i++){
            shared_ptr<MeshNode> childReference = dynamic_pointer_cast<MeshNode>((*childs)[i]);
            if(childReference && (childReference->getID() >= REFERENCE_NODE_START_ID && childReference->getID() <= REFERENCE_NODE_START_ID + rigKeys.size()))
                renderingScene->loader->initEnvelope(envelopes, childReference->getID() - REFERENCE_NODE_START_ID);
        }
    }
}

void RenderHelper::renderEnvelopes()
{
    if(renderingScene && renderingScene->isRigMode) {
        std::map<int , SGNode*> &envelopes = renderingScene->rigMan->envelopes;
        for(std::map<int,SGNode *> :: iterator it = envelopes.begin(); it!=envelopes.end(); it++){
            if(it->second->node->getVisible()) {
                it->second->node->shouldUpdateMesh = true;
                smgr->RenderNodeAlone(it->second->node);
            }
        }
    }
}

void RenderHelper::renderAndSaveImage(char *imagePath , int shaderType,bool isDisplayPrepared, bool removeWaterMark, int frame, Vector4 bgColor)
{
    if(!renderingScene || !smgr || renderingScene->isRigMode)
        return;
    
    if(!renderingScene->checkNodeSize())
        return;
    isExporting1stTime = false;
    
    bool isLightOn = ShaderManager::sceneLighting;
    bool isShadowsOff = ShaderManager::shadowsOff;
    ShaderManager::shadowsOff = false;
    
    if(!isLightOn)
        renderingScene->setLightingOn();
    
    if(smgr->device == OPENGLES2)
        rttShadowMap();
    
    bool displayPrepared = smgr->PrepareDisplay(renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->width, renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]]->height,false,true,false,Vector4(bgColor));
    if(!displayPrepared)
        return;
    setRenderCameraOrientation();
    setControlsVisibility(false);
    setJointSpheresVisibility(false);
    renderingScene->rotationCircle->node->setVisible(false);
    renderingScene->greenGrid->node->setVisible(false);
    renderingScene->blueGrid->node->setVisible(false);
    renderingScene->redGrid->node->setVisible(false);
    
    int selectedObjectId = 0;
    if(renderingScene->selectedNodeId != NOT_SELECTED) {
        selectedObjectId = renderingScene->selectedNodeId;
        renderingScene->selectMan->unselectObject(renderingScene->selectedNodeId);
    }
    
    vector<string> previousMaterialNames;
    if(renderingType != shaderType)
    {
        renderingScene->updater->resetMaterialTypes(true);
    }
    
    std::map< int , Vector3 > nPositions;
    for(unsigned long i = 0; i < renderingScene->nodes.size(); i++){
        if(!(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x)) {
            renderingScene->nodes[i]->node->setVisible(false);
            if(renderingScene->nodes[i]->node->type == NODE_TYPE_INSTANCED) {
                nPositions.insert(std::pair<int, Vector3>(i, renderingScene->nodes[i]->node->getAbsolutePosition()));
                renderingScene->nodes[i]->node->setPosition(Vector3(5000.0));
            }
        }
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(false);
    }
    
    smgr->setRenderTarget(renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]],true,true,false,Vector4(bgColor));
    
    smgr->Render(false);
    
    if(renderingScene->watermarkTexture)
        smgr->RemoveTexture(renderingScene->watermarkTexture);
    
    int totalImgs = 118;
    int index = 0;
    if(frame > -1) {
        int divisor = (frame > totalImgs) ? frame/totalImgs : 1;
        index = (frame > totalImgs) ? frame - (divisor * totalImgs) : frame;
    }
    string watermarkPath = constants::BundlePath + "/wm" + to_string(index) + ".png";
    renderingScene->watermarkTexture = smgr->loadTexture("waterMarkTexture", watermarkPath, TEXTURE_RGBA8, TEXTURE_BYTE, true);
    int waterMarkSize = SceneHelper::screenWidth * 0.2;
    
    if(!removeWaterMark)
        smgr->draw2DImage(renderingScene->watermarkTexture,Vector2(SceneHelper::screenWidth - waterMarkSize ,SceneHelper::screenHeight - waterMarkSize),Vector2(SceneHelper::screenWidth,SceneHelper::screenHeight),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    if(smgr->device == METAL)
        rttShadowMap();
    
    smgr->EndDisplay();
    smgr->writeImageToFile(renderingScene->renderingTextureMap[RESOLUTION[renderingScene->cameraResolutionType][0]],imagePath,(renderingScene->shaderMGR->deviceType == OPENGLES2) ?FLIP_VERTICAL : NO_FLIP);
    
    smgr->setActiveCamera(renderingScene->viewCamera);
    smgr->setRenderTarget(NULL,true,true,false,Vector4(bgColor));
    
    
    for(unsigned long i = 0; i < renderingScene->nodes.size(); i++){
        if(!(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x)) {
            renderingScene->nodes[i]->node->setVisible(true);
            if(renderingScene->nodes[i]->node->type == NODE_TYPE_INSTANCED)
                renderingScene->nodes[i]->node->setPosition(nPositions[i]);
        }
        if(renderingScene->nodes[i]->getType() == NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            renderingScene->nodes[i]->node->setVisible(true);
    }
    
    renderingScene->greenGrid->node->setVisible(true);
    renderingScene->blueGrid->node->setVisible(true);
    renderingScene->redGrid->node->setVisible(true);

    ShaderManager::shadowsOff = isShadowsOff;
    
    if(renderingType != shaderType)
        renderingScene->updater->resetMaterialTypes(false);
    
    if(selectedObjectId != NOT_SELECTED)
        renderingScene->selectMan->selectObject(selectedObjectId,false);
}

bool RenderHelper::displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue)
{
    if(!renderingScene || !smgr)
        return false;

    bool status = false;;
    
    removeJointSpheres();
    int bonesCount = (int)animNode->getJointCount();
    if(bonesCount > renderingScene->jointSpheres.size())
        status = createJointSpheres(bonesCount - (int)renderingScene->jointSpheres.size());
    
    for(int i = 0;i < bonesCount;i++){
        shared_ptr<JointNode> jointNode = animNode->getJointNode(i);
        renderingScene->jointSpheres[i]->node->setParent(jointNode);
        if(bonesCount != renderingScene->tPoseJoints.size())
            renderingScene->jointSpheres[i]->node->setScale(scaleValue);
        else {
            float radius = renderingScene->tPoseJoints[i].sphereRadius;
            Vector3 nodeScale = animNode->getScale();
            Vector3 jointScale = jointNode->getAbsoluteTransformation().getScale();
            Vector3 finalScaleValue;
            if(jointScale.x > nodeScale.x || jointScale.y > nodeScale.y || jointScale.z > nodeScale.z || radius > nodeScale.x || radius > nodeScale.y || radius > nodeScale.z)
                finalScaleValue = Vector3(radius/jointNode->getAbsoluteTransformation().getScale().x, radius/jointNode->getAbsoluteTransformation().getScale().y, radius/jointNode->getAbsoluteTransformation().getScale().z) * animNode->getScale();
            else
                finalScaleValue = Vector3(radius);
            
            renderingScene->jointSpheres[i]->node->setScale(finalScaleValue);
        }
    }
    setJointSpheresVisibility(true);
    displayJointsBasedOnSelection();
    return status;
}

bool RenderHelper::createJointSpheres(int additionalJoints)
{
    if(!renderingScene || !smgr)
        return false;

    if(additionalJoints <= 0)
        return false;
    int maxCount = (int)renderingScene->jointSpheres.size() + additionalJoints;
    for(int i = (int)renderingScene->jointSpheres.size();i < maxCount;i++){
        SGNode *sgNode = new SGNode(NODE_SGM);
        sgNode->materialProps.push_back(new MaterialProperty(NODE_SGM));

        Mesh *jointSphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",smgr->device);
        if(jointSphereMesh == NULL) {
            delete sgNode;
            return false;
        }
        
        shared_ptr<MeshNode> sphereNode = smgr->createNodeFromMesh(jointSphereMesh,"setJointSpheresUniforms");
        if(!sphereNode){
            Logger::log(ERROR,"SGScene", "Unable to create sgrSpheres");
            delete sgNode;
            break;
        }
        sphereNode->updateAbsoluteTransformation();
        sphereNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        sgNode->node = sphereNode;
        sgNode->addOrUpdateProperty(VERTEX_COLOR, Vector4(constants::sgrJointDefaultColor, 1.0), MATERIAL_PROPS);
        renderingScene->jointSpheres.push_back(sgNode);
        sgNode->node->setID(JOINT_SPHERES_START_ID + (int)renderingScene->jointSpheres.size() - 1);
    }
    return true;
}

void RenderHelper::displayJointsBasedOnSelection()
{
    if(!renderingScene || !smgr)
        return;
    
    SGNode* selectedNode = renderingScene->getSelectedNode();
    if(!selectedNode)
        return;

    for(int i = 0;i < renderingScene->jointSpheres.size();i++){
        int selectedJointId = (!renderingScene->isRigMode) ? renderingScene->selectedJointId : renderingScene->rigMan->selectedJointId;
        //jointSpheres[i]->props.vertexColor = (i == selectedJointId) ? selectionColor :sgrJointDefaultColor;
        if(i == selectedJointId){
            if(renderingScene->isRigMode)
                renderingScene->rigMan->selectedJoint = selectedNode->joints[i];
            else
                renderingScene->selectedJoint = selectedNode->joints[i];
            
        }
    }
}

void RenderHelper::removeJointSpheres()
{
    if(!renderingScene || !smgr)
        return;

    for(int i = 0; i < renderingScene->jointSpheres.size();i++){
        if(renderingScene->jointSpheres[i]) {
            smgr->RemoveNode(renderingScene->jointSpheres[i]->node);
            delete renderingScene->jointSpheres[i];
        }
    }
    renderingScene->jointSpheres.clear();
}

void RenderHelper::rttShadowMap()
{
    if(!renderingScene || !smgr)
        return;
    
    bool isRotationCircleVisible = false;
    if(renderingScene->rotationCircle && renderingScene->rotationCircle->node) {
        isRotationCircleVisible = renderingScene->rotationCircle->node->getVisible();
        renderingScene->rotationCircle->node->setVisible(false);
    }
    renderingScene->greenGrid->node->setVisible(false);
    renderingScene->blueGrid->node->setVisible(false);
    renderingScene->redGrid->node->setVisible(false);
    bool isDirectionLineVisible = renderingScene->directionLine->node->getVisible();
    bool lightCirclesVisible = renderingScene->lightCircles->node->getVisible();

    renderingScene->directionLine->node->setVisible(false);
    renderingScene->lightCircles->node->setVisible(false);
    
    setControlsVisibility(false);
    bool indState = renderingScene->directionIndicator->node->getVisible();
    renderingScene->directionIndicator->node->setVisible(false);
    ShaderManager::isRenderingDepthPass = true;
    Vector3 posn = renderingScene->lightCamera->getAbsolutePosition();
    
    smgr->setActiveCamera(renderingScene->lightCamera);
    smgr->setRenderTarget(renderingScene->shaderMGR->shadowTexture,true,true,true,Vector4(255,255,255,255));
    setJointSpheresVisibility(false); //hide joints
    renderingScene->nodes[NODE_CAMERA]->node->setVisible(false);//hide camera
    renderingScene->nodes[NODE_LIGHT]->node->setVisible(false);//hide light
    vector<std::string> previousMaterialNames;
    for(unsigned long i = 2;i < renderingScene->nodes.size();i++){ // set shadow 1st pass shaders
        SGNode* sgNode = renderingScene->nodes[i];
        if(!sgNode->getProperty(VISIBILITY).value.x || !sgNode->getProperty(LIGHTING).value.x)
            sgNode->node->setVisible(false);
        
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT || sgNode->getType() == NODE_PARTICLES)
            sgNode->node->setVisible(false);
        previousMaterialNames.push_back(sgNode->node->material->name);
        
        if(sgNode->node->skinType == CPU_SKIN) {
            sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
        } else {
            if(sgNode->getType() == NODE_RIG)
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_SKIN));
            else if (sgNode->getType() == NODE_TEXT_SKIN)
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_TEXT));
            else
                sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
            
        }
    }

    smgr->Render(false);
    setJointSpheresVisibility(true); // Unhide joints
    renderingScene->directionIndicator->node->setVisible(indState);
    renderingScene->nodes[NODE_CAMERA]->node->setVisible(true);// Unhide camera
    renderingScene->nodes[NODE_LIGHT]->node->setVisible(true);// Unhide light
    for(unsigned long i = 2;i < renderingScene->nodes.size();i++){// set previous shaders
        SGNode* sgNode = renderingScene->nodes[i];
        if(!sgNode->getProperty(VISIBILITY).value.x || !sgNode->getProperty(LIGHTING).value.x)
            sgNode->node->setVisible(true);
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT || sgNode->getType() == NODE_PARTICLES)
            sgNode->node->setVisible(true);
        sgNode->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i - 2]));
    }
    
    smgr->setActiveCamera(renderingScene->viewCamera);
    smgr->setRenderTarget(NULL,true,true,true,Vector4(255,255,255,255));
    if(renderingScene->rotationCircle && renderingScene->rotationCircle->node)
        renderingScene->rotationCircle->node->setVisible(isRotationCircleVisible);
    renderingScene->greenGrid->node->setVisible(true);
    renderingScene->blueGrid->node->setVisible(true);
    renderingScene->redGrid->node->setVisible(true);
    renderingScene->directionLine->node->setVisible(isDirectionLineVisible);
    renderingScene->lightCircles->node->setVisible(lightCirclesVisible);
    ShaderManager::isRenderingDepthPass = false;
}

bool RenderHelper::rttControlSelectionAnim(Vector2 touchPosition)
{
    if(!renderingScene || !smgr)
        return false;

    if(renderingScene->shaderMGR->deviceType == METAL){
        bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return false;
    }
    int controlStartIndex = (renderingScene->controlType == MOVE) ? X_MOVE : (renderingScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndIndex = (renderingScene->controlType == MOVE) ? Z_MOVE : (renderingScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    renderingScene->rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    renderingScene->updater->updateControlsOrientaion(true);
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        renderingScene->sceneControls[i]->addOrUpdateProperty(VERTEX_COLOR, Vector4(i/255.0, 1.0, 1.0, 1.0), MATERIAL_PROPS);
        renderingScene->sceneControls[i]->getProperty(TRANSPARENCY).value.x = 1.0;
        Vector3 ctrlToCam = (renderingScene->viewCamera->getPosition() - renderingScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        int nodeIndex = smgr->getNodeIndexByID(renderingScene->sceneControls[i]->node->getID());
        if(angle < 0.9)
            smgr->RenderNode(true, nodeIndex);
    }
    bool status = true;
    if(renderingScene->shaderMGR->deviceType == OPENGLES2)
        status = renderingScene->selectMan->getCtrlColorFromTouchTextureAnim(touchPosition);
    smgr->setRenderTarget(NULL,false,false);
    for(int i = controlStartIndex;i <= controlEndIndex;i++) {
        renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    }
    renderingScene->updater->updateControlsOrientaion();
    if(renderingScene->shaderMGR->deviceType == METAL)
        smgr->EndDisplay();
    return status;
}

void RenderHelper::AttachSkeletonModeRTTSelection(Vector2 touchPosition)
{
    if(!renderingScene || !smgr || !renderingScene->isRigMode)
        return;

    renderingScene->rigMan->touchPosForSkeletonSelection = touchPosition;
    setControlsVisibility(false);
    renderingScene->rotationCircle->node->setVisible(false);
    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth,SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    std::map<int, RigKey>& rigKeys = renderingScene->rigMan->rigKeys;
    //vector<Vector3> scaleValues;
    for(int i = 0; i < renderingScene->tPoseJoints.size(); i++){
        if(rigKeys[renderingScene->tPoseJoints[i].id].parentId > 0){
            Vector4 vertColor = rigKeys[renderingScene->tPoseJoints[i].id].bone->getProperty(VERTEX_COLOR).value;
            float transparency = rigKeys[renderingScene->tPoseJoints[i].id].bone->getProperty(TRANSPARENCY).value.x;
            Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
            rigKeys[renderingScene->tPoseJoints[i].id].bone->node->setMaterial(mat);
            rigKeys[renderingScene->tPoseJoints[i].id].bone->addOrUpdateProperty(VERTEX_COLOR, Vector4(i/255.0, 255/255.0, 255/255.0, 1.0), MATERIAL_PROPS);
            rigKeys[renderingScene->tPoseJoints[i].id].bone->getProperty(TRANSPARENCY).value.x = 1.0;
            int nodeId = smgr->getNodeIndexByID(rigKeys[renderingScene->tPoseJoints[i].id].bone->node->getID());
            smgr->RenderNode(true, nodeId);
            rigKeys[renderingScene->tPoseJoints[i].id].bone->getProperty(TRANSPARENCY).value.x = transparency;
            rigKeys[renderingScene->tPoseJoints[i].id].bone->addOrUpdateProperty(VERTEX_COLOR, vertColor, MATERIAL_PROPS);
        }
        
        if(renderingScene->tPoseJoints[i].id != 0){
            Vector4 vertColor = rigKeys[renderingScene->tPoseJoints[i].id].sphere->getProperty(VERTEX_COLOR).value;
            float transparency = rigKeys[renderingScene->tPoseJoints[i].id].sphere->getProperty(TRANSPARENCY).value.x;
            int nodeId = smgr->getNodeIndexByID(rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->getID());
            //scaleValues.push_back(rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->getScale());
            //rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->setScale(rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->getScale() * 1.3);
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->addOrUpdateProperty(VERTEX_COLOR, Vector4(i/255.0, 255/255.0, 255/255.0, 1.0), MATERIAL_PROPS);
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->getProperty(TRANSPARENCY).value.x = 1.0;
            smgr->RenderNode(true, nodeId);
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->getProperty(TRANSPARENCY).value.x = transparency;
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->addOrUpdateProperty(VERTEX_COLOR, vertColor, MATERIAL_PROPS);
        }
    }
    /*
    for(int i = 0; i < renderingScene->tPoseJoints.size(); i++) {
        if(renderingScene->tPoseJoints[i].id != 0)
            rigKeys[renderingScene->tPoseJoints[i].id].sphere->node->setScale(scaleValues[i]);
    }
    */
    // Draw Joints
    if(renderingScene->shaderMGR->deviceType == OPENGLES2)
        renderingScene->selectMan->readSkeletonSelectionTexture();
    smgr->setRenderTarget(NULL,false,false);
    smgr->EndDisplay();
}

void RenderHelper::rttSGRNodeJointSelection(Vector2 touchPosition)
{
    if(!renderingScene || !smgr || !renderingScene->isRigMode || renderingScene->rigMan->sceneMode != RIG_MODE_PREVIEW)
        return;

    renderingScene->rigMan->touchPosForSkeletonSelection = touchPosition;
    SGNode* sgrSGNode = renderingScene->rigMan->getRiggedNode();
    if(sgrSGNode == NULL)
        return;
    shared_ptr<AnimatedMeshNode> animNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node));
    setControlsVisibility(false);
    renderingScene->rotationCircle->node->setVisible(false);
    //render SGR
    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    sgrSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    Vector4 vertexColor = sgrSGNode->getProperty(VERTEX_COLOR).value;
    sgrSGNode->getProperty(TRANSPARENCY).value.x = 1.0;
    sgrSGNode->addOrUpdateProperty(VERTEX_COLOR, Vector4(0.0, 1.0, 1.0, 1.0), MATERIAL_PROPS);
    
    smgr->RenderNode(true, smgr->getNodeIndexByID(sgrSGNode->node->getID()));
    
    sgrSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_SKIN));
    sgrSGNode->getProperty(TRANSPARENCY).value.x = 1.0;
    sgrSGNode->addOrUpdateProperty(VERTEX_COLOR, vertexColor, MATERIAL_PROPS);
    
    // render Joints
    if(renderingScene->rigMan->isNodeSelected){
        if(!renderingScene->rigMan->isSGRJointSelected)
            setJointSpheresVisibility(true);
        for(int i = 1;i < animNode->getJointCount();i++){
            SGNode* jointSphere = renderingScene->jointSpheres[i];
            Vector4 vertexColors = jointSphere->getProperty(VERTEX_COLOR).value;
            float transparency = jointSphere->getProperty(TRANSPARENCY).value.x;
            jointSphere->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            jointSphere->addOrUpdateProperty(VERTEX_COLOR, Vector4(0.0, i/255.0, 1.0, 1.0), MATERIAL_PROPS);
            jointSphere->getProperty(TRANSPARENCY).value.x = 1.0;
            smgr->RenderNode(true, smgr->getNodeIndexByID(jointSphere->node->getID()),(i == 1) ? true:false);
            jointSphere->addOrUpdateProperty(VERTEX_COLOR, vertexColors, MATERIAL_PROPS);
            jointSphere->getProperty(TRANSPARENCY).value.x = transparency;
        }
        if(!renderingScene->rigMan->isSGRJointSelected)
            setJointSpheresVisibility(false);
    }
    if(renderingScene->shaderMGR->deviceType == OPENGLES2)
        renderingScene->selectMan->readSGRSelectionTexture();
    smgr->setRenderTarget(NULL,false,false);
    smgr->EndDisplay();
    //animNode.reset();
}
