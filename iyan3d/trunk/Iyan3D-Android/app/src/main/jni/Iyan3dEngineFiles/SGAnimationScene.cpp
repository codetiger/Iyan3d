#include "HeaderFiles/SGAnimationScene.h"
#include "../SGEngine2/Loaders/OBJMeshFileLoader.h"
#include "unistd.h"
#include "HeaderFiles/SGCloudRenderingHelper.h"

#ifdef ANDROID
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm/usr/include/android/log.h"
#include "../opengl.h"
#endif

string constants::BundlePath = " ";
string constants::CachesStoragePath = " ";
string constants::DocumentsStoragePath="";
string constants::impotedImagesPathAndroid="";
SGAction changeKeysAction, scaleAction, propertyAction;
bool shadowMapAvailable;
SGAnimationScene::SGAnimationScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight):SGScene(device,smgr,screenWidth,screenHeight)
{

    isExporting1stTime = true;
    viewType = ANIMATION;
    initControls();
    nodes.clear();
    this->smgr = smgr;
    //string textureFileName = constants::BundlePath + "/" + "grandfather.png";
    cameraResolutionType = assetIDCounter = currentFrame = currentAction = actionObjectsSize = previousFrame = 0;
    selectedNodeId = NOT_EXISTS;
    selectedJointId = NOT_EXISTS;
    totalFrames = 24;
    cameraFOV = 72.0;
    actions.clear();
    isPlaying = isMoved = exportImage = pauseRendering = false;
    renderingType = SHADER_COMMON_L1;
    shadowMapAvailable = isRTTCompleted = isExportingImages = false;
    Logger::log(INFO , "SG ANimation scene" , "Init Textures");
    initTextures();
    initRenderCamera();
    isPremiumUnlocked = false;

    propertyAction.drop();
    changeKeysAction.drop();
    scaleAction.drop();
    Logger::log(INFO , "SG Animation Scene" , "Constructor exit");
    resetMaterialTypes(false);
}
SGAnimationScene::~SGAnimationScene()
{    
    
    ShaderManager::lightPosition.clear();
    ShaderManager::lightColor.clear();
    ShaderManager::lightFadeDistances.clear();
    
    scaleAction.drop();
    for(int i = 0;i < nodes.size();i++){
        if(nodes[i])
            delete nodes[i];
    }
    nodes.clear();
    
    if(objNode)
        objNode.reset();
    if (renderCamera)
        renderCamera.reset();
    
    if(ikJointsPositionMap.size())
        ikJointsPositionMap.clear();
    if(smgr)
        delete smgr;
}
void SGAnimationScene::setDocumentsDirectory(string documentsPath)
{
    FileHelper::setDocumentsDirectory(documentsPath);
}
void SGAnimationScene::initTextures()
{

    previewTexture = smgr->createRenderTargetTexture("previewTexture", TEXTURE_RGBA8, TEXTURE_BYTE, PREVIEW_TEXTURE_WIDTH, PREVIEW_TEXTURE_HEIGHT);
    thumbnailTexture = smgr->createRenderTargetTexture("thumbnailTexture", TEXTURE_RGBA8, TEXTURE_BYTE, THUMBNAIL_TEXTURE_WIDTH, THUMBNAIL_TEXTURE_HEIGHT);
    shadowTexture = smgr->createRenderTargetTexture("shadowTexture", TEXTURE_DEPTH32, TEXTURE_BYTE, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);
    whiteBorderTexture = smgr->loadTexture("whiteborder",constants::BundlePath + "/whiteborder.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    
    renderingTextureMap[RESOLUTION[0][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[0][0] , RESOLUTION[0][1]);
    renderingTextureMap[RESOLUTION[1][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[1][0] , RESOLUTION[1][1]);
    renderingTextureMap[RESOLUTION[2][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[2][0] , RESOLUTION[2][1]);
}
bool SGAnimationScene::checkNodeSize(){
    if(nodes.size() < NODE_LIGHT + 1)
        return false;
    return true;
}
void SGAnimationScene::renderAll()
{
    if(!pauseRendering) {
        if(!checkNodeSize())
            return;
        bool displayPrepared = smgr->PrepareDisplay(screenWidth, screenHeight, true, true, false,
                                                    Vector4(0, 0, 0, 255));
        if(!displayPrepared)
            return;
        
        setControlsVisibility(false);
        rotationCircle->node->setVisible(false);
        smgr->draw2DImage(bgTexture, Vector2(0, 0), Vector2(screenWidth, screenHeight), true,
                          smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
        smgr->Render();
        
        drawGrid();
        drawCircle();
        DrawMoveAxisLine();
        renderControls();
        // rtt division atlast post and pre stage
        postRTTDrawCall();
        rttDrawCall();
        smgr->EndDisplay(); // draws all the rendering command
        
        if(fabs(xAcceleration) > 0.0 || fabs(yAcceleration) > 0.0) {
            swipeToRotate();
            updateLightCamera();
        }
        
        if(selectedNodeId != NOT_SELECTED)
            setTransparencyForIntrudingObjects();
    }
}
void SGAnimationScene::rttShadowMap()
{
    if(!checkNodeSize())
        return;
    bool isRotationCircleVisible = rotationCircle->node->getVisible();
    rotationCircle->node->setVisible(false);
    setControlsVisibility(false);
    ShaderManager::isRenderingDepthPass = true;
    smgr->setActiveCamera(lightCamera);
    smgr->setRenderTarget(shadowTexture,true,true,true,Vector4(255,255,255,255));
    setJointSpheresVisibility(false); //hide joints
    nodes[NODE_CAMERA]->node->setVisible(false);//hide camera
    nodes[NODE_LIGHT]->node->setVisible(false);//hide light
    vector<std::string> previousMaterialNames;
    for(unsigned long i = 2;i < nodes.size();i++){ // set shadow 1st pass shaders
        if(!nodes[i]->props.isVisible || !nodes[i]->props.isLighting)
            nodes[i]->node->setVisible(false);
        
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(false);
        previousMaterialNames.push_back(nodes[i]->node->material->name);
        
        if(nodes[i]->node->skinType == CPU_SKIN) {
            nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
        } else {
            if(nodes[i]->getType() == NODE_RIG)
                nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_SKIN));
            else if (nodes[i]->getType() == NODE_TEXT)
                nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS_TEXT));
            else
                nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_SHADOW_DEPTH_PASS));
            
        }
            }
    smgr->Render();
    setJointSpheresVisibility(true); // Unhide joints
    nodes[NODE_CAMERA]->node->setVisible(true);// Unhide camera
    nodes[NODE_LIGHT]->node->setVisible(true);// Unhide light
    for(unsigned long i = 2;i < nodes.size();i++){// set previous shaders
        if(!nodes[i]->props.isVisible || !nodes[i]->props.isLighting)
            nodes[i]->node->setVisible(true);
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(true);
        nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i - 2]));
    }
    
    smgr->setActiveCamera(viewCamera);
    smgr->setRenderTarget(NULL,true,true,true,Vector4(255,255,255,255));
    rotationCircle->node->setVisible(isRotationCircleVisible);
    ShaderManager::isRenderingDepthPass = false;
}
void SGAnimationScene::postRTTDrawCall()
{
    if(previewTexture && (selectedNodeId == NODE_CAMERA || isPlaying)) {
        Vector4 previewLayout = getCameraPreviewLayout1();
        smgr->draw2DImage(previewTexture,Vector2(previewLayout.x,previewLayout.y),Vector2(previewLayout.z,previewLayout.w), false, smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE),true);
    }
}
void SGAnimationScene::rttDrawCall()
{
    if(selectedNodeId == NODE_CAMERA || isPlaying)
        drawCameraPreview();
    rttShadowMap();
    shadowMapAvailable = true;
}
void SGAnimationScene::DrawMoveAxisLine(){
    if(controlType == MOVE && selectedControlId != NOT_SELECTED && selectedNodeId != NOT_SELECTED){
        Vector3 nodePos = nodes[selectedNodeId]->node->getAbsolutePosition();
        if(selectedJointId != NOT_SELECTED)
            nodePos = (dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node))->getJointNode(selectedJointId)->getAbsolutePosition();
        drawMoveControlLine(nodePos);
    }
}
void SGAnimationScene::initRenderCamera()
{
    renderCamera = smgr->createCameraNode("RenderingUniforms");
    (renderCamera)->setTargetAndRotationBindStatus(true);
    renderCamera->setFOVInRadians(cameraFOV * PI / 180.0f);
}
void SGAnimationScene::setRenderCameraOrientation()
{
    if(nodes.size() <= NODE_CAMERA)
        return;
    renderCamera->setPosition(nodes[NODE_CAMERA]->node->getPosition());
    renderCamera->updateAbsoluteTransformation();
    Vector3 rot  = nodes[NODE_CAMERA]->node->getRotationInDegrees();
    rot.x += 180.0f;
    renderCamera->setRotation(rot);
    renderCamera->updateAbsoluteTransformation();
    
    Vector3 upReal = Vector3(0.0,1.0,0.0);
    Mat4 rotmat;
    rotmat.setRotationRadians(Vector3(rot.x - 180.0,rot.y,rot.z) * DEGTORAD);
    rotmat.rotateVect(upReal);
    renderCamera->setUpVector(upReal);
    float aspectRatio = (float)renderingTextureMap[RESOLUTION[cameraResolutionType][0]]->width/renderingTextureMap[RESOLUTION[cameraResolutionType][0]]->height;
    renderCamera->setAspectRatio(aspectRatio);
    smgr->setActiveCamera(renderCamera);
}
void SGAnimationScene::drawCameraPreview()
{
    if(!checkNodeSize())
        return;
    setRenderCameraOrientation();
    rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(previewTexture,true,true,false,Vector4(255,255,255,255));
    Vector4 camprevlay = getCameraPreviewLayout1();
    for(unsigned long i = 1; i < nodes.size(); i++){
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(false);
        if(!(nodes[i]->props.isVisible))
            nodes[i]->node->setVisible(false);
    }
    smgr->draw2DImage(bgTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    setControlsVisibility(false);


    smgr->Render();
    if(whiteBorderTexture)
        smgr->draw2DImage(whiteBorderTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    smgr->setRenderTarget(NULL,false,false);
    smgr->setActiveCamera(viewCamera);

    for(unsigned long i = 1; i < nodes.size(); i++){
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(true);

        if(!(nodes[i]->props.isVisible))
            nodes[i]->node->setVisible(true);
    }

}

Vector4 SGAnimationScene::getCameraPreviewLayout1()
{
    float camPrevWidth = (screenWidth) * CAM_PREV_PERCENT;
    float camPrevHeight = (screenHeight) * CAM_PREV_PERCENT;
    float camPrevRatio = RESOLUTION[cameraResolutionType][1] / camPrevHeight;
    
    float originX = screenWidth - camPrevWidth * CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE;
    float originY = screenHeight - camPrevHeight * CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE;
    float endX = originX + RESOLUTION[cameraResolutionType][0] / camPrevRatio;
    float endY = originY + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
    return Vector4(originX,originY,endX,endY);
}
//void SGAnimationScene::shaderCallBackForNode(int nodeID,string matName)
//{
//    for(int i = 0; i < nodes.size();i++){
//        if(nodes[i]->node->getID() == nodeID){
//            shaderMGR->setUniforms(nodes[i],matName);
//            break;
//        }
//    }
//}
//bool SGAnimationScene::isNodeTransparent(int nodeId)
//{
//    if(nodeId == -1)
//        return false;
//    else{
//        for(int i = 0; i < nodes.size();i++){
//            if(nodes[i]->node->getID() == nodeId){
//                return (nodes[i]->props.transparency < 1.0) || nodes[i]->props.isSelected || (!nodes[i]->props.isVisible);
//                break;
//            }
//        }
//    }
//}

SGNode* SGAnimationScene::loadNode(NODE_TYPE type,int assetId,wstring name,int imgwidth,int imgheight,int actionType, Vector4 textColor, string fontFilePath)
{
    pauseRendering = true;
    SGNode *sgnode = new SGNode(type);
    sgnode->node = sgnode->loadNode(assetId,type,smgr,name,imgwidth,imgheight,textColor,fontFilePath);
    if(!sgnode->node){
        delete sgnode;
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        return NULL;
    }
    if(sgnode->getType() == NODE_TEXT)
        textJointsBasePos[(int)nodes.size()] = SGAnimationSceneHelper::storeTextInitialPositions(sgnode);
    sgnode->assetId = assetId;
    sgnode->name = name;
    sgnode->setInitialKeyValues(actionType);
    sgnode->node->updateAbsoluteTransformation();
    sgnode->node->updateAbsoluteTransformationOfChildren();
    if(type == NODE_CAMERA)
        ShaderManager::camPos = sgnode->node->getAbsolutePosition();
    else if (type == NODE_LIGHT){
        initLightCamera(sgnode->node->getPosition());
        addLight(sgnode);
    }else if(type == NODE_IMAGE){
        sgnode->props.isLighting = false;
    } else if (type == NODE_RIG) {
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache(CHARACTER_RIG);
    } else if (type == NODE_TEXT) {
        dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node)->updateMeshCache(TEXT_RIG);
    } else if (type == NODE_ADDITIONAL_LIGHT) {
        addLight(sgnode);
    }
    
    //if (type >= NODE_LIGHT && type != NODE_ADDITIONAL_LIGHT)
        sgnode->node->setTexture(shadowTexture,2);
    
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
        sgnode->actionId = ++actionObjectsSize;
    nodes.push_back(sgnode);

    sgnode->node->setID(assetIDCounter++);
    performUndoRedoOnNodeLoad(sgnode,actionType);
    setDataForFrame(currentFrame);
    resetMaterialTypes(false);
    pauseRendering = false;
    return sgnode;
}

bool SGAnimationScene::loadNode(SGNode *sgNode,int actionType)
{
    Vector4 nodeSpecificColor = Vector4(sgNode->props.vertexColor.x,sgNode->props.vertexColor.y,sgNode->props.vertexColor.z,1.0);
    sgNode->node = sgNode->loadNode(sgNode->assetId,sgNode->getType(),smgr,sgNode->name,sgNode->props.fontSize,sgNode->props.nodeSpecificFloat,nodeSpecificColor,sgNode->optionalFilePath);
    
        if(!sgNode->node){
        Logger::log(INFO,"SGANimationScene","Node not loaded");
        return false;
    }
    if(sgNode->getType() == NODE_TEXT)
        textJointsBasePos[(int)nodes.size()] = SGAnimationSceneHelper::storeTextInitialPositions(sgNode);
    
    sgNode->setInitialKeyValues(actionType);
    sgNode->node->updateAbsoluteTransformation();
    sgNode->node->updateAbsoluteTransformationOfChildren();
    //if(sgNode->getType() >= NODE_LIGHT)
        sgNode->node->setTexture(shadowTexture,2);
    sgNode->node->setVisible(true);
    if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
        sgNode->actionId = ++actionObjectsSize;
    nodes.push_back(sgNode);
    sgNode->node->setID(assetIDCounter++);
    performUndoRedoOnNodeLoad(sgNode,actionType);
    if(sgNode->getType() == NODE_LIGHT) {
        initLightCamera(sgNode->node->getPosition());
        addLight(sgNode);
    } else if(sgNode->getType() == NODE_ADDITIONAL_LIGHT)
        addLight(sgNode);
    else if(sgNode->getType() == NODE_IMAGE && actionType != OPEN_SAVED_FILE && actionType != UNDO_ACTION)
        sgNode->props.isLighting = false;

    setDataForFrame(currentFrame);
    resetMaterialTypes(false);
    
     return true;
}

void SGAnimationScene::performUndoRedoOnNodeLoad(SGNode* meshObject,int actionType){
    if(actionType == UNDO_ACTION) {
        int jointsCnt = (int)meshObject->joints.size();
        SGAction &deleteAction = actions[currentAction-1];
        if(deleteAction.nodePositionKeys.size())
            meshObject->positionKeys = deleteAction.nodePositionKeys;
        if(deleteAction.nodeRotationKeys.size())
            meshObject->rotationKeys = deleteAction.nodeRotationKeys;
        if(deleteAction.nodeSCaleKeys.size())
            meshObject->scaleKeys = deleteAction.nodeSCaleKeys;
        if(deleteAction.nodeVisibilityKeys.size())
            meshObject->visibilityKeys = deleteAction.nodeVisibilityKeys;
        for(int i = 0; i < jointsCnt; i++){
            if(deleteAction.jointRotKeys.find(i) != deleteAction.jointRotKeys.end() && deleteAction.jointRotKeys[i].size()) {
                //meshObject->joints[i]->rotationKeys = deleteAction.jointsRotationKeys[i];
                meshObject->joints[i]->rotationKeys = deleteAction.jointRotKeys[i];
            }
            if(meshObject->getType() == NODE_TEXT) {
                if(deleteAction.jointPosKeys.find(i) != deleteAction.jointPosKeys.end())
                    meshObject->joints[i]->positionKeys = deleteAction.jointPosKeys[i];
                if(deleteAction.jointScaleKeys.find(i) != deleteAction.jointScaleKeys.end())
                    meshObject->joints[i]->scaleKeys = deleteAction.jointScaleKeys[i];
            }
        }
        meshObject->props.prevMatName = ConversionHelper::getStringForWString(deleteAction.actionSpecificStrings[0]);
        meshObject->actionId = actions[currentAction-1].objectIndex;
        currentAction--;
    }
    
    if(actionType == REDO_ACTION) {
        SGAction &deleteAction = actions[currentAction];
        meshObject->props.prevMatName = ConversionHelper::getStringForWString(deleteAction.actionSpecificStrings[0]);
        meshObject->actionId = deleteAction.objectIndex;
        currentAction++;
    }
}
void SGAnimationScene::checkSelection(Vector2 touchPosition,bool isDisplayPrepared)
{
    rttNodeJointSelection(touchPosition);
    if(shaderMGR->deviceType == METAL){
        rttNodeJointSelection(touchPosition);
        getNodeColorFromTouchTexture();
    }
    isRTTCompleted = true;

    postNodeJointSelection();
    setTransparencyForIntrudingObjects();
}
void SGAnimationScene::postNodeJointSelection(){
    if(isJointSelected && selectedJointId <= HIP && selectedNode->joints.size() >= HUMAN_JOINTS_SIZE)
        getIKJointPosition();
    
    if(selectedNodeId > 0) {
        if(nodes[selectedNodeId]->joints.size() != HUMAN_JOINTS_SIZE)
            setMirrorState(MIRROR_OFF);
    }
    
    if(isJointSelected && selectedJointId != NOT_SELECTED)
        highlightJointSpheres();
    
    if(!isNodeSelected || (nodes[selectedNodeId]->getType() != NODE_RIG && nodes[selectedNodeId]->getType() != NODE_TEXT))
        setJointSpheresVisibility(false);
}
void SGAnimationScene::changeObjectScale(Vector3 scale, bool isChanged)
{
    if((isJointSelected && nodes[selectedNodeId]->getType() != NODE_TEXT) || !isNodeSelected) return;
    
    if(scaleAction.actionType == ACTION_EMPTY){
        if(isJointSelected && nodes[selectedNodeId]->getType() == NODE_TEXT) {
            scaleAction.actionType = ACTION_CHANGE_JOINT_KEYS;
            scaleAction.objectIndex = nodes[selectedNodeId]->actionId;
            for (int i = 0; i < nodes[selectedNodeId]->joints.size(); i++)
                scaleAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
        } else {
            scaleAction.actionType = ACTION_CHANGE_NODE_KEYS;
            scaleAction.objectIndex = nodes[selectedNodeId]->actionId;
            scaleAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
        }
    }
    if(isChanged){
        if(isJointSelected && nodes[selectedNodeId]->getType() == NODE_TEXT) {
            for (int i = 0; i < nodes[selectedNodeId]->joints.size(); i++)
                scaleAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
        } else {
            scaleAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
        }
        addAction(scaleAction);
        scaleAction.drop();
    }
    if(isJointSelected && nodes[selectedNodeId]->getType() == NODE_TEXT){
        nodes[selectedNodeId]->joints[selectedJointId]->setScale(scale, currentFrame);
    } else {
    nodes[selectedNodeId]->setScale(scale, currentFrame);
    }
    setDataForFrame(currentFrame);
    reloadKeyFrameMap();
    updateControlsOrientaion();
}
Vector3 SGAnimationScene::getSelectedNodeScale()
{
    if(isNodeSelected) {
        if(isJointSelected && selectedNode->getType() == NODE_TEXT)
            return selectedNode->joints[selectedJointId]->jointNode->getScale();
        else
            return selectedNode->node->getScale();
    }
    
    return Vector3(1.0);
}
void SGAnimationScene::rttNodeJointSelection(Vector2 touchPosition)
{
    if(!checkNodeSize())
        return;
    if(shaderMGR->deviceType == METAL){
        bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return;
    }
    nodeJointPickerTouchPosition = touchPosition;
    setControlsVisibility(false);
    rotationCircle->node->setVisible(false);
    if(!isNodeSelected || (nodes[selectedNodeId]->getType() != NODE_RIG && nodes[selectedNodeId]->getType() != NODE_TEXT))
        setJointSpheresVisibility(false);
    smgr->setRenderTarget(touchTexture,true,true,false,Vector4(255,255,255,255));
    vector<Vector3> vertexColors;
    vector<float> transparency;
    vector<std::string> previousMaterialNames;
    vector<bool> nodesVisibility;
    for(int i = 0; i < nodes.size(); i++){
        vertexColors.push_back(nodes[i]->props.vertexColor);
        previousMaterialNames.push_back(nodes[i]->node->material->name);
        transparency.push_back(nodes[i]->props.transparency);
        nodesVisibility.push_back(nodes[i]->props.isVisible);
        nodes[i]->props.transparency = 1.0;
        nodes[i]->props.isSelected = false;
        nodes[i]->props.isVisible = true;
        nodes[i]->props.vertexColor = Vector3((i/255.0),1.0,1.0);
        if(nodes[i]->getType() == NODE_RIG)
            nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        else if (nodes[i]->getType() == NODE_TEXT)
            nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
        else
            nodes[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        
    }
    smgr->Render();
    // Draw Joints
    if(isNodeSelected && (nodes[selectedNodeId]->getType() == NODE_RIG || nodes[selectedNodeId]->getType() == NODE_TEXT))
        drawJointsSpheresForRTT(nodes[selectedNodeId],true);
    
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i]->props.vertexColor = vertexColors[i];
        nodes[i]->props.transparency = transparency[i];
        nodes[i]->node->setMaterial(smgr->getMaterialByName(previousMaterialNames[i]));
        nodes[i]->props.isVisible = nodesVisibility[i];
    }
    previousMaterialNames.clear(); vertexColors.clear(); transparency.clear();
    
    if(shaderMGR->deviceType == OPENGLES2)
        getNodeColorFromTouchTexture();
    smgr->setRenderTarget(NULL,false,false);
    if(shaderMGR->deviceType == METAL)
        smgr->EndDisplay();
}

