//
// Created by shankarganesh on 20/10/15.
//

#include "autorigscene.h"
#include "opengl.h"
#include "Iyan3dEngineFiles/HeaderFiles/SGAutoRigScene.h"


SGAutoRigScene *autoRigScene;
GLuint _colorRenderBufferautoRig;
GLuint _depthRenderBufferautoRig;
GLuint _frameBufferautoRig;
int oglWidthautoRig , oglHeightautoRig;
float screenScaleautoRig = 1.0;
SceneManager *autoRigSceneManager;
int isScaleChanged = 0;
float envelopeScale = 1.0;
bool isBoneLimitReached;
bool switchMirrorState = false;



void addCameraLightInAutoRigScene()
{

}

void nodeShaderCallBackAutoRig(int nodeID,string matName,string callbackFuncName)
{
    if(callbackFuncName.compare("ObjUniforms") == 0){
        autoRigScene->objNodeCallBack(matName);
    }else if(callbackFuncName.compare("jointUniforms") == 0){
        autoRigScene->jointNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("BoneUniforms") == 0){
        autoRigScene->boneNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("setCtrlUniforms") == 0){
        autoRigScene->setControlsUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("envelopeUniforms") == 0){
        autoRigScene->setEnvelopeUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("sgrUniforms") == 0){
        autoRigScene->setSGRUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("setJointSpheresUniforms") == 0){
        autoRigScene->setJointsUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("RotationCircle") == 0)
        autoRigScene->setRotationCircleUniforms(nodeID,matName);
}
bool transparentCallBackAutoRig(int nodeId,string callbackFuncName)
{
    if(callbackFuncName.compare("ObjUniforms") == 0){
        return autoRigScene->isOBJTransparent(callbackFuncName);
    }
    else if(callbackFuncName.compare("jointUniforms") == 0){

        return false;
    }
    else if(callbackFuncName.compare("BoneUniforms") == 0){
        return false;
    }
    else if(callbackFuncName.compare("setCtrlUniforms") == 0){
        return autoRigScene->isControlsTransparent(nodeId,callbackFuncName);
    }
    else if(callbackFuncName.compare("envelopeUniforms") == 0){
        return false;
    }
    else if(callbackFuncName.compare("sgrUniforms") == 0){
        return autoRigScene->isSGRTransparent(nodeId,callbackFuncName);
    }
    else if(callbackFuncName.compare("setJointSpheresUniforms") == 0){
        return autoRigScene->isJointTransparent(nodeId,callbackFuncName);
    }
    else if(callbackFuncName.compare("RotationCircle") == 0){
        return false;
    }




    return false;
}

void objLoaderCallBack(int status)
{

    if(status == OBJ_CROSSED_MAX_VERTICES_LIMIT){
        Logger::log(INFO , "autorig callback" , "Max vertices");
    }else if(status == OBJ_NOT_LOADED){
        Logger::log(INFO , "autorig callback" , " not loaded");
    }else{
        Logger::log(INFO , "autorig callback" , "something else");
    }
}

void setupRenderBufferAutoRig()
{
    glGenRenderbuffers(1, &_colorRenderBufferautoRig);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBufferautoRig);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA4,oglWidthautoRig,oglHeightautoRig);
//    checkGlError("setup render buffer ");
//[_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
void setupDepthBufferAutoRig()
{
    glGenRenderbuffers(1, &_depthRenderBufferautoRig);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBufferautoRig);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, oglWidthautoRig * screenScaleautoRig, oglHeightautoRig * screenScaleautoRig);
    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Depth buffer setup successfull");
//    checkGlError("setup depth buffer");
}
void setupFrameBufferAutoRig()
{
    glGenFramebuffers(1, &_frameBufferautoRig);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBufferautoRig);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufferautoRig);
//    checkGlError("setup frame buffer");
}



