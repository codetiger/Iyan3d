#include "HeaderFiles/SGAutoRigScene.h"
#include "../SGEngine2/Loaders/OBJMeshFileLoader.h"
string constants::BundlePathRig = " ";
string constants::CachesStoragePathRig = " ";
std::map<int, RigKey> rigKeys;
std::map<int, RigKey>::iterator rigKeyIterator;
std::map<int, SGNode*> envelopes;
std::map<int, vector<InfluencedObject> > influencedVertices;    //For each joint
std::map<int, vector<InfluencedObject> > influencedJoints;      //For each vertex
SGAction moveAction,_scaleAction,addJointAction;

SGAutoRigScene::SGAutoRigScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight):SGScene(device,smgr,screenWidth,screenHeight)
{
    viewType = AUTO_RIG;
    initControls();
    this->smgr = smgr;
    isRTTCompleted =  isOBJimported = false;
    string textureFileName = constants::BundlePath + "/" + "grandfather.png";
    constants::BundlePath = constants::BundlePathRig;
    constants::CachesStoragePath = constants::CachesStoragePathRig;
    sceneMode = RIG_MODE_OBJVIEW;
    objSGNode = new SGNode(NODE_OBJ);
    sgrSGNode = NULL;
    clearNodeSelections();
    shadowTexture = smgr->createRenderTargetTexture("shadowTexture", TEXTURE_DEPTH32, TEXTURE_BYTE, SHADOW_TEXTURE_AUTORIG_WIDTH, SHADOW_TEXTURE_AUTORIG_WIDTH);
    ShaderManager::shadowDensity = 0.0;
    initLightCamera(Vector3(0.0,10.0,10.0));
    updateLightProperties();
    initViewCamera();
}
SGAutoRigScene::~SGAutoRigScene()
{
    moveAction.drop();
    if(objSGNode)
    delete objSGNode;
    if(sgrSGNode)
        delete sgrSGNode;
    
    for(std::map<int, SGNode *>::iterator it = envelopes.begin(); it!=envelopes.end(); it++)
    {
        if (it->second) {
            delete it->second;
        }
    }
    envelopes.clear();
    
    removeRigKeys();
    
    if(smgr)
        delete smgr;
}

void SGAutoRigScene::removeRigKeys()
{
    for (int i = 0; i < tPoseJoints.size(); i++) {
        if(rigKeys.find(tPoseJoints[i].id) != rigKeys.end()){
            if(rigKeys[tPoseJoints[i].id].bone){
                if(rigKeys[tPoseJoints[i].id].bone->node)
                    smgr->RemoveNode(rigKeys[tPoseJoints[i].id].bone->node);
                delete rigKeys[tPoseJoints[i].id].bone;
                rigKeys[tPoseJoints[i].id].bone = NULL;
            }
            if(rigKeys[tPoseJoints[i].id].sphere){
                if(rigKeys[tPoseJoints[i].id].sphere->node)
                    smgr->RemoveNode(rigKeys[tPoseJoints[i].id].sphere->node);
                delete rigKeys[tPoseJoints[i].id].sphere;
                rigKeys[tPoseJoints[i].id].sphere = NULL;
            }
            if(rigKeys[tPoseJoints[i].id].referenceNode){
                if(rigKeys[tPoseJoints[i].id].referenceNode->node)
                    smgr->RemoveNode(rigKeys[tPoseJoints[i].id].referenceNode->node);
                delete rigKeys[tPoseJoints[i].id].referenceNode;
                rigKeys[tPoseJoints[i].id].referenceNode = NULL;
            }
        }
    }
    rigKeys.clear();
    tPoseJoints.clear();
}

void SGAutoRigScene::updateLightProperties()
{
    int index = 0;
           Vector3 position = lightCamera->getPosition();
            
            Vector3 lightColor = Vector3(1.0);
            float fadeDistance = 999.0;
            
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
}