void SGAnimationScene::getNodeColorFromTouchTexture(){
    Vector2 touchPixel = nodeJointPickerTouchPosition;
    if(selectNodeOrJointInPixel(touchPixel))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y-1.0)))
        return;
    selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y+1.0));
}

bool SGAnimationScene::selectNodeOrJointInPixel(Vector2 touchPixel)
{
    float xCoord = (touchPixel.x/screenWidth) * touchTexture->width;
    float yCoord = (touchPixel.y/screenHeight) * touchTexture->height;
    
    limitPixelCoordsWithinTextureRange(touchTexture->width,touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),touchTexture);
    bool status = updateNodeSelectionFromColor(pixel);
    if(status)
        reloadKeyFrameMap();
    return status;
}

bool SGAnimationScene::updateNodeSelectionFromColor(Vector3 pixel)
{
    int prevSelectedNodeId = selectedNodeId;
    unselectObject(prevSelectedNodeId);
    
    int nodeId = (int) pixel.x,jointId = pixel.y;
    if(nodeId != 255 && nodeId >= nodes.size()){
        Logger::log(ERROR, "SganimationSceneRTT","Wrong Color from RTT texture colorx:" + to_string(nodeId));
        return;
    }
    
    isNodeSelected = (selectedNodeId = (nodeId != 255) ? nodeId : NOT_EXISTS) != NOT_EXISTS ? true:false;
    if(selectedNodeId != NOT_EXISTS){
        highlightSelectedNode();
        if(nodes[selectedNodeId]->getType() == NODE_RIG || nodes[selectedNodeId]->getType() == NODE_TEXT){
            isJointSelected = (selectedJointId = (jointId != 255) ? jointId : NOT_EXISTS) != NOT_EXISTS ? true:false;
            if(isJointSelected)
                displayJointsBasedOnSelection();
        }
    }
    if(isNodeSelected || isJointSelected) {
        updateControlsOrientaion();
        if(isJointSelected)
            highlightJointSpheres();
        return true;
    } else
        return false;
}

