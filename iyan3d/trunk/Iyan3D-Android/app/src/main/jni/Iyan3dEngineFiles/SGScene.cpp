#include "HeaderFiles/SGScene.h"

#ifdef ANDROID
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm64/usr/include/android/asset_manager.h"
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm/usr/include/android/log.h"
#include "../opengl.h"
#endif

static Vector3 sgrJointDefaultColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
static Vector3 selectionColor = Vector3(SELECT_COLOR_R,SELECT_COLOR_G,SELECT_COLOR_B);
static Vector3 redColor = Vector3(1.0,0.2,0.2);
static Vector3 greenColor = Vector3(0.2,1.0,0.2);
static Vector3 blueColor = Vector3(0.2,0.2,1.0);
float constants::iOS_Version = 0;

SGScene::SGScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight, VIEW_TYPE fromView)
{
    prevRotX = prevRotY = prevRotZ = 0.0;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->smgr = smgr;
    this->viewType = fromView;
    initViewCamera();
    shaderMGR = new ShaderManager(smgr,device);
    Logger::log(INFO , "SGAnimationscene" , "shaders loaded");
    bgTexture = smgr->loadTexture("bgtex",constants::BundlePath +  "/bgImageforall.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    if(viewType != ASSET_SELECTION && viewType != ALL_ANIMATION_VIEW) {
    cmgr = new CollisionManager();
    isJointSelected = isNodeSelected = isControlSelected = false;
    selectedJointId = selectedNodeId = NOT_EXISTS;
    jointSpheres.clear();
    tPoseJoints.clear();
    
    controlType = MOVE;
    selectedControlId = NOT_EXISTS;
    selectedNode = NULL;
    touchTexture = smgr->createRenderTargetTexture("TouchTexture", TEXTURE_RGBA8, TEXTURE_BYTE, TOUCH_TEXTURE_WIDTH, TOUCH_TEXTURE_HEIGHT);
    planeFacingDirection[0] = Vector3(0,1,0);
    planeFacingDirection[1] = Vector3(0,0,1);
    planeFacingDirection[2] = Vector3(0,1,0);
    controlsPlane = new Plane3D();
    CameraViewHelper::readData();

    jointSphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",smgr->device);
    watermarkTexture = smgr->loadTexture("waterMarkTexture",constants::BundlePath + "/watermark.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    objLoader = new OBJMeshFileLoader;

    mirrorSwitchState = MIRROR_OFF;
    BoneLimitsHelper::init();
    createCircle();
        Logger::log(INFO , "SG SCene" , "Created circle");
    AutoRigJointsDataHelper::getTPoseJointsData(tPoseJoints);
    }
    swipeTiming = 0;
}
SGScene::~SGScene()
{    
    if(shaderMGR)
        delete shaderMGR;
    
    if(viewType != ASSET_SELECTION && viewType != ALL_ANIMATION_VIEW) {
    if(cmgr)
        delete cmgr;
    if(controlsPlane)
        delete controlsPlane;
    for(int i = 0; i < sceneControls.size();i++){
        if(sceneControls[i])
            delete sceneControls[i];
    }
    for(int i = 0; i < jointSpheres.size();i++){
        if(jointSpheres[i])
            delete jointSpheres[i];
    }
    if(objLoader)
    delete objLoader;
        
    if(viewCamera)
        viewCamera.reset();
    if(lightCamera)
        lightCamera.reset();
        
    if(rotationCircle)
        delete rotationCircle;
        
    sceneControls.clear();
    jointSpheres.clear();
    tPoseJoints.clear();
    actions.clear();
    }
}
void SGScene::createCircle(){
    rotationCircle = new SGNode(NODE_SGM);
    rotationCircle->node = smgr->createCircleNode(100,1.0,"RotationCircle");
    rotationCircle->node->setID(CIRCLE_NODE_ID);
    rotationCircle->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    rotationCircle->props.vertexColor = Vector3(0.0,1.0,0.0);
    rotationCircle->props.transparency = 1.0;
}
void SGScene::initViewCamera()
{
    viewCamera = smgr->createCameraNode("NoUniformCallbackFunctions");
    smgr->setActiveCamera(viewCamera);
    viewCamera->setPosition(Vector3(VIEW_CAM_INIT_POS_X,VIEW_CAM_INIT_POS_Y,VIEW_CAM_INIT_POS_Z));
    viewCamera->updateAbsoluteTransformation();
//    SGShaderCallBack::camPos = viewCamera->getPosition();
    //TODO: viewCamera->setID(3);
    cameraTarget = Vector3(0.0);
    viewCamera->setTarget(Vector3(0.0,10.0,0.0));//cameraTarget);
    viewCamera->setFOVInRadians(65.0 * (PI / 180));
    float aspectRatio = this->screenWidth/this->screenHeight;
    viewCamera->setAspectRatio(aspectRatio);
    cameraRadius = 20.0;
    testPlane.setPositionAndNormal(Vector3(0.0,0.0,100.0),Vector3(0.0,1.0,0.0));
}
bool SGScene::calculateControlMovements(Vector2 curPoint,Vector2 prevTouchPoint,Vector3 &outputValue,bool isSGJoint)
{
    Vector3 center;
    if(isJointSelected)
        center = selectedJoint->jointNode->getAbsoluteTransformation().getTranslation();
    else if(isNodeSelected)
        center = selectedNode->node->getAbsoluteTransformation().getTranslation();
    else
        return false;
    
    Vector3 cameraDir = Vector3(0.0,1.0,0.0);
    Mat4 rotmat;
    rotmat.setRotationRadians(viewCamera->getRotationInDegrees() * DEGTORAD);
    rotmat.rotateVect(cameraDir);
    
    controlsPlane->setPositionAndNormal(center,cameraDir);
    
    Vector2 p1;
    p1.x = prevTouchPoint.x; p1.y = prevTouchPoint.y;
    Vector2 p2;
    p2.x = curPoint.x;	p2.y = curPoint.y;
    
    Line3D oldRay = cmgr->getRayFromScreenCoordinates(prevTouchPoint, smgr->getActiveCamera(), screenWidth, screenHeight);
    Line3D newRay = cmgr->getRayFromScreenCoordinates(curPoint, smgr->getActiveCamera(), screenWidth, screenHeight);
    
    Vector3 oldPos;
    Vector3 newPos;
    Vector3 oldRayDir = (oldRay.end - oldRay.start).normalize();
    Vector3 newRayDir = (newRay.end - newRay.start).normalize();
    controlsPlane->getIntersectionWithLine(oldRay.start,oldRayDir,oldPos);
    controlsPlane->getIntersectionWithLine(newRay.start,newRayDir,newPos);
    if(controlType == MOVE){ // position and scale calculations are same
        Vector3 delta = newPos - oldPos;
//        if(delta.getLength() > 5.0f) {
//            delta = delta.normalize() * 5.0f;
//        }
        outputValue.x = ((selectedControlId == X_MOVE) ? 1.0 : 0.0) * delta.x;
        outputValue.y = ((selectedControlId == Y_MOVE) ? 1.0 : 0.0) * delta.y;
        outputValue.z = ((selectedControlId == Z_MOVE) ? 1.0 : 0.0) * delta.z;
        return true;
    }
    else if(controlType == ROTATE){

        controlsPlane->setPositionAndNormal(center, (selectedControlId == Z_ROTATE) ? Vector3(1, 0, 0) : planeFacingDirection[selectedControlId % 3]);
        
        Vector3 parentToNewPos = (newPos - center).normalize();
        Vector3 parentToOldPos = (oldPos - center).normalize();
        
        Quaternion delta = MathHelper::rotationBetweenVectors(parentToNewPos,parentToOldPos);
        
//        Vector3 o;
//        delta.toEuler(o);
//
//        if(selectedControlId == Z_ROTATE && o.x > 0.04) {
//            o.x = 0.04; o.y = 0.0; o.z = 0.0;
//            circleTouchPoint = oldPos;
//        } else if(selectedControlId == X_ROTATE && o.y > 0.03) {
//            o.y = 0.04; o.z = 0.0;  o.x = 0.0;
//            circleTouchPoint = oldPos;
//        } else if(selectedControlId == Y_ROTATE && o.z > 0.03) {
//            o.z = 0.04; o.x = 0.0; o.y = 0.0;
//            circleTouchPoint = oldPos;
//        } else if(selectedControlId == Z_ROTATE && o.x < -0.04) {
//            o.x = -0.04; o.y = 0.0; o.z = 0.0;
//            circleTouchPoint = oldPos;
//        } else if(selectedControlId == X_ROTATE && o.y < -0.04) {
//            o.y = -0.04; o.z = 0.0;  o.x = 0.0;
//            circleTouchPoint = oldPos;
//        } else if(selectedControlId == Y_ROTATE && o.z < -0.04) {
//            o.z = -0.04; o.x = 0.0; o.y = 0.0;
//            circleTouchPoint = oldPos;
//        } else {
//            circleTouchPoint = newPos;
//        }
//        Logger::log(INFO, "SGScene", "Reset Deg " + to_string(o.x) + " " + to_string(o.y) + " " + to_string(o.z) + " control " + to_string(selectedControlId) );
//
//        delta = Quaternion(o);

        Vector3 nodeRot;
        if(isJointSelected || !isSGJoint) {
            if(!isSGJoint){
                shared_ptr<Node> jointNode = selectedNode->node;
                MathHelper::getGlobalQuaternion((jointNode)).toEuler(nodeRot);
                //jointNode.reset();
            }else{
                shared_ptr<JointNode> jointNode = selectedJoint->jointNode;
                MathHelper::getGlobalQuaternion(jointNode).toEuler(nodeRot);
                //jointNode.reset();
            }
            nodeRot = nodeRot * RADTODEG;
            Quaternion jointGlobalRot = MathHelper::RotateNodeInWorld(nodeRot, delta);
            if(!isSGJoint){
                    delta = MathHelper::irrGetLocalQuaternion((selectedNode->node),jointGlobalRot);
            }else{
                delta = MathHelper::irrGetLocalQuaternion(selectedJoint->jointNode,jointGlobalRot);
            }
        }
        else if(selectedNode){
            selectedNode->node->updateAbsoluteTransformation();
            nodeRot = selectedNode->node->getRotationInDegrees();
            delta =  MathHelper::RotateNodeInWorld(nodeRot,delta);
        }
        delta.toEuler(outputValue);

//        if(prevRotX == 0.0 && prevRotY == 0.0 && prevRotZ == 0.0) {
//            prevRotX = outputValue.x;
//            prevRotY = outputValue.y;
//            prevRotZ = outputValue.z;
//        }
//
//        float diffX = (outputValue.x - prevRotX) * RADTODEG;
//        float diffY = (outputValue.y - prevRotY) * RADTODEG;
//        float diffZ = (outputValue.z - prevRotZ) * RADTODEG;
//
//        if(diffX >= 360)
//            diffX -= 360;
//        if(diffY >= 360)
//            diffY -= 360;
//        if(diffZ >= 360)
//            diffZ -= 360;
//
//        if(diffX < 0)
//            diffX += 360;
//        if(diffY < 0)
//            diffY += 360;
//        if(diffZ < 0)
//            diffZ += 360;
//
//        if(abs(diffX) > 3)
//            diffX = (diffX > 0 ? 1 : -1) * 3.0f;
//        if(abs(diffY) > 3)
//            diffY = (diffY > 0 ? 1 : -1) * 3.0f;
//        if(abs(diffZ) > 3)
//            diffZ = (diffZ > 0 ? 1 : -1) * 3.0f;

//        outputValue.x = prevRotX + diffX * DEGTORAD;
//        outputValue.y = prevRotY + diffY * DEGTORAD;
//        outputValue.z = prevRotZ + diffZ * DEGTORAD;

//        if(!isJointSelected && (prevRotX != 0.0 || prevRotY != 0.0 || prevRotZ != 0.0)) {
//            float diffX = outputValue.x - prevRotX;
//            float diffY = outputValue.y - prevRotY;
//            float diffZ = outputValue.z - prevRotZ;
//
//            if((abs(diffX) > 0.1 && abs(diffX) < 3.0) || (abs(diffY) > 0.1 && abs(diffY) < 3.0) || (abs(diffZ) > 0.1 && abs(diffZ) < 3.0)) {
//                circleTouchPoint = oldPos;
//                outputValue = Vector3(prevRotX, prevRotY, prevRotZ);
//            } else if(abs(diffX) > 6.28 || abs(diffY) > 6.28 || abs(diffZ) > 6.28) {
//                circleTouchPoint = oldPos;
//                outputValue = Vector3(prevRotX, prevRotY, prevRotZ);
//            }
//          }

//        Vector3 v = outputValue * RADTODEG;
//
//
//        while(!((v.x > 0 && v.x < 360) || (v.y > 0 && v.y < 360) || (v.z > 0 && v.z < 360))) {
//            if(v.x >= 360)
//                v.x -= 360;
//            if(v.y >= 360)
//                v.y -= 360;
//            if(v.z >= 360)
//                v.z -= 360;
//
//            if(v.x <= 0)
//                v.x += 360;
//            if(v.y <= 0)
//                v.y += 360;
//            if(v.z <= 0)
//                v.z += 360;
//
//            Logger::log(INFO,"SGScene","Reset Deg" + to_string(v.x)+" "+to_string(v.y)+" " +to_string(v.z));
//        }
//
//        outputValue = v * DEGTORAD;

            prevRotX = outputValue.x;
            prevRotY = outputValue.y;
            prevRotZ = outputValue.z;
        return true;

    }
    return false;
}
void SGScene::swipeProgress(float angleX , float angleY)
{
    if(isControlSelected || swipeTiming < 3) {
        swipeTiming++;
        return;
    }
    
    Vector3 camForward = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    Vector3 cameraPos = viewCamera->getPosition();
    Vector3 camOldPos = cameraPos;
    
    float angleBWCamYAndUpVec = camForward.dotProduct(viewCamera->getUpVector());
    if(angleBWCamYAndUpVec < CAM_UPVEC_UPREAL_MAX_DIF && angleBWCamYAndUpVec > -CAM_UPVEC_UPREAL_MAX_DIF)
        viewCamera->setUpVector(camUpReal);
    cameraPos += camUpReal * (-angleY / CAM_SWIPE_SPEED) + camRight * (-angleX / CAM_SWIPE_SPEED);
    viewCamera->setPosition(cameraPos);
    float swipeAdjustDis = cameraPos.getDistanceFrom(viewCamera->getTarget()) - camOldPos.getDistanceFrom(viewCamera->getTarget());
    if(swipeAdjustDis > 0.0){
        Vector3 camNewFwd = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
        cameraPos -= (camNewFwd * -(swipeAdjustDis));
        viewCamera->setPosition(cameraPos);
    }
    updateControlsOrientaion();
}
void SGScene::panBegan(Vector2 touch1, Vector2 touch2)
{
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    panBeganPoints[0] = touch1; panBeganPoints[1] = touch2;
    previousTarget = cameraTarget;
    previousRadius = cameraRadius;
    prevZoomDif = 0.0;
}
void SGScene::panProgress(Vector2 touch1, Vector2 touch2)
{
    
    Vector3 camPos = viewCamera->getPosition();
    Vector3 targetToCam = (viewCamera->getPosition() - viewCamera->getTarget()).normalize();
    Vector3 camForward = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    
    //ZOOM
    float startDist = prevTouchPoints[0].getDistanceFrom(prevTouchPoints[1]);
    float endDist = touch1.getDistanceFrom(touch2);
    float zoomDif = ((startDist - endDist) / CAM_ZOOM_SPEED);
    if((camPos + (targetToCam * zoomDif)).getDistanceFrom(viewCamera->getTarget()) > CAM_ZOOM_IN_LIMIT)
        camPos += (targetToCam * zoomDif);
    
    //MOVE
    Vector2 centerStart = (prevTouchPoints[0] + prevTouchPoints[1]) / 2.0;
    Vector2 centerEnd = (touch1 + touch2) / 2.0;
    Vector2 moveDif = (centerEnd - centerStart) / CAM_PAN_SPEED;
    cameraTarget = viewCamera->getTarget() + camRight*moveDif.x + camUpReal*moveDif.y;
    viewCamera->setTarget(cameraTarget);
    camPos += (camRight * moveDif.x) + (camUpReal * moveDif.y);
    
    viewCamera->setPosition(camPos);
    
    previousRadius = cameraRadius;
    previousTarget = cameraTarget;
    prevZoomDif = zoomDif;
    
    //ROTATE
    float startRotation = atan2(prevTouchPoints[1].y - prevTouchPoints[0].y, prevTouchPoints[1].x - prevTouchPoints[0].x);
    float endRotation = atan2(touch2.y - touch1.y, touch2.x - touch1.x);
    float rotationDif = endRotation - startRotation;
    Vector3 cameraRotation = viewCamera->getRotation();
    cameraRotation.z += rotationDif;
    Quaternion quat;
    quat.fromAngleAxis(rotationDif, camForward);  //This takes rotation in rad, so it's already right
    Mat4 mat = quat.getMatrix();
    mat.rotateVect(camUpReal);
    mat.rotateVect(camRight);
    viewCamera->setUpVector(camUpReal);
    
    prevTouchPoints[0] = touch1; prevTouchPoints[1] = touch2;
    updateControlsOrientaion();
}
bool SGScene::createJointSpheres(int additionalJoints)
{
    if(additionalJoints <= 0)
        return false;
    int maxCount = (int)jointSpheres.size() + additionalJoints;
    for(int i = (int)jointSpheres.size();i < maxCount;i++){
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
        sgNode->props.vertexColor = sgrJointDefaultColor;
        jointSpheres.push_back(sgNode);
        sgNode->node->setID(JOINT_SPHERES_START_ID + (int)jointSpheres.size() - 1);
    }
    return true;
}
void SGScene::initLightCamera(Vector3 position)
{
    lightCamera = smgr->createCameraNode("LightUniforms");
    lightCamera->setFOVInRadians(150.0 * DEGTORAD);
    lightCamera->setNearValue(5.0);
    updateLightCam(position);
}
void SGScene::updateLightCam(Vector3 position)
{
    lightCamera->setPosition(position);
    lightCamera->setTarget(Vector3(0.0));
    ShaderManager::lighCamViewMatrix = lightCamera->getViewMatrix();
    ShaderManager::lighCamProjMatrix = lightCamera->getProjectionMatrix();
    ShaderManager::camPos = smgr->getActiveCamera()->getAbsolutePosition();
}
bool SGScene::displayJointSpheresForNode(shared_ptr<AnimatedMeshNode> animNode , float scaleValue){
    
    bool status;
    removeJointSpheres();
    int bonesCount = (int)animNode->getJointCount();
    if(bonesCount > jointSpheres.size())
        status = createJointSpheres(bonesCount - (int)jointSpheres.size());
    
    for(int i = 0;i < bonesCount;i++){
        shared_ptr<JointNode> jointNode = animNode->getJointNode(i);
        jointSpheres[i]->node->setParent(jointNode);
        if(bonesCount != tPoseJoints.size())
            jointSpheres[i]->node->setScale((animNode->getScale() * scaleValue)/animNode->getScale());
        else
            jointSpheres[i]->node->setScale(Vector3(tPoseJoints[i].sphereRadius/jointNode->getAbsoluteTransformation().getScale().x) * animNode->getScale());
    }
    setJointSpheresVisibility(true);
    displayJointsBasedOnSelection();
    return status;
    //animNode.reset();
}
void SGScene::removeJointSpheres()
{
    for(int i = 0; i < jointSpheres.size();i++){
        if(jointSpheres[i])
            delete jointSpheres[i];
    }
    jointSpheres.clear();
}
void SGScene::setJointSpheresVisibility(bool visibilityFlag)
{
    visibilityFlag = (selectedNodeId == NOT_EXISTS)?false:visibilityFlag;
    
    if (selectedNodeId != NOT_EXISTS && selectedNode)
        visibilityFlag = (selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT) ? visibilityFlag : false;
        
    if(jointSpheres.size())
        jointSpheres[0]->node->setVisible(false);
    for(int i=1;i<jointSpheres.size();i++){
        if(selectedNodeId != NOT_EXISTS && selectedNode && (selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT) && i < selectedNode->joints.size())
            jointSpheres[i]->node->setVisible(visibilityFlag);
        else
            jointSpheres[i]->node->setVisible(false);
    }
}
void SGScene::highlightJointSpheres(){
    if(isNodeSelected  || isJointSelected){
        setJointSpheresVisibility(true);
        for(int i = 0; i< jointSpheres.size(); i++)
            if(jointSpheres[i]){
                int mirrorjointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
                if((i == selectedJointId) || (getMirrorState() && (i == mirrorjointId))) {
                    jointSpheres[i]->props.vertexColor = selectionColor;
                }else{
                    jointSpheres[i]->props.vertexColor = sgrJointDefaultColor;
                }
            }
    }
    updateControlsOrientaion();
}
void SGScene::displayJointsBasedOnSelection()
{
    for(int i = 0;i < jointSpheres.size();i++){
        //jointSpheres[i]->props.vertexColor = (i == selectedJointId) ? selectionColor :sgrJointDefaultColor;
        if(i == selectedJointId){
            selectedJoint = selectedNode->joints[i];
        }
    }
}
void SGScene::setJointsUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(jointSpheres[nodeID - JOINT_SPHERES_START_ID],matName);
}
void SGScene::setRotationCircleUniforms(int nodeID,string matName){
    shaderMGR->setUniforms(rotationCircle,matName);
}
bool SGScene::isJointTransparent(int nodeID,string matName)
{
    return (jointSpheres[nodeID - JOINT_SPHERES_START_ID]->props.transparency < 1.0);
}
void SGScene::setControlsUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(sceneControls[nodeID - CONTROLS_START_ID],matName);
}
bool SGScene::isControlsTransparent(int nodeID,string matName)
{
    return (sceneControls[nodeID - CONTROLS_START_ID]->props.transparency < 1.0);
}
void SGScene::drawJointsSpheresForRTT(SGNode* sgNode,bool enableDepthTest)
{
// Draw Joints above nodes if nodeSelected
    if(jointSpheres.size() <= 0)
        return;
    vector<Vector3> vertexColors;
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++) {
        vertexColors.push_back(jointSpheres[j]->props.vertexColor);
        jointSpheres[j]->props.vertexColor = Vector3(selectedNodeId/255.0,j/255.0,1.0);
        smgr->RenderNode(smgr->getNodeIndexByID(jointSpheres[j]->node->getID()), (j == 0) ? enableDepthTest:false);
    }
    // Reset joints
    for(int j = 0;j < (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->getJointCount();j++)
        jointSpheres[j]->props.vertexColor = vertexColors[j];
    vertexColors.clear();
}
void SGScene::removeActions(){
    if(currentAction < 0)
        currentAction = 0;
    
    while(actions.size()>currentAction)
        actions.pop_back();
}