void SGAutoRigScene::touchBeganRig(Vector2 curTouchPos)
{
    moveAction.drop();
    isMoved = false;
    prevTouchPoints[0] = curTouchPos;
    updateControlsOrientaion();
    touchPosForCtrlPos = curTouchPos;
    rttControlSelectionAutoRig(touchPosForCtrlPos);
    if(shaderMGR->deviceType == METAL){
        rttControlSelectionAutoRig(touchPosForCtrlPos);
        getCtrlColorFromTouchTexture(touchPosForCtrlPos);
    }
    moveAction.actionType = (sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId >= 0) ? ACTION_CHANGE_SKELETON_KEYS : ACTION_CHANGE_SGR_KEYS;
    moveAction.keys = getNodeKeys();
    xAcceleration = yAcceleration = 0.0;
}
void SGAutoRigScene::rttControlSelectionAutoRig(Vector2 touchPosition)
{
    int controlStartIndex = (controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    rotationCircle->node->setVisible(false);
    bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->setRenderTarget(touchTexture,true,true,false,Vector4(255,255,255,255));
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        sceneControls[i]->props.vertexColor = Vector3(i/255.0,1.0,1.0);
        sceneControls[i]->props.transparency = 1.0;
        int nodeIndex = smgr->getNodeIndexByID(sceneControls[i]->node->getID());
        smgr->RenderNode(nodeIndex);
    }
    if(shaderMGR->deviceType == OPENGLES2)
        getCtrlColorFromTouchTexture(touchPosition);
    smgr->setRenderTarget(NULL,false,false);
    for(int i = controlStartIndex;i <= controlEndIndex;i++)
        sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
    smgr->EndDisplay();
}
void SGAutoRigScene::touchMoveRig(Vector2 curTouchPos,Vector2 prevTouchPos,float width,float height)
{
    if(!isNodeSelected)
        return;
    if(isControlSelected){
        Vector3 outputValue;
        calculateControlMovements(curTouchPos,prevTouchPoints[0] ,outputValue,(isJointSelected) ? true:false);
        prevTouchPoints[0] = curTouchPos;
        switch(controlType){
            case MOVE:
                if(isNodeSelected){
                    if(sceneMode == RIG_MODE_MOVE_JOINTS)
                        isMoved |= changeSkeletonPosition(outputValue);
                    else
                        isMoved |= changeSGRPosition(outputValue);
                    
                }
                break;
            case ROTATE:
                if(isNodeSelected){
                    changeSkeletonRotation(outputValue);
                    changeSGRRotation(outputValue);
                }
                break;
            default:
                break;
        }
        updateControlsOrientaion();
        //        changeObjectOrientation(outputValue);
    }
    return;
}
bool SGAutoRigScene::changeSGRPosition(Vector3 outputValue){
    
    if(sceneMode != RIG_MODE_PREVIEW)
        return false;
    if(isJointSelected){
        Vector3 target = selectedJoint->jointNode->getAbsolutePosition() + outputValue;
        selectedJoint->jointNode->getParent()->setRotationInDegrees(MathHelper::getRelativeParentRotation(selectedJoint->jointNode,target));
        (dynamic_pointer_cast<JointNode>(selectedJoint->jointNode->getParent()))->updateAbsoluteTransformationOfChildren();
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
            if(mirrorJointId != -1){
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(mirrorJointId);
                mirrorNode->getParent()->setRotationInDegrees(selectedJoint->jointNode->getParent()->getRotationInDegrees() * Vector3(1.0,-1.0,-1.0));
                (dynamic_pointer_cast<JointNode>(mirrorNode->getParent()))->updateAbsoluteTransformationOfChildren();
                //mirrorNode.reset();
            }
        }
        return true;
    }
    else if(isNodeSelected){
        selectedNode->node->setPosition(selectedNode->node->getAbsolutePosition() + outputValue);
        selectedNode->node->updateAbsoluteTransformationOfChildren();
        return true;
    }
    return false;
}
bool SGAutoRigScene::changeSGRRotation(Vector3 outputValue){
    if(sceneMode != RIG_MODE_PREVIEW)
        return false;
    outputValue *= RADTODEG;
    if(isJointSelected){
        selectedJoint->jointNode->setRotationInDegrees(outputValue);
        selectedJoint->jointNode->updateAbsoluteTransformationOfChildren();
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
            if(mirrorJointId != -1){
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(mirrorJointId);
                mirrorNode->setRotationInDegrees(outputValue * Vector3(1.0,-1.0,-1.0));
                mirrorNode->updateAbsoluteTransformationOfChildren();
                //mirrorNode.reset();
            }
        }
        return true;
    }
    else if(selectedNode){
        selectedNode->node->setRotationInDegrees(outputValue);
        if(isNodeSelected)
            selectedNode->node->updateAbsoluteTransformation();
        else
            selectedNode->node->updateAbsoluteTransformationOfChildren();
        return true;
    }
    return false;
}
void SGAutoRigScene::changeNodeScale(Vector3 scale, bool isChanged)
{
    if(isNodeSelected && !isJointSelected) {
        if(_scaleAction.actionType == ACTION_EMPTY){
            _scaleAction.actionType = (sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId == 0)?ACTION_CHANGE_SKELETON_KEYS:ACTION_CHANGE_SGR_KEYS;
            _scaleAction.keys = getNodeKeys();
        }
        
        if(sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId == 0){
            
            selectedNode->setScaleOnNode(scale);
            selectedNode->node->updateAbsoluteTransformationOfChildren();
            updateSkeletonBones();
        } else if (sceneMode == RIG_MODE_PREVIEW && isNodeSelected && !isJointSelected) {
            selectedNode->setScaleOnNode(scale);
            selectedNode->node->updateAbsoluteTransformationOfChildren();
        }
        if(isChanged){
            vector<ActionKey> keys = getNodeKeys();
            _scaleAction.keys.insert(_scaleAction.keys.end(), keys.begin(), keys.end());
            SGAutoRigScene::addAction(_scaleAction);
            _scaleAction.drop();
        }
    }
}
vector<ActionKey> SGAutoRigScene::getNodeKeys()
{
    vector<ActionKey> keys;
    if(sceneMode == RIG_MODE_MOVE_JOINTS) {
        for(int i=0; i<tPoseJoints.size();i++){
            int id = tPoseJoints[i].id;
            if(rigKeys.find(id) != rigKeys.end()){
                ActionKey key;
                key.position = rigKeys[id].referenceNode->getNodePosition();
                key.rotation = Quaternion(rigKeys[id].referenceNode->node->getRotationInRadians());
                key.scale = rigKeys[id].referenceNode->getNodeScale();
                keys.push_back(key);
            }
        }
    }
    else {
        if(sgrSGNode && sgrSGNode->node) {
            ActionKey key;
            key.position = sgrSGNode->getNodePosition();
            key.rotation =sgrSGNode->getNodeRotation();
            key.scale = sgrSGNode->getNodeScale();
            keys.push_back(key);
            int jointsCnt = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointCount();
            for(int i=0; i<jointsCnt; i++){
                ActionKey key;
                shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(i);
                key.position = jointNode->getPosition();
                key.rotation = Quaternion(jointNode->getRotationInRadians());
                key.scale = jointNode->getScale();
                keys.push_back(key);
                //jointNode.reset();
            }
        }
    }
    return keys;
}