void SGAnimationScene::highlightSelectedNode()
{
    nodes[selectedNodeId]->props.prevMatName = nodes[selectedNodeId]->node->material->name;
    nodes[selectedNodeId]->props.isSelected = true;
    nodes[selectedNodeId]->props.isLighting = false;
    if(nodes[selectedNodeId]->getType() == NODE_RIG)
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (nodes[selectedNodeId]->getType() == NODE_TEXT)
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    nodes[selectedNodeId]->props.transparency = NODE_SELECTION_TRANSPARENCY;
    selectedNode = nodes[selectedNodeId];
    
    bool status = true;
    if(nodes[selectedNodeId]->getType() == NODE_RIG)
        status = displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node));
    else if (nodes[selectedNodeId]->getType() == NODE_TEXT)
        status = displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node), nodes[selectedNodeId]->props.fontSize/3.0);
    if(!status)
        unselectObject(selectedNodeId);
    
}

void SGAnimationScene::setKeysForFrame(int frame)
{
    for (unsigned long i = 0; i < nodes.size(); i++) {
        
        int posKeyindex = KeyHelper::getKeyIndex(nodes[i]->positionKeys, frame);
        if(posKeyindex != -1){
            nodes[i]->setPositionOnNode(nodes[i]->positionKeys[posKeyindex].position);
        }
        int rotKeyindex = KeyHelper::getKeyIndex(nodes[i]->rotationKeys, frame);
        int scaleKeyindex = KeyHelper::getKeyIndex(nodes[i]->scaleKeys, frame);
        if(nodes[i]->getType() != NODE_LIGHT && nodes[i]->getType() != NODE_ADDITIONAL_LIGHT) {
        
        if(rotKeyindex != -1){
            nodes[i]->setRotationOnNode(nodes[i]->rotationKeys[rotKeyindex].rotation);
        }

        if(scaleKeyindex != -1){
            nodes[i]->setScaleOnNode(nodes[i]->scaleKeys[scaleKeyindex].scale);
        }

        int visibilityKeyindex = KeyHelper::getKeyIndex(nodes[i]->visibilityKeys, frame);
        if(visibilityKeyindex != -1){
            nodes[i]->props.isVisible =  nodes[i]->visibilityKeys[visibilityKeyindex].visibility;
        }
        }
        
        for (int j = 0; j < nodes[i]->joints.size(); j++) {
            SGJoint *joint = nodes[i]->joints[j];
            
            rotKeyindex = KeyHelper::getKeyIndex(joint->rotationKeys, frame);
            if(rotKeyindex != -1){
                joint->setRotationOnNode(joint->rotationKeys[rotKeyindex].rotation);
            }

            if(nodes[i]->getType() == NODE_TEXT) {
                
                posKeyindex = KeyHelper::getKeyIndex(joint->positionKeys, frame);
                if(posKeyindex != -1){
                    joint->setPositionOnNode(joint->positionKeys[posKeyindex].position);
                }
                
                scaleKeyindex = KeyHelper::getKeyIndex(joint->scaleKeys, frame);
                if(scaleKeyindex != -1){
                    joint->setScaleOnNode(joint->scaleKeys[scaleKeyindex].scale);
                }
            }
        }
    }
    updateControlsOrientaion();
    updateLightCamera();
}
void SGAnimationScene::setDataForFrame(int frame)
{
    bool lightChanged = false;
    for (unsigned long i = 0; i < nodes.size(); i++) {
        Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame,nodes[i]->positionKeys);
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,nodes[i]->rotationKeys,true);
        Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame,nodes[i]->scaleKeys);
        int visibilityKeyindex = KeyHelper::getKeyIndex(nodes[i]->visibilityKeys, frame);
        if(visibilityKeyindex != -1){
            nodes[i]->props.isVisible =  nodes[i]->visibilityKeys[visibilityKeyindex].visibility;
        }
        nodes[i]->setPositionOnNode(position);
        if(nodes[i]->getType() == NODE_ADDITIONAL_LIGHT) {
            if(nodes[i]->rotationKeys.size() > 0 && rotation.w >= 10.0) {
                nodes[i]->props.vertexColor = Vector3(rotation.x,rotation.y,rotation.z);
                nodes[i]->props.nodeSpecificFloat = rotation.w;
                lightChanged = true;
            }
        } else if(nodes[i]->getType() == NODE_LIGHT) {
            if(nodes[i]->scaleKeys.size() > 0) {
                lightChanged = true;
            }
        } else {
            nodes[i]->setRotationOnNode(rotation);
            nodes[i]->setScaleOnNode(scale);
        }
        for (int j = 0; j < nodes[i]->joints.size(); j++) {
            SGJoint *joint = nodes[i]->joints[j];
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,joint->rotationKeys,true);
            bool changed = joint->setRotationOnNode(rotation);
            if(nodes[i]->getType() == NODE_TEXT) {
                Vector3 jointPosition = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, joint->positionKeys);
                changed = joint->setPositionOnNode(jointPosition);
                Vector3 jointScale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, joint->scaleKeys);
                changed = joint->setScaleOnNode(jointScale);
            }
        }

    }
    updateControlsOrientaion();
    updateLightCamera();
    if(lightChanged && !isPlaying)
        updateLightProperties(frame);
}
bool SGAnimationScene::storeAnimations(int assetId)
{
    NODE_TYPE currentType = nodes[selectedNodeId]->getType();
    setDataForFrame(currentFrame);
    string extension = (currentType == NODE_RIG) ? ".sgra" : ".sgta";

#ifdef IOS
    string filePath =  FileHelper::getDocumentsDirectory()+ "Resources/Animations/" + to_string(assetId) + extension;
    string thumbnailPath = FileHelper::getDocumentsDirectory() + "Resources/Animations/" + to_string(assetId) + ".png";
#elif ANDROID
    string filePath =  constants::DocumentsStoragePath + "/animations/" + to_string(assetId) + extension;
    string thumbnailPath = constants::DocumentsStoragePath + "/animations/" + to_string(assetId) + ".png";
#endif
    int oldResolution = cameraResolutionType;
    
    cameraResolutionType = 2;
    renderAndSaveImage((char*)thumbnailPath.c_str(), renderingType,true,true);
    if(shaderMGR->deviceType == METAL)
        renderAndSaveImage((char*)thumbnailPath.c_str(), renderingType,true,true);
    cameraResolutionType = oldResolution;
    
    vector<int> totalKeyFrames;
    for (int frame = 0; frame < totalFrames; frame++) {
        if(isKeySetForFrame.find(frame) != isKeySetForFrame.end())
            totalKeyFrames.push_back(frame);
    }
    
    if(currentType == NODE_RIG)
        SGAnimationSceneHelper::storeSGRAnimations(filePath, nodes[selectedNodeId], totalFrames,totalKeyFrames);
    else
        SGAnimationSceneHelper::storeTextAnimations(filePath, nodes[selectedNodeId], totalFrames , textJointsBasePos[selectedNodeId],totalKeyFrames);

    if(FILE *file = fopen(filePath.c_str(), "r")) {
        fclose(file);
        return true;
    }
    return false;
}
void SGAnimationScene::applyAnimations(string filePath , int nodeIndex)
{
    changeAction = true;
    nodeIndex = (nodeIndex == -1) ? selectedNodeId : nodeIndex;
    animFilePath = filePath;
    animStartFrame = currentFrame;
    if(nodes[nodeIndex]->getType() == NODE_RIG) {
    SGAnimationSceneHelper::applySGRAnimations(filePath, nodes[nodeIndex], totalFrames, currentFrame, animTotalFrames);
    } else if (nodes[nodeIndex]->getType() == NODE_TEXT) {
        SGAnimationSceneHelper::applyTextAnimations(filePath, nodes[nodeIndex], totalFrames, currentFrame ,textJointsBasePos[nodeIndex],animTotalFrames);
    }
    setDataForFrame(currentFrame);
    reloadKeyFrameMap();
}
void SGAnimationScene::applySGAOnNode(std::string *filePath)
{
    animFilePath  = *(filePath);
    animStartFrame = currentFrame;
    ifstream sgaFilePtr(*filePath,ios::in | ios::binary);
    FileHelper::resetSeekPosition();
    short sgaFrames = FileHelper::readShort(&sgaFilePtr);
    
    if(currentFrame + sgaFrames > totalFrames)
        totalFrames = currentFrame + sgaFrames;
    
    animTotalFrames = totalFrames;
    // calculate height ratio -- todo Enums for boneID
    SGNode *meshObject = nodes[selectedNodeId];
    Vector3 headPosition = meshObject->getJointPosition(HEAD,true);
    Vector3 anklePosition = meshObject->getJointPosition(ANKLE,true);
    float nodeHeight = headPosition.getDistanceFrom(anklePosition);
    float bvhModelHeight = FileHelper::readFloat(&sgaFilePtr);
    float heightRatio = (bvhModelHeight != nodeHeight) ? (bvhModelHeight / nodeHeight):1.0;
    
    applySGARotationData(sgaFrames,&sgaFilePtr,currentFrame);
    setDataForFrame(currentFrame);
    applySGAPositionData(sgaFrames,currentFrame,&sgaFilePtr,heightRatio);
    setDataForFrame(currentFrame);
    changeAction = true;
    reloadKeyFrameMap();
}
void SGAnimationScene::applySGAPositionData(short sgaFrames,int currentFrame,ifstream* filePointer,float heightRatio)
{
    SGNode *meshObject = nodes[selectedNodeId];
    Vector3 initialPosition, nodePosition;
    nodePosition = meshObject->getNodePosition();
    int resetCurrentFrame = currentFrame;
    for(int i = 0; i <  sgaFrames; i++) {
        float posX, posY, posZ;
        posX = FileHelper::readFloat(filePointer);
        posY = FileHelper::readFloat(filePointer);
        posZ = FileHelper::readFloat(filePointer);
        
        if(i == 0) {
            initialPosition.x = (posX / heightRatio);
            initialPosition.y = (posZ / heightRatio);
            initialPosition.z = ((-posY) / heightRatio);
        }
        currentFrame = i + resetCurrentFrame;
        Vector3 positionToApply;
        positionToApply.x = nodePosition.x + ((posX / heightRatio) - initialPosition.x);
        positionToApply.y = nodePosition.y + ((posZ / heightRatio) - initialPosition.y);
        positionToApply.z = nodePosition.z + (((-posY) / heightRatio) - initialPosition.z);
        meshObject->setPosition(positionToApply, currentFrame);
    }
}
void SGAnimationScene::applySGARotationData(short sgaFrames,ifstream* filePointer,int currentFrame)
{
    
    SGNode *meshObject = nodes[selectedNodeId];
    int jointIdOrder[18] = {1, 44, 45, 46, 49, 50, 51, 2, 25, 26, 27, 28, 6, 7, 8, 9, 4, 5};
    
    float boneRoll[18];
    for(int i = 0; i < 18; i++) {
        float rolValue = FileHelper::readFloat(filePointer);
        boneRoll[i] = rolValue;
    }
    
    
    Quaternion nodeRotQuat = meshObject->node->getRotationInDegrees();
    Vector3 nodeRotation;
    nodeRotQuat.toEuler(nodeRotation);
    nodeRotQuat = Quaternion(Vector3(nodeRotation.x - 90.0,nodeRotation.y,nodeRotation.z) * DEGTORAD);
    
    float resetCurrentFrame = currentFrame;
    int bone = 0;
    
    for(int frame = 0;frame < sgaFrames;frame++) {
        currentFrame = frame + resetCurrentFrame;
        for(int j = 0; j < 18; j++){
            Quaternion globRot;
            bone = jointIdOrder[j];
            if(bone >= 6 && bone <= 9) //rightHand to LeftHand
                bone = bone + 19;
            else if(bone >= 25 && bone <= 28)
                bone = bone - 19;
            Quaternion delta = Quaternion(Vector3(0.0, boneRoll[j], 0.0) * DEGTORAD);
            Quaternion boneGlobalRotation = readRotationFromSGAFile(boneRoll[j],filePointer);
            SGJoint *joint = meshObject->joints[bone];
            Quaternion rotationToApply = delta * meshObject->GetLocalQuaternion(bone, boneGlobalRotation * nodeRotQuat);
            joint->setRotation(rotationToApply,currentFrame);
            joint->setRotationOnNode(rotationToApply);
        }
    }
    currentFrame = resetCurrentFrame;
    setDataForFrame(currentFrame);
}
Quaternion SGAnimationScene::readRotationFromSGAFile(float rollValue,ifstream* filePointer)
{
    Quaternion rotation;
    rotation.x = FileHelper::readFloat(filePointer);
    rotation.y = FileHelper::readFloat(filePointer);
    rotation.z = FileHelper::readFloat(filePointer);
    rotation.w = FileHelper::readFloat(filePointer);
    return rotation;
}

