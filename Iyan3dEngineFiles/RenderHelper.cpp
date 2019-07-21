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

#define JOINT_MARKED_DISTANCE_FROM_CAMERA 6.17
#define JOINT_MARKED_SCALE 0.27


#include "HeaderFiles/RenderHelper.h"
#include "HeaderFiles/SGEditorScene.h"
#include "SceneImporter.h"

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
        
        smgr->draw3DLines(vPosBB, Vector3(1.0, 1.0, 0.0), mat, SHADER_COLOR_mvp, SHADER_COLOR_vertexColor, SHADER_COLOR_transparency);
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
        smgr->draw3DLine(nodePos, nodePos - lineVect, Vector3(0.0,1.0,0.0), mat, SHADER_COLOR_mvp, SHADER_COLOR_vertexColor, SHADER_COLOR_transparency);
        
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
            Vector3 vertexPos1 = nodePos + (dir * Vector3((circleAxis.x * radius), (circleAxis.y * radius), (circleAxis.z * radius)));
            Vector3 vertexPos2 = nodePos + (dir * Vector3((circleAxis.x * radius - lineLenght), (circleAxis.y * radius - lineLenght), (circleAxis.z * radius - lineLenght)));
            smgr->draw3DLine(vertexPos1, vertexPos2, Vector3(0.0,1.0,0.0), mat, SHADER_COLOR_mvp, SHADER_COLOR_vertexColor, SHADER_COLOR_transparency);
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
    
    for(int i = controlStartIndex; i <= controlEndIndex; i++) {
        Vector3 ctrlToCam = (renderingScene->viewCamera->getPosition() - renderingScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        float ctrlTransparency = (angle > 0.9) ? (0.95 - angle) * 20.0 : 1.0;
        ctrlTransparency = (ctrlTransparency < 0.0) ? 0.0 : ctrlTransparency;
        renderingScene->sceneControls[i]->getProperty(TRANSPARENCY).value.x = ctrlTransparency;
        
        if(renderingScene->selectedControlId == NOT_SELECTED) {
            renderingScene->sceneControls[i]->getProperty(VERTEX_COLOR).value = Vector4(Vector3(-1.0), 0.0);
            renderingScene->sceneControls[i]->getProperty(VISIBILITY).value.x = true;
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
        } else if(i == renderingScene->selectedControlId) {
            renderingScene->sceneControls[i]->getProperty(VISIBILITY).value.x = true;
            renderingScene->sceneControls[i]->getProperty(VERTEX_COLOR).value = Vector4(SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B, 1.0);
            renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        } else {
            renderingScene->sceneControls[i]->getProperty(VISIBILITY).value.x = false;
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
        
        if(renderingScene->whiteBorderTexture)
            smgr->draw2DImage(renderingScene->whiteBorderTexture, Vector2(previewLayout.x, previewLayout.y),Vector2(previewLayout.z, previewLayout.w), smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
        
        smgr->draw2DImage(renderingScene->previewTexture,Vector2(previewLayout.x + 2.0, previewLayout.y + 2.0),Vector2(previewLayout.z - 2.0, previewLayout.w - 2.0), smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE),true);
    }
}

void RenderHelper::rttDrawCall()
{
    if (!renderingScene)
        return;
    
    if (renderingScene->selectedNodeId == NODE_CAMERA || renderingScene->isPlaying) {
        drawCameraPreview();
    }
    
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
    bool dirLineVisible =     renderingScene->directionLine->node->getVisible();
    renderingScene->directionLine->node->setVisible(false);
    bool lightCircleVisible = renderingScene->lightCircles->node->getVisible();
    renderingScene->lightCircles->node->setVisible(false);
    
    smgr->setRenderTarget(renderingScene->previewTexture, true, true, false, Vector4(0.1, 0.1, 0.1, 1.0));
    Vector4 camprevlay = renderingScene->getCameraPreviewLayout();
    std::map< int, Vector3 > nPositions;
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++){
        if(renderingScene->nodes[i]->getType() <= NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
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
    
    //    if(renderingScene->whiteBorderTexture)
    //        smgr->draw2DImage(renderingScene->whiteBorderTexture, Vector2(0, 0), Vector2(SceneHelper::screenWidth, SceneHelper::screenHeight), smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->Render(true);
    
    smgr->setRenderTarget(NULL, false, false);
    smgr->setActiveCamera(renderingScene->viewCamera);
    
    for(unsigned long i = 1; i < renderingScene->nodes.size(); i++) {
        if(renderingScene->nodes[i]->getType() <= NODE_LIGHT || renderingScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
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
    renderingScene->directionLine->node->setVisible(dirLineVisible);
    renderingScene->lightCircles->node->setVisible(lightCircleVisible);
    
}

void RenderHelper::setRenderCameraOrientation()
{
    if(!renderingScene || !smgr || renderingScene->nodes.size() <= NODE_CAMERA)
        return;
    renderingScene->renderCamera->setPosition(renderingScene->nodes[NODE_CAMERA]->node->getPosition());
    renderingScene->renderCamera->updateAbsoluteTransformation();
    Quaternion rot  = renderingScene->nodes[NODE_CAMERA]->node->getRotation();
    renderingScene->renderCamera->setRotation(rot);
    renderingScene->renderCamera->updateAbsoluteTransformation();
    
    Vector3 upReal = Vector3(0.0, 1.0, 0.0);
    Mat4 rotmat;
    rotmat.setRotation(rot);
    rotmat.rotateVect(upReal);
    renderingScene->renderCamera->setUpVector(upReal);
    
    int rT = renderingScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    float texWidth = (float)renderingScene->renderingTextureMap[RESOLUTION[rT][0]]->width;
    float texHeight = renderingScene->renderingTextureMap[RESOLUTION[rT][0]]->height;
    float aspectRatio = texWidth/ texHeight;
    renderingScene->renderCamera->setAspectRatio(aspectRatio);
    smgr->setActiveCamera(renderingScene->renderCamera);
}

void RenderHelper::rttNodeJointSelection(Vector2 touchPosition, bool isMultiSelectenabled, bool touchMove)
{
    if(!renderingScene || !smgr)
        return;
    
    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    
    renderingScene->nodeJointPickerPosition = touchPosition;
    setControlsVisibility(false);
    renderingScene->rotationCircle->node->setVisible(false);
    SGNode *selectedSGNode = (renderingScene->isNodeSelected) ? renderingScene->nodes[renderingScene->selectedNodeId] : NULL;
    if(!selectedSGNode || (selectedSGNode->getType() != NODE_RIG && selectedSGNode->getType() != NODE_TEXT_SKIN))
        setJointSpheresVisibility(false);
    
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    vector< vector<Vector4> > vertexColors;
    vector<float> transparency;
    vector<std::string> previousMaterialNames;
    vector<bool> nodesVisibility;
    vector<bool> nodeSelection;
    vector< vector<bool> > bufferSelection;
    vector< vector<bool> > isVcolored;
    vector< vector<float> > reflections;
    vector<bool> isLighting;
    
    for(int i = 0; i < renderingScene->nodes.size(); i++){
        
        nodeSelection.push_back(renderingScene->nodes[i]->getProperty(SELECTED).value.x);
        nodesVisibility.push_back(renderingScene->nodes[i]->getProperty(VISIBILITY).value.x);
        isLighting.push_back(renderingScene->nodes[i]->getProperty(LIGHTING).value.x);
        transparency.push_back(renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x);
        previousMaterialNames.push_back(renderingScene->nodes[i]->node->material->name);
        
        renderingScene->nodes[i]->getProperty(SELECTED).value.x = false;
        
        renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x = 1.0;
        
        renderingScene->nodes[i]->getProperty(VISIBILITY).value.x = renderingScene->nodes[i]->isTempNode ? false : true;
        
        if(renderingScene->nodes[i]->getType() == NODE_RIG || renderingScene->nodes[i]->getType() == NODE_TEXT_SKIN)
            renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(renderingScene->nodes[i]->node->skinType == CPU_SKIN ? SHADER_COLOR : SHADER_COLOR_SKIN));
        else {
            if(renderingScene->nodes[i]->getType() == NODE_CAMERA)
                renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
            renderingScene->nodes[i]->getProperty(LIGHTING).value.x = false;
        }
        
        vector<Vector4> mbVColors;
        vector<bool> mbIsVColored;
        vector<bool> mbIsSelected;
        vector<float> mbReflections;
        
        for(int j = 0; j < renderingScene->nodes[i]->materialProps.size(); j ++) {
            mbVColors.push_back(renderingScene->nodes[i]->getProperty(VERTEX_COLOR, j).value);
            mbIsSelected.push_back(renderingScene->nodes[i]->getProperty(SELECTED, j).value.x);
            mbIsVColored.push_back(renderingScene->nodes[i]->getProperty(IS_VERTEX_COLOR, j).value.x);
            mbReflections.push_back(renderingScene->nodes[i]->getProperty(REFLECTION, j).value.x);
            
            renderingScene->nodes[i]->getProperty(REFLECTION, j).value.x = 0.0;
            renderingScene->nodes[i]->getProperty(SELECTED, j).value.x = false;
            renderingScene->nodes[i]->getProperty(IS_VERTEX_COLOR, j).value.x = true;
            
            Vector3 packed = MathHelper::packInterger(i);
            renderingScene->nodes[i]->getProperty(VERTEX_COLOR, j).value = Vector4(packed/255.0 ,1.0);
        }
        
        reflections.push_back(mbReflections);
        vertexColors.push_back(mbVColors);
        isVcolored.push_back(mbIsVColored);
        bufferSelection.push_back(mbIsSelected);
    }
    
    smgr->Render(true);
    bool drawMeshBufferRTT = false;
    if((selectedSGNode && renderingScene->selectedNodeIds.size() <= 0 && !touchMove) || renderingScene->isJointSelected) {
        if(renderingScene->selectedMeshBufferId == NOT_SELECTED) {
            drawJointsSpheresForRTT(true);
        }
        drawMeshBufferRTT = true;
    }
    
    for (int i = 0; i < renderingScene->nodes.size(); i++) {
        renderingScene->nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i]));
        renderingScene->nodes[i]->getProperty(TRANSPARENCY).value.x = transparency[i];
        renderingScene->nodes[i]->getProperty(VISIBILITY).value.x = nodesVisibility[i];
        renderingScene->nodes[i]->getProperty(SELECTED).value.x = nodeSelection[i];
        renderingScene->nodes[i]->getProperty(LIGHTING).value.x = isLighting[i];
        
        for( int j = 0; j < renderingScene->nodes[i]->materialProps.size(); j ++) {
            renderingScene->nodes[i]->getProperty(SELECTED, j).value.x = bufferSelection[i][j];
            renderingScene->nodes[i]->getProperty(VERTEX_COLOR, j).value = vertexColors[i][j];
            renderingScene->nodes[i]->getProperty(IS_VERTEX_COLOR, j).value.x = isVcolored[i][j];
            renderingScene->nodes[i]->getProperty(REFLECTION, j).value.x = reflections[i][j];
        }
        reflections[i].clear();
        bufferSelection[i].clear();
        vertexColors[i].clear();
        isVcolored[i].clear();
    }
    previousMaterialNames.clear(); vertexColors.clear(); transparency.clear();
    isVcolored.clear(); bufferSelection.clear(); reflections.clear();
    smgr->setRenderTarget(NULL,false,false);
    smgr->EndDisplay();
}