bool initAutorig(JNIEnv *env, jclass type, int width, int height){
    autoRigSceneManager= new SceneManager(width,height,1,OPENGLES2,constants::BundlePath);
    autoRigScene = new SGAutoRigScene(OPENGLES2,autoRigSceneManager,width,height);
    autoRigScene->screenScale = screenScaleautoRig;
    setupRenderBufferAutoRig();
    setupDepthBufferAutoRig();
    setupFrameBufferAutoRig();
    autoRigSceneManager->setFrameBufferObjects(0,_colorRenderBufferautoRig,_depthRenderBufferautoRig);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    autoRigScene->boneLimitsCallBack = &boneLimitCallbackInit;
    isBoneLimitReached = false;
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", "glCheckFramebufferStatus", error);
    }
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            LOGI("\n\n\nFLIPBOOM : FBO complete  GL_FRAMEBUFFER_COMPLETE %x\n\n\n", status);
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGI("\n\n\nFLIPBOOM : FBO GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT  %x\n\n\n", status);
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGI("\n\n\nFLIPBOOM : FBO FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT  %x\n\n\n",
                 status);
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            LOGI("\n\n\nFLIPBOOM : FBO FRAMEBUFFER_INCOMPLETE_DIMENSIONS  %x\n\n\n", status);
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGI("\n\n\nFLIPBOOM : FBO GL_FRAMEBUFFER_UNSUPPORTED  %x\n\n\n", status);
            break;

        default :
            LOGI("\n\n\nFLIPBOOM : failed to make complete framebuffer object %x\n\n\n", status);
    }

    autoRigSceneManager->ShaderCallBackForNode = &nodeShaderCallBackAutoRig;
    autoRigSceneManager->isTransparentCallBack = &transparentCallBackAutoRig;
    autoRigScene->objLoaderCallBack = &objLoaderCallBack;
    return true;

}

bool renderAutoRig(){
    if(isScaleChanged > 0) {
        autoRigScene->changeEnvelopeScale(Vector3(envelopeScale), isScaleChanged == 1);
        isScaleChanged = 0;
    }

    if(switchMirrorState) {
        autoRigScene->switchMirrorState();
        switchMirrorState = false;
    }

    autoRigScene->renderAll();

    if(autoRigScene->isRTTCompleted){
        autoRigScene->isRTTCompleted = false;
        return true;
    }
    else
        return false;

}
void addObjToScene(string objPath,string texturePath){

    autoRigScene->addObjToScene(objPath,texturePath);
}
void panBeginAutorig(float x1, float y1, float x2, float y2){
    autoRigScene->panBegan(Vector2(x1,y1),Vector2(x2,y2));

}
void panProgressAutorig(float cordX1, float cordY1,float cordX2, float cordY2){
    autoRigScene->panProgress(Vector2(cordX1,cordY1),Vector2(cordX2,cordY2));
}
void swipe(float velocityX, float velocityY){
    autoRigScene->swipeProgress(-velocityX/50.0,-velocityY/50.0);
}
void tap(Vector2 tapPosition){
    autoRigScene->checkSelection(tapPosition);
}
void rotateaction(){
    autoRigScene->controlType = ROTATE;
    autoRigScene->updateControlsOrientaion();
}
void moveAction(){
    autoRigScene->controlType=MOVE;
    autoRigScene->updateControlsOrientaion();
}
void ownRigging(){
    autoRigScene->skeletonType = SKELETON_OWN;
    autoRigScene->rigBoneCount = 3;
    autoRigScene->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_MOVE_JOINTS));
}
void humanRigging(){
    autoRigScene->skeletonType = SKELETON_HUMAN;
    autoRigScene->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigScene->sceneMode+1));
}
void checkcontrolSelection(Vector2 coords){
    autoRigScene->touchBeganRig(coords);
}
void  tapAutoRig(Vector2 tapPositionAutorig, JNIEnv *env, jclass type){
    autoRigScene->isRTTCompleted= true;
    autoRigScene->checkSelection(tapPositionAutorig);

    jstring state;
    if(autoRigScene->sceneMode == RIG_MODE_MOVE_JOINTS && autoRigScene->selectedNodeId > 0)
        state = env->NewStringUTF(to_string(1).c_str());
    else
        state = env->NewStringUTF(to_string(0).c_str());
    jmethodID mid = env->GetStaticMethodID(type, "addJointBtnHandler", "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(type, mid, state);

}
void tapBegin(float x , float y,Vector2 coords, float width, float height){
    autoRigScene->touchMoveRig(Vector2(x,y),coords,width,height);
}
void tapEnd(float x , float y){
    autoRigScene->touchEndRig(Vector2(x,y));
}
void nextBtnAction(string cacheDir){
    if(autoRigScene->sceneMode + 1 == RIG_MODE_PREVIEW){
        Logger::log(INFO, "IYAN3D.CPP", "Preview"+cacheDir);
        autoRigScene->exportSGR(cacheDir);
    }

    if(autoRigScene->sceneMode == RIG_MODE_OBJVIEW){

    }
    else{
        autoRigScene->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigScene->sceneMode+1));
    }

}
void ScaleAutoRig(float x,bool state){
    envelopeScale = x;
    if(state == true)
        isScaleChanged = 1;
    else
        isScaleChanged = 2;
}
void moveToPrevMode(){
    autoRigScene->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigScene->sceneMode-1));
}
void undoFunction(){
    autoRigScene->undo();
}
void redoFunction(){
    autoRigScene->redo();
}

