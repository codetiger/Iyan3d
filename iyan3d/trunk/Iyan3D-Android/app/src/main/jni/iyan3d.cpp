#include <jni.h>
#include "preview_scene.h"
#include "Iyan3dEngineFiles/HeaderFiles/SGAnimationScene.h"
#include "Iyan3dEngineFiles/HeaderFiles/SGAutoRigScene.h"

#include "opengl.h"
#include "autorigscene.h"
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/string.h"
#include "SceneManager/SceneManager.h"



SGAnimationScene *animationScene;
SceneManager *sceneManager;
SGAutoRigScene *sgAutoRigScene;
GLuint _colorRenderBuffer;
GLuint _depthRenderBuffer;
GLuint _frameBuffer;
int oglWidth, oglHeight;
float screenScale = 1.0;
Vector2 touchBegan;
bool assetView = false;
bool checktapposition = false;
bool checktappositionAutorig = false;
bool assetItemClicked = false;
bool checkControlSelection = false;
int clickedAssetId = 0, clickedAssetType;
int selectedNodeid = -1;
int selectedAssetId = 0;
NODE_TYPE selectedAssetType = NODE_UNDEFINED;
int selectedAnimId = 0;
bool slectedAnimation = false;
bool pauseRendering = false;
bool displayTextAsset = false;
bool deleteObjectAction = false;
bool renderingAction = false;
bool storeAsTemplate= false;
bool savingTemplatestatus= false;
string cachePath;
int shadertype;
bool waterMark;
string fontdisplayText, fontFilePath;
int fontSize, fontbevelSize, fontColorRed, fontColorGreen, fontColorBlue;
bool import3DText= false;
bool importImage=false;
string imagePath;
int imgWidth, imgHeight;
string clickedAssetName;
ActionType assetActionType;
Vector2 tapPosition;
Vector2 tapPositionAutorig;
Vector2 touchBeganAutoRig;
NODE_TYPE selectedNodeType = NODE_UNDEFINED;
bool isPlaying = false;
bool isSaving = false;
bool animationApplied = false;
bool selectObject = false;
bool redoAction=false;
bool undoAction=false;
int objectTobeSelected = -1;
int objectToBeDeleted = -1;
string assetStoragePath, meshStoragePath,importedImagesPath;
string animationFilePath;
int undoValue;
string projectFileNameSaveData, projectFileNameLoadData;
string autorigObjPath="";
string autorigTexturePath="";
string loadFilepath="init";
bool ownRig=false;
bool isAutorigScene=false;
bool humanRig=false;
int templateAssetId;
bool checkControlSelectionAutoRig= false;
bool nextBtnpressed=false,backBtnPressed= false;
bool isAddExtraLight = false;
string rigFilePath;
int lightCount = 1;
int ASSET_ADDITIONAL_LIGHT = 900;
int light_asset_id = 0;
bool isLightValueChanged = false;
float Red,Green,Blue,shadowDensity;
bool isFileSaveCompleted = false;
bool isAddNewJoint = false;
string importedAssetName;
int resolution;
bool autorigUndo = false;
bool autorigRedo = false;
bool mirrorUpdateAutoRig = false;
float sceneShadow;
Vector3 sceneLightColor;

static void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

void getLightCount()
{
    lightCount = 1;
    if(animationScene) {
        for(int i = 0; i < animationScene->nodes.size(); i++) {
            if(animationScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
                lightCount += 1;
        }
    }
}

void setupRenderBuffer() {
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, oglWidth, oglHeight);
    checkGlError("setup render buffer ");
    //[_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}

void setupDepthBuffer() {
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, oglWidth * screenScale,
                          oglHeight * screenScale);
    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Depth buffer setup successfull");
    checkGlError("setup depth buffer");
}



void setupFrameBuffer() {
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                              _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              _depthRenderBuffer);
    checkGlError("setup frame buffer");
}

void addCameraLight() {
    animationScene->loadNode(NODE_CAMERA, 0,L"CAMERA", 0, 0, IMPORT_ASSET_ACTION);
    animationScene->loadNode(NODE_LIGHT, 0, L"LIGHT", 0, 0, IMPORT_ASSET_ACTION);
    // animationScene->loadNode(NODE_RIG,32554,"FATMAN",0,0,IMPORT_ASSET_ACTION);
}

void stopPlaying() {
    if (isPlaying) {
        isPlaying = false;
        animationScene->isPlaying = false;
    }
    animationScene->switchFrame(animationScene->currentFrame);

}

void importAssetIntoScene() {
    wstring assetName = ConversionHelper::getWStringForString(importedAssetName);
    if (assetActionType == IMPORT_ASSET_ACTION) {
        animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, clickedAssetId);
        animationScene->loadNode(selectedNodeType, clickedAssetId, assetName, 0, 0,
                                 assetActionType);
    }
    else {
        animationScene->loadNode(selectedNodeType, clickedAssetId, assetName, 0, 0,
                                 assetActionType);
    }
    selectedNodeType = NODE_UNDEFINED;

    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D","Selected Node iD A: %d",animationScene->selectedNodeId);
    clickedAssetId = 0;
}

void shaderCallBackForNode(int nodeID, string matName, string callbackFuncName) {
    if (callbackFuncName.compare("setUniforms") == 0)
        animationScene->shaderCallBackForNode(nodeID, matName);
    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
        animationScene->setJointsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
        animationScene->setControlsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("RotationCircle") == 0)
        animationScene->setRotationCircleUniforms(nodeID, matName);
}

bool isTransparentCallBack(int nodeId, string callbackFuncName) {
    if (callbackFuncName.compare("setUniforms") == 0)
        return animationScene->isNodeTransparent(nodeId);
    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
        return animationScene->isJointTransparent(nodeId, callbackFuncName);
    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
        return animationScene->isControlsTransparent(nodeId, callbackFuncName);
    else if (callbackFuncName.compare("setOBJUniforms") == 0)
        return false;
    else if (callbackFuncName.compare("RotationCircle") == 0)
        return false;
    return false;
}

void applyAnimation(string filepath) {
    animationScene->animFilePath = "";
    animationScene->applyAnimations(filepath,animationScene->selectedNodeId);
    animationScene->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);

}

void checkTapposition() {
    animationScene->checkSelection(tapPosition);
    selectedNodeid = animationScene->selectedNodeId;

    if (selectedNodeid != -1) {
        selectedAssetType = NODE_UNDEFINED;
        selectedAssetType = animationScene->nodes[animationScene->selectedNodeId]->getType();
    }
}

void saveThumbnail() {
    string savePath =
            constants::DocumentsStoragePath + "/scenes/" + projectFileNameSaveData + ".png";
    char *imagePath = new char[savePath.length() + 1];
    strcpy(imagePath, savePath.c_str());
    //Logger::log(INFO, "IYAN3D", "Path for Image: " + to_string(imagePath));
    animationScene->saveThumbnail(imagePath);
    animationScene->reloadKeyFrameMap();
    isFileSaveCompleted = false;
}

void saveAnimation(string projectFileNameSaveData) {
    string savePath =
            constants::DocumentsStoragePath + "/projects/" + projectFileNameSaveData + ".sgb";
    Logger::log(INFO, "IYAN3D", "Path for animation: " + savePath);
    animationScene->saveSceneData(&savePath);
    saveThumbnail();
}

void setCallBack() {
    sceneManager->ShaderCallBackForNode = &shaderCallBackForNode;
    sceneManager->isTransparentCallBack = &isTransparentCallBack;

}