void SGAutoRigScene::checkSelection(Vector2 touchPosition)
{
    
    //    ISceneNode *touchNode = NULL;
    switch(sceneMode){
        case RIG_MODE_OBJVIEW:
            break;
        case RIG_MODE_MOVE_JOINTS:{
            clearNodeSelections();
            AttachSkeletonModeRTTSelection(touchPosition);
            if(shaderMGR->deviceType == METAL){
                AttachSkeletonModeRTTSelection(touchPosition);
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_EDIT_ENVELOPES:{
            clearNodeSelections();
            AttachSkeletonModeRTTSelection(touchPosition);
            if(shaderMGR->deviceType == METAL){
                AttachSkeletonModeRTTSelection(touchPosition);
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_PREVIEW:{
            rttSGRNodeJointSelection(touchPosition);
            if(shaderMGR->deviceType == METAL){
                rttSGRNodeJointSelection(touchPosition);
                readSGRSelectionTexture();
            }
            break;
        }
        default:
            break;
    }
    
    if(sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId > 0)
        selectedJointId = selectedNodeId;
//    if((isSkeletonSelected || isNodeSelected) && controlType == SCALE)
//        controlType = MOVE;
    updateControlsOrientaion();
}
float SGAutoRigScene::getSelectedJointScale()
{
    if(sceneMode == RIG_MODE_EDIT_ENVELOPES && isNodeSelected && selectedNodeId>0){
        return rigKeys[selectedNodeId].envelopeRadius;
    }
    return 1.0;
}
Vector3 SGAutoRigScene::getSelectedNodeScale()
{
    if(sceneMode == RIG_MODE_MOVE_JOINTS && isNodeSelected)
        return rigKeys[0].referenceNode->getNodeScale();
    else if(sceneMode == RIG_MODE_PREVIEW && isNodeSelected)
        return selectedNode->getNodeScale();
    return Vector3(1.0);
}
void SGAutoRigScene::touchEndRig(Vector2 curTouchPos)
{
    swipeTiming = 0;
    updateControlsMaterial();
    if(isControlSelected) {
        vector<ActionKey> keys;
        keys = getNodeKeys();
        prevRotX = prevRotY = prevRotZ = 0.0;
        moveAction.keys.insert(moveAction.keys.end(), keys.begin() , keys.end());
        SGAutoRigScene::addAction(moveAction);
        
        selectedControlId = NOT_SELECTED;
        isControlSelected = false;
    }
}
void SGAutoRigScene::addAction(SGAction& action)
{
    removeActions();
    SGScene::addAction(action);
}
bool SGAutoRigScene::setSceneMode(AUTORIG_SCENE_MODE mode){
    switch(mode){
        case RIG_MODE_OBJVIEW:{
            setLighting(true);
            if(sgrSGNode && sgrSGNode->node)
                Logger::log(ERROR,"SGAutoRigScene:setScenMode","SGrNod enot removed");
            
            if (rigKeys.size() > 0)
                removeRigKeys();
            if(objSGNode){
                objSGNode->node->setVisible(true);
                objSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
                objSGNode->props.transparency = 1.0;
                objSGNode->props.isLighting = true;
            }
            break;
        }
            
        case RIG_MODE_MOVE_JOINTS:{
            setLighting(false);
            if(objSGNode){
                objSGNode->node->setVisible(true);
                objSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
                objSGNode->props.transparency = 0.5;
            }
            setControlsVisibility(true);
            if(rigKeys.size()==0)
                initSkeletonJoints();
            else
                setJointAndBonesVisibility(true);
            
            setEnvelopVisibility(false);
            
            if(sgrSGNode && sgrSGNode->node)
                sgrSGNode->node->setVisible(false);
            
            controlType = MOVE;
            isNodeSelected = true;
            selectedNode = rigKeys[0].referenceNode;
            selectedNodeId = 0;
            //            highlightSkeletonJoints();
            updateSkeletonSelectionColors(0);
            updateControlsOrientaion();
            break;
            
        }
            
        case RIG_MODE_EDIT_ENVELOPES:{
            setLighting(false);
            //            action.drop();
            envelopes.clear();
            selectedNodeId = NOT_SELECTED;
            if(objSGNode){
                objSGNode->node->setVisible(true);
                objSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
                objSGNode->props.transparency = 0.5;
            }
            if(rigKeys.size() > 0 && rigKeys[0].referenceNode)
                setJointAndBonesVisibility(true);
            
                if(sgrSGNode && sgrSGNode->node) {
                    setJointSpheresVisibility(false);

                    sgrSGNode->clearSGJoints();
                    smgr->RemoveNode(sgrSGNode->node);
                }
            sceneMode = RIG_MODE_EDIT_ENVELOPES;
            controlType = MOVE;
            setControlsVisibility(false);
            updateSkeletonSelectionColors(0);
            updateOBJVertexColor();
            break;
        }
            
        case RIG_MODE_PREVIEW:{
            setControlsVisibility(true);
            setEnvelopVisibility(false);
            if(objSGNode){
                objSGNode->node->setVisible(false);
            }
            setJointAndBonesVisibility(false);
            if(animatedSGRPath.compare(" ") != 0){
                if(sgrSGNode == NULL){
                    sgrSGNode = new SGNode(NODE_RIG);
                }
                if(sgrSGNode->node){
                    setJointSpheresVisibility(false);
                    sgrSGNode->clearSGJoints();
                    smgr->RemoveNode(sgrSGNode->node);
                }
                AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(animatedSGRPath,shaderMGR->deviceType);
                sgrSGNode->setSkinningData((SkinMesh*)mesh);
                sgrSGNode->node = smgr->createAnimatedNodeFromMesh(mesh,"sgrUniforms",CHARACTER_RIG, MESH_TYPE_HEAVY);
                if(sgrSGNode->node){
                    sgrSGNode->createSGJoints();
                    sgrSGNode->node->setID(SGR_ID);
                    sgrSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_SKIN_L1));
                    sgrSGNode->node->setTexture(objSGNode->node->getActiveTexture(),1);
                    sgrSGNode->node->setTexture(shadowTexture,2);
                    sgrSGNode->props.transparency = 1.0;
                    sgrSGNode->props.isLighting = true;
                    sgrSGNode->node->setVisible(true);
                    printf("setscenemode - Vertices count %d ", dynamic_pointer_cast<MeshNode>(sgrSGNode->node)->getMesh()->getVerticesCount());
                }
            }
            else{
                Logger::log(ERROR,"SGAutoRigScene","SGR File Path Missing : " + std::string(animatedSGRPath));
            }
            setLighting(true);
            controlType = MOVE;
            break;
        }
            
        default:
            return false;
    }
    return true;
}
void SGAutoRigScene::setJointAndBonesVisibility(bool isVisible)
{
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
void SGAutoRigScene::addNewJoint()
{
    if(selectedNodeId == NOT_SELECTED || selectedNodeId == 0)
        return;
    
    if(rigKeys.size() >= RIG_MAX_BONES){
        boneLimitsCallBack();
        return;
    }
    addJointAction.drop();
    addJointAction.actionType = ACTION_ADD_JOINT;
    

        AutoRigJointsDataHelper::addNewTPoseJointData(tPoseJoints,selectedNodeId);
        addJointAction.actionSpecificIntegers.push_back(selectedNodeId);
    
    addAction(addJointAction);
    resetRigKeys();
    
    
    int id = tPoseJoints[tPoseJoints.size()-1].id;
    if (rigKeys.find(id) != rigKeys.end()) {
        int prevSelectedNodeId = selectedNodeId;
        selectedNodeId = id;
        selectedNode = rigKeys[selectedNodeId].referenceNode;
        updateSkeletonSelectionColors(prevSelectedNodeId);
        updateControlsOrientaion();
    }
    
}
void SGAutoRigScene::removeJoint()
{
    int id = tPoseJoints[tPoseJoints.size()-1].id;
    rigKeyIterator = rigKeys.find(id);
    if(rigKeys[id].bone && rigKeys[id].bone->node){
        smgr->RemoveNode(rigKeys[id].bone->node);
        delete rigKeys[id].bone;
        rigKeys[id].bone = NULL;
    }
    if(rigKeys[id].sphere && rigKeys[id].sphere->node){
        smgr->RemoveNode(rigKeys[id].sphere->node);
        delete rigKeys[id].sphere;
        rigKeys[id].sphere = NULL;
    }
    if(rigKeys[id].referenceNode && rigKeys[id].referenceNode->node){
    smgr->RemoveNode(rigKeys[id].referenceNode->node);
    delete rigKeys[id].referenceNode;
    rigKeys[id].referenceNode = NULL;
    }
    
    rigKeys.erase(rigKeyIterator);
    tPoseJoints.pop_back();
    resetRigKeys();
}
void SGAutoRigScene::initSkeletonJoints()
{
    rigKeys.clear();
    tPoseJoints.clear();
    if(skeletonType == SKELETON_OWN)
        AutoRigJointsDataHelper::getTPoseJointsDataForOwnRig(tPoseJoints,rigBoneCount);
    else
        AutoRigJointsDataHelper::getTPoseJointsData(tPoseJoints);
    resetRigKeys();
}
    
void SGAutoRigScene::resetRigKeys(){
    for(int i = 0;i < tPoseJoints.size();i++){
        if(rigKeys.find(tPoseJoints[i].id) == rigKeys.end()){
            rigKeys[tPoseJoints[i].id].referenceNode->node = smgr->addEmptyNode();
            rigKeys[tPoseJoints[i].id].parentId = tPoseJoints[i].parentId;
            rigKeys[tPoseJoints[i].id].referenceNode->node->setPosition(Vector3(tPoseJoints[i].position));
            rigKeys[tPoseJoints[i].id].referenceNode->node->setRotationInDegrees(Vector3(tPoseJoints[i].rotation));
            rigKeys[tPoseJoints[i].id].referenceNode->node->updateAbsoluteTransformation();
            rigKeys[tPoseJoints[i].id].referenceNode->node->setID(REFERENCE_NODE_START_ID + i);
            rigKeys[tPoseJoints[i].id].envelopeRadius = tPoseJoints[i].envelopeRadius;
            if(tPoseJoints[i].id != 0){
                sphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",shaderMGR->deviceType);
                rigKeys[tPoseJoints[i].id].sphere->node = smgr->createNodeFromMesh(sphereMesh,"jointUniforms");
                rigKeys[tPoseJoints[i].id].sphere->node->setID(JOINT_START_ID + i);
                rigKeys[tPoseJoints[i].id].sphere->node->setParent(rigKeys[tPoseJoints[i].id].referenceNode->node);
                rigKeys[tPoseJoints[i].id].sphere->node->setScale(Vector3(tPoseJoints[i].sphereRadius));
                rigKeys[tPoseJoints[i].id].sphere->node->updateAbsoluteTransformation();
                rigKeys[tPoseJoints[i].id].sphere->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
                rigKeys[tPoseJoints[i].id].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
            }
        }
    }
    
    //setting parent, child relationships for referenceNodes
    for(int i=0;i<tPoseJoints.size();i++){
        int parentId = rigKeys[tPoseJoints[i].id].parentId;
        if(parentId == -1)
            continue;
        rigKeys[tPoseJoints[i].id].referenceNode->node->setParent(rigKeys[parentId].referenceNode->node);
        rigKeys[tPoseJoints[i].id].referenceNode->node->updateAbsoluteTransformationOfChildren();
    }
    //creating bones from each parentSphere
    for(int i=0;i<tPoseJoints.size();i++){
        //if(rigKeys.find(tPoseJoints[i].id) != rigKeys.end()){
        if(!(rigKeys[tPoseJoints[i].id].bone->node)){
            int parentId = rigKeys[tPoseJoints[i].id].parentId;
            if(parentId <= 0) continue; //ignoring hip's parent
            boneMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/BoneMesh.sgm",shaderMGR->deviceType);
            rigKeys[tPoseJoints[i].id].bone->node = smgr->createNodeFromMesh(boneMesh,"BoneUniforms");
            rigKeys[tPoseJoints[i].id].bone->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            rigKeys[tPoseJoints[i].id].bone->props.vertexColor = Vector3(1.0,1.0,1.0);
            rigKeys[tPoseJoints[i].id].bone->props.transparency = 0.6;
            rigKeys[tPoseJoints[i].id].bone->node->setID(BONE_START_ID + i);
            rigKeys[tPoseJoints[i].id].bone->node->setParent(rigKeys[parentId].referenceNode->node);
            rigKeys[tPoseJoints[i].id].bone->node->updateAbsoluteTransformation();
        }
    }
    for(int i=0;i<tPoseJoints.size();i++)
        updateSkeletonBone(tPoseJoints[i].id);
}
void SGAutoRigScene::changeEnvelopeScale(Vector3 scale, bool isChanged)
{
    if(selectedNodeId > 0){
        if(_scaleAction.actionType == ACTION_EMPTY){
            _scaleAction.actionType = ACTION_CHANGE_SKELETON_KEYS;
            _scaleAction.actionSpecificFloats.push_back(rigKeys[selectedNodeId].envelopeRadius);
        }
        setEnvelopeScale(scale);
        if(isChanged){
            _scaleAction.actionType = ACTION_CHANGE_ENVELOPE_SCALE;
            _scaleAction.actionSpecificFloats.push_back(rigKeys[selectedNodeId].envelopeRadius);
            _scaleAction.actionSpecificIntegers.push_back(selectedNodeId);
            SGAutoRigScene::addAction(_scaleAction);
            _scaleAction.drop();
        }
    }
}
void SGAutoRigScene::setEnvelopeScale(Vector3 scale)
{
    if(selectedNodeId > 0)
    {
        rigKeys[selectedNodeId].envelopeRadius =  scale.x;
        if(getMirrorState() == MIRROR_ON)
        {
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
            if(mirrorJointId != -1){
                rigKeys[mirrorJointId].envelopeRadius = scale.x;
            }
        }
        updateEnvelopes();
        updateOBJVertexColor();
        //AutoRigHelper::updateOBJVertexColors(objMesh, envelopes, rigKeys);
    }
}
void SGAutoRigScene::updateEnvelopes()
{
    //update all visible envelopes.
    for(std::map<int, SGNode *>::iterator it = envelopes.begin(); it!=envelopes.end(); it++)
    {
        if(it->second && (selectedNodeId == rigKeys[it->first].parentId || it->first == selectedNodeId))
            initEnvelope(it->first);
        
        int mirrorJoint = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
        
        if(getMirrorState() == MIRROR_ON && it->second && (mirrorJoint == rigKeys[it->first].parentId || it->first == mirrorJoint))
            initEnvelope(it->first);
    }
}
void SGAutoRigScene::AttachSkeletonModeRTTSelection(Vector2 touchPosition)
{
    touchPosForSkeletonSelection = touchPosition;
    setControlsVisibility(false);
    rotationCircle->node->setVisible(false);
    bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->setRenderTarget(touchTexture,true,true,false,Vector4(255,255,255,255));
    vector<Vector3> scaleValues;
    for(int i = 0; i < tPoseJoints.size(); i++){
        if(rigKeys[tPoseJoints[i].id].parentId > 0){
            Vector3 vertColor = rigKeys[tPoseJoints[i].id].bone->props.vertexColor;
            float transparency = rigKeys[tPoseJoints[i].id].bone->props.transparency;
            Material *mat = smgr->getMaterialByIndex(SHADER_COLOR);
            rigKeys[tPoseJoints[i].id].bone->node->setMaterial(mat);
            rigKeys[tPoseJoints[i].id].bone->props.vertexColor = Vector3(i/255.0,255/255.0,255/255.0);
            rigKeys[tPoseJoints[i].id].bone->props.transparency = 1.0;
            int nodeId = smgr->getNodeIndexByID(rigKeys[tPoseJoints[i].id].bone->node->getID());
            smgr->RenderNode(nodeId);
            rigKeys[tPoseJoints[i].id].bone->props.vertexColor = transparency;
            rigKeys[tPoseJoints[i].id].bone->props.vertexColor = vertColor;
        }
        
        if(tPoseJoints[i].id != 0){
            Vector3 vertColor = rigKeys[tPoseJoints[i].id].sphere->props.vertexColor;
            float transparency = rigKeys[tPoseJoints[i].id].sphere->props.transparency;
            int nodeId = smgr->getNodeIndexByID(rigKeys[tPoseJoints[i].id].sphere->node->getID());
            scaleValues.push_back(rigKeys[tPoseJoints[i].id].sphere->node->getScale());
            rigKeys[tPoseJoints[i].id].sphere->node->setScale(rigKeys[tPoseJoints[i].id].sphere->node->getScale() * 1.3);
            rigKeys[tPoseJoints[i].id].sphere->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            rigKeys[tPoseJoints[i].id].sphere->props.vertexColor = Vector3(i/255.0,255/255.0,255/255.0);
            rigKeys[tPoseJoints[i].id].sphere->props.transparency = 1.0;
            smgr->RenderNode(nodeId);
            rigKeys[tPoseJoints[i].id].sphere->props.vertexColor = transparency;
            rigKeys[tPoseJoints[i].id].sphere->props.vertexColor = vertColor;
        }
    }
    
    for(int i = 0; i < tPoseJoints.size(); i++) {
        if(tPoseJoints[i].id != 0)
            rigKeys[tPoseJoints[i].id].sphere->node->setScale(scaleValues[i]);
    }
    // Draw Joints
    if(shaderMGR->deviceType == OPENGLES2)
        readSkeletonSelectionTexture();
    smgr->setRenderTarget(NULL,false,false);
    smgr->EndDisplay();
}

void SGAutoRigScene::readSkeletonSelectionTexture() {
    
    Vector2 touchPixel = touchPosForSkeletonSelection;
    if(selectSkeletonJointInPixel(touchPixel))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x, touchPixel.y+1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x, touchPixel.y-1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y-1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y+1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y-1.0)))
        return;
    selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y+1.0));
}