void SGAnimationScene::setLightingOff()
{
    ShaderManager::sceneLighting = false;
}

void SGAnimationScene::setTransparencyForIntrudingObjects()
{
    return;
    
    if(selectedNodeId != NOT_SELECTED) {
        Vector3 cameraToSelection = (nodes[selectedNodeId]->node->getPosition() - viewCamera->getPosition()).normalize();
        float selectionDistance = viewCamera->getPosition().getDistanceFrom(nodes[selectedNodeId]->node->getPosition());
        
        for(int index = 0; index < nodes.size(); index++) {
            if(index != selectedNodeId) {
                Vector3 camToObject = (nodes[index]->node->getPosition() - viewCamera->getPosition()).normalize();
                float objectDistance = viewCamera->getPosition().getDistanceFrom(nodes[index]->node->getPosition());
                
                float angle = fabs(cameraToSelection.dotProduct(camToObject));
                printf(" index %d distance %f select distance %f ", index, objectDistance, selectionDistance);
                if(objectDistance <= selectionDistance + 1.0) {
                    float nodeTrans = (angle > 0.9) ? (0.95 - angle) * 20.0 : 1.0;
                    nodeTrans = (nodeTrans < 0.0) ? 0.0 : nodeTrans;
                    nodes[index]->props.transparency = nodeTrans;
                    nodes[index]->node->setVisible((nodeTrans == 0.0) ? false : true);
                } else {
                    nodes[index]->node->setVisible(true);
                    nodes[index]->props.transparency = 1.0;
                }
            }
        }
    } else {
        for(int index = 0; index < nodes.size(); index++) {
            nodes[index]->node->setVisible(true);
            nodes[index]->props.transparency = 1.0;
        }
    }
}

void SGAnimationScene::touchBegan(Vector2 curTouchPos)
{
    xAcceleration = yAcceleration = 0.0;
}
void SGAnimationScene::checkCtrlSelection(Vector2 curTouchPos,bool isDisplayPrepared){
    prevTouchPoints[0] = curTouchPos;
    if(!isNodeSelected)
        return;
    
    updateControlsOrientaion();
    

    rttControlSelectionAnim(curTouchPos);
    if(shaderMGR->deviceType == METAL){
        rttControlSelectionAnim(curTouchPos);
        getCtrlColorFromTouchTextureAnim(curTouchPos);
    }
    isRTTCompleted = true;

    storeInitialKeyForUndo();
}
void SGAnimationScene::rttControlSelectionAnim(Vector2 touchPosition)
{
    if(shaderMGR->deviceType == METAL){
        bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,false,true,false,Vector4(0,0,0,255));
        if(!displayPrepared)
            return;
    }
    int controlStartIndex = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    rotationCircle->node->setVisible(false);
    smgr->setRenderTarget(touchTexture,true,true,false,Vector4(255,255,255,255));
    updateControlsOrientaion(true);
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        sceneControls[i]->props.vertexColor = Vector3(i/255.0,1.0,1.0);
        sceneControls[i]->props.transparency = 1.0;
        Vector3 ctrlToCam = (viewCamera->getPosition() - sceneControls[i]->node->getPosition()).normalize();
        float angle = fabs(ctrlToCam.dotProduct(controlDirection[i%3]));
        int nodeIndex = smgr->getNodeIndexByID(sceneControls[i]->node->getID());
        if(angle < 0.9)
            smgr->RenderNode(nodeIndex);
    }
    if(shaderMGR->deviceType == OPENGLES2)
        getCtrlColorFromTouchTextureAnim(touchPosition);
    smgr->setRenderTarget(NULL,false,false);
    for(int i = controlStartIndex;i <= controlEndIndex;i++) {
        sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
    }
    updateControlsOrientaion();
    if(shaderMGR->deviceType == METAL)
        smgr->EndDisplay();

}
void SGAnimationScene::getCtrlColorFromTouchTextureAnim(Vector2 touchPosition)
{
    int controlStartIndex = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    float xCoord = (touchPosition.x/screenWidth) * touchTexture->width;
    float yCoord = (touchPosition.y/screenHeight) * touchTexture->height;
    limitPixelCoordsWithinTextureRange(touchTexture->width,touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),touchTexture,Vector4(255,255,255,255));
    int selectedCol = (int)pixel.x;
    if(selectedCol != 255 && (selectedCol >= controlStartIndex && selectedCol <= controlEndIndex)){
        isControlSelected = true;
        selectedControlId = selectedCol;
    }else if(selectedCol >= controlStartIndex && selectedCol <= controlEndIndex){
        Logger::log(ERROR,"SGAnimationScene::getCtrlColorFromTouchTextureAnim","Wrong RTT Color");
    }
}
void SGAnimationScene::storeInitialKeyForUndo(){
    if(selectedControlId != NOT_SELECTED) {
        isControlSelected = true;
        changeKeysAction.drop();
        changeKeysAction.objectIndex = nodes[selectedNodeId]->actionId;
        
        if(!isJointSelected){
            changeKeysAction.actionType = ACTION_CHANGE_NODE_KEYS;
            changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
        }
        else if(isJointSelected){
            if(nodes[selectedNodeId]->joints.size() >= HUMAN_JOINTS_SIZE && selectedJointId == HIP) {
                changeKeysAction.actionType = ACTION_CHANGE_NODE_JOINT_KEYS;
                changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
                for(unsigned long i=0; i<nodes[selectedNodeId]->joints.size(); i++){
                    changeKeysAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
                }
            }
            else {
                changeKeysAction.actionType = ACTION_CHANGE_JOINT_KEYS;
                for(unsigned long i=0; i<nodes[selectedNodeId]->joints.size(); i++){
                    changeKeysAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
                }
            }
//            if(nodes[selectedNodeId]->getType() == NODE_RIG)
//                dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(CHARACTER_RIG,selectedJointId);
//            else if(nodes[selectedNodeId]->getType() == NODE_TEXT)
//                dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(TEXT_RIG,selectedJointId);
        }
    }
}
void SGAnimationScene::switchFrame(int frame)
{
    if(!isPlaying) {
        SGAction switchFrameAction;
        switchFrameAction.actionType = ACTION_SWITCH_FRAME;
        switchFrameAction.actionSpecificIntegers.push_back(previousFrame);
        switchFrameAction.frameId = currentFrame;
        addAction(switchFrameAction);
    }

    setDataForFrame(frame);
}
void SGAnimationScene::touchEnd(Vector2 curTouchPos)
{
    setLightingOn();
    swipeTiming = 0;
    updateControlsMaterial();
    if(isControlSelected) {
        Logger::log(INFO , "SGScene diff ", "touch end");
        prevRotX = prevRotY = prevRotZ = 0.0;
        selectedControlId = NOT_SELECTED;
        isControlSelected = false;
        storeMovementAction();
    }
}

void SGAnimationScene::setLightingOn()
{
    ShaderManager::sceneLighting = true;
}