void undoRedoButtonState(JNIEnv *env, jclass type) {
    int buttonState;
    if (animationScene) {
        if (animationScene->actions.size() > 0 && animationScene->currentAction > 0 &&
            animationScene->currentAction >= (animationScene->actions.size())) {
            buttonState = (int) DEACTIVATE_REDO;
            Logger::log(INFO,"IYAN3D.CPP","Deactivate Redo");
        }
        else if (animationScene->actions.size() > 0 &&
                 animationScene->currentAction < (animationScene->actions.size()) &&
                 animationScene->currentAction == 0) {
            buttonState = (int) DEACTIVATE_UNDO;
            Logger::log(INFO,"IYAN3D.CPP","Deactivate Undo");
        }
        else if (animationScene->actions.size() > 0 &&
                 animationScene->currentAction < (animationScene->actions.size()) &&
                 animationScene->currentAction > 0) {
            buttonState = (int) ACTIVATE_BOTH;
            Logger::log(INFO,"IYAN3D.CPP","Activate Both");
        }
        else {
            buttonState = (int) DEACTIVATE_BOTH;
            Logger::log(INFO,"IYAN3D.CPP","Deactivate Both");
        }
        if (animationScene->actions.size() == 0) {
            buttonState = DEACTIVATE_BOTH;
            Logger::log(INFO,"IYAN3D.CPP","Deactivate Both Second");
        }
        Logger::log(INFO,"IYAN3D.CPP","Action Size : " + to_string(animationScene->actions.size())+" " + "Current Action : " + to_string(animationScene->currentAction));
    }

    jmethodID mid = env->GetStaticMethodID(type, "undoRedoButtonUpdate", "(I)V");
    env->CallStaticVoidMethod(type, mid, (jint) buttonState);
}

void mirrorStateUpdater(JNIEnv *env, jclass type,jint jointSize){
    jmethodID mid = env->GetStaticMethodID(type, "mirrorStateUpdater", "(I)V");
    env->CallStaticVoidMethod(type, mid, jointSize);
}