bool SGAutoRigScene::selectSkeletonJointInPixel(Vector2 touchPixel)
{
    int prevSelectedNodeId = selectedNodeId;

    float xCoord = (touchPixel.x/screenWidth) * touchTexture->width;
    float yCoord = (touchPixel.y/screenHeight) * touchTexture->height;
    limitPixelCoordsWithinTextureRange(touchTexture->width,touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),touchTexture);
    
    isJointSelected = isSkeletonJointSelected = false;
    isNodeSelected = isSkeletonSelected = true;
    selectedJoint = NULL;
    if(pixel.x != 255.0 && pixel.x != 0.0){
        selectedNodeId = (int)pixel.x;
        selectedNode = rigKeys[selectedNodeId].referenceNode;
    }else{
        selectedNodeId = 0;
        selectedNode = rigKeys[0].referenceNode;
    }
    updateSkeletonSelectionColors(prevSelectedNodeId);
    updateControlsOrientaion();
   
    return selectedNode;
}

void SGAutoRigScene::setEnvelopVisibility(bool isVisible)
{
    for(std::map<int,SGNode *> :: iterator it = envelopes.begin(); it!=envelopes.end(); it++){
        if(it->second != NULL){
            it->second->node->setVisible(isVisible);
        }
    }
}
void SGAutoRigScene::drawEnvelopes(int jointId)
{
    setEnvelopVisibility(false);
    if(jointId<=0)  return;
    if(jointId != 1)   //skipping the envelop between hip and pivot
        initEnvelope(jointId);
    
    shared_ptr< vector< shared_ptr<Node> > > childs = rigKeys[jointId].referenceNode->node->Children;
    int childCount = (int)childs->size();
    for(int i = 0; i < childCount;i++){
        shared_ptr<MeshNode> childReference = dynamic_pointer_cast<MeshNode>((*childs)[i]);
        if(childReference && (childReference->getID() >= REFERENCE_NODE_START_ID && childReference->getID() <= REFERENCE_NODE_START_ID + rigKeys.size()))
            initEnvelope(childReference->getID() - REFERENCE_NODE_START_ID);
        
        //childReference.reset();
    }
    //childs.reset();
}
void SGAutoRigScene::updateOBJVertexColor()
{
    int mirrorJointIdToUpdate = getMirrorState() ? BoneLimitsHelper::getMirrorJointId(selectedNodeId) : NOT_EXISTS;
    AutoRigHelper::updateOBJVertexColors(dynamic_pointer_cast<MeshNode>(objSGNode->node),envelopes,rigKeys,selectedNodeId, mirrorJointIdToUpdate);
    
    int nodeIndex = smgr->getNodeIndexByID(objSGNode->node->getID());
    dynamic_pointer_cast<MeshNode>(objSGNode->node)->getMesh()->Commit();
    smgr->updateVertexBuffer(nodeIndex);
}
bool SGAutoRigScene::findInRigKeys(int key){
    return (rigKeys.find(key) != rigKeys.end());
}
void SGAutoRigScene::updateSkeletonSelectionColors(int prevSelectedNodeId)
{
    if(prevSelectedNodeId > 0 && prevSelectedNodeId < tPoseJoints.size() && findInRigKeys(prevSelectedNodeId))
        rigKeys[prevSelectedNodeId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
    
    if((sceneMode == RIG_MODE_MOVE_JOINTS || sceneMode == RIG_MODE_EDIT_ENVELOPES) && prevSelectedNodeId > 0) {
        int jointId = BoneLimitsHelper::getMirrorJointId(prevSelectedNodeId);
        if((!getMirrorState() || jointId != NOT_EXISTS) && findInRigKeys(jointId))
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
    }
    
    if(selectedNodeId > 0 && findInRigKeys(selectedNodeId))
        rigKeys[selectedNodeId].sphere->props.vertexColor = Vector3(SELECTION_COLOR_R,SELECTION_COLOR_G,SELECTION_COLOR_B);
    
    if((sceneMode == RIG_MODE_MOVE_JOINTS || sceneMode == RIG_MODE_EDIT_ENVELOPES) && selectedNodeId > 0) {
        int jointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
        if(jointId != NOT_EXISTS && getMirrorState() && findInRigKeys(jointId))
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SELECTION_COLOR_R,SELECTION_COLOR_G,SELECTION_COLOR_B);
        else if(jointId != NOT_EXISTS && findInRigKeys(jointId)) {
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
        }
        
    }
    
    if(selectedNodeId == 0 || selectedNodeId == NOT_EXISTS) {
        for (int i = 1; i < tPoseJoints.size(); i++) {
            int id = tPoseJoints[i].id;
            if(findInRigKeys(id))
                rigKeys[id].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
        }
    }
    
    if(sceneMode == RIG_MODE_EDIT_ENVELOPES){
        drawEnvelopes(selectedNodeId);
        if(getMirrorState() == MIRROR_ON && selectedNodeId > 0 && BoneLimitsHelper::getMirrorJointId(selectedNodeId) != -1)
            drawEnvelopes(BoneLimitsHelper::getMirrorJointId(selectedNodeId));
        
        updateEnvelopes();
        updateOBJVertexColor();
    }
    
}
void SGAutoRigScene::exportSGR(std::string filePath)
{
    AutoRigHelper::initWeights(dynamic_pointer_cast<MeshNode>(objSGNode->node),rigKeys,influencedVertices,influencedJoints);
    animatedSGRPath = filePath;
    exportSGR::createSGR(filePath,dynamic_pointer_cast<MeshNode>(objSGNode->node),rigKeys,influencedVertices,influencedJoints);
}
void SGAutoRigScene::updateSelectionOfAttachSkeletonMode(){
    
}
bool SGAutoRigScene::changeSkeletonRotation(Vector3 outputValue){
    if(sceneMode != RIG_MODE_MOVE_JOINTS)
        return false;
    outputValue *= RADTODEG;
    
    if (sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId > 0){
        selectedNode->node->setRotationInDegrees(outputValue);
        selectedNode->node->updateAbsoluteTransformation();
        updateSkeletonBone(selectedNodeId);
        if(getMirrorState() == MIRROR_ON)
        {
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
            if(mirrorJointId != -1){
                shared_ptr<Node> mirrorNode = rigKeys[mirrorJointId].referenceNode->node;
                mirrorNode->setRotationInDegrees(outputValue * Vector3(1.0,-1.0,-1.0));
                mirrorNode->updateAbsoluteTransformation();
                updateSkeletonBone(mirrorJointId);
                //mirrorNode.reset();
            }
        }
        //        updateEnvelopes();
        return true;
    }
    else if(selectedNode){
        selectedNode->node->setRotationInDegrees(outputValue);
        if(isNodeSelected)
            selectedNode->node->updateAbsoluteTransformationOfChildren();
        else selectedNode->node->updateAbsoluteTransformation();
        return true;
    }
    return false;
}
bool SGAutoRigScene::changeSkeletonPosition(Vector3 outputValue){
    
    if(!isNodeSelected || sceneMode == RIG_MODE_EDIT_ENVELOPES || sceneMode == RIG_MODE_PREVIEW)
        return false;
    
    if(selectedNodeId > 0){
        Mat4 parentGlobalMat = selectedNode->node->getParent()->getAbsoluteTransformation();
        selectedNode->node->setPosition(MathHelper::getRelativePosition(parentGlobalMat, selectedNode->node->getAbsolutePosition() + outputValue));
        selectedNode->node->updateAbsoluteTransformation();
        updateSkeletonBone(selectedNodeId);
        
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
            if(mirrorJointId != -1){
                {
                    shared_ptr<Node> mirrorNode = rigKeys[mirrorJointId].referenceNode->node;
                    mirrorNode->setPosition(selectedNode->node->getPosition() * Vector3(-1.0,1.0,1.0));
                    mirrorNode->updateAbsoluteTransformation();
                    updateSkeletonBone(mirrorJointId);
                    //mirrorNode.reset();
                }
            }
        }
        updateSkeletonBones();
        return true;
    }
    else if(selectedNodeId == 0){
        selectedNode->node->setPosition(selectedNode->node->getAbsolutePosition() + outputValue);
        selectedNode->node->updateAbsoluteTransformation();
        return true;
    }
    return false;
}
void SGAutoRigScene::updateSkeletonBone(int jointId)
{
    if(jointId <= 1) return;    //skipping the bone between hip and pivot.
    int parentId = rigKeys[jointId].parentId;
    if(parentId <= 0) return;
    Vector3 currentDir = BONE_BASE_DIRECTION;
    if(rigKeys[jointId].bone && rigKeys[jointId].bone->node) {
        Vector3 targetDir = Vector3(rigKeys[jointId].referenceNode->node->getPosition()).normalize();
        rigKeys[jointId].bone->node->setRotationInDegrees(MathHelper::toEuler(MathHelper::rotationBetweenVectors(targetDir,currentDir))*RADTODEG);
        rigKeys[jointId].bone->node->updateAbsoluteTransformation();
        f32 boneLength = rigKeys[jointId].referenceNode->node->getAbsolutePosition().getDistanceFrom(rigKeys[parentId].referenceNode->node->getAbsolutePosition());
        Mat4 childGlobal;
        childGlobal.scale(boneLength);
        Mat4 parentGlobal;
        parentGlobal.scale(rigKeys[0].referenceNode->node->getAbsoluteTransformation().getScale());
        parentGlobal.invert();
        Mat4 childLocal = parentGlobal * childGlobal;
        rigKeys[jointId].bone->node->setScale(Vector3(rigKeys[jointId].bone->node->getScale().x,childLocal.getScale().y,rigKeys[jointId].bone->node->getScale().z));
        rigKeys[jointId].bone->node->updateAbsoluteTransformationOfChildren();
    }
}
void SGAutoRigScene::updateSkeletonBones()
{
    for(int i=0;i<tPoseJoints.size();i++)
        updateSkeletonBone(tPoseJoints[i].id);
}
void SGAutoRigScene::clearNodeSelections(){
    if(isSkeletonSelected){
        //        removeSkeletonJointHighlights();
        //        if(isSkeletonJointSelected)
        //            removeEnvelopes();
    }
    if(isNodeSelected){
        updateControlsMaterial();
    }
    isSkeletonSelected = false;
    isSkeletonJointSelected = false;
    isNodeSelected = false;
    isJointSelected = false;
    selectedJointId = NOT_SELECTED;
    selectedNode = NULL;
    selectedJoint = NULL;
}
void SGAutoRigScene::switchSceneMode(AUTORIG_SCENE_MODE mode){
    if((objSGNode == NULL || !objSGNode->node) && mode == RIG_MODE_MOVE_JOINTS){
        objLoaderCallBack(OBJ_NOT_LOADED);
        return;
    }
    sceneMode = mode;
    clearNodeSelections();
    if(setSceneMode(mode)){
        actions.clear();
        currentAction = 0;
    }
}
bool SGAutoRigScene::switchMirrorState()
{
    SGAction action;
    action.actionType = ACTION_CHANGE_MIRROR_STATE;
    action.actionSpecificFlags.push_back(getMirrorState());
    addAction(action);
    setMirrorState((MIRROR_SWITCH_STATE)!getMirrorState());
    bool state = getMirrorState();
    if(isNodeSelected && selectedNodeId > 0)
        updateSkeletonSelectionColors(0);
    else if(isJointSelected)
        highlightJointSpheres();
    return state;
}