void SGAnimationScene::storeMovementAction()
{
    if(!isNodeSelected)
        return;
    
    if(!isJointSelected && selectedNodeId != NOT_SELECTED) {
        changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
        if(nodes[selectedNodeId]->getType() == NODE_LIGHT || nodes[selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
            updateLightProperties(currentFrame);

    }
    else if(isJointSelected){
        if(selectedNodeId > 0) {
            if((nodes[selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) && (selectedJointId == HIP)) {
                changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
                for(unsigned long i=0; i<nodes[selectedNodeId]->joints.size(); i++){
                    changeKeysAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
                }
                
            } else {
                for(unsigned long i=0; i<nodes[selectedNodeId]->joints.size(); i++){
                    changeKeysAction.keys.push_back(nodes[selectedNodeId]->joints[i]->getKeyForFrame(currentFrame));
                }
            }
//            if(nodes[selectedNodeId]->getType() == NODE_RIG)
//                dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(CHARACTER_RIG,selectedJointId);
//            else if(nodes[selectedNodeId]->getType() == NODE_TEXT)
//                dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(TEXT_RIG,selectedJointId);
        }
    }
    reloadKeyFrameMap();
    if(selectedNodeId != NOT_SELECTED)
        addAction(changeKeysAction);
}
void SGAnimationScene::touchMove(Vector2 curTouchPos,Vector2 prevTouchPos,float width,float height)
{
    updateLightCam(nodes[NODE_LIGHT]->node->getPosition());
    if(!isNodeSelected)
        return;
    
    //if(selectedNodeId != NOT_SELECTED && nodes[selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
        //updateLightProperties(currentFrame);
    
    if(isControlSelected) {
        Vector3 outputValue;
        calculateControlMovements(curTouchPos,prevTouchPoints[0] ,outputValue);
        prevTouchPoints[0] = curTouchPos;
        isMoved |= changeObjectOrientation(outputValue);
    }
    return;
}
void SGAnimationScene::addAction(SGAction& action)
{
    changeAction = true;
    removeActions();
    if(action.actionType == ACTION_CHANGE_JOINT_KEYS|| action.actionType == ACTION_CHANGE_NODE_JOINT_KEYS|| action.actionType == ACTION_CHANGE_NODE_KEYS) {
        action.objectIndex = nodes[selectedNodeId]->actionId;
        if(action.actionType != ACTION_CHANGE_NODE_KEYS)
            action.actionSpecificIntegers.push_back(selectedJointId);
        action.frameId = currentFrame;
    }
    SGScene::addAction(action);
}
bool SGAnimationScene::changeObjectOrientation(Vector3 outputValue)
{
    if(!isNodeSelected) return false;
    bool success = false;
    switch(controlType){
        case MOVE:{
            if(isJointSelected){
                Vector3 target = selectedJoint->jointNode->getAbsolutePosition() + outputValue;
                if((nodes[selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) && isIKJoint(selectedJointId)){
                    success = true;
                    if(selectedJointId <= HIP){
                        shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
                        if(rigNode) {
                            selectedNode->setPosition(rigNode->getPosition() + outputValue, currentFrame);
                            selectedNode->setPositionOnNode(rigNode->getPosition() + outputValue);
                            rigNode->setPosition(rigNode->getPosition() + outputValue);
                            rigNode->updateAbsoluteTransformation();
                            for (ikJointsPositoinMapItr = ikJointsPositionMap.begin(); ikJointsPositoinMapItr != ikJointsPositionMap.end(); ikJointsPositoinMapItr++){
                                int jointId = ((*ikJointsPositoinMapItr).first);
                                selectedNode->MoveBone((shared_ptr<JointNode>)(selectedNode->joints[jointId]->jointNode),ikJointsPositionMap.find((*ikJointsPositoinMapItr).first)->second,currentFrame);
                            }
                        }
                    }else{
                        if(selectedJoint)
                            selectedNode->MoveBone(selectedJoint->jointNode,target,currentFrame);
                    }
                } else if(nodes[selectedNodeId]->getType() == NODE_TEXT){
                    
                    selectedJoint = nodes[selectedNodeId]->joints[selectedJointId];
                    Vector3 jointLocalPos = selectedJoint->jointNode->getPosition();

                    Vector3 moveDir = Vector3((selectedControlId == X_MOVE ? 1.0 : 0.0),(selectedControlId == Y_MOVE ? 1.0 : 0.0),(selectedControlId == Z_MOVE ? 1.0 : 0.0));
                                        
                    float trnsValue = (selectedControlId == X_MOVE) ? outputValue.x : (selectedControlId == Y_MOVE) ? outputValue.y : outputValue.z;
                    moveDir = moveDir * trnsValue;
                    
                    Vector3 position = jointLocalPos + moveDir;
                    selectedJoint->setPosition(position, currentFrame);
                    selectedJoint->setPositionOnNode(position);
                    
                }
                else{
                    shared_ptr<Node> parent = nodes[selectedNodeId]->joints[selectedJointId]->jointNode->getParent();
                    Vector3 jointPos = nodes[selectedNodeId]->joints[selectedJointId]->jointNode->getAbsoluteTransformation().getTranslation();
                    success = true;
                    Quaternion rot = Quaternion(MathHelper::getRelativeParentRotation(nodes[selectedNodeId]->joints[selectedJointId]->jointNode,(jointPos + outputValue)) * DEGTORAD);
                    nodes[selectedNodeId]->joints[parent->getID()]->setRotation(rot,currentFrame);
                    nodes[selectedNodeId]->joints[parent->getID()]->setRotationOnNode(rot);
                    
                    if(getMirrorState() == MIRROR_ON){
                        int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
                        if(mirrorJointId == -1)
                            break;
                        selectedJoint = nodes[selectedNodeId]->joints[selectedJointId];
                        shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node))->getJointNode(mirrorJointId);
                        rot = Quaternion(nodes[selectedNodeId]->joints[selectedJoint->jointNode->getParent()->getID()]->jointNode->getRotationInDegrees()*Vector3(1.0,-1.0,-1.0)*DEGTORAD);
                        nodes[selectedNodeId]->joints[mirrorNode->getParent()->getID()]->setRotation(rot,currentFrame);
                        nodes[selectedNodeId]->joints[mirrorNode->getParent()->getID()]->setRotationOnNode(rot);
                    }
                }
//                if(nodes[selectedNodeId]->getType() == NODE_RIG)
//                    dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(CHARACTER_RIG,selectedJointId);
//                else if(nodes[selectedNodeId]->getType() == NODE_TEXT)
//                    dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(TEXT_RIG,selectedJointId);
                
            }else if(isNodeSelected){
                success = true;
                
                nodes[selectedNodeId]->setPosition(nodes[selectedNodeId]->node->getPosition() + outputValue,currentFrame);
                nodes[selectedNodeId]->setPositionOnNode(nodes[selectedNodeId]->node->getPosition() + outputValue);
                break;
            }
            break;
        }
        case ROTATE:{
            outputValue*=RADTODEG;
            if(isJointSelected){
                success = true;
                selectedJoint->setRotation(Quaternion(outputValue*DEGTORAD), currentFrame);
                selectedJoint->setRotationOnNode(Quaternion(outputValue*DEGTORAD));
                if(getMirrorState() == MIRROR_ON){
                    int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
                    if(mirrorJointId == -1)
                        break;
                    Quaternion rot = Quaternion(outputValue*Vector3(1.0,-1.0,-1.0)*DEGTORAD);
                    nodes[selectedNodeId]->joints[mirrorJointId]->setRotation(rot, currentFrame);
                    nodes[selectedNodeId]->joints[mirrorJointId]->setRotationOnNode(rot);
                }
//                if(nodes[selectedNodeId]->getType() == NODE_RIG)
//                    dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(CHARACTER_RIG,selectedJointId);
//                else if(nodes[selectedNodeId]->getType() == NODE_TEXT)
//                    dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node)->updatePartOfMeshCache(TEXT_RIG,selectedJointId);
                
            }else if(isNodeSelected){
                success = true;
                Quaternion r = Quaternion(outputValue * DEGTORAD);
                nodes[selectedNodeId]->setRotation(r,currentFrame);
                nodes[selectedNodeId]->setRotationOnNode(r);
                break;
            }
            break;
        }
        default:
        break;
    }
    updateControlsOrientaion();
    updateLightCamera();
    return success;
}
void SGAnimationScene::findAndInsertInIKPositionMap(int jointId)
{
    shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
    if(ikJointsPositionMap.find(jointId) == ikJointsPositionMap.end())
        ikJointsPositionMap.insert(pair<int,Vector3>(jointId,rigNode->getJointNode(jointId)->getAbsolutePosition()));
    else
        ikJointsPositionMap.find(jointId)->second = rigNode->getJointNode(jointId)->getAbsolutePosition();
    //rigNode.reset();
}
void SGAnimationScene::getIKJointPosition()
{
    shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
    if(rigNode) {
        findAndInsertInIKPositionMap(LEG_RIGHT);
        findAndInsertInIKPositionMap(LEG_LEFT);
        findAndInsertInIKPositionMap(HAND_RIGHT);
        findAndInsertInIKPositionMap(HAND_LEFT);
        //rigNode.reset();
    }
}
bool SGAnimationScene::loadSceneData(std::string *filePath)
{
    ifstream inputSGBFile(*filePath,ios::in | ios::binary );
    FileHelper::resetSeekPosition();
    if(!readScene(&inputSGBFile)){
        inputSGBFile.close();
        return false;
    }
    inputSGBFile.close();
    return true;
}

#ifdef ANDROID
bool SGAnimationScene::loadSceneData(std::string *filePath, JNIEnv *env, jclass type)
{
    ifstream inputSGBFile(*filePath,ios::in | ios::binary );
    FileHelper::resetSeekPosition();
    if(!readScene(&inputSGBFile, env, type)){
        inputSGBFile.close();
        return false;
    }
    inputSGBFile.close();
    return true;
}
bool SGAnimationScene::readScene(ifstream *filePointer, JNIEnv *env, jclass type)
{
//    actions.clear();
    int nodeCount = 0;
    SGAnimationSceneHelper::readScene(filePointer, totalFrames, cameraFOV, nodeCount);
    totalFrames = (totalFrames < 24) ? 24 : totalFrames;
    nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;

    vector<SGNode*> tempNodes;
    for(int i = 0;i < nodeCount;i++){
        SGNode *sgNode = new SGNode(NODE_UNDEFINED);
        sgNode->readData(filePointer);
        bool status = true;

        if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ)
        {
            status = downloadMissingAssetsCallBack(to_string(sgNode->assetId),sgNode->getType(), env, type);
        }
        else if (sgNode->getType() == NODE_TEXT) {

            status = downloadMissingAssetsCallBack(sgNode->optionalFilePath,sgNode->getType(),env,type);
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = downloadMissingAssetsCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType(), env, type);
        }

        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }
    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;
        if(sgNode)
            nodeLoaded = loadNode(sgNode,OPEN_SAVED_FILE);

        if(!nodeLoaded)
            delete sgNode;
    }

    return true;
}
#endif