void animationEditorButtonHandler(JNIEnv *env, jclass type){
    jstring jstr1 = env->NewStringUTF(to_string(animationScene->selectedNodeId).c_str());
    jmethodID mid = env->GetStaticMethodID(type, "updateSelectednodeId",
                                           "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(type, mid, jstr1);

    if (animationScene->selectedNodeId != -1) {
        Logger::log(INFO, "IYAN3D.cpp", "Function comes here");
        jstring jstr2 = env->NewStringUTF(to_string(
                animationScene->nodes[animationScene->selectedNodeId]->getType()).c_str());
        jmethodID mid2 = env->GetStaticMethodID(type, "updateSelectednodeType",
                                                "(Ljava/lang/String;)V");
        env->CallStaticVoidMethod(type, mid2, jstr2);
    }
    jstring jstr3 = env->NewStringUTF(to_string(
            animationScene->selectedNodeId).c_str());
    jmethodID mid2 = env->GetStaticMethodID(type, "ButtonHandler",
                                            "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(type, mid2, jstr3);

    if (animationScene->selectedNodeId != -1) {
        mirrorStateUpdater(env, type,
                           (jint) animationScene->nodes[animationScene->selectedNodeId]->joints.size());
        Logger::log(INFO, "Iyan3d.CPP", "Joint Size : " + to_string(
                animationScene->nodes[animationScene->selectedNodeId]->joints.size()));
    }

}

void renderingFinsihWithText(JNIEnv *env,jclass type,jint isFinish){
    jmethodID mid = env->GetStaticMethodID(type, "isRenderingFinishWithText", "(I)V");
    env->CallStaticVoidMethod(type, mid, isFinish);
}

void boneLimitCallback(JNIEnv *env,jclass type){
    Logger::log(INFO,"AUTORIG.CPP","Bone Limit Reached");
    jmethodID mid = env->GetStaticMethodID(type, "boneLimitCallback", "()V");
    env->CallStaticVoidMethod(type, mid);
}

void callBackSurfaceCreatedAssetView(JNIEnv *env,jclass type){
    jmethodID mid = env->GetStaticMethodID(type, "callBackAssetViewSurfaceCreated", "()V");
    env->CallStaticVoidMethod(type, mid);
}

void callBackSurfaceCreatedAnimationEditor(JNIEnv *env,jclass type){
    jmethodID mid = env->GetStaticMethodID(type, "callBackIsRenderedUpdater", "()V");
    env->CallStaticVoidMethod(type, mid);
}

void callBackGetSceneMode(JNIEnv *env, jclass type, jint sceneMode){
    jmethodID mid = env->GetStaticMethodID(type, "callBackGetSceneMode", "(I)V");
    env->CallStaticVoidMethod(type, mid,sceneMode);
}

void callBackAutoRigUndoRedoBtnState(JNIEnv *env, jclass type,  string str){

    jstring state = env->NewStringUTF(str.c_str());
    jmethodID mid = env->GetStaticMethodID(type, "undoOrRedoBtnUpdater",
                                           "(Ljava/lang/String;)V");
    env->CallStaticVoidMethod(type, mid,state);
}

void downloadMissingAssetsCallBack(string fileName, NODE_TYPE nodeType, JNIEnv *env, jclass type){
    jstring fileNameStr = env->NewStringUTF(fileName.c_str());
    jstring nodeTypeStr = env->NewStringUTF(to_string(nodeType).c_str());
    jmethodID mid = env->GetStaticMethodID(type, "downloadCallBack",
                                           "(Ljava/lang/String;Ljava/lang/String;)V");
    env->CallStaticVoidMethod(type, mid,fileNameStr,nodeTypeStr);
}

void saveFinish(JNIEnv *env, jclass type){
    jmethodID mid = env->GetStaticMethodID(type,"saveFinish","()V");
    env->CallStaticVoidMethod(type,mid);
}




extern "C" {
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type,jint width, jint height);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type,jfloat velocityX,jfloat velocityY);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_paBegin(JNIEnv *env, jclass type,jfloat cordX1,jfloat cordY1,jfloat cordX2,
                                                                           jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env,
                                                                               jclass type,
                                                                               jfloat cordX1,
                                                                               jfloat cordY1,
                                                                               jfloat cordX2,
                                                                               jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_initAssetView(
        JNIEnv *env, jclass type, jint width, jint height);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_stepAssetView(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetViewFinish(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type,
                                                                       jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetItemDisplay(
        JNIEnv *env, jclass type, jint AssetId, jstring Assetname_, jint AssetType);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_addAssetToScene(
        JNIEnv *env, jclass type, jint assetId, jint nodeType, jstring Assetname_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env,
                                                                                    jclass type,
                                                                                    jfloat x,
                                                                                    jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type,
                                                                           jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapEnd(JNIEnv *env, jclass type,
                                                                          jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rotateBtn(JNIEnv *env,
                                                                             jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_moveBtn(JNIEnv *env,
                                                                           jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env,
                                                                                 jclass type,
                                                                                 jstring assetPath_,
                                                                                 jstring setMeshpath_,
                                                                                 jstring setImagepath_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetSwipetoEnd(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetSwipetoRotate(
        JNIEnv *env, jclass type, jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_setViewType(JNIEnv *env,
                                                                                        jclass type,
                                                                                        jint viewtype);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env,
                                                                                     jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env,
                                                                              jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetType(JNIEnv *env,
                                                                                jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_loadNodeAnimationSelection(
        JNIEnv *env, jclass type, jint assetId, jint assetType, jstring assetName_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_applyAnimationtoNode(
        JNIEnv *env, jclass type, jint assetId);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_getBoneCount(
        JNIEnv *env, jclass type, jint assetId);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_stopAllAnimations(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_playAnimation(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_remove3dText(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_load3DText(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jstring displayText_,
                                                                                       jint size,
                                                                                       jint bevelValue,
                                                                                       jint red,
                                                                                       jint blue,
                                                                                       jint green,
                                                                                       jstring filename_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_add3DTextToScene(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleAction(JNIEnv *env,
                                                                               jclass type,
                                                                               jfloat x, jfloat y,
                                                                               jfloat z,jboolean state);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getScaleZval(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getScaleYval(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getScaleXval(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteObject(JNIEnv *env,
                                                                                jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraTopView(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraLeftView(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraRightView(JNIEnv *env,
                                                                                   jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraBottomView(JNIEnv *env,
                                                                                    jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraFrontView(JNIEnv *env,
                                                                                   jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraBackView(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMeshBrightness(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMeshSpecular(JNIEnv *env,
                                                                                     jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightingSwitch(
        JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVisibilitySwitch(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_meshPropertyChanged(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jfloat brightness,
                                                                                       jfloat specular,
                                                                                       jboolean lighting,
                                                                                       jboolean visibility);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getCameraFov(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getCameraResolution(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(
        JNIEnv *env, jclass type, jfloat fov, jint resolution);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_frameClicked(JNIEnv *env,
                                                                                jclass type,
                                                                                jint position);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_playAnimation(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_playTimer(JNIEnv *env,
                                                                             jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_applyAnimationToNodeAnimationEditor(
        JNIEnv *env, jclass type, jstring filePath_);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAnimationFrameCount(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_stopAnimation(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectObject(JNIEnv *env,
                                                                                jclass type,
                                                                                jint position);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env,
                                                                                 jclass type,
                                                                                 jstring projectFileName_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveThumbnail(JNIEnv *env,
                                                                                 jclass type,
                                                                                 jstring projectFileName_);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_undoFunction(JNIEnv *env,
                                                                                jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getReturnValue(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleObejectBtNodeId(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jint nodeid);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getCurrentFrame(JNIEnv *env,
                                                                                   jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addFramebtn(JNIEnv *env,
                                                                               jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_redoFunction(JNIEnv *env,
                                                                                jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addAssetToScenenAnimationEditor(
        JNIEnv *env, jclass type, jint assetId, jint nodeType, jint assetAddType,jstring Assetname_);
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addTextImageBack(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env,
                                                                            jclass type,
                                                                            jstring filePath_);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetCount(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetList(JNIEnv *env,
                                                                                   jclass type,
                                                                                   jint i);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importImageAction(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jstring imageName_,
                                                                                     jint imageWidth,
                                                                                     jint imageHeight);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_renderFrame(JNIEnv *env,
                                                                               jclass type,
                                                                               jint frame,
                                                                               jint shaderType,
                                                                               jboolean watermark,
                                                                               jstring path_,
                                                                               jint resolution_value);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveTemplate(JNIEnv *env,
                                                                                    jclass type,
                                                                                    jstring assetId_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_initAutorig(JNIEnv *env,
                                                                                      jclass type,
                                                                                      jint width,
                                                                                      jint height);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_stepAutoRig(JNIEnv *env,
                                                                                      jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_panProgress(JNIEnv *env,
                                                                                      jclass type,
                                                                                      jfloat cordX1,
                                                                                      jfloat cordY1,
                                                                                      jfloat cordX2,
                                                                                      jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_panBeginAutorig(
        JNIEnv *env, jclass type, jfloat cordX1, jfloat cordY1, jfloat cordX2, jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_swipeAutorig(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jfloat velocityX,
                                                                                       jfloat velocityY);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapAutorig(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jfloat x,
                                                                                     jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_addObjToScene(JNIEnv *env,
                                                                                        jclass type,
                                                                                        jstring objPath_,
                                                                                        jstring texturePath_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_rotateAction(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_moveAction(JNIEnv *env,
                                                                                     jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_singleBone(JNIEnv *env,
                                                                                     jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_humanRigging(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_paBeginanim(JNIEnv *env,
                                                                                        jclass type,
                                                                                        jfloat cordX1,
                                                                                        jfloat cordY1,
                                                                                        jfloat cordX2,
                                                                                        jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_panProgressanim(
        JNIEnv *env, jclass type, jfloat cordX1, jfloat cordY1, jfloat cordX2, jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteScene(JNIEnv *env,
                                                                               jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_deletePreviewScene(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_controlSelectionAutoRig(
        JNIEnv *env, jclass type, jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapMoveAutorig(
        JNIEnv *env, jclass type, jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapEndAutorig(JNIEnv *env,
                                                                                        jclass type,
                                                                                        jfloat x,
                                                                                        jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_nextBtnAurorig(
        JNIEnv *env, jclass type, jstring filePath_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_scaleAutoRig(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jfloat x,jboolean state);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_prevBtnAction(JNIEnv *env,
                                                                                        jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_unodBtnAction(JNIEnv *env,
                                                                                        jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_redoBtnAction(JNIEnv *env,
                                                                                        jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addExtraLight(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jfloat R,
                                                                                       jfloat G,
                                                                                       jfloat B,
                                                                                       jfloat shadow);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getShadowDensity(JNIEnv *env,
                                                                                      jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesRed(
        JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesGreen(
        JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesBlue(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setLightingOff(JNIEnv *env,
                                                                                  jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setLightingOn(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resetClickedAssetId(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getSelectedNodeOrJointScaleAutoRig(
        JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_exportingCompleted(JNIEnv *env,
                                                                                      jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_undoRedoDeleteObject(JNIEnv *env,
                                                                                        jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchMirrorState(JNIEnv *env,
                                                                                     jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMirrorState(JNIEnv *env,
                                                                                      jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getJointSize(JNIEnv *env,
                                                                                jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_scaleJointAutorig(
        JNIEnv *env, jclass type, jfloat x, jfloat y, jfloat z,jboolean state);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightcount(JNIEnv *env,
                                                                                 jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteanimation(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isKeySetForNode(JNIEnv *env,
                                                                                       jclass type,
                                                                                       jint currentFrame);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_addNewJoint(JNIEnv *env,
                                                                                      jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_deleteAutoRigScene(
        JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_isPreviewScene(
        JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env,
                                                                                       jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_isAutoRigJointSelected(
        JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_updateMirror(
        JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueX(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueY(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueZ(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_skeletonType(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_selectedNodeId(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resetContext(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_storeShaderProps(JNIEnv *env, jclass type);


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type, jint width,
                                                 jint height) {

    if (animationScene) {
        LOGI("Scene already exists");
        return;
    }

    oglWidth = width;
    oglHeight = height;
    Logger::log(INFO, "IYAN3D.CPP", constants::BundlePath);
    sceneManager = new SceneManager(width, height, 1, OPENGLES2, constants::BundlePath);
    animationScene = new SGAnimationScene(OPENGLES2, sceneManager, width, height);

    animationScene->screenScale = screenScale;


    sceneManager->setFrameBufferObjects(0, _colorRenderBuffer, _depthRenderBuffer);


    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    checkGlError("glCheckFramebufferStatus");
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

    setCallBack();
    animationScene->downloadMissingAssetsCallBack = &downloadMissingAssetsCallBack;

    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Frame buffr objects");
}


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type) {

    if (!assetView && animationScene) {
        if (clickedAssetId != 0 && !assetItemClicked) {
            importAssetIntoScene();
            undoRedoButtonState(env, type);
        }

        if (checktapposition) {
            selectedNodeid = -1;
            checkTapposition();
            animationEditorButtonHandler(env,type);
            checktapposition = false;


        }
        if (checkControlSelection) {
            Logger::log(INFO, "IYAN3D.cpp", "Touch Began called");
            animationScene->checkCtrlSelection(touchBegan);
            checkControlSelection = false;
        }
        if (deleteObjectAction) {
            animationScene->removeObject(objectToBeDeleted);
            deleteObjectAction = false;
            objectToBeDeleted = -1;
            undoRedoButtonState(env, type);
        }
        if (import3DText) {
            std::wstring wstringFontDisplayText = ConversionHelper::getWStringForString(
                    fontdisplayText);
            animationScene->loadNode(NODE_TEXT, 0, wstringFontDisplayText, fontSize, fontbevelSize,
                                     IMPORT_ASSET_ACTION, Vector4((float) fontColorRed / 255.0,
                                                                  (float) fontColorGreen / 255.0,
                                                                  (float) fontColorBlue / 255.0,
                                                                  1.0), fontFilePath);
            animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0, fontFilePath);
            __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Selected Node iD A: %d",
                                animationScene->selectedNodeId);
            undoRedoButtonState(env, type);
            import3DText = false;
        }
        if (importImage) {
            Logger::log(INFO, "IYAN3D", "path: " + imagePath);
            std::wstring wstringImagePath = ConversionHelper::getWStringForString(imagePath);
            animationScene->loadNode(NODE_IMAGE, 0, wstringImagePath, imgWidth, imgHeight,
                                     assetActionType, Vector4(imgWidth, imgHeight, 0.0, 0.0));
            animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0);
            undoRedoButtonState(env, type);
            importImage = false;
        }
        if (animationApplied) {
            applyAnimation(animationFilePath);
            undoRedoButtonState(env, type);
            animationApplied = false;
        }
        if (isSaving) {
            saveAnimation(projectFileNameSaveData);
            saveFinish(env,type);
            //saveThumbnail();
            isSaving = false;
        }
        if (selectObject) {
            animationScene->selectObject(objectTobeSelected);
            animationEditorButtonHandler(env,type);
            selectObject = false;
        }
        if (loadFilepath == "init") {
            addCameraLight();
            getLightCount();
            loadFilepath = "";
            undoRedoButtonState(env, type);
            callBackSurfaceCreatedAnimationEditor(env, type);
        }
        else if (loadFilepath.size() > 0) {
            animationScene->loadSceneData(&loadFilepath, env, type);
            getLightCount();
            jstring totalFrameCount = env->NewStringUTF(
                    to_string(animationScene->totalFrames).c_str());
            jmethodID mid = env->GetStaticMethodID(type, "updateFrames", "(Ljava/lang/String;)V");
            env->CallStaticVoidMethod(type, mid, totalFrameCount);
            loadFilepath = "";
            undoRedoButtonState(env, type);
            callBackSurfaceCreatedAnimationEditor(env, type);
        }
        if (renderingAction) {
            animationScene->cameraResolutionType = resolution;
            animationScene->renderAndSaveImage((char *) cachePath.c_str(), shadertype, false,
                                               waterMark);
            Logger::log(INFO, "IYAN3D", "Rendering Image completed" + cachePath);
            renderingAction = false;

        } else if (!animationScene->isExportingImages) {
            animationScene->renderAll();
        }
        if (storeAsTemplate) {
            savingTemplatestatus = animationScene->storeAnimations(templateAssetId);
            storeAsTemplate = false;

        }
        if (redoAction) {
            int returnValue = animationScene->redo();
            jstring jstr1 = env->NewStringUTF(to_string(returnValue).c_str());
            jmethodID mid = env->GetStaticMethodID(type, "redoValue", "(Ljava/lang/String;)V");
            env->CallStaticVoidMethod(type, mid, jstr1);
            redoAction = false;
            undoRedoButtonState(env, type);
        }
        if (undoAction) {
            int returnValue2 = NOT_SELECTED;
            ACTION_TYPE actionType = (ACTION_TYPE) animationScene->undo(returnValue2);
            undoValue = returnValue2;
            jstring jstr2 = env->NewStringUTF(to_string(undoValue).c_str());
            jstring jstr3 = env->NewStringUTF(to_string((int) actionType).c_str());
            jmethodID mid = env->GetStaticMethodID(type, "undoValue",
                                                   "(Ljava/lang/String;Ljava/lang/String;)V");
            env->CallStaticVoidMethod(type, mid, jstr2, jstr3);
            undoAction = false;
            undoRedoButtonState(env, type);
        }

        if (isAddExtraLight) {
            string lightName = "LIGHT" + (to_string(light_asset_id - 900));
            animationScene->loadNode(NODE_ADDITIONAL_LIGHT, light_asset_id,
                                     ConversionHelper::getWStringForString(lightName), 20, 50,
                                     assetActionType, Vector4(1.0));
            Logger::log(INFO, "IYAN3D.CPP", "Add Extra Light");
            undoRedoButtonState(env, type);
            isAddExtraLight = false;
        }
        if (isLightValueChanged) {
            animationScene->changeLightProperty(Red, Green, Blue, shadowDensity, true);
            animationScene->storeLightPropertyChangeAction(Red, Green, Blue, shadowDensity);
            Logger::log(INFO, "IYAN3D", "Light value Changed");
            undoRedoButtonState(env, type);
            isLightValueChanged = false;
        }
    }
    //renderFrame();
}


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type, jfloat velocityX,
                                                  jfloat velocityY) {


    animationScene->swipeProgress(-velocityX / 50.0, -velocityY / 50.0);
    animationScene->updateLightCamera();
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_paBegin(JNIEnv *env, jclass type, jfloat cordX1,
                                                    jfloat cordY1, jfloat cordX2, jfloat cordY2) {


    animationScene->panBegan(Vector2(cordX1, cordY1), Vector2(cordX2, cordY2));
    animationScene->updateLightCamera();
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env, jclass type, jfloat cordX1,
                                                        jfloat cordY1, jfloat cordX2,
                                                        jfloat cordY2) {


    animationScene->panProgress(Vector2(cordX1, cordY1), Vector2(cordX2, cordY2));
    animationScene->updateLightCamera();
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_initAssetView(JNIEnv *env, jclass type,
                                                                   jint width, jint height) {


    assetView = true;
    initpreviewScene(width, height);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_stepAssetView(JNIEnv *env, jclass type) {


    if (assetItemClicked) {
        DisplayAsset(clickedAssetId, clickedAssetName, clickedAssetType);
        callBackSurfaceCreatedAssetView(env, type);
        assetItemClicked = false;
    }
    if (displayTextAsset) {
        Logger::log(INFO, "IYAN3D", "Load 3D TEXT");
        load3DText(fontdisplayText, fontSize, fontbevelSize, fontColorRed, fontColorGreen,
                   fontColorBlue, fontFilePath);
        renderingFinsihWithText(env, type, 1);
        displayTextAsset = false;
    }
    if (!pauseRendering) {
        renderPreviewScene();
    }
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetViewFinish(JNIEnv *env, jclass type) {
    pauseRendering = true;
    assetView = false;
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type, jfloat x, jfloat y) {


    tapPosition = Vector2(x, y) * screenScale;
    animationScene->isRTTCompleted = true;
    checktapposition = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetItemDisplay(JNIEnv *env, jclass type,
                                                                      jint AssetId,
                                                                      jstring Assetname_,
                                                                      jint AssetType) {
    pauseRendering = false;
    clickedAssetId = AssetId;
    clickedAssetType = AssetType;
    const char *nativeString = env->GetStringUTFChars(Assetname_, 0);
    clickedAssetName = nativeString;
    assetItemClicked = true;
    env->ReleaseStringUTFChars(Assetname_, nativeString);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_addAssetToScene(JNIEnv *env, jclass type,
                                                                     jint assetId, jint nodeType,
                                                                     jstring Assetname_) {
    assetView = false;
    const char *assetName = env->GetStringUTFChars(Assetname_, 0);

    assetActionType = IMPORT_ASSET_ACTION;
    if (nodeType == 1)
        selectedNodeType = NODE_RIG;
    else if (nodeType == 6)
        selectedNodeType = NODE_OBJ;
    else
        selectedNodeType = NODE_SGM;
    importedAssetName = assetName;
    clickedAssetId = assetId;
    env->ReleaseStringUTFChars(Assetname_, assetName);
}


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env, jclass type, jfloat x,
                                                             jfloat y) {

    checkControlSelection = true;
    touchBegan = Vector2(x, y);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type, jfloat x, jfloat y) {
    animationScene->touchMove(Vector2(x, y), touchBegan, oglWidth, oglHeight);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_tapEnd(JNIEnv *env, jclass type, jfloat x, jfloat y) {
    animationScene->touchEnd(Vector2(x, y));
    undoRedoButtonState(env, type);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_moveBtn(JNIEnv *env, jclass type) {

    // TODO
    animationScene->controlType = MOVE;
    animationScene->updateControlsOrientaion();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_rotateBtn(JNIEnv *env, jclass type) {

    animationScene->controlType = ROTATE;
    animationScene->updateControlsOrientaion();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env, jclass type,
                                                          jstring assetPath_, jstring setMeshpath_,
                                                          jstring setImagepath_) {
    const char *assetPath = env->GetStringUTFChars(assetPath_, 0);
    const char *setMeshpath = env->GetStringUTFChars(setMeshpath_, 0);
    const char *setImagepath = env->GetStringUTFChars(setImagepath_, 0);
    assetStoragePath = assetPath;
    meshStoragePath = setMeshpath;
    importedImagesPath = setImagepath;
    constants::BundlePathRig = constants::BundlePath = assetStoragePath;
    constants::DocumentsStoragePath = meshStoragePath;
    constants::impotedImagesPathAndroid = importedImagesPath;
    Logger::log(INFO, "IYAN3D", "image path" + importedImagesPath);


    env->ReleaseStringUTFChars(assetPath_, assetPath);
    env->ReleaseStringUTFChars(setMeshpath_, setMeshpath);
    env->ReleaseStringUTFChars(setImagepath_, setImagepath);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetSwipetoRotate(JNIEnv *env, jclass type,
                                                                        jfloat x, jfloat y) {
    swipeRotate(x, y);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_assetSwipetoEnd(JNIEnv *env, jclass type) {

    swipeEnd();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_setViewType(JNIEnv *env, jclass type,
                                                                 jint viewtype) {

    if (viewtype == 5) {
        setViewType(ALL_ANIMATION_VIEW);
    }
    if (viewtype == 3) {
        setViewType(ASSET_SELECTION);
    }

}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env, jclass type) {
    checktapposition = true;

    return (jint) selectedNodeid;

}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env, jclass type) {

    if (animationScene->selectedNodeId == -1 ||
        animationScene->selectedNodeId > animationScene->nodes.size())
        return -1;
    selectedAssetId = animationScene->nodes[animationScene->selectedNodeId]->assetId;
    return (jint) selectedAssetId;

}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getAssetType(JNIEnv *env, jclass type) {


    checktapposition = true;
    return (jint) selectedAssetType;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_loadNodeAnimationSelection(JNIEnv *env,
                                                                                jclass type,
                                                                                jint assetId,
                                                                                jint assetType,
                                                                                jstring assetName_) {

    const char *assetName = env->GetStringUTFChars(assetName_, 0);
    pauseRendering = false;
    if (assetType == 3)
        animationSelectionAssetItemToBeDisplayed(NODE_RIG, assetId, assetName);
    if (assetType == 4) {
        string animtextname = ConversionHelper::getStringForWString(
                animationScene->nodes[animationScene->selectedNodeId]->name);
        int animfontsize = animationScene->nodes[animationScene->selectedNodeId]->props.fontSize;
        float animbevelValue = animationScene->nodes[animationScene->selectedNodeId]->props.nodeSpecificFloat;
        string animfontfilepath = animationScene->nodes[animationScene->selectedNodeId]->optionalFilePath;
        Vector3 animfontColor = Vector3(
                animationScene->nodes[animationScene->selectedNodeId]->props.vertexColor.x,
                animationScene->nodes[animationScene->selectedNodeId]->props.vertexColor.y,
                animationScene->nodes[animationScene->selectedNodeId]->props.vertexColor.z);
        animationSelectionTextItemToBeDisplayed(animtextname, animfontsize, animbevelValue,
                                                animfontColor, animfontfilepath);
    }
    env->ReleaseStringUTFChars(assetName_, assetName);
    callBackSurfaceCreatedAssetView(env, type);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_applyAnimationtoNode(JNIEnv *env, jclass type,
                                                                          jint assetId) {
    Logger::log(INFO, "IYAN 3D.CPP", "Apply ANimation To Node");
    selectedAnimId = assetId;
    slectedAnimation = true;
    if (slectedAnimation) {
        applyAnimationToNode(selectedAnimId);
        slectedAnimation = false;
    }
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_getBoneCount(JNIEnv *env, jclass type,
                                                                  jint assetId) {

    return (jint) animationScene->nodes[animationScene->selectedNodeId]->joints.size();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_stopAllAnimations(JNIEnv *env, jclass type) {

    pauseRendering = true;
    stopAllAnimations();
    pauseRendering = false;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_playAnimation(JNIEnv *env, jclass type) {
    pauseRendering = false;
    playAnimation();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_remove3dText(JNIEnv *env, jclass type) {

    remove3DText();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_load3DText(JNIEnv *env, jclass type,
                                                                jstring displayText_, jint size,
                                                                jint bevelValue, jint red,
                                                                jint blue,
                                                                jint green, jstring filename_) {
    Logger::log(INFO, "IYAN3D", "JNI CALL");
    const char *displayText = env->GetStringUTFChars(displayText_, 0);
    const char *filename = env->GetStringUTFChars(filename_, 0);
    pauseRendering = false;
    fontdisplayText = displayText;
    fontSize = size;
    fontbevelSize = bevelValue;
    fontColorRed = red;
    fontColorGreen = green;
    fontColorBlue = blue;
    fontFilePath = filename;
    displayTextAsset = true;
    // TODO

    env->ReleaseStringUTFChars(displayText_, displayText);
    env->ReleaseStringUTFChars(filename_, filename);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_add3DTextToScene(JNIEnv *env, jclass type) {


    import3DText = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_scaleAction(JNIEnv *env, jclass type, jfloat x,
                                                        jfloat y,
                                                        jfloat z,jboolean state) {

    animationScene->changeObjectScale(Vector3(x, y, z), state);

}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getScaleXval(JNIEnv *env, jclass type) {

    Vector3 currentvalue = animationScene->getSelectedNodeScale();
    return (jfloat) currentvalue.x;

}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getScaleYval(JNIEnv *env, jclass type) {

    Vector3 currentvalue = animationScene->getSelectedNodeScale();
    return (jfloat) currentvalue.y;

}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getScaleZval(JNIEnv *env, jclass type) {

    Vector3 currentvalue = animationScene->getSelectedNodeScale();
    return (jfloat) currentvalue.z;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_deleteObject(JNIEnv *env, jclass type) {

    if (animationScene->selectedNodeId != NOT_SELECTED) {
        if (animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT ||
            animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_IMAGE) {
            animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
        } else {
            animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
        }
        objectToBeDeleted = animationScene->selectedNodeId;
        deleteObjectAction = true;


    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_undoRedoDeleteObject(JNIEnv *env, jclass type) {

    if (animationScene->selectedNodeId != NOT_SELECTED) {
        objectToBeDeleted = animationScene->selectedNodeId;
        deleteObjectAction = true;
    }
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraTopView(JNIEnv *env, jclass type) {

    if (animationScene) {
        animationScene->changeCameraView(VIEW_TOP);
        animationScene->updateLightCamera();
    }
    else {
        topViewAutoRig();
    }
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraLeftView(JNIEnv *env, jclass type) {
    if (animationScene) {
        animationScene->changeCameraView(VIEW_LEFT);
        animationScene->updateLightCamera();
    }
    else {
        leftViewAutoRig();
    }


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraRightView(JNIEnv *env, jclass type) {
    if (animationScene) {
        animationScene->changeCameraView(VIEW_RIGHT);
        animationScene->updateLightCamera();
    }
    else {
        rightViewAutoRig();
    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraBottomView(JNIEnv *env, jclass type) {
    if (animationScene) {
        animationScene->changeCameraView(VIEW_BOTTOM);
        animationScene->updateLightCamera();
    }
    else {
        bottomViewAutoRig();
    }
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraFrontView(JNIEnv *env, jclass type) {
    if (animationScene) {
        animationScene->changeCameraView(VIEW_FRONT);
        animationScene->updateLightCamera();
    }
    else {
        frontViewAutoRig();
    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraBackView(JNIEnv *env, jclass type) {

    if (animationScene) {
        animationScene->changeCameraView(VIEW_BACK);
        animationScene->updateLightCamera();
    }
    else {
        backViewAutoRig();

    }
}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getMeshBrightness(JNIEnv *env, jclass type) {

    return (jfloat) animationScene->nodes[animationScene->selectedNodeId]->props.brightness;

}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getMeshSpecular(JNIEnv *env, jclass type) {

    return (jfloat) animationScene->nodes[animationScene->selectedNodeId]->props.shininess;

}

JNIEXPORT jboolean JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getLightingSwitch(JNIEnv *env, jclass type) {

    return (jboolean) animationScene->nodes[animationScene->selectedNodeId]->props.isLighting;

}

JNIEXPORT jboolean JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getVisibilitySwitch(JNIEnv *env, jclass type) {

    return (jboolean) animationScene->nodes[animationScene->selectedNodeId]->props.isVisible;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_meshPropertyChanged(JNIEnv *env, jclass type,
                                                                jfloat brightness, jfloat specular,
                                                                jboolean lighting,
                                                                jboolean visibility) {
    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Visibility : %d ", visibility);
    animationScene->changeMeshProperty(brightness, specular, lighting, visibility, true);

}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getCameraFov(JNIEnv *env, jclass type) {

    return (jfloat) animationScene->cameraFOV;

}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getCameraResolution(JNIEnv *env, jclass type) {

    return (jint) animationScene->cameraResolutionType;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(JNIEnv *env, jclass type,
                                                                  jfloat fov, jint resolution) {

    if (animationScene->cameraResolutionType != resolution) {
        animationScene->changeCameraProperty(fov, resolution, true);
    }
    else {
        animationScene->changeCameraProperty(fov, resolution, false);
    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_frameClicked(JNIEnv *env, jclass type, jint position) {

    animationScene->previousFrame = animationScene->currentFrame;
    animationScene->currentFrame = position;
    animationScene->switchFrame(animationScene->currentFrame);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_playAnimation(JNIEnv *env, jclass type) {

    isPlaying = !isPlaying;
    animationScene->isPlaying = isPlaying;

    if (animationScene->selectedNodeId != NOT_SELECTED) {
        animationScene->unselectObject(animationScene->selectedNodeId);
    }


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_playTimer(JNIEnv *env, jclass type) {

    if (isPlaying) {
        if (animationScene->currentFrame + 1 < animationScene->totalFrames) {

            animationScene->currentFrame++;
            animationScene->switchFrame((float) animationScene->currentFrame);
        }
        else if (animationScene->currentFrame + 1 >= animationScene->totalFrames) {
            //STOP
            stopPlaying();
        }
        else if (animationScene->currentFrame == animationScene->totalFrames) {
            //STOP
            stopPlaying();
        }
        animationScene->setDataForFrame(animationScene->currentFrame);
    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_applyAnimationToNodeAnimationEditor(
        JNIEnv *env,
        jclass type,
        jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

    animationFilePath = filePath;
    applyAnimation(filePath);
    animationApplied = true;
    env->ReleaseStringUTFChars(filePath_, filePath);
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getAnimationFrameCount(JNIEnv *env, jclass type) {

    return (jint) animationScene->totalFrames;
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_stopAnimation(JNIEnv *env, jclass type) {

    stopPlaying();

}


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_selectObject(JNIEnv *env, jclass type, jint position) {
    objectTobeSelected = position;

    selectObject = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env, jclass type,
                                                          jstring projectFileName_) {
    const char *projectFileName = env->GetStringUTFChars(projectFileName_, 0);

    projectFileNameSaveData = projectFileName;
    isSaving = true;
    isFileSaveCompleted = true;
    while (isFileSaveCompleted)
        sleep(1);
    env->ReleaseStringUTFChars(projectFileName_, projectFileName);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_saveThumbnail(JNIEnv *env, jclass type,
                                                          jstring projectFileName_) {
    const char *projectFileName = env->GetStringUTFChars(projectFileName_, 0);
      // TODO
    env->ReleaseStringUTFChars(projectFileName_, projectFileName);
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_undoFunction(JNIEnv *env, jclass type) {


    undoAction = true;
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getReturnValue(JNIEnv *env, jclass type) {

    return (jint) undoValue;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_deleObejectBtNodeId(JNIEnv *env, jclass type,
                                                                jint nodeid) {

    objectToBeDeleted = nodeid;
    deleteObjectAction = true;

}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getCurrentFrame(JNIEnv *env, jclass type) {

    return (jint) animationScene->currentFrame;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_addFramebtn(JNIEnv *env, jclass type) {

    animationScene->totalFrames++;
    animationScene->currentFrame = animationScene->totalFrames - 1;
    animationScene->switchFrame(animationScene->currentFrame);


}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_redoFunction(JNIEnv *env,
                                                                                jclass type) {
//
//    int returnValue = animationScene->redo();
//
//    return returnValue;
    redoAction = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_addAssetToScenenAnimationEditor(JNIEnv *env,
                                                                            jclass type,
                                                                            jint assetId,
                                                                            jint nodeType,
                                                                            jint assetAddType,jstring Assetname_) {
    assetView = false;


    assetView = false;
    const char *assetName = env->GetStringUTFChars(Assetname_, 0);
    importedAssetName = assetName;
    env->ReleaseStringUTFChars(Assetname_, assetName);

    if (nodeType == 1)
        selectedNodeType = NODE_RIG;
    else if (nodeType == 6)
        selectedNodeType = NODE_OBJ;
    else
        selectedNodeType = NODE_SGM;


    if (assetAddType == 1) {
        assetActionType = UNDO_ACTION;
    }
    else if (assetAddType == 2) {
        assetActionType = REDO_ACTION;
    }

    if (assetId > 900 && assetId < 1000) {
        light_asset_id = assetId;
        isAddExtraLight = true;
    }
    else
        clickedAssetId = assetId;

    Logger::log(INFO, "IYAN3D",
                "Node Type : " + to_string(nodeType) + " " + "Asset Id : " + to_string(assetId) +
                " " + "Asset Add Type : " + to_string(assetAddType));
}

JNIEXPORT jstring JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_addTextImageBack(JNIEnv *env, jclass type) {
    Logger::log(INFO, "IYAN3D", "ADD TEXT IMAGE");
    std::string name = ConversionHelper::getStringForWString(
            animationScene->nodes[animationScene->nodes.size() - 1]->name);


    return env->NewStringUTF(name.c_str());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env, jclass type, jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

//    std::string s = filePath;
    loadFilepath = filePath;
//    Logger::log(INFO,"IYAN3D","file path loa: "+s);
    env->ReleaseStringUTFChars(filePath_, filePath);
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getAssetCount(JNIEnv *env, jclass type) {

    return (jint) animationScene->nodes.size();

}

JNIEXPORT jstring JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getAssetList(JNIEnv *env, jclass type, jint i) {

    // TODO
    string assetName = ConversionHelper::getStringForWString(animationScene->nodes[i]->name);
    string returnValue;
    if (animationScene->nodes[i]->getType() == NODE_TEXT) {
        returnValue = "TEXT: " + assetName;
    }
    else if (animationScene->nodes[i]->getType() == NODE_IMAGE) {
        returnValue = "IMAGE: " + assetName;
    } else {
        returnValue = assetName;
    }

    Logger::log(INFO, "IYAN3D.CPP", "Assets Names For ListViews : " + assetName);

    return env->NewStringUTF(returnValue.c_str());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_importImageAction(JNIEnv *env, jclass type,
                                                              jstring imageName_, jint imageWidth,
                                                              jint imageHeight) {
    const char *imageName = env->GetStringUTFChars(imageName_, 0);
    imagePath = imageName;
    imgWidth = imageWidth;
    imgHeight = imageHeight;
    assetActionType = IMPORT_ASSET_ACTION;
    importImage = true;


    env->ReleaseStringUTFChars(imageName_, imageName);
}
JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_renderFrame(JNIEnv *env, jclass type, jint frame,
                                                        jint shaderType, jboolean watermark,
                                                        jstring path_, jint resolution_value) {
    const char *path = env->GetStringUTFChars(path_, 0);


    animationScene->isExportingImages = true;
    animationScene->setDataForFrame(frame);
    //Logger::log(INFO,"IYAN3D","Cache path: "+to_string(path));

    cachePath = path;
    shadertype = shaderType;
    waterMark = watermark;
    resolution = resolution_value;
    renderingAction = true;
    while (renderingAction) sleep(1);
    env->ReleaseStringUTFChars(path_, path);
}

JNIEXPORT jboolean JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_saveTemplate(JNIEnv *env, jclass type,
                                                         jstring assetId_) {
    const char *assetId = env->GetStringUTFChars(assetId_, 0);
    Logger::log(INFO, "IYAN3D", "Store animation Started");
    templateAssetId = stoi(assetId);
    storeAsTemplate = true;
    return (jboolean) savingTemplatestatus;

    env->ReleaseStringUTFChars(assetId_, assetId);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_initAutorig(JNIEnv *env, jclass type, jint width,
                                                               jint height) {

    isAutorigScene = initAutorig(env, type, width, height);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_stepAutoRig(JNIEnv *env, jclass type) {
    if (isAutorigScene && autorigObjPath.length() > 1 && autorigTexturePath.length() > 1) {
        addObjToScene(autorigObjPath, autorigTexturePath);
        Logger::log(INFO, "IYAN3D.CPP", "Add obj to scene render all");
        Logger::log(INFO, "IYAN3D.CPP", "Add obj path: " + autorigObjPath);
        Logger::log(INFO, "IYAN3D.CPP", "Add obj Text path: " + autorigTexturePath);
        autorigObjPath = "";
        autorigTexturePath = "";
    }
    if (checkControlSelectionAutoRig) {
        checkcontrolSelection(touchBeganAutoRig);
        checkControlSelectionAutoRig = false;
    }
    if (checktappositionAutorig) {
        tapAutoRig(tapPositionAutorig, env, type);
        checktappositionAutorig = false;
        callBackGetSceneMode(env,type,getSceneMode());
    }

    if (ownRig) {
        ownRigging();
        callBackGetSceneMode(env,type,getSceneMode());
        ownRig = false;
    }
    if (humanRig) {
        humanRigging();
        callBackGetSceneMode(env,type,getSceneMode());
        humanRig = false;
    }
    if (nextBtnpressed) {
        nextBtnAction(rigFilePath);
        jstring totalFrameCount;
        jmethodID mid = env->GetStaticMethodID(type, "sgrCreated", "()V");
        env->CallStaticVoidMethod(type, mid);
        callBackGetSceneMode(env,type,getSceneMode());
        nextBtnpressed = false;
    }
    if (backBtnPressed) {
        moveToPrevMode();
        callBackGetSceneMode(env,type,getSceneMode());
        backBtnPressed = false;
    }

    if(autorigRedo){
        redoFunction();
        callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
        autorigRedo = false;
    }
    if(autorigUndo){
        undoFunction();
        callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
        autorigUndo = false;
    }

    if (isAddNewJoint) {
        addNewJoint();
        if (sgAutoRigScene->isReachMaxBoneCount())
            boneLimitCallback(env, type);
        callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());

        isAddNewJoint = false;
    }
    if(renderAutoRig())
        callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_addObjToScene(JNIEnv *env, jclass type,
                                                                 jstring objPath_,
                                                                 jstring texturePath_) {
    const char *objPath = env->GetStringUTFChars(objPath_, 0);
    const char *texturePath = env->GetStringUTFChars(texturePath_, 0);

    autorigObjPath = objPath;
    autorigTexturePath = texturePath;
    Logger::log(INFO, "IYAN3D.CPP", "Add obj to scene");

    env->ReleaseStringUTFChars(objPath_, objPath);
    env->ReleaseStringUTFChars(texturePath_, texturePath);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_panBeginAutorig(JNIEnv *env, jclass type,
                                                                   jfloat cordX1, jfloat cordY1,
                                                                   jfloat cordX2, jfloat cordY2) {

    panBeginAutorig(cordX1, cordY1, cordX2, cordY2);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_panProgress(JNIEnv *env, jclass type,
                                                               jfloat cordX1, jfloat cordY1,
                                                               jfloat cordX2, jfloat cordY2) {
    panProgressAutorig(cordX1, cordY1, cordX2, cordY2);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_swipeAutorig(JNIEnv *env, jclass type,
                                                                jfloat velocityX,
                                                                jfloat velocityY) {

    swipe(velocityX, velocityY);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapAutorig(JNIEnv *env, jclass type, jfloat x,
                                                              jfloat y) {


    tapPositionAutorig = Vector2(x, y);


    checktappositionAutorig = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_rotateAction(JNIEnv *env, jclass type) {

    rotateaction();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_moveAction(JNIEnv *env, jclass type) {

    moveAction();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_singleBone(JNIEnv *env, jclass type) {

    ownRig = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_humanRigging(JNIEnv *env, jclass type) {

    humanRig = true;
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_paBeginanim(JNIEnv *env, jclass type,
                                                                 jfloat cordX1, jfloat cordY1,
                                                                 jfloat cordX2, jfloat cordY2) {
    panBegin(cordX1, cordY1, cordX2, cordY2);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_panProgressanim(JNIEnv *env, jclass type,
                                                                     jfloat cordX1, jfloat cordY1,
                                                                     jfloat cordX2, jfloat cordY2) {
    panprogress(cordX1, cordY1, cordX2, cordY2);


}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_deleteScene(JNIEnv *env, jclass type) {
    if (animationScene) {

        Logger::log(INFO, "iyan3d.cpp", "delete scedne");
        delete animationScene;
        animationScene = NULL;
        loadFilepath = "init";
        Logger::log(INFO, "iyan3d.cpp", "delete scene after");


    }

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAssetView_deletePreviewScene(JNIEnv *env, jclass type) {

    destroyScene();

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_controlSelectionAutoRig(JNIEnv *env, jclass type,
                                                                           jfloat x, jfloat y) {

    touchBeganAutoRig = Vector2(x, y);

    checkControlSelectionAutoRig = true;

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapMoveAutorig(JNIEnv *env, jclass type,
                                                                  jfloat x, jfloat y) {

    tapBegin(x, y, touchBeganAutoRig, oglWidth, oglHeight);

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_tapEndAutorig(JNIEnv *env, jclass type, jfloat x,
                                                                 jfloat y) {

    // TODO
    tapEnd(x, y);
    callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_nextBtnAurorig(JNIEnv *env, jclass type,
                                                                  jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

    rigFilePath = filePath;

    nextBtnpressed = true;

    env->ReleaseStringUTFChars(filePath_, filePath);
    callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_scaleAutoRig(JNIEnv *env, jclass type,
                                                                jfloat x,jboolean state) {

    ScaleAutoRig(x,state);
    if(state == true)
        callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_prevBtnAction(JNIEnv *env, jclass type) {


    backBtnPressed = true;
    callBackAutoRigUndoRedoBtnState(env,type,getUndoOrRedoButtonAction());

}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_unodBtnAction(JNIEnv *env, jclass type) {
    autorigUndo = true;
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_redoBtnAction(JNIEnv *env, jclass type) {
    autorigRedo = true;
}
JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_addExtraLight(JNIEnv *env, jclass type) {
    if (ShaderManager::lightPosition.size() < 5) {
        light_asset_id = ASSET_ADDITIONAL_LIGHT + lightCount;
        animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, light_asset_id, "Light" +
                                                                                       (light_asset_id -
                                                                                        ASSET_ADDITIONAL_LIGHT));
        lightCount++;
        assetActionType = IMPORT_ASSET_ACTION;
        isAddExtraLight = true;
    }
    else {
        Logger::log(INFO, "Iyan3D.cpp", "Light Reach Maximum ");
    }

    return ShaderManager::lightPosition.size();
}
JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env, jclass type, jfloat R,
                                                                jfloat G, jfloat B, jfloat shadow) {
    Red = R;
    Green = G;
    Blue = B;
    shadowDensity = shadow;
    isLightValueChanged = true;
}
JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getShadowDensity(JNIEnv *env, jclass type) {
    if (animationScene->getSelectedNodeType() == 7) {
        Quaternion lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->rotationKeys, true);
        return lightProps.w;
    }
    else
        return ShaderManager::shadowDensity;
}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesRed(JNIEnv *env, jclass type) {
    if (animationScene->getSelectedNodeType() == 7) {
        Quaternion lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->rotationKeys, true);
        return lightProps.x;
    }
    else {
        Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->scaleKeys);
        return mainLight.x;
    }
}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesGreen(JNIEnv *env, jclass type) {
    if (animationScene->getSelectedNodeType() == 7) {
        Quaternion lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->rotationKeys, true);
        return lightProps.y;
    }
    else {
        Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->scaleKeys);
        return mainLight.y;

    }
}
JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getLightPropertiesBlue(JNIEnv *env, jclass type) {
    if (animationScene->getSelectedNodeType() == 7) {
        Quaternion lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->rotationKeys, true);
        return lightProps.z;
    }
    else {
        Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(
                animationScene->currentFrame,
                animationScene->nodes[animationScene->selectedNodeId]->scaleKeys);
        return mainLight.z;
    }
}
JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_setLightingOff(JNIEnv *env, jclass type) {
    animationScene->setLightingOff();
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_setLightingOn(JNIEnv *env, jclass type) {
    animationScene->setLightingOn();
}

JNIEXPORT jfloat JNICALL
Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getSelectedNodeOrJointScaleAutoRig(JNIEnv *env,
                                                                                      jclass type) {
    return getSelectedJointScale();
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_resetClickedAssetId(JNIEnv *env, jclass type) {
    clickedAssetId = 0;
}

JNIEXPORT void
JNICALL Java_com_smackall_animator_opengl_GL2JNILib_exportingCompleted(JNIEnv *env, jclass type) {
    animationScene->isExportingImages = false;
}

JNIEXPORT void
JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchMirrorState(JNIEnv *env, jclass type) {
    animationScene->switchMirrorState();
}

JNIEXPORT jboolean
JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMirrorState(JNIEnv *env, jclass type) {
    return animationScene->getMirrorState();
}

JNIEXPORT jint
JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getJointSize(JNIEnv *env, jclass type) {
    return (jint) animationScene->nodes[animationScene->selectedNodeId]->joints.size();

}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_scaleJointAutorig(
        JNIEnv *env, jclass type, jfloat x, jfloat y, jfloat z,jboolean state) {
    ScaleboneAutoRig(x, y, z,state);
}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightcount(JNIEnv *env,
                                                                                 jclass type) {
    return ShaderManager::lightPosition.size();
}


JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isKeySetForNode(JNIEnv *env,
                                                                                       jclass type, jint currentFrame) {
    if(animationScene)
        return animationScene->isKeySetForFrame.find(currentFrame) != animationScene->isKeySetForFrame.end();
    else
        return false;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_addNewJoint(JNIEnv *env,
                                                                                      jclass type) {
    isAddNewJoint = true;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_deleteAutoRigScene(
        JNIEnv *env, jclass type) {
    deleteAutoRigScene();
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteanimation(JNIEnv *env,
                                                                                       jclass type) {
    bool isKeySetForNode = animationScene->removeAnimationForSelectedNodeAtFrame(
            animationScene->currentFrame);
    return isKeySetForNode;
}


JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAssetView_isPreviewScene(
        JNIEnv *env, jclass type) {
    return isPreviewScene();
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env,
                                                                                       jclass type) {
    return animationScene->isJointSelected;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_isAutoRigJointSelected(
        JNIEnv *env, jclass type) {
    isJointSelected();
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_updateMirror(
        JNIEnv *env, jclass type) {
    return updateMirror();
}

JNIEXPORT jfloat Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueX(JNIEnv *env, jclass type) {
    Vector3 value = getSelectedNodeScale();
    return value.x;
}
JNIEXPORT jfloat Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueY(JNIEnv *env, jclass type) {
    Vector3 value = getSelectedNodeScale();
    return value.y;
}
JNIEXPORT jfloat Java_com_smackall_animator_opengl_GL2JNILibAutoRig_getAutoRigScalevalueZ(JNIEnv *env, jclass type) {
    Vector3 value = getSelectedNodeScale();
    return value.z;
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_skeletonType(JNIEnv *env, jclass type){
    return getskeletonType();
}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILibAutoRig_selectedNodeId(JNIEnv *env, jclass type){
    return getselectedNodeId();
 }

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_storeShaderProps(JNIEnv *env, jclass type){
    sceneShadow = ShaderManager::shadowDensity;
    sceneLightColor = ShaderManager::lightColor[0];
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resetContext(JNIEnv *env, jclass type){
    if(animationScene) {
        ShaderManager::shadowDensity = sceneShadow;
        animationScene->clearLightProps();
        animationScene->setDataForFrame(animationScene->currentFrame);
        animationScene->updateLightCamera();
    }
  }
}