float getSelectedJointScale()
{
    return autoRigScene->getSelectedJointScale();
}
void topViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_TOP);

}
void bottomViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_BOTTOM);

}
void leftViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_LEFT);

}
void rightViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_RIGHT);

}
void frontViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_FRONT);
}
void backViewAutoRig(){
    autoRigScene->changeCameraView(VIEW_BACK);
}
void ScaleboneAutoRig(float x,float y,float z, bool state){
    autoRigScene->changeNodeScale(Vector3(x,y,z),state);
}
void addNewJoint(){
    autoRigScene->addNewJoint();
}
void deleteAutoRigScene(){
    if(autoRigScene){
        Logger::log(INFO,"AutoRigScene","Autorig Scene Deleted");
    delete autoRigScene;
    }
}

void boneLimitCallbackInit(){
    Logger::log(INFO,"AUTORIG.CPP","Bone Limit Reached");
    isBoneLimitReached = true;
}

bool isJointSelected(){
    return autoRigScene->isJointSelected;
}

bool updateMirror(){
    //autoRigScene->switchMirrorState();
    switchMirrorState = true;
    return  !(autoRigScene->getMirrorState());
}

Vector3 getSelectedNodeScale(){
    return autoRigScene->getSelectedNodeScale();
}

int getSceneMode(){
    Logger::log(INFO,"AutoRig Scene : ", to_string(autoRigScene->sceneMode));
    return autoRigScene->sceneMode;
}

int getskeletonType(){
    return autoRigScene->skeletonType;
}
int getselectedNodeId(){
    return autoRigScene->selectedNodeId;
}
string getUndoOrRedoButtonAction(){
    string state = "DEACTIVATE_BOTH";
    if(autoRigScene->actions.size() > 0 && autoRigScene->currentAction > 0)
        state = "DEACTIVATE_REDO";
    if(autoRigScene->actions.size() > 0 && autoRigScene->currentAction < (autoRigScene->actions.size()))
        state = "DEACTIVATE_UNDO";
    if(autoRigScene->actions.size() > 0 && autoRigScene->currentAction > 0 && autoRigScene->currentAction < (autoRigScene->actions.size()))
        state = "ACTIVATE_BOTH";
    return state;
}
string getRedoButtonAction(){
    if(autoRigScene->currentAction == autoRigScene->actions.size()) {
        if(autoRigScene->currentAction <= 0)
            return "DEACTIVATE_BOTH";
        else
            return "DEACTIVATE_REDO";
    }
    else
        return "DEACTIVATE_BOTH";
}