bool SGAnimationScene::readScene(ifstream *filePointer)
{
//    actions.clear();
    int nodeCount = 0;
    SGAnimationSceneHelper::readScene(filePointer, totalFrames, cameraFOV, nodeCount);
    totalFrames = (totalFrames < 24) ? 24 : totalFrames;
    nodes.clear();
    if(nodeCount < NODE_LIGHT+1)
        return false;
    
    vector<SGNode*> tempNodes;
    for(int i = 0;i < nodeCount;i++){
        SGNode *sgNode = new SGNode(NODE_UNDEFINED);
        sgNode->readData(filePointer);
        bool status = true;

        if(sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_RIG || sgNode->getType() == NODE_OBJ)
        {
            status = downloadMissingAssetCallBack(to_string(sgNode->assetId),sgNode->getType());
        }
        else if (sgNode->getType() == NODE_TEXT) {
            status = downloadMissingAssetCallBack(sgNode->optionalFilePath,sgNode->getType());
        } else if (sgNode->getType() == NODE_IMAGE) {
            status = downloadMissingAssetCallBack(ConversionHelper::getStringForWString(sgNode->name), sgNode->getType());
        }

        if(!status)
            sgNode = NULL;
        tempNodes.push_back(sgNode);
    }
    for (int i = 0; i < tempNodes.size(); i++) {
        SGNode *sgNode = tempNodes[i];
        bool nodeLoaded = false;
        if(sgNode)
            nodeLoaded = loadNode(sgNode,OPEN_SAVED_FILE);
        
        if(!nodeLoaded)
            delete sgNode;
    }

    return true;
}
void SGAnimationScene::saveSceneData(std::string *filePath)
{
    ofstream outputFile(*filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    writeScene(&outputFile);
    outputFile.close();
}
void SGAnimationScene::writeScene(ofstream *filePointer)
{
    SGAnimationSceneHelper::writeScene(filePointer, totalFrames, cameraFOV, int(nodes.size()));
    for(unsigned long i = 0;i < nodes.size();i++)
        nodes[i]->writeData(filePointer);
}

void SGAnimationScene::saveThumbnail(char* targetPath)
{
    if(!checkNodeSize())
        return;
    
    bool displayPrepared = smgr->PrepareDisplay(thumbnailTexture->width,thumbnailTexture->height,false,true,false,Vector4(255,255,255,255));
    if(!displayPrepared)
        return;
    setControlsVisibility(false);
    setJointSpheresVisibility(false);
    rotationCircle->node->setVisible(false);
    //jointSpheres.clear();
    for(unsigned long i = NODE_CAMERA; i < nodes.size(); i++) {
        if(!(nodes[i]->props.isVisible))
            nodes[i]->node->setVisible(false);
    }
    
    unselectObject(selectedNodeId);
        
    smgr->setRenderTarget(thumbnailTexture,true,true,false,Vector4(255,255,255,255));
    smgr->draw2DImage(bgTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    drawGrid();

    smgr->Render();
    smgr->EndDisplay();
    smgr->writeImageToFile(thumbnailTexture, targetPath , (shaderMGR->deviceType == OPENGLES2) ? FLIP_VERTICAL : NO_FLIP);
    
    smgr->setRenderTarget(NULL,true,true,false,Vector4(255,255,255,255));
    
    for(unsigned long i = NODE_CAMERA; i < nodes.size(); i++) {
        if(!(nodes[i]->props.isVisible))
            nodes[i]->node->setVisible(true);
    }
    
    if(selectedNodeId != NOT_SELECTED)
        nodes[selectedNodeId]->props.isSelected = true;
    
    selectObject(selectedNodeId);
    setControlsVisibility(true);
    smgr->EndDisplay();
}

void SGAnimationScene::renderAndSaveImage(char *imagePath , int shaderType,bool isDisplayPrepared, bool removeWaterMark)
{
//    SGCloudRenderingHelper::writeFrameData(this, currentFrame);
//    return;

    if(!checkNodeSize())
        return;
    isExporting1stTime = false;
    
    if(smgr->device == OPENGLES2)
        rttShadowMap();
    
    bool displayPrepared = smgr->PrepareDisplay(renderingTextureMap[RESOLUTION[cameraResolutionType][0]]->width,renderingTextureMap[RESOLUTION[cameraResolutionType][0]]->height,false,true,false,Vector4(255,255,255,255));
    if(!displayPrepared)
        return;
    setRenderCameraOrientation();
    setControlsVisibility(false);
    setJointSpheresVisibility(false);
    rotationCircle->node->setVisible(false);
    
    int selectedObjectId;
    if(selectedNodeId != NOT_SELECTED) {
        selectedObjectId = selectedNodeId;
        unselectObject(selectedNodeId);
    }

    
    vector<string> previousMaterialNames;
    if(renderingType != shaderType)
    {
        resetMaterialTypes(true);
    }
    
    for(unsigned long i = 0; i < nodes.size(); i++){
       if(!(nodes[i]->props.isVisible))
           nodes[i]->node->setVisible(false);
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(false);
    }
    
    smgr->setRenderTarget(renderingTextureMap[RESOLUTION[cameraResolutionType][0]],true,true,false,Vector4(255,255,255,255));
    smgr->draw2DImage(bgTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));

    smgr->Render();

    if(!removeWaterMark)
        smgr->draw2DImage(watermarkTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),false,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    if(smgr->device == METAL)
        rttShadowMap();

    smgr->EndDisplay();
    smgr->writeImageToFile(renderingTextureMap[RESOLUTION[cameraResolutionType][0]],imagePath,(shaderMGR->deviceType == OPENGLES2) ?FLIP_VERTICAL : NO_FLIP);
    
    smgr->setActiveCamera(viewCamera);
    smgr->setRenderTarget(NULL,true,true,false,Vector4(255,255,255,255));
    
    
    for(unsigned long i = 0; i < nodes.size(); i++){
        if(!(nodes[i]->props.isVisible))
            nodes[i]->node->setVisible(true);
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            nodes[i]->node->setVisible(true);
    }

    if(renderingType != shaderType)
        resetMaterialTypes(false);

    if(selectedObjectId != NOT_SELECTED)
        selectObject(selectedObjectId);
}

int SGAnimationScene::undo(int &returnValue2)
{
    int returnValue = DO_NOTHING;
    SGAction &recentAction = actions[currentAction-1];
    int indexOfAction = 0;
    
    if(currentAction <= 0) {
        currentAction = 0;
        return (actions.size() > 0) ? DEACTIVATE_UNDO:DEACTIVATE_BOTH;
    }
    for(int i = 0; i < (int)nodes.size(); i++) {
        if(nodes[i]->actionId == recentAction.objectIndex)
            returnValue2 = indexOfAction = i;
    }
    switch(recentAction.actionType){
        case ACTION_CHANGE_NODE_KEYS:
            nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[0]);
            reloadKeyFrameMap();
            break;
        case ACTION_CHANGE_JOINT_KEYS: {
            for(int i = 0; i <(int)nodes[indexOfAction]->joints.size(); i++){
                nodes[indexOfAction]->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[i]);
            }
            reloadKeyFrameMap();
            break;
            
        }
        case ACTION_CHANGE_NODE_JOINT_KEYS:{
            nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[0]);
            for(unsigned long i=0; i<nodes[indexOfAction]->joints.size(); i++){
                nodes[indexOfAction]->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[i+1]);
            }
            reloadKeyFrameMap();
            break;
        }
        case ACTION_SWITCH_FRAME:{
            currentFrame = recentAction.actionSpecificIntegers[0];
            returnValue = SWITCH_FRAME;
            break;
        }
        case ACTION_CHANGE_PROPERTY_MESH:{
            nodes[indexOfAction]->setShaderProperties(recentAction.actionSpecificFloats[0], recentAction.actionSpecificFloats[1], recentAction.actionSpecificFlags[0], recentAction.actionSpecificFlags[1], recentAction.frameId);
            break;
        }
        case ACTION_CHANGE_PROPERTY_LIGHT: {
            //TODO to do for all lights
            ShaderManager::lightColor[0] = Vector3(recentAction.actionSpecificFloats[0],recentAction.actionSpecificFloats[1],recentAction.actionSpecificFloats[2]);
            ShaderManager::shadowDensity = recentAction.actionSpecificFloats[3];
            break;
        }
        case ACTION_CHANGE_PROPERTY_CAMERA:{
            setCameraProperty(recentAction.actionSpecificFloats[0], recentAction.actionSpecificIntegers[0]);
            break;
        }
        case ACTION_CHANGE_MIRROR_STATE:{
            unselectObject(selectedNodeId);
            setMirrorState((MIRROR_SWITCH_STATE)(bool)recentAction.actionSpecificFlags[0]);
            returnValue = SWITCH_MIRROR;
            break;
        }
        case ACTION_NODE_ADDED: {
            recentAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(nodes[indexOfAction]->props.prevMatName));
            unselectObject(selectedNodeId);
            returnValue = DELETE_ASSET;
            break;
        }
        case ACTION_NODE_DELETED: {
            returnValue = ADD_ASSET_BACK;
            returnValue2 = recentAction.frameId;
            break;
        }
        case ACTION_TEXT_IMAGE_ADD: {
            unselectObject(selectedNodeId);
            returnValue = DELETE_ASSET;
            break;
        }
        case ACTION_TEXT_IMAGE_DELETE: {
            SGNode *sgNode = new SGNode(NODE_UNDEFINED);
            sgNode->setType((NODE_TYPE)recentAction.actionSpecificIntegers[0]);
            sgNode->props.fontSize = recentAction.actionSpecificIntegers[1];
//            node->props.shaderType = recentAction.actionSpecificIntegers[1];
            sgNode->props.vertexColor.x = recentAction.actionSpecificFloats[0];
            sgNode->props.vertexColor.y = recentAction.actionSpecificFloats[1];
            sgNode->props.vertexColor.z = recentAction.actionSpecificFloats[2];
            sgNode->props.nodeSpecificFloat = recentAction.actionSpecificFloats[3];

            sgNode->props.prevMatName = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[0]);
            sgNode->optionalFilePath = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[1]);
            sgNode->name = recentAction.actionSpecificStrings[2];
            if(!loadNode(sgNode,UNDO_ACTION)){
                delete sgNode;
                returnValue = DO_NOTHING;
                break;
            }
            returnValue = ADD_TEXT_IMAGE_BACK;
            break;
        }
        case ACTION_APPLY_ANIM: {
            unselectObject(selectedNodeId);
            selectedNodeId = indexOfAction;
            reloadKeyFrameMap();
            for (int i = recentAction.frameId; i < recentAction.actionSpecificIntegers[0]; i++) {
                removeAnimationForSelectedNodeAtFrame(i);
            }
            reloadKeyFrameMap();
            unselectObject(selectedNodeId);
            returnValue = RELOAD_FRAMES;
            break;
        }
        default:
            return DO_NOTHING;
    }
    if(recentAction.actionType != ACTION_NODE_DELETED && recentAction.actionType != ACTION_TEXT_IMAGE_DELETE)
        currentAction--;
    //currentFrame = recentAction.frameId;

    if(recentAction.actionType != ACTION_SWITCH_FRAME) {
        setKeysForFrame(recentAction.frameId);
        setKeysForFrame(recentAction.frameId);
    }
    if(recentAction.actionType == ACTION_CHANGE_JOINT_KEYS || recentAction.actionType == ACTION_CHANGE_NODE_JOINT_KEYS) {
        dynamic_pointer_cast<AnimatedMeshNode>(nodes[indexOfAction]->node)->updatePartOfMeshCache(CHARACTER_RIG,recentAction.actionSpecificIntegers[0]);
    }
    
    if(recentAction.actionType == ACTION_CHANGE_NODE_KEYS && (nodes[indexOfAction]->getType() == NODE_LIGHT || nodes[indexOfAction]->getType() == NODE_ADDITIONAL_LIGHT))
        updateLightProperties(recentAction.frameId);
    
    return returnValue;
}
int SGAnimationScene::redo()
{
    int returnValue = DO_NOTHING;
    if(currentAction < 0) {
        currentAction = 0;
        return DEACTIVATE_BOTH;
    }
    
    if(currentAction == actions.size()) {
        if(currentAction == 0)
            return DEACTIVATE_BOTH;
        else
            return DEACTIVATE_REDO;
    }
    
    SGAction &recentAction = actions[currentAction];
    int indexOfAction = 0;
    for(int i = 0; i < (int)nodes.size(); i++) {
        if(nodes[i]->actionId == recentAction.objectIndex)
            indexOfAction = i;
    }
    
    switch(recentAction.actionType){
        case ACTION_CHANGE_NODE_KEYS:
            nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[1]);
            reloadKeyFrameMap();
            break;
        case ACTION_CHANGE_JOINT_KEYS:{
            int jointsCnt = (int)nodes[indexOfAction]->joints.size();
            for(unsigned long i=0; i<nodes[indexOfAction]->joints.size(); i++){
                nodes[indexOfAction]->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt+i]);
            }
            reloadKeyFrameMap();
            break;
        }
        case ACTION_CHANGE_NODE_JOINT_KEYS:{
            int jointsCnt = (int)nodes[indexOfAction]->joints.size();
            nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt +1]);
            for(unsigned long i=1; i<=nodes[indexOfAction]->joints.size(); i++){
                nodes[indexOfAction]->joints[i-1]->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt+i+1]);
            }
            break;
        }
        case ACTION_SWITCH_FRAME:
            currentFrame = recentAction.frameId;
            break;
        case ACTION_CHANGE_PROPERTY_MESH:
            nodes[indexOfAction]->setShaderProperties(recentAction.actionSpecificFloats[2], recentAction.actionSpecificFloats[3], recentAction.actionSpecificFlags[2], recentAction.actionSpecificFlags[3], recentAction.frameId);
            break;
        case ACTION_CHANGE_PROPERTY_LIGHT:
            //TODO to do for all lights
            ShaderManager::lightColor[0] = Vector3(recentAction.actionSpecificFloats[4],recentAction.actionSpecificFloats[5],recentAction.actionSpecificFloats[6]);
            ShaderManager::shadowDensity = recentAction.actionSpecificFloats[7];
            break;
        case ACTION_CHANGE_PROPERTY_CAMERA:
            setCameraProperty(recentAction.actionSpecificFloats[1], recentAction.actionSpecificIntegers[1]);
            break;
        case ACTION_CHANGE_MIRROR_STATE:
            unselectObject(selectedNodeId);
            setMirrorState((MIRROR_SWITCH_STATE)!recentAction.actionSpecificFlags[0]);
            returnValue = SWITCH_MIRROR;
            break;
        case ACTION_NODE_ADDED:
        {
            returnValue = recentAction.frameId;
            break;
        }
        case ACTION_NODE_DELETED:
            unselectObject(selectedNodeId);
            selectedNodeId = indexOfAction;
            returnValue = DELETE_ASSET;
            break;
        case ACTION_TEXT_IMAGE_ADD: {
            SGNode *sgNode = new SGNode(NODE_UNDEFINED);
            sgNode->setType((NODE_TYPE)recentAction.actionSpecificIntegers[0]);
            sgNode->props.fontSize = recentAction.actionSpecificIntegers[1];
            //node->props.shaderType = recentAction.actionSpecificIntegers[1];
            sgNode->props.vertexColor.x = recentAction.actionSpecificFloats[0];
            sgNode->props.vertexColor.y = recentAction.actionSpecificFloats[1];
            sgNode->props.vertexColor.z = recentAction.actionSpecificFloats[2];
            sgNode->props.nodeSpecificFloat = recentAction.actionSpecificFloats[3];
            
            sgNode->props.prevMatName = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[0]);
            sgNode->optionalFilePath = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[1]);
            sgNode->name = recentAction.actionSpecificStrings[2];
            if(!loadNode(sgNode,REDO_ACTION)){
                delete sgNode;
                returnValue = DO_NOTHING;
                break;
            }
            returnValue = ADD_TEXT_IMAGE_BACK;
            break;
        }
        case ACTION_TEXT_IMAGE_DELETE: {
            unselectObject(selectedNodeId);
            selectedNodeId = indexOfAction;
            returnValue = DELETE_ASSET;
            break;
        }
        case ACTION_APPLY_ANIM: {
            selectedNodeId = indexOfAction;
            setDataForFrame(currentFrame);
            applyAnimations(ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[0]),selectedNodeId);
            reloadKeyFrameMap();
            unselectObject(selectedNodeId);
            break;
        }
        default:
            return DO_NOTHING;
    }
    if(recentAction.actionType != ACTION_NODE_ADDED && recentAction.actionType != ACTION_TEXT_IMAGE_ADD)
        currentAction++;
    if(recentAction.actionType != ACTION_SWITCH_FRAME)
    setKeysForFrame(currentFrame);
    updateControlsOrientaion();
    
    if(recentAction.actionType == ACTION_CHANGE_NODE_KEYS && (nodes[indexOfAction]->getType() == NODE_LIGHT || nodes[indexOfAction]->getType() == NODE_ADDITIONAL_LIGHT))
        updateLightProperties(recentAction.frameId);
    
    if(recentAction.actionType == ACTION_CHANGE_JOINT_KEYS || recentAction.actionType == ACTION_CHANGE_NODE_JOINT_KEYS) {
        //dynamic_pointer_cast<AnimatedMeshNode>(nodes[indexOfAction]->node)->updatePartOfMeshCache(CHARACTER_RIG,recentAction.actionSpecificIntegers[0]);
    }

    return returnValue;
}
void SGAnimationScene::removeObject(u16 nodeIndex,bool deAllocScene)
{
    if(nodeIndex >= nodes.size())
        return;
    setControlsVisibility(false);
    setJointSpheresVisibility(false);
    
    if(nodes[nodeIndex]->getType() == NODE_ADDITIONAL_LIGHT) {
        popLightProps();
        resetMaterialTypes(false);
    }
    
    if(nodes[nodeIndex]->getType() != NODE_TEXT && nodes[nodeIndex]->getType() != NODE_ADDITIONAL_LIGHT)
        smgr->RemoveTexture(nodes[nodeIndex]->node->getActiveTexture());
    
    smgr->RemoveNode(nodes[nodeIndex]->node);
    delete nodes[nodeIndex];
    if(!deAllocScene) {
        nodes.erase(nodes.begin() + nodeIndex);
    }
    updateLightProperties(currentFrame);
    selectedNodeId = NOT_EXISTS;
    selectedNode = NULL;
    selectedJoint = NULL;
    isNodeSelected = isJointSelected = false;
    if(!deAllocScene)
        reloadKeyFrameMap();
    changeAction = true;
}
void SGAnimationScene::reloadKeyFrameMap()
{
    if(selectedNodeId != NOT_SELECTED) {
        SGNode *selectedMesh = nodes[selectedNodeId];
        bool searchPos = true,searchRot = false,searchScale = false;
        isKeySetForFrame.clear();
        searchRot = (nodes[selectedNodeId]->getType() == NODE_LIGHT) ? false:true;
        searchScale = (nodes[selectedNodeId]->getType() > NODE_LIGHT) ? true:false;
        if(searchPos){
            for(unsigned long i = 0; i < selectedMesh->positionKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->positionKeys[i].id,selectedMesh->positionKeys[i].id));
        }
        if(searchRot){
            for(unsigned long i = 0; i < selectedMesh->rotationKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->rotationKeys[i].id,selectedMesh->rotationKeys[i].id));
        }
        if(searchScale){
            for(unsigned long i = 0; i < selectedMesh->scaleKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->scaleKeys[i].id,selectedMesh->scaleKeys[i].id));
        }
        
        for( unsigned long i = 0; i < selectedMesh->joints.size(); i++){
            for(unsigned long j = 0; j < selectedMesh->joints[i]->rotationKeys.size(); j++) {
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->rotationKeys[j].id,selectedMesh->joints[i]->rotationKeys[j].id));
            }
            if(selectedMesh->getType() == NODE_TEXT) {
                for(unsigned long j = 0; j < selectedMesh->joints[i]->positionKeys.size(); j++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->positionKeys[j].id,selectedMesh->joints[i]->positionKeys[j].id));
                for(unsigned long j = 0; j < selectedMesh->joints[i]->scaleKeys.size(); j++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->scaleKeys[j].id,selectedMesh->joints[i]->scaleKeys[j].id));
            }
        }
    }
    else
        isKeySetForFrame.clear();
}
void SGAnimationScene::setCameraProperty(float fov , int resolutionType)
{
    cameraFOV = fov;
    cameraResolutionType = resolutionType;
    renderCamera->setFOVInRadians(cameraFOV * PI / 180.0f);
}
void SGAnimationScene::selectObject(int objectId)
{
    unselectObject(selectedNodeId);
    
    if(objectId < 0 || objectId >= nodes.size() || objectId == selectedNodeId)
        return;
    
    selectedNodeId = objectId;
    isNodeSelected = nodes[selectedNodeId]->props.isSelected = true;
    nodes[selectedNodeId]->props.prevMatName = nodes[selectedNodeId]->node->material->name;
    
    if(nodes[selectedNodeId]->getType() == NODE_RIG)
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (nodes[selectedNodeId]->getType() == NODE_TEXT)
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        nodes[selectedNodeId]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));

    nodes[selectedNodeId]->props.transparency = NODE_SELECTION_TRANSPARENCY;
    selectedNode = nodes[selectedNodeId];
    
    bool status = true;
    if(nodes[selectedNodeId]->getType() == NODE_RIG)
        status = displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node));
    else if (nodes[selectedNodeId]->getType() == NODE_TEXT)
        status = displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(nodes[selectedNodeId]->node), nodes[selectedNodeId]->props.fontSize/3.0);
    if(!status)
        unselectObject(selectedNodeId);
    
    if(nodes[selectedNodeId]->getType() == NODE_RIG || nodes[selectedNodeId]->getType() == NODE_TEXT){
    if(selectedNodeId != NOT_SELECTED)
        highlightJointSpheres();
        
    }else if(nodes[selectedNodeId]->getType() == NODE_CAMERA){
        nodes[selectedNodeId]->props.perVertexColor = false;
    }
    if(!isNodeSelected || (nodes[selectedNodeId]->getType() != NODE_RIG && nodes[selectedNodeId]->getType() != NODE_TEXT))
        setJointSpheresVisibility(false);
    
    updateControlsOrientaion();
    reloadKeyFrameMap();
}
void SGAnimationScene::unselectObject(int objectId)
{
    if(objectId != selectedNodeId)
        return;
    
    if(objectId >= 0 && objectId < nodes.size())
    {
        nodes[objectId]->props.transparency = 1.0;
        nodes[objectId]->props.isSelected = false;
        //nodes[objectId]->node->setMaterial(smgr->getMaterialByName(nodes[objectId]->props.prevMatName)); TODO
        selectedJoint = NULL;
        selectedNode = NULL;
        selectedJointId = NOT_EXISTS;
        selectedNodeId = NOT_EXISTS;
        isJointSelected = isNodeSelected = false;
        resetMaterialTypes(false);
        setJointSpheresVisibility(false);

        if(nodes[objectId]->getType() == NODE_CAMERA)
            nodes[objectId]->props.perVertexColor = true;
    }
    clearSelections();
    updateControlsOrientaion();
    reloadKeyFrameMap();
}
void SGAnimationScene::setMirrorState(MIRROR_SWITCH_STATE flag)
{
    SGScene::setMirrorState(flag);
    if(isJointSelected)
        highlightJointSpheres();
}
bool SGAnimationScene::switchMirrorState()
{
    SGAction action;
    action.actionType = ACTION_CHANGE_MIRROR_STATE;
    action.actionSpecificFlags.push_back(getMirrorState());
    addAction(action);
    setMirrorState((MIRROR_SWITCH_STATE)!getMirrorState());
    
        if(isJointSelected)
            highlightJointSpheres();
    
    return getMirrorState();
}
bool SGAnimationScene::removeAnimationForSelectedNodeAtFrame(int selectedFrame)
{
    bool isKeySetForNode = true;
    if(selectedNodeId != NOT_SELECTED) {
        
        if(isKeySetForFrame.find(selectedFrame) == isKeySetForFrame.end())
        {
            return false;
        }
        if(selectedFrame == 0){
            Vector3 initPos = Vector3(0.0);
            int type = nodes[selectedNodeId]->getType();
            if(type <= NODE_LIGHT){
                initPos = (type == NODE_CAMERA) ? Vector3(RENDER_CAM_INIT_POS_X,RENDER_CAM_INIT_POS_Y,RENDER_CAM_INIT_POS_Z):Vector3(-LIGHT_INIT_POS_X,LIGHT_INIT_POS_Y,LIGHT_INIT_POS_Z);
            }

            nodes[selectedNodeId]->setPosition(initPos, 0);
            nodes[selectedNodeId]->setRotation(nodes[selectedNodeId]->nodeInitialRotation, 0);
            if(selectedNodeId != NODE_LIGHT)
                nodes[selectedNodeId]->setScale( Vector3(1.0), 0);
            
            for(unsigned long i = 0; i < nodes[selectedNodeId]->joints.size(); i++){
                    nodes[selectedNodeId]->joints[i]->setRotation((nodes[selectedNodeId]->jointsInitialRotations[i]), 0);
            }
            
            setDataForFrame(selectedFrame);
//            updateLightWithRender();
            updateControlsOrientaion();
        }
        else {
            nodes[selectedNodeId]->removeAnimationInCurrentFrame(selectedFrame);
            setDataForFrame(selectedFrame);
            reloadKeyFrameMap();
            updateControlsOrientaion();
        }
        changeAction = true;
        return isKeySetForNode;
    }
    else
        return isKeySetForNode;
}
void SGAnimationScene::StoreDeleteObjectKeys(int nodeIndex)
{
    if(nodes[nodeIndex]->positionKeys.size())
        assetAction.nodePositionKeys = nodes[nodeIndex]->positionKeys;
    if(nodes[nodeIndex]->rotationKeys.size())
        assetAction.nodeRotationKeys = nodes[nodeIndex]->rotationKeys;
    if(nodes[nodeIndex]->scaleKeys.size())
        assetAction.nodeSCaleKeys = nodes[nodeIndex]->scaleKeys;
    if(nodes[nodeIndex]->visibilityKeys.size())
        assetAction.nodeVisibilityKeys = nodes[nodeIndex]->visibilityKeys;
    for (int i = 0; i < (int)nodes[nodeIndex]->joints.size(); i++) {
        //assetAction.jointsRotationKeys[i] = nodes[nodeIndex]->joints[i]->rotationKeys;
        assetAction.jointRotKeys[i] = nodes[nodeIndex]->joints[i]->rotationKeys;
        if(nodes[nodeIndex]->getType() == NODE_TEXT){
            assetAction.jointPosKeys[i] = nodes[nodeIndex]->joints[i]->positionKeys;
            assetAction.jointScaleKeys[i] = nodes[nodeIndex]->joints[i]->scaleKeys;
        }
    }
    
}
void SGAnimationScene::storeAddOrRemoveAssetAction(int actionType, int assetId, string optionalFilePath)
{
    if (actionType == ACTION_NODE_ADDED) {
        assetAction.drop();
        assetAction.actionType = ACTION_NODE_ADDED;
        assetAction.frameId = assetId;
        assetAction.objectIndex = actionObjectsSize + 1;
        addAction(assetAction);
    } else if(actionType == ACTION_NODE_DELETED) {
        assetAction.drop();
        assetAction.actionType = ACTION_NODE_DELETED;
        assetAction.frameId = nodes[selectedNodeId]->assetId;
        assetAction.objectIndex = nodes[selectedNodeId]->actionId;
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(nodes[selectedNodeId]->props.prevMatName));
        StoreDeleteObjectKeys(selectedNodeId);
        addAction(assetAction);
    } else if (actionType == ACTION_TEXT_IMAGE_DELETE|| actionType == ACTION_TEXT_IMAGE_ADD) {
        assetAction.drop();
        assetAction.actionType = actionType == ACTION_TEXT_IMAGE_DELETE?ACTION_TEXT_IMAGE_DELETE : ACTION_TEXT_IMAGE_ADD;
        int indexOfAsset = actionType == ACTION_TEXT_IMAGE_DELETE ? selectedNodeId : (int)nodes.size()-1;
        assetAction.objectIndex = nodes[indexOfAsset]->actionId;
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(nodes[indexOfAsset]->node->material->name));
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(nodes[indexOfAsset]->optionalFilePath));
        assetAction.actionSpecificStrings.push_back(nodes[indexOfAsset]->name);
        assetAction.actionSpecificFloats.push_back(nodes[indexOfAsset]->props.vertexColor.x);
        assetAction.actionSpecificFloats.push_back(nodes[indexOfAsset]->props.vertexColor.y);
        assetAction.actionSpecificFloats.push_back(nodes[indexOfAsset]->props.vertexColor.z);
        assetAction.actionSpecificFloats.push_back(nodes[indexOfAsset]->props.nodeSpecificFloat);
        assetAction.actionSpecificIntegers.push_back(nodes[indexOfAsset]->getType());
        assetAction.actionSpecificIntegers.push_back(nodes[indexOfAsset]->props.fontSize);
        if(actionType == ACTION_TEXT_IMAGE_DELETE)
            StoreDeleteObjectKeys(indexOfAsset);
        addAction(assetAction);
    } else if (actionType == ACTION_APPLY_ANIM) {
        assetAction.drop();
        assetAction.actionType = ACTION_APPLY_ANIM;
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(animFilePath));
        assetAction.objectIndex = nodes[selectedNodeId]->actionId;
        assetAction.frameId = animStartFrame;
        assetAction.actionSpecificIntegers.push_back(animTotalFrames);
        addAction(assetAction);
    }
    changeAction = true;
}
void SGAnimationScene::changeMeshProperty(float brightness, float specular, bool isLighting, bool isVisible, bool isChanged)
{
    if(propertyAction.actionType == ACTION_EMPTY){
        propertyAction.actionType = ACTION_CHANGE_PROPERTY_MESH;
        propertyAction.objectIndex = nodes[selectedNodeId]->actionId;
        propertyAction.frameId = currentFrame;
        propertyAction.actionSpecificFloats.push_back(nodes[selectedNodeId]->props.brightness);
        propertyAction.actionSpecificFloats.push_back(nodes[selectedNodeId]->props.shininess);
        propertyAction.actionSpecificFlags.push_back(nodes[selectedNodeId]->props.isLighting);
        propertyAction.actionSpecificFlags.push_back(nodes[selectedNodeId]->props.isVisible);
    }
    
    nodes[selectedNodeId]->setShaderProperties(brightness, specular, isLighting, isVisible, currentFrame);
    if(isChanged){
        propertyAction.actionSpecificFloats.push_back(brightness);
        propertyAction.actionSpecificFloats.push_back(specular);
        propertyAction.actionSpecificFlags.push_back(isLighting);
        propertyAction.actionSpecificFlags.push_back(isVisible);
        addAction(propertyAction);
        propertyAction.drop();
    }
    setDataForFrame(currentFrame);
}
void SGAnimationScene::changeCameraProperty(float fov , int resolutionType, bool isChanged)
{
    if(propertyAction.actionType == ACTION_EMPTY){
        propertyAction.actionType = ACTION_CHANGE_PROPERTY_CAMERA;
        propertyAction.actionSpecificFloats.push_back(cameraFOV);
        propertyAction.actionSpecificIntegers.push_back(cameraResolutionType);
    }
    setCameraProperty(fov, resolutionType);
    if(isChanged){
        propertyAction.actionSpecificFloats.push_back(cameraFOV);
        propertyAction.actionSpecificIntegers.push_back(cameraResolutionType);
        addAction(propertyAction);
        propertyAction.drop();
    }

}
void SGAnimationScene::updateLightCamera()
{
    if(nodes.size() > NODE_LIGHT) {
        nodes[NODE_LIGHT]->faceUserCamera(viewCamera,currentFrame);
        updateLightCam(nodes[NODE_LIGHT]->node->getAbsolutePosition());
    }
}
void SGAnimationScene::changeLightProperty(float red , float green, float blue, float shadow, bool isChanged)
{
    //TODO for all lights
    if(selectedNodeId == NOT_EXISTS)
        return;

    if(propertyAction.actionType == ACTION_EMPTY){
        propertyAction.actionType = ACTION_CHANGE_PROPERTY_LIGHT;
        propertyAction.actionSpecificFloats.push_back(ShaderManager::lightColor[0].x);
        propertyAction.actionSpecificFloats.push_back(ShaderManager::lightColor[0].y);
        propertyAction.actionSpecificFloats.push_back(ShaderManager::lightColor[0].z);
        propertyAction.actionSpecificFloats.push_back(ShaderManager::shadowDensity);
        
        changeKeysAction.drop();
        changeKeysAction.actionType = ACTION_CHANGE_NODE_KEYS;
        changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
    }
    
    if(nodes[selectedNodeId]->getType() == NODE_LIGHT)
        ShaderManager::shadowDensity = shadow;
    else if(nodes[selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT) {
        nodes[selectedNodeId]->props.nodeSpecificFloat = (shadow + 0.001) * 300.0;
    }
    
    //nodes[selectedNodeId]->props.vertexColor = Vector3(red,green,blue);
    Quaternion lightPropKey = Quaternion(red,green,blue,nodes[selectedNodeId]->props.nodeSpecificFloat);
    Vector3 mainLightColor = Vector3(red,green,blue);
    
    if(nodes[selectedNodeId]->getType() == NODE_LIGHT)
        nodes[selectedNodeId]->setScale(mainLightColor, currentFrame);
    else
        nodes[selectedNodeId]->setRotation(lightPropKey, currentFrame);
    
    updateLightProperties(currentFrame);
//    updateLightWithRender();
}
void SGAnimationScene::storeLightPropertyChangeAction(float red , float green , float blue , float shadowDensity)
{
    if(selectedNodeId == NOT_EXISTS)
        return;
    
    changeKeysAction.keys.push_back(nodes[selectedNodeId]->getKeyForFrame(currentFrame));
    
    propertyAction.actionSpecificFloats.push_back(red);
    propertyAction.actionSpecificFloats.push_back(green);
    propertyAction.actionSpecificFloats.push_back(blue);
    propertyAction.actionSpecificFloats.push_back(shadowDensity);
    addAction(changeKeysAction);
    changeKeysAction.drop();
    propertyAction.drop();
    changeAction = true;
}

void SGAnimationScene::updateLightProperties(int frameId)
{
    int index = 0;
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->getType() == NODE_LIGHT || nodes[i]->getType() == NODE_ADDITIONAL_LIGHT) {
            Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frameId,nodes[i]->positionKeys);
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frameId,nodes[i]->rotationKeys,true);
            Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frameId, nodes[i]->scaleKeys);
            
            Vector3 lightColor = (nodes[i]->getType() == NODE_LIGHT) ? Vector3(scale.x,scale.y,scale.z) : Vector3(rotation.x, rotation.y, rotation.z);
            float fadeDistance = (nodes[i]->getType() == NODE_LIGHT) ? 999.0 : rotation.w;
            
                if(index < ShaderManager::lightPosition.size())
                    ShaderManager::lightPosition[index] = position;
                else
                    ShaderManager::lightPosition.push_back(position);
                
                if(index < ShaderManager::lightColor.size())
                    ShaderManager::lightColor[index] = Vector3(lightColor.x,lightColor.y,lightColor.z);
                else
                    ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
                    
                if(index < ShaderManager::lightFadeDistances.size())
                    ShaderManager::lightFadeDistances[index] = fadeDistance;
                else
                    ShaderManager::lightFadeDistances.push_back(fadeDistance);
            
                nodes[i]->props.vertexColor = Vector3(lightColor.x,lightColor.y,lightColor.z);
                index++;
        }
    }
    
    while (index < ShaderManager::lightPosition.size()) {
        if(ShaderManager::lightPosition.size() > 1)
            ShaderManager::lightPosition.pop_back();
        if(ShaderManager::lightColor.size() > 1)
            ShaderManager::lightColor.pop_back();
        if(ShaderManager::lightFadeDistances.size() > 1)
            ShaderManager::lightFadeDistances.pop_back();
    }
}