void SGAutoRigScene::renderAll()
{
    bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,true,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->draw2DImage(bgTexture,Vector2(0,0),Vector2(screenWidth,screenHeight),true,smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
    rotationCircle->node->setVisible(false);
    setControlsVisibility(false);
    smgr->Render();
    drawGrid();
    drawCircle();
    if(sceneMode == RIG_MODE_MOVE_JOINTS || sceneMode == RIG_MODE_PREVIEW)
        renderControls();
    smgr->EndDisplay();
    
    if(fabs(xAcceleration) > 0.0 || fabs(yAcceleration) > 0.0) {
        swipeToRotate();
    }

}
void SGAutoRigScene::addObjToScene(string objPath,string texturePath)
{
    if(objSGNode->node){
        smgr->RemoveNode(objSGNode->node);
    }
    int objLoadStatus = 0;
    Mesh *objMes = objLoader->createMesh(objPath,objLoadStatus,shaderMGR->deviceType);
    if(objMes == NULL){
        objLoaderCallBack(objLoadStatus);
        return;
    }
    //printf("Vertex count %d ",objMes->getVerticesCount());
    shared_ptr<MeshNode> objNode = smgr->createNodeFromMesh(objMes,"ObjUniforms");
    objSGNode->node = objNode;

    // scale to fit all obj in same proportion-----
    float extendX = objNode->getMesh()->getBoundingBox()->getXExtend();
    float extendY = objNode->getMesh()->getBoundingBox()->getYExtend();
    float extendZ = objNode->getMesh()->getBoundingBox()->getZExtend();
    float max = ((extendX >= extendY) ? extendX:extendY);
    max = ((max >= extendZ) ? max:extendZ);
    float scaleRatio = (max / OBJ_BOUNDINGBOX_MAX_LIMIT);
    objNode->setScale(Vector3(1.0/scaleRatio));
    //-----------
    
    objNode->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_L1));
    objSGNode->props.isLighting = true;
    Texture *nodeTex = smgr->loadTexture(texturePath,texturePath,TEXTURE_RGBA8,TEXTURE_BYTE);
    objNode->setTexture(nodeTex,1);
    objNode->setTexture(shadowTexture,2);
    objNode->setID(OBJ_ID);
    objSGNode->node->updateAbsoluteTransformation();
    if(objMes == NULL){
        Logger::log(ERROR, "Loaded OBJ is NULL", "SGAutoRigScene");
        return;
    }
    isOBJimported = true;
    actions.clear();
    currentAction = 0;
}
void SGAutoRigScene::setEnvelopeUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(envelopes.find(nodeID - ENVELOPE_START_ID)->second,matName);
}
void SGAutoRigScene::objNodeCallBack(string materialName){
    shaderMGR->setUniforms(objSGNode,materialName);
}
bool SGAutoRigScene::isOBJTransparent(string materialName){
    return objSGNode->props.transparency < 1.0;
}
void SGAutoRigScene::boneNodeCallBack(int id,string materialName){
    shaderMGR->setUniforms(rigKeys[id - BONE_START_ID].bone,materialName);
}
void SGAutoRigScene::jointNodeCallBack(int id,string materialName){
    shaderMGR->setUniforms(rigKeys[id - JOINT_START_ID].sphere,materialName);
}
void SGAutoRigScene::setSGRUniforms(int jointId,string matName)
{
    shaderMGR->setUniforms(sgrSGNode,matName);
}
bool SGAutoRigScene::isSGRTransparent(int jointId,string matName)
{
    return sgrSGNode->props.transparency < 1.0;
}
void SGAutoRigScene::initEnvelope(int jointId)
{
    SGNode *envelopeSgNod = (envelopes.find(jointId) == envelopes.end()) ? NULL:envelopes[jointId];
    if(jointId<=1) return;  //skipping envelope between hip and it's parent.
    int parentId = rigKeys[jointId].parentId;
    
    if(envelopeSgNod == NULL){
        envelopeSgNod = new SGNode(NODE_RIG);
        AnimatedMesh *mesh = CSGRMeshFileLoader::LoadMesh(constants::BundlePath + "/Envelop.sgr",shaderMGR->deviceType);
        envelopeSgNod->setSkinningData((SkinMesh*)mesh);
        shared_ptr<AnimatedMeshNode> envelopeNode = smgr->createAnimatedNodeFromMesh(mesh,"envelopeUniforms",CHARACTER_RIG, MESH_TYPE_HEAVY);
        envelopeNode->setID(ENVELOPE_START_ID + jointId);
        envelopeNode->setParent(rigKeys[parentId].referenceNode->node);
        envelopeNode->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
        envelopeSgNod->node = envelopeNode;
        envelopeSgNod->props.vertexColor = Vector3(1.0);
        envelopeSgNod->props.transparency = 0.4;
    }
    if(envelopeSgNod->node) {
    envelopeSgNod->node->updateAbsoluteTransformation();
    
    Vector3 currentDir = BONE_BASE_DIRECTION;
    Vector3 targetDir = Vector3(rigKeys[jointId].referenceNode->node->getPosition()).normalize();
    envelopeSgNod->node->setRotationInDegrees(MathHelper::toEuler(MathHelper::rotationBetweenVectors(targetDir,currentDir))*RADTODEG);
    envelopeSgNod->node->updateAbsoluteTransformation();
    shared_ptr<JointNode> topJoint = (dynamic_pointer_cast<AnimatedMeshNode>(envelopeSgNod->node))->getJointNode(ENVELOPE_TOP_JOINT_ID);
    shared_ptr<JointNode> bottomJoint = (dynamic_pointer_cast<AnimatedMeshNode>(envelopeSgNod->node))->getJointNode(ENVELOPE_BOTTOM_JOINT_ID);
    
    float height = rigKeys[parentId].referenceNode->node->getAbsolutePosition().getDistanceFrom(rigKeys[jointId].referenceNode->node->getAbsolutePosition());
    topJoint->setPosition(Vector3(0,height,0));
    topJoint->updateAbsoluteTransformation();
    
    bottomJoint->setScale(Vector3(rigKeys[parentId].envelopeRadius));
    bottomJoint->updateAbsoluteTransformation();
    topJoint->setScale(Vector3(rigKeys[jointId].envelopeRadius));
    topJoint->updateAbsoluteTransformation();
    envelopes[jointId] = envelopeSgNod;
    envelopes[jointId]->node->setVisible(true);
    //topJoint.reset();
    //bottomJoint.reset();
    }
}
void SGAutoRigScene::updateEnvelope(int jointId, shared_ptr<AnimatedMeshNode> envelopeNode)
{
    if(jointId<=1) return;  //skipping envelope between hip and it's parent.
    
}
void SGAutoRigScene::rttSGRNodeJointSelection(Vector2 touchPosition)
{
    touchPosForSGRSelection = touchPosition;
    if(sgrSGNode == NULL)
        return;
    shared_ptr<AnimatedMeshNode> animNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node));
    setControlsVisibility(false);
    rotationCircle->node->setVisible(false);
    //render SGR
    bool displayPrepared = smgr->PrepareDisplay(screenWidth,screenHeight,false,true,false,Vector4(0,0,0,255));
    if(!displayPrepared)
        return;
    smgr->setRenderTarget(touchTexture,true,true,false,Vector4(255,255,255,255));
    sgrSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    sgrSGNode->props.transparency = 1.0;
    sgrSGNode->props.vertexColor = Vector3(0.0,1.0,1.0);
    
    smgr->RenderNode(smgr->getNodeIndexByID(sgrSGNode->node->getID()));
    
    sgrSGNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COMMON_SKIN_L1));
    sgrSGNode->props.transparency = 1.0;
    
    // render Joints
    if(isNodeSelected){
        if(!isJointSelected)
            setJointSpheresVisibility(true);
        for(int i = 1;i < animNode->getJointCount();i++){
            Vector3 vertexColors = Vector3(jointSpheres[i]->props.vertexColor);
            float transparency = (jointSpheres[i]->props.transparency);
            jointSpheres[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
            jointSpheres[i]->props.vertexColor = Vector3(0.0,i/255.0,1.0);
            jointSpheres[i]->props.transparency = 1.0;
            smgr->RenderNode(smgr->getNodeIndexByID(jointSpheres[i]->node->getID()),(i == 1) ? true:false);
            jointSpheres[i]->props.vertexColor = vertexColors;
            jointSpheres[i]->props.transparency = transparency;
        }
        if(!isJointSelected)
            setJointSpheresVisibility(false);
    }
    if(shaderMGR->deviceType == OPENGLES2)
        readSGRSelectionTexture();
    smgr->setRenderTarget(NULL,false,false);
    smgr->EndDisplay();
    //animNode.reset();
}
void SGAutoRigScene::readSGRSelectionTexture()
{
    float xCoord = (touchPosForSGRSelection.x/screenWidth) * touchTexture->width;
    float yCoord = (touchPosForSGRSelection.y/screenHeight) * touchTexture->height;
    limitPixelCoordsWithinTextureRange(touchTexture->width,touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),touchTexture);
    updateSGRSelection(pixel.x,pixel.y,dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node));
}
void SGAutoRigScene::updateSGRSelection(int selectedNodeColor,int selectedJointColor, shared_ptr<AnimatedMeshNode> animNode){
    int prevJointId = selectedJointId;
    if(selectedNodeColor != 255){
        isNodeSelected = true;
        selectedNodeId = selectedNodeColor;
        selectedNode = sgrSGNode;
        sgrSGNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
        if(selectedJointColor >= 1 && selectedJointColor < animNode->getJointCount()){
            selectedJointId = selectedJointColor;
            isJointSelected = true;
            selectedJoint = sgrSGNode->joints[selectedJointId];
            highlightJointSpheres();
        }
        bool status = displayJointSpheresForNode(animNode);
        if(!status)
            updateSGRSelection(255, 0, animNode);
    }else{
        clearNodeSelections();
        setJointSpheresVisibility(false);
        selectedNodeId = 0;
        selectedJointId = NOT_SELECTED;
        sgrSGNode->props.transparency = 1.0;
    }
    highlightJointSpheres();
    //animNode.reset();
}
void SGAutoRigScene::setSkeletonKeys(vector<ActionKey> &keys)
{
    for(int i=0; i<tPoseJoints.size(); i++){
        int id = tPoseJoints[i].id;
        if(rigKeys.find(id) != rigKeys.end()){
            rigKeys[id].referenceNode->setPositionOnNode(keys[i].position);
            rigKeys[id].referenceNode->setRotationOnNode(keys[i].rotation);
            rigKeys[id].referenceNode->setScaleOnNode(keys[i].scale);
            rigKeys[id].referenceNode->node->updateAbsoluteTransformation();
        }
    }
}