void RenderHelper::drawMeshBuffersForRTT()
{
    if(renderingScene->selectedNodeId != NOT_SELECTED) {
        smgr->EndDisplay();
        
        bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return;
        
        smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
        
        SGNode *sgNode = renderingScene->selectedNode;
        int totalMeshBufferIndex = sgNode->materialProps.size();
        vector<Vector4> vertexColors;
        vector<float> isMeshColored;
        vector<bool> mBSelected;
        vector<float> transparancies;
        vector<float> reflections;
        
        
        bool isLighting = sgNode->getProperty(LIGHTING).value.x;
        bool isNodeSelected = sgNode->getProperty(SELECTED).value.x;
        float nodeTrans = sgNode->getProperty(TRANSPARENCY).value.x;
        sgNode->getProperty(SELECTED).value.x = false;
        sgNode->getProperty(TRANSPARENCY).value.x = 1.0;
        sgNode->getProperty(LIGHTING).value.x = false;
        
        for(int i = 0; i < sgNode->materialProps.size(); i++) {
            vertexColors.push_back(sgNode->getProperty(VERTEX_COLOR, i).value);
            isMeshColored.push_back(sgNode->getProperty(IS_VERTEX_COLOR, i).value.x);
            mBSelected.push_back(sgNode->getProperty(SELECTED, i).value.x);
            transparancies.push_back(sgNode->getProperty(TRANSPARENCY, i).value.x);
            reflections.push_back(sgNode->getProperty(REFLECTION, i).value.x);
            
            sgNode->getProperty(REFLECTION, i).value.x = 0.0;
            sgNode->getProperty(TRANSPARENCY, i).value.x = 1.0;
            sgNode->getProperty(SELECTED, i).value.x = false;
            sgNode->getProperty(IS_VERTEX_COLOR, i).value.x = true;
            Vector3 packed = MathHelper::packInterger(renderingScene->selectedNodeId);
            packed.z = i;
            sgNode->getProperty(VERTEX_COLOR, i).value = Vector4( packed/255.0, 1.0 );
        }
        
        smgr->Render(true);
        
        sgNode->getProperty(SELECTED).value.x = isNodeSelected;
        sgNode->getProperty(TRANSPARENCY).value.x = nodeTrans;
        sgNode->getProperty(LIGHTING).value.x = isLighting;
        
        for(int i = 0; i < sgNode->materialProps.size(); i++) {
            sgNode->getProperty(SELECTED, i).value.x = mBSelected[i];
            sgNode->getProperty(IS_VERTEX_COLOR, i).value.x = isMeshColored[i];
            sgNode->getProperty(VERTEX_COLOR, i).value = vertexColors[i];
            sgNode->getProperty(TRANSPARENCY, i).value = transparancies[i];
            sgNode->getProperty(REFLECTION, i).value.x = reflections[i];
        }
        vertexColors.clear();
        isMeshColored.clear();
        transparancies.clear();
        reflections.clear();
        mBSelected.clear();
        
        smgr->EndDisplay();
        
        renderingScene->selectMan->getNodeColorFromTouchTexture(false, false);
    }
}