void SGAnimationScene::addLight(SGNode *light)
{
    Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(currentFrame,light->rotationKeys,true);
    Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(currentFrame, light->scaleKeys);
    Vector3 lightColor = (light->getType() == NODE_LIGHT) ? Vector3(scale.x,scale.y,scale.z) : Vector3(rotation.x, rotation.y, rotation.z);
    float fadeDistance = (light->getType() == NODE_LIGHT) ? 999.0 : rotation.w;
    
    ShaderManager::lightPosition.push_back(light->node->getAbsolutePosition());
    ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
    ShaderManager::lightFadeDistances.push_back(fadeDistance);
}

void SGAnimationScene::popLightProps()
{
    /*
    if(ShaderManager::lightPosition.size() > 1) {
        ShaderManager::lightPosition.erase(ShaderManager::lightPosition.begin() + (ShaderManager::lightPosition.size() -1));
        ShaderManager::lightColor.erase(ShaderManager::lightColor.begin() + (ShaderManager::lightColor.size() - 1));
        ShaderManager::lightFadeDistances.erase(ShaderManager::lightFadeDistances.begin() + (ShaderManager::lightFadeDistances.size() -1));
    }
    */
    
    if(ShaderManager::lightPosition.size() > 1) {
        ShaderManager::lightPosition.pop_back();
        ShaderManager::lightColor.pop_back();
        ShaderManager::lightFadeDistances.pop_back();
    }

}