void SGAutoRigScene::setSGRKeys(vector<ActionKey> &keys)
{
    sgrSGNode->setPositionOnNode(keys[0].position);
    sgrSGNode->setRotationOnNode(keys[0].rotation);
    sgrSGNode->setScaleOnNode(keys[0].scale);
    sgrSGNode->node->updateAbsoluteTransformation();
    int jointsCnt = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointCount();
    for(int i=0; i<jointsCnt; i++){
        shared_ptr<JointNode> jointNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(i);
        jointNode->setPosition(keys[i+1].position);
        jointNode->setRotationInDegrees(MathHelper::toEuler(keys[i+1].rotation)*RADTODEG);
        jointNode->setScale(keys[i+1].scale);
        jointNode->updateAbsoluteTransformationOfChildren();
        //jointNode.reset();
    }
}


void SGAutoRigScene::undo(){
    if(currentAction == 0) return;
    SGAction &recentAction = actions[currentAction-1];
    switch(recentAction.actionType){
        case ACTION_SWITCH_MODE:
            setSceneMode((AUTORIG_SCENE_MODE)recentAction.actionSpecificIntegers[0]);
            break;
        case ACTION_CHANGE_MIRROR_STATE:
            if (recentAction.actionSpecificFlags.size() > 0)
                setMirrorState((MIRROR_SWITCH_STATE)(bool)recentAction.actionSpecificFlags[0]);
            break;
        case ACTION_CHANGE_SKELETON_KEYS:{
            vector<ActionKey> newKeys(recentAction.keys.begin(), recentAction.keys.begin()+(recentAction.keys.size()/2));
            setSkeletonKeys(newKeys);
            updateSkeletonBones();
            break;
        }
        case ACTION_CHANGE_SGR_KEYS:{
            vector<ActionKey> newKeys(recentAction.keys.begin(), recentAction.keys.begin()+(recentAction.keys.size()/2));
            
            setSGRKeys(newKeys);
            break;
        }
        case ACTION_CHANGE_ENVELOPE_SCALE:{
            int preveSelectedNodeId = selectedNodeId != NOT_EXISTS ? selectedNodeId : 0;
            isNodeSelected = true;
            selectedJointId = selectedNodeId = recentAction.actionSpecificIntegers[0];
            selectedNode = rigKeys[0].referenceNode;
            setEnvelopeScale(Vector3(recentAction.actionSpecificFloats[0]));
            updateOBJVertexColor();
            updateSkeletonSelectionColors(preveSelectedNodeId);
            break;
        }
        case ACTION_ADD_JOINT:{
            selectedNode = rigKeys[0].referenceNode;
            selectedNodeId = 0;
            removeJoint();
            break;
        }
        default:
            return;
    }
    currentAction--;
    updateControlsOrientaion();
    updateSkeletonSelectionColors(0);
    //    updateControls();
}
void SGAutoRigScene::redo(){
    if(currentAction == actions.size())
        return;
    SGAction &recentAction = actions[currentAction];
    switch(recentAction.actionType){
        case ACTION_SWITCH_MODE:
            setSceneMode((AUTORIG_SCENE_MODE)recentAction.actionSpecificIntegers[1]);
            break;
        case ACTION_CHANGE_MIRROR_STATE:
            if (recentAction.actionSpecificFlags.size() > 0)
                setMirrorState((MIRROR_SWITCH_STATE)!recentAction.actionSpecificFlags[0]);
            break;
        case ACTION_CHANGE_SKELETON_KEYS:{
            vector<ActionKey> newKeys(recentAction.keys.begin()+(recentAction.keys.size()/2),recentAction.keys.end());
            setSkeletonKeys(newKeys);
            updateSkeletonBones();
            break;
        }
        case ACTION_CHANGE_SGR_KEYS:{
            vector<ActionKey> newKeys(recentAction.keys.begin()+(recentAction.keys.size()/2),recentAction.keys.end());
            setSGRKeys(newKeys);
            break;
        }
        case ACTION_CHANGE_ENVELOPE_SCALE: {
            int preveSelectedNodeId = selectedNodeId != NOT_EXISTS ? selectedNodeId : 0;
            isNodeSelected = true;
            selectedJointId = selectedNodeId = recentAction.actionSpecificIntegers[0];
            selectedNode = rigKeys[0].referenceNode;
            setEnvelopeScale(Vector3(recentAction.actionSpecificFloats[1]));
            updateOBJVertexColor();
            updateSkeletonSelectionColors(preveSelectedNodeId);
            break;
        }
        case ACTION_ADD_JOINT: {
            AutoRigJointsDataHelper::addNewTPoseJointData(tPoseJoints,recentAction.actionSpecificIntegers[0]);
            resetRigKeys();
            break;
        }
        default:
            return;
    }
    currentAction++;
    updateControlsOrientaion();
    updateSkeletonSelectionColors(0);
    //    updateControls();
}

void SGAutoRigScene::setLighting(bool status)
{
    ShaderManager::sceneLighting = status;
}

bool SGAutoRigScene::isReachMaxBoneCount()
{
    if(rigKeys.size() >= RIG_MAX_BONES)
        return true;
    else
        return false;
}