void RenderHelper::drawJointsSpheresForRTT(bool enableDepthTest)
{
    if(!renderingScene || !smgr)
        return;
    
    smgr->clearDepthBuffer();
    ShaderManager::isRendering = true;
    SGNode *sgNode = renderingScene->selectedNode;
    setJointSpheresVisibility(true);
    // Draw Joints above nodes if nodeSelected
    int totalMeshBuffers = sgNode->materialProps.size();
    
    if(renderingScene->jointSpheres.size() <= 0 || (sgNode->getType() != NODE_RIG && sgNode->getType() != NODE_TEXT_SKIN))
        return;
    vector<Vector4> vertexColors;
    vector<Vector3> scaleValues;
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        vertexColors.push_back(renderingScene->jointSpheres[j]->getProperty(VERTEX_COLOR).value);
        scaleValues.push_back(renderingScene->jointSpheres[j]->node->getScale());
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j] * 1.3);
        Vector3 packed = MathHelper::packInterger(renderingScene->selectedNodeId);
        packed.z = j + totalMeshBuffers;
        renderingScene->jointSpheres[j]->getProperty(IS_VERTEX_COLOR).value.x = true;
        renderingScene->jointSpheres[j]->getProperty(VERTEX_COLOR).value = Vector4( packed/255.0, 1.0 );
        smgr->RenderNode(true, smgr->getNodeIndexByID(renderingScene->jointSpheres[j]->node->getID()), (j == 0) ? enableDepthTest:false);
    }
    // Reset joints
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        renderingScene->jointSpheres[j]->getProperty(VERTEX_COLOR).value = vertexColors[j];
        renderingScene->jointSpheres[j]->setScaleOnNode(scaleValues[j]);
    }
    ShaderManager::isRendering = false;
    vertexColors.clear();
    scaleValues.clear();
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
    
    for(int i = 1; i < renderingScene->jointSpheres.size(); i++) {
        if(isNodeSelected && selectedNode && (selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT_SKIN) && i < selectedNode->joints.size())
            renderingScene->jointSpheres[i]->node->setVisible(visibilityFlag);
        else if (renderingScene->selectedNodeIds.size() > 0)
            renderingScene->jointSpheres[i]->node->setVisible(false);
        else
            renderingScene->jointSpheres[i]->node->setVisible(false);
    }
}