void SGAnimationScene::clearLightProps()
{
    while(ShaderManager::lightPosition.size() > 1)
        popLightProps();
}

NODE_TYPE SGAnimationScene::getSelectedNodeType()
{
    if(selectedNodeId != NOT_SELECTED)
        return nodes[selectedNodeId]->getType();
    
    return NODE_UNDEFINED;
}

void SGAnimationScene::resetMaterialTypes(bool isToonShader)
{
    int count = (int)ShaderManager::lightPosition.size();
    
    switch (count) {
        
        case 1: {
            commonType = SHADER_COMMON_L1;
            commonSkinType = SHADER_COMMON_SKIN_L1;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L1;
            break;
        }
        case 2: {
            commonType = SHADER_COMMON_L2;
            commonSkinType = SHADER_COMMON_SKIN_L2;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L2;
            break;
        }
        case 3: {
            commonType = SHADER_COMMON_L3;
            commonSkinType = SHADER_COMMON_SKIN_L3;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L3;
            break;
        }
        case 4: {
            commonType = SHADER_COMMON_L4;
            commonSkinType = SHADER_COMMON_SKIN_L4;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L4;
            break;
        }
        case 5: {
            commonType = SHADER_COMMON_L5;
            commonSkinType = SHADER_COMMON_SKIN_L5;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L5;
            break;
        }
        default:
            break;
    }
    
    for(int index = 0; index < nodes.size(); index++)
    {
        if(selectedNodeId == index) {
            //DO NOTHING
        } else {
            switch (nodes[index]->getType()) {
                case NODE_CAMERA: {
                    nodes[index]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
                    break;
                }
                case NODE_LIGHT:
                case NODE_SGM:
                case NODE_OBJ:
                case NODE_IMAGE: {
                    nodes[index]->node->setMaterial(smgr->getMaterialByIndex((isToonShader && nodes[index]->getType() != NODE_LIGHT) ? SHADER_TOON : commonType));
                    break;
                }
                case NODE_RIG: {
                    nodes[index]->node->setMaterial(smgr->getMaterialByIndex((isToonShader) ? SHADER_TOON_SKIN :commonSkinType));
                    break;
                }
                case NODE_TEXT: {
                    nodes[index]->node->setMaterial(smgr->getMaterialByIndex((isToonShader) ? SHADER_VERTEX_COLOR_SKIN_TOON: vertexColorTextType));
                    break;
                }
                    
                default:
                    break;
            }
        }
    }
    
}

/*
void SGAnimationScene::exportSceneAsFBX()
{
    FBXSceneCreator *fbxCreator = new FBXSceneCreator();
    fbxCreator->exportSceneAsFBX(this);
    delete fbxCreator;
    fbxCreator = NULL;
}
*/

vector<string> SGAnimationScene::generateSGFDFiles(int startFrame , int endFrame)
{
    vector<string> textureFileNames;
    if(startFrame == endFrame) {
        SGCloudRenderingHelper::writeFrameData(this, startFrame, "1.sgfd", textureFileNames);
        return textureFileNames;
    }
    
    for(int index = startFrame; index <= endFrame; index++) {
        string outFileName = to_string(index+1 - startFrame) + ".sgfd";
        if(index < totalFrames)
            setDataForFrame(index);
        SGCloudRenderingHelper::writeFrameData(this, index, outFileName, textureFileNames);

    }
    return textureFileNames;
    
}