void SGScene::addAction(SGAction& action){
    if(currentAction < 0)
        currentAction = 0;
    
    actions.push_back(action);
    while(actions.size() > MAXUNDO){
        actions.erase(actions.begin());
        currentAction--;
    }
    currentAction++;
}


void SGScene::initControls()
{
    sceneControls.clear();
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
    setControlsVisibility();
    
    if(viewType == AUTO_RIG)
        setControlsVariations();
}
void SGScene::setControlsVariations()
{
    for (int i = 0; i < sceneControls.size(); i++) {
        Vector3 controlOriginalScale = sceneControls[i]->node->getScale();
        sceneControls[i]->props.transparency = 0.5;
        sceneControls[i]->node->setScale(controlOriginalScale/1.1);
    }
}

MIRROR_SWITCH_STATE SGScene::getMirrorState(){
    return mirrorSwitchState;
}
void SGScene::setMirrorState(MIRROR_SWITCH_STATE flag){
    mirrorSwitchState = flag;
}
void SGScene::drawGrid()
{
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    for (int i = -20; i <= 20; i+= 5) {
        smgr->draw3DLine(Vector3(i, 0, -20), Vector3(i, 0, 20), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
        smgr->draw3DLine(Vector3(-20, 0, i), Vector3(20, 0, i), Vector3(0.6,0.6,1.0),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    smgr->draw3DLine(Vector3(-20, 0, 0), Vector3(20, 0, 0), Vector3(1.0,0.2,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    smgr->draw3DLine(Vector3(0, 0, -20), Vector3(0, 0, 20), Vector3(0.2,1.0,0.2),mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
}
void SGScene::drawMoveControlLine(Vector3 nodePos){
    
    Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
    float farValue = viewCamera->getFarValue();
    if(selectedControlId == X_MOVE){
        smgr->draw3DLine(Vector3(-farValue,nodePos.y,nodePos.z),Vector3(farValue,nodePos.y,nodePos.z),redColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else if(selectedControlId == Y_MOVE){
        smgr->draw3DLine(Vector3(nodePos.x,-farValue,nodePos.z),Vector3(nodePos.x,farValue,nodePos.z),blueColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
    else{
        smgr->draw3DLine(Vector3(nodePos.x,nodePos.y,-farValue), Vector3(nodePos.x,nodePos.y,farValue),greenColor,mat,SHADER_COLOR_mvp,SHADER_COLOR_vertexColor,SHADER_COLOR_transparency);
    }
}
void SGScene::renderControls()
{
    if(!isNodeSelected)
        return;
    setControlsVisibility();
    smgr->clearDepthBuffer();
    int controlStartIndex = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        if(i == selectedControlId){
            sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            sceneControls[i]->props.vertexColor = Vector3(0.0,1.0,0.0);
            sceneControls[i]->props.transparency = 1.0;
        }else{
            sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
        }
        int nodeIndex = smgr->getNodeIndexByID(sceneControls[i]->node->getID());
//        printf("\n nodeIndex %d controlStartIndex %d ",nodeIndex,controlStartIndex);
        smgr->RenderNode(nodeIndex,(i == controlStartIndex)?true:false);
    }
}
void SGScene::updateControlsMaterial()
{
    setControlsVisibility();
    if(!isControlSelected)
        return;
    int controlStartToVisible = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndToVisible = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    for(int i = controlStartToVisible;i <= controlEndToVisible;i++){
        if(i == selectedControlId)
            sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        else
            sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    }
    if(viewType == AUTO_RIG)
        setControlsVariations();
}
void SGScene::updateControlsOrientaion(bool forRTT)
{
    setControlsVisibility();
    
    if(!isNodeSelected)
        return;
    int controlStartIndex = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    Vector3 nodePos;
    if(isJointSelected && selectedJoint)
        nodePos = selectedJoint->jointNode->getAbsoluteTransformation().getTranslation();
    else if( isNodeSelected && selectedNode)
        nodePos = selectedNode->node->getAbsoluteTransformation().getTranslation();
    
    float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
    float ctrlScale = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_SCALE);
    
    ctrlScale = forRTT ? (ctrlScale * 1.5) : ctrlScale;
    
    float ctrlDistanceFromNode = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
    
    ctrlDistanceFromNode = forRTT ? (ctrlDistanceFromNode/1.5) : ctrlDistanceFromNode;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        sceneControls[i]->node->setScale(Vector3(ctrlScale));
        switch(i%3){
            case 0:
                sceneControls[i]->node->setPosition(Vector3(nodePos) + Vector3(ctrlDistanceFromNode,0,0));
            break;
            case 1:
                sceneControls[i]->node->setPosition(Vector3(nodePos) + Vector3(0,ctrlDistanceFromNode,0));
            break;
            case 2:
                sceneControls[i]->node->setPosition(Vector3(nodePos) + Vector3(0,0,ctrlDistanceFromNode));
            break;
        }
        if(viewType == AUTO_RIG)
            setControlsVariations();
        
        sceneControls[i]->node->updateAbsoluteTransformation();
    }
}
void SGScene::changeCameraView(CAMERA_VIEW_MODE mode){
    Vector3 cmapos = CameraViewHelper::getCameraPosition(mode);
    viewCamera->setPosition(CameraViewHelper::getCameraPosition(mode));
    viewCamera->updateAbsoluteTransformation();
    cameraTarget = Vector3(0.0);
    cameraRadius = 20.0;
    viewCamera->setTarget(cameraTarget);
    viewCamera->setUpVector(Vector3(0.0,1.0,0.0));
    viewCamera->updateAbsoluteTransformation();
    Vector3 camForward = (viewCamera->getTarget() - viewCamera->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(viewCamera->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    viewCamera->setUpVector(camUpReal);
}

void SGScene::setControlsVisibility(bool isVisible)
{
    if(sceneControls.size() == 0)
        return;
    
    if(viewType == ANIMATION){
        if(isNodeSelected && selectedNode->getType() == NODE_LIGHT)
            controlType = MOVE;
    }
    
    int controlStartToVisible = NOT_EXISTS,controlEndToVisible = NOT_EXISTS;
    if(isNodeSelected && isVisible){
        if(controlType == MOVE){
            controlStartToVisible = X_MOVE;
            controlEndToVisible = Z_MOVE;
        }else if(controlType == ROTATE){
            controlStartToVisible = X_ROTATE;
            controlEndToVisible = Z_ROTATE;
        } else {
            for (int i = 0; i < sceneControls.size(); i++) {
                sceneControls[i]->node->setVisible(false);
            }
            return;
        }
    }
    for(int i = X_MOVE;i <= Z_ROTATE;i++){
        isVisible = (i >= controlStartToVisible && i <= controlEndToVisible) ? true:false;
        sceneControls[i]->node->setVisible(isVisible);
    }
}

void SGScene::getCtrlColorFromTouchTexture(Vector2 touchPosition)
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
        Logger::log(ERROR,"SGScene::getCtrlColorFromTouchTexture","Wrong Rtt color");
    }
}
bool SGScene::isIKJoint(int jointId)
{
    return (jointId == HAND_LEFT || jointId == HAND_RIGHT || jointId == LEG_LEFT || jointId == LEG_RIGHT || jointId == HIP || jointId == LEG_LEFT1 || jointId == LEG_RIGHT1);
}
void SGScene::limitPixelCoordsWithinTextureRange(float texWidth,float texHeight,float &xCoord,float &yCoord)
{
    xCoord = (xCoord >= texWidth)?(texWidth-1.0):xCoord;
    yCoord = (yCoord >= texHeight)?(texHeight-1.0):yCoord;
}
void SGScene::clearSelections()
{
    isNodeSelected = isJointSelected = false;
    selectedNode = NULL;
    selectedJoint = NULL;
    selectedNodeId = selectedJointId = NOT_SELECTED;
}
void SGScene::drawCircle()
{
    if(controlType == ROTATE && selectedControlId != NOT_SELECTED && selectedNode) {
        Vector3 circleRotation = Vector3(90.0,0.0,0.0);
        Vector3 circleAxis = Vector3(1.0,0.0,1.0);
        if(selectedControlId == Y_ROTATE){
            circleRotation = Vector3(0.0,0.0,0.0);
            circleAxis = Vector3(1.0,1.0,0.0);
        }else if(selectedControlId == Z_ROTATE){
            circleRotation = Vector3(0.0,90.0,0.0);
            circleAxis = Vector3(0.0,1.0,1.0);
        }
        Vector3 nodePos = Vector3(0.0);
        if(selectedJoint)
            nodePos = selectedJoint->jointNode->getAbsolutePosition();
        else if(selectedNode)
            nodePos = selectedNode->node->getAbsolutePosition();
        else
            return;
        
        float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
        float radius = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
        float longLineLength = ROTATION_CIRCLE_LONG_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS) ,shorLineLength = ROTATION_CIRCLE_LINE_LENGTH * (radius/ROTATION_CIRCLE_LINE_MARKER_RADIUS);
        rotationCircle->node->setPosition(nodePos);
        rotationCircle->node->setScale(Vector3(radius));
        rotationCircle->node->setRotationInDegrees(circleRotation);
        rotationCircle->node->setVisible(true);
        smgr->RenderNode(smgr->getNodeIndexByID(rotationCircle->node->getID()),true);
        
        Vector3 lineVect = nodePos - circleTouchPoint;
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