void RenderHelper::renderAndSaveImage(char *imagePath, bool isDisplayPrepared, int frame, Vector4 bgColor)
{
    if(!renderingScene || !smgr)
        return;
    
    if(!renderingScene->checkNodeSize())
        return;
    
    isExporting1stTime = false;
    
    bool isLightOn = ShaderManager::sceneLighting;
    bool isShadowsOff = ShaderManager::shadowsOff;
    ShaderManager::shadowsOff = false;
    
    if(!isLightOn)
        renderingScene->setLightingOn();
    
    int rT = renderingScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    bool displayPrepared = smgr->PrepareDisplay(renderingScene->renderingTextureMap[RESOLUTION[rT][0]]->width, renderingScene->renderingTextureMap[RESOLUTION[rT][0]]->height,false,true,false,Vector4(bgColor));
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
    int selectedMeshBufferId = NOT_SELECTED;
    if(renderingScene->selectedNodeId != NOT_SELECTED) {
        selectedObjectId = renderingScene->selectedNodeId;
        selectedMeshBufferId = renderingScene->selectedMeshBufferId;
        renderingScene->selectMan->unselectObject(renderingScene->selectedNodeId);
    }
    
    vector<string> previousMaterialNames;
    
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
    
    smgr->setRenderTarget(renderingScene->renderingTextureMap[RESOLUTION[rT][0]], true, true, false, Vector4(bgColor));
    
    smgr->Render(false);
    
    int totalImgs = 118;
    int index = 0;
    if(frame > -1) {
        int divisor = (frame > totalImgs) ? frame/totalImgs : 1;
        index = (frame > totalImgs) ? frame - (divisor * totalImgs) : frame;
    }
    
    rttShadowMap();
    
    smgr->EndDisplay();
    smgr->writeImageToFile(renderingScene->renderingTextureMap[RESOLUTION[rT][0]], imagePath, NO_FLIP);
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
    
    if(selectedObjectId != NOT_SELECTED)
        renderingScene->selectMan->selectObject(selectedObjectId, selectedMeshBufferId, false);
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
    
    renderingScene->updater->updateJointSpheresPosition();
    
    for(int i = 0;i < bonesCount;i++){
        
        Vector3 pos = renderingScene->jointSpheres[i]->node->getPosition();
        float distanceFromCamera = pos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
        float jointScale = ((distanceFromCamera / JOINT_MARKED_DISTANCE_FROM_CAMERA) * JOINT_MARKED_SCALE);
        jointScale = jointScale * 1.5;
        Vector3 sphereScale = Vector3(jointScale);
        
        renderingScene->jointSpheres[i]->node->setScale(sphereScale);
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
        
        SceneImporter *importer = new SceneImporter();
        Mesh *jointSphereMesh = importer->loadMeshFromFile(constants::BundlePath + "/sphere.sgm");
        delete importer;
        
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
        sgNode->getProperty(VERTEX_COLOR).value = Vector4(constants::sgrJointDefaultColor, 1.0);
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
        int selectedJointId = renderingScene->selectedJointId;

        if(i == selectedJointId){
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
        
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT)
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
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT)
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
    
    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return false;

    int controlStartIndex = (renderingScene->controlType == MOVE) ? X_MOVE : (renderingScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndIndex = (renderingScene->controlType == MOVE) ? Z_MOVE : (renderingScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    renderingScene->rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(renderingScene->touchTexture,true,true,false,Vector4(255,255,255,255));
    renderingScene->updater->updateControlsOrientaion(true);
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        renderingScene->sceneControls[i]->getProperty(VERTEX_COLOR).value = Vector4(i/255.0, 1.0, 1.0, 1.0);
        renderingScene->sceneControls[i]->getProperty(TRANSPARENCY).value.x = 1.0;
        Vector3 ctrlToCam = (renderingScene->viewCamera->getPosition() - renderingScene->sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(SceneHelper::controlDirection(i%3)));
        int nodeIndex = smgr->getNodeIndexByID(renderingScene->sceneControls[i]->node->getID());
        if(angle < 0.9)
            smgr->RenderNode(true, nodeIndex);
    }
    bool status = true;
    smgr->setRenderTarget(NULL,false,false);
    for(int i = controlStartIndex;i <= controlEndIndex;i++) {
        renderingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
    }
    renderingScene->updater->updateControlsOrientaion();
    smgr->EndDisplay();
    return status;
}
