#include <jni.h>
#include "Iyan3dEngineFiles/HeaderFiles/SGEditorScene.h"
#include "SGEngine2/SceneManager/SceneManager.h"
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/string.h"

int PREVIEW_LEFTBOTTOM = 0;
int PREVIEW_LEFTTOP = 1;
int PREVIEW_RIGHTBOTTOM = 2;
int PREVIEW_RIGHTTOP = 3;
int TOOLBAR_RIGHT = 1;
int TOOLBAR_LEFT  =0;

int FRONT_VIEW = 0;
int TOP_VIEW = 1;
int LEFT_VIEW = 2;
int BACK_VIEW = 3;
int RIGHT_VIEW = 4;
int BOTTOM_VIEW = 5;

int ASSET_RIGGED = 1;
int ASSET_BACKGROUNDS = 2;
int ASSET_ACCESSORIES = 3;
int ASSET_ANIMATION = 4;
int ASSET_OBJ = 6;
int ASSET_CAMERA = 7;
int ASSET_LIGHT = 8;
int ASSET_IMAGE = 9;
int ASSET_TEXT_RIG = 10;
int ASSET_TEXT = 11;
int ASSET_VIDEO = 12;
int ASSET_PARTICLES = 13;
int ASSET_ADDITIONAL_LIGHT = 900;
int OWN_RIGGING = 0;
int HUMAN_RIGGING = 1;

SGEditorScene *editorScene;
SceneManager *sceneManager;
GLuint _colorRenderBuffer;
GLuint _depthRenderBuffer;
GLuint _frameBuffer;
int oglWidth, oglHeight;
float screenScale = 1.0;
string loadFilepath="init";
string assetStoragePath, meshStoragePath,importedImagesPath;
bool checkControlSelection = false;
Vector2 touchBegan;
float touchX = 0.0;
float touchY = 0.0;
Vector2 prevPos = Vector2(0.0,0.0);
Vector2 currentPos = Vector2(0.0,0.0);
Vector2 tapPosition;
bool checktapposition = false;
int selectedNodeId = -1;
bool displayPrepared = false;
string fileName = "";
bool sceneSaved = false;


void addCameraLight()
{
	editorScene->loader->loadNode(NODE_CAMERA, 0,"",L"CAMERA", 0, 0, IMPORT_ASSET_ACTION);
	editorScene->loader->loadNode(NODE_LIGHT, 0,"", L"LIGHT", 0, 0, IMPORT_ASSET_ACTION);
	// animationScene->loadNode(NODE_RIG,32554,"FATMAN",0,0,IMPORT_ASSET_ACTION);
}

void shaderCallBackForNode(int nodeID, string matName, string callbackFuncName)
{
	if(!editorScene) return;
	if (callbackFuncName.compare("setUniforms") == 0)
		editorScene->shaderCallBackForNode(nodeID, matName);
	else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
		editorScene->setJointsUniforms(nodeID, matName);
	else if (callbackFuncName.compare("setCtrlUniforms") == 0)
		editorScene->setControlsUniforms(nodeID, matName);
	else if (callbackFuncName.compare("RotationCircle") == 0)
		editorScene->setRotationCircleUniforms(nodeID, matName);
	else if(callbackFuncName.compare("ObjUniforms") == 0){
		editorScene->rigMan->objNodeCallBack(matName);
	}else if(callbackFuncName.compare("jointUniforms") == 0){
		editorScene->rigMan->jointNodeCallBack(nodeID,matName);
	}else if(callbackFuncName.compare("BoneUniforms") == 0){
		editorScene->rigMan-> boneNodeCallBack(nodeID,matName);
	}else if(callbackFuncName.compare("envelopeUniforms") == 0){
		editorScene->rigMan->setEnvelopeUniforms(nodeID,matName);
	}else if(callbackFuncName.compare("sgrUniforms") == 0){
		editorScene->rigMan->setSGRUniforms(nodeID,matName);
	}
}

bool isTransparentCallBack(int nodeId, string callbackFuncName)
{
	if(!editorScene) return false;
	if (callbackFuncName.compare("setUniforms") == 0)
		return editorScene->isNodeTransparent(nodeId);
	else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
		return editorScene->isJointTransparent(nodeId, callbackFuncName);
	else if (callbackFuncName.compare("setCtrlUniforms") == 0)
		return editorScene->isControlsTransparent(nodeId, callbackFuncName);
	else if (callbackFuncName.compare("setOBJUniforms") == 0)
		return false;
	else if (callbackFuncName.compare("RotationCircle") == 0)
		return false;
	else if(callbackFuncName.compare("ObjUniforms") == 0)
		return editorScene->rigMan->isOBJTransparent(callbackFuncName);
	else if(callbackFuncName.compare("jointUniforms") == 0)
		return false;
	else if(callbackFuncName.compare("BoneUniforms") == 0)
		return false;
	else if(callbackFuncName.compare("envelopeUniforms") == 0)
		return false;
	else if(callbackFuncName.compare("sgrUniforms") == 0)
		return editorScene->rigMan->isSGRTransparent(nodeId,callbackFuncName);
	return false;
}

void setCallBack()
{
	sceneManager->ShaderCallBackForNode = &shaderCallBackForNode;
	sceneManager->isTransparentCallBack = &isTransparentCallBack;
}

void saveScene()
{
	string savePath1 = constants::DocumentsStoragePath + "/scenes/" + fileName + ".png";
	char *imagePath = new char[savePath1.length() + 1];
	strcpy(imagePath, savePath1.c_str());
	//Logger::log(INFO, "IYAN3D", "Path for Image: " + to_string(imagePath));
	editorScene->saveThumbnail(imagePath);
	string savePath = constants::DocumentsStoragePath + "/projects/" +fileName + ".sgb";
	Logger::log(INFO, "IYAN3D", "Path for animation: " + savePath);
	editorScene->saveSceneData(&savePath);
	sceneSaved = false;
}

void callBackIsDisplayPrepared(JNIEnv *env,jclass type){
	jmethodID mid = env->GetStaticMethodID(type, "callBackIsDisplayPrepared", "()V");
	env->CallStaticVoidMethod(type, mid);
}

Quaternion getLightProps()
{
	Quaternion lightProps;
	if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
		lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(editorScene->currentFrame,editorScene->nodes[editorScene->selectedNodeId]->rotationKeys,true);
	if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT){
		Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
		lightProps = Quaternion(mainLight.x,mainLight.y,mainLight.z,ShaderManager::shadowDensity);
	}
	return lightProps;
}

void cameraPreviewPosition(int position, int previewSize,float topHeight,float toolBarWidth)
{
	if(!editorScene)
		return;
	editorScene->camPreviewScale = (previewSize == 1) ? 1.0 : 2.0;
	float camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
	if(position==PREVIEW_LEFTBOTTOM){
		camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
		if(editorScene->camPreviewScale==1.0){
			editorScene->camPreviewOrigin.x=0;
			editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
			editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

		}
		if(editorScene->camPreviewScale==2.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewOrigin.x=0.0000;
			editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

		}
	}
	if(position==PREVIEW_LEFTTOP){
		camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
		if(editorScene->camPreviewScale==1.0){
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
			editorScene->camPreviewOrigin.x=0.0000;
			editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
		}
		if(editorScene->camPreviewScale==2.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
			editorScene->camPreviewOrigin.x=0.0000;
			editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
		}
	}

	if(position==PREVIEW_RIGHTBOTTOM){
		if(editorScene->camPreviewScale==1.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale-camPrevRatio;
			editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
			editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;


		}
		if(editorScene->camPreviewScale==2.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
			editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

		}
	}
	if(position==PREVIEW_RIGHTTOP){

		if(editorScene->camPreviewScale==1.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
			editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
			editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
		}
		if(editorScene->camPreviewScale==2.0){
			camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
			editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
			editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
			editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
			editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
		}
	}

	Logger::log(INFO,"IYAN3D.CPP","Camera Position "  +to_string(editorScene->camPreviewOrigin.x)+" "+to_string(editorScene->camPreviewOrigin.y)+" "+ to_string(editorScene->camPreviewEnd.x)+" " + to_string(editorScene->camPreviewEnd.y));
}

void cameraPositionViaToolBarPosition(int selectedIndex, float rightWidth,float topViewHeight)
{
	Logger::log(INFO,"Iyan 3D ", "Camera Size " + to_string(selectedIndex) +" "+to_string(rightWidth)+" " + to_string(topViewHeight));

	if(editorScene) {
		editorScene->topLeft = Vector2((selectedIndex==TOOLBAR_LEFT) ? rightWidth : 0.0 , topViewHeight) * editorScene->screenScale;
		editorScene->topRight = Vector2((selectedIndex==TOOLBAR_LEFT) ? oglWidth : oglWidth-rightWidth, topViewHeight) * editorScene->screenScale;
		editorScene->bottomLeft = Vector2((selectedIndex==TOOLBAR_LEFT) ? rightWidth : 0.0, oglHeight) * editorScene->screenScale;
		editorScene->bottomRight = Vector2((selectedIndex==TOOLBAR_LEFT) ?  oglWidth : oglWidth-rightWidth, oglHeight) * editorScene->screenScale;
		editorScene->renHelper->movePreviewToCorner();
	}
}

void boneLimitsCallBack()
{

}

void updateRenderer(JNIEnv *env, jclass type)
{
	displayPrepared = sceneManager->PrepareDisplay(oglWidth, oglHeight, true, true, false,
												   Vector4(0.1, 0.1, 0.1, 1.0));
	if (loadFilepath == "init") {
		addCameraLight();
		loadFilepath = "";
		callBackIsDisplayPrepared(env, type);
	}
	else if (loadFilepath.size() > 0) {
		editorScene->loadSceneData(&loadFilepath);
		loadFilepath = "";
		callBackIsDisplayPrepared(env, type);
	}
	editorScene->renderAll();
	if(fileName.size() > 0){
		saveScene();
		fileName = "";
	}
}



extern "C" {
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type,jint width, jint height);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env,jclass type,jstring assetPath_,jstring setMeshpath_,jstring setImagepath_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapEnd(JNIEnv *env, jclass type,jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env, jclass type, jfloat x,jfloat y,jboolean isMultiSelect);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type,jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_touchBegan(JNIEnv *env, jclass type,jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type,jfloat velocityX,jfloat velocityY);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_panBegin(JNIEnv *env, jclass type,jfloat cordX1,jfloat cordY1,jfloat cordX2,jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env,jclass type,jfloat cordX1,jfloat cordY1,jfloat cordX2,jfloat cordY2);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type, jfloat x, jfloat y,jboolean isMultiSelect);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env, jclass type, jstring filePath_);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_save(JNIEnv *env, jclass type, jstring filePath);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_dealloc(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempNode(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeCount(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeType(JNIEnv *env, jclass type,jint nodeId);
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeName(JNIEnv *env, jclass type,jint nodeId);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectNode(JNIEnv *env, jclass type,jint nodeId,jboolean isMultiSelect);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeNode(JNIEnv *env, jclass type,jint nodeId,jboolean isUndoOrRedo);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyPropsOfNode(JNIEnv *env, jclass type,jint from,jint to);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetIdWithNodeId(JNIEnv *env, jclass type,jint nodeId);
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getTexture(JNIEnv *env, jclass type,jint selectedNode);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorX(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorY(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZ(JNIEnv *env, jclass type);
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePath(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloat(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSize(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_refractionValue(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_reflectionValue(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isLightning(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isVisible(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_jointSize(JNIEnv *env, jclass type,jint selectedNode);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_mirrorState(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraFov(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resolutionType(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightx(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lighty(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightz(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_shadowDensity(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeMeshProperty(JNIEnv *env, jclass type,jfloat refraction,jfloat reflection,jboolean light,jboolean visible,jboolean storeAction);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchMirror(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env, jclass type,jfloat x, jfloat y, jfloat z, jfloat w,jfloat distance,jboolean storeAction);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_previewPosition(JNIEnv *env, jclass type,jint position,jint previewSize,jfloat topHeight,jfloat toolbarWidth);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchFrame(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setCurrentFrame(JNIEnv *env, jclass type,jint currentFrame);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_stopPlaying(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_play(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_totalFrame(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addFrame(JNIEnv *env, jclass type,jint frame);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_currentFrame(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPlaying(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setIsPlaying(JNIEnv *env, jclass type,jboolean isPlaying);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPositionViaToolBarPosition(JNIEnv *env, jclass type,jint selectedIndex, jfloat rightWidth,jfloat topHeight);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAsset(JNIEnv *env,jclass type,jint assetType,jint assetId,jstring assetName,jstring textureName,
		jint width,jint height,jboolean isTempNode,jfloat x,jfloat y, jfloat z,int assetActionType);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(JNIEnv *env, jclass type,jint fov,jint resolution,jboolean storeAction);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadText(JNIEnv *env, jclass type,jfloat red,jfloat green,jfloat blue,jint typeOfNode,jstring textureName,jstring assetName,jint fontSize,jint bevalValue,jint assetAddType,jstring filePath,jboolean isTempNode);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importImageOrVideo(JNIEnv *env, jclass type,jint nodeType,jstring assetName,jint imgWidth,jint imgHeight
		,jint assetAddType,jboolean isTempNode);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAdditionalLight(JNIEnv *env, jclass type,jint lightCount);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightCount(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAsSGM(JNIEnv *env, jclass type,jstring fileName,jstring textureName,jint assetId,jboolean haveTexture,jfloat x,jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeTexture(JNIEnv *env, jclass type,jint selectedNodeId,jstring textureName,jfloat x,jfloat y,jfloat z,jboolean isTemp);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempTexture(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_canEditRigBones(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_beginRigging(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cancelRig(JNIEnv *env, jclass type,jboolean completed);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addJoint(JNIEnv *env, jclass type,jobject obj);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchRigSceneMode(JNIEnv *env, jclass type,jobject obj,jint scene);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setSkeletonType(JNIEnv *env, jclass type,jint skeletonType);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObjects(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_hideNode(JNIEnv *env, jclass type,jint nodeId,jboolean hide);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyKeysOfNode(JNIEnv *env, jclass type,jint fromNodeId,jint toNodeId);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObject(JNIEnv *env, jclass type,jint nodeId);
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getBoneCount(JNIEnv *env, jclass type,jint nodeId);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTotalFrame(JNIEnv *env, jclass type,jint frame);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_applyAnimation(JNIEnv *env, jclass type,jint fromNodeId,jint applyedNodeId,jstring path);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scale(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueX(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueY(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueZ(JNIEnv *env, jclass type);
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_envelopScale(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setScaleValue(JNIEnv *env, jclass type,jfloat x,jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_move(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rotate(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rigNodeScale(JNIEnv *env, jclass type,float x, float y, float z);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isRigMode(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraView(JNIEnv *env, jclass type,int indexValue);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isNodeSelected(JNIEnv *env, jclass type,int position);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteAnimation(JNIEnv *env, jclass type);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env, jclass type,jobject obj,jint assetId,jstring name, jint animType);
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env, jclass type);















JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type, jint width,jint height)
{
if (editorScene)
{
LOGI("Scene already exists");
return;
}

oglWidth = width;
oglHeight = height;
Logger::log(INFO, "IYAN3D.CPP", constants::BundlePath);
sceneManager = new SceneManager(width, height, 1, OPENGLES2, constants::BundlePath);
editorScene = new SGEditorScene(OPENGLES2, sceneManager, width, height);
editorScene->screenScale = screenScale;
sceneManager->setFrameBufferObjects(0, _colorRenderBuffer, _depthRenderBuffer);

setCallBack();
//animationScene->downloadMissingAssetsCallBack = &downloadMissingAssetsCallBack;
}


JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type)
{
if (editorScene)
{
	updateRenderer(env,type);
}
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env, jclass type, jstring assetPath_, jstring setMeshpath_, jstring setImagepath_)
{
const char *assetPath = env->GetStringUTFChars(assetPath_, 0);
const char *setMeshpath = env->GetStringUTFChars(setMeshpath_, 0);
const char *setImagepath = env->GetStringUTFChars(setImagepath_, 0);
assetStoragePath = assetPath;
meshStoragePath = setMeshpath;
importedImagesPath = setImagepath;
constants::BundlePath = assetStoragePath;
constants::DocumentsStoragePath = meshStoragePath;
env->ReleaseStringUTFChars(assetPath_, assetPath);
env->ReleaseStringUTFChars(setMeshpath_, setMeshpath);
env->ReleaseStringUTFChars(setImagepath_, setImagepath);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapEnd(JNIEnv *env, jclass type, jfloat x, jfloat y)
{
if(editorScene->isRigMode)
editorScene->moveMan->touchEndRig(Vector2(x,y));
else
editorScene->moveMan->touchEnd(Vector2(x, y));
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env, jclass type, jfloat x,jfloat y,jboolean isMultiSelect)
{
touchBegan = Vector2(x, y);
editorScene->selectMan->checkCtrlSelection(Vector2(x, y),isMultiSelect,displayPrepared);
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type, jfloat x, jfloat y)
{

if(editorScene){
if(editorScene->isRigMode)
	editorScene->moveMan->touchMoveRig(Vector2(x, y), touchBegan, oglWidth * screenScale, oglHeight * screenScale);
else
	editorScene->moveMan->touchMove(Vector2(x, y), touchBegan, oglWidth, oglHeight);
}
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_touchBegan(JNIEnv *env, jclass type,jfloat x, jfloat y){
if(editorScene->isRigMode)
	editorScene->moveMan->touchBeganRig(Vector2(x,y)*screenScale);
else
	editorScene->moveMan->touchBegan(Vector2(x,y) * screenScale);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type, jfloat velocityX, jfloat velocityY)
{

if(editorScene){
	if(!editorScene->renHelper->isMovingPreview)
	editorScene->moveMan->swipeProgress(-velocityX / 50.0, -velocityY / 50.0);
}
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_panBegin(JNIEnv *env, jclass type, jfloat cordX1,
jfloat cordY1, jfloat cordX2, jfloat cordY2)
{

if (editorScene)
{
editorScene->moveMan->panBegan(Vector2(cordX1, cordY1), Vector2(cordX2, cordY2));
}
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env, jclass type, jfloat cordX1, jfloat cordY1, jfloat cordX2,jfloat cordY2)
{
editorScene->moveMan->panProgress(Vector2(cordX1, cordY1),Vector2(cordX2, cordY2));
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type, jfloat x, jfloat y,jboolean isMultiSelect)
{
tapPosition = Vector2(x, y) * screenScale;

if(editorScene->renHelper->isMovingCameraPreview(tapPosition)){
editorScene->setLightingOn();
editorScene->camPreviewScale = (editorScene->camPreviewScale == 1.0) ? 2.0 : 1.0;
return;
}

editorScene->isRTTCompleted = true;
if(editorScene->isRigMode)
	editorScene->selectMan->checkSelectionForAutoRig(Vector2(x, y));
else
	editorScene->selectMan->checkSelection(tapPosition,isMultiSelect, displayPrepared);
selectedNodeId = editorScene->selectedNodeId;
}

JNIEXPORT void JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env, jclass type, jstring filePath_) {
const char *filePath = env->GetStringUTFChars(filePath_, 0);
	loadFilepath = filePath;
    Logger::log(INFO,"IYAN3D","file path loa: "+loadFilepath);
env->ReleaseStringUTFChars(filePath_, filePath);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_save(JNIEnv *env, jclass type, jstring filePath)
{
const char *nativeString = env->GetStringUTFChars(filePath, 0);
fileName = nativeString;
sceneSaved = true;
while(sceneSaved)
	sleep(1);
env->ReleaseStringUTFChars(filePath, nativeString);
}

JNIEXPORT jint JNICALL
Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env, jclass type)
{
	checktapposition = true;
	return (jint) selectedNodeId;
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAsset(JNIEnv *env,jclass type,jint assetType ,jint assetId,jstring assetName,jstring textureName,
		jint width,jint height,jboolean isTempNode,jfloat x,jfloat y, jfloat z,int assetActionType)
{
const char *assetName_ = env->GetStringUTFChars(assetName, 0);
wstring assetNameW = ConversionHelper::getWStringForString(assetName_);
const char *nativeString = env->GetStringUTFChars(textureName, 0);
string texture = nativeString;
Vector4 color = Vector4(x,y,z,-1);
NODE_TYPE nodeType;
ActionType actionType;
if(assetType == 1)
	nodeType = NODE_RIG;
else
	nodeType = NODE_SGM;

if(assetId > 50000 && assetId < 60000)
	nodeType = NODE_PARTICLES;

if(assetActionType == 1)
	actionType = UNDO_ACTION;
else if(assetActionType == 2)
	actionType = REDO_ACTION;
else
	actionType = IMPORT_ASSET_ACTION;
editorScene->loader->removeTempNodeIfExists();
SGNode *sgnode = editorScene->loader->loadNode(nodeType, assetId, texture,assetNameW,0, 0,actionType,color,"",isTempNode);
if(sgnode)
	sgnode->isTempNode = isTempNode;
env->ReleaseStringUTFChars(textureName, nativeString);
env->ReleaseStringUTFChars(assetName, assetName_);

}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempNode(JNIEnv *env, jclass type)
{
editorScene->loader->removeTempNodeIfExists();
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeCount(JNIEnv *env, jclass type)
{
	return editorScene->nodes.size();
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeType(JNIEnv *env, jclass type,jint nodeId)
{
	return editorScene->nodes[nodeId]->getType();
}

JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeName(JNIEnv *env, jclass type,jint nodeId)
{
	return  env->NewStringUTF(ConversionHelper::getStringForWString(editorScene->nodes[nodeId]->name).c_str());
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectNode(JNIEnv *env, jclass type,jint nodeId,jboolean isMultiSelect)
{
	editorScene->selectMan->selectObject(nodeId,isMultiSelect);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeNode(JNIEnv *env, jclass type,jint nodeId,jboolean isUndoOrRedo)
{
if(editorScene->nodes.size()>nodeId){
if(editorScene->isNodeInSelection(editorScene->nodes[nodeId]))
editorScene->selectMan->multipleSelections(nodeId);
if(editorScene->isNodeSelected && editorScene->selectedNodeId != nodeId)
editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
editorScene->selectedNodeId = nodeId;
if(!isUndoOrRedo){
if(editorScene->nodes[nodeId]->getType() == NODE_TEXT_SKIN || editorScene->nodes[nodeId]->getType() == NODE_TEXT || editorScene->nodes[nodeId]->getType() == NODE_IMAGE || editorScene->nodes[nodeId]->getType() == NODE_VIDEO)
editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
else if (editorScene->nodes[nodeId]->getType() == NODE_OBJ || editorScene->nodes[nodeId]->getType() == NODE_SGM || editorScene->nodes[nodeId]->getType() == NODE_RIG || editorScene->nodes[nodeId]->getType() == NODE_ADDITIONAL_LIGHT|| editorScene->nodes[nodeId]->getType() == NODE_PARTICLES)
editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
}
editorScene->loader->removeObject(nodeId);

}
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyPropsOfNode(JNIEnv *env, jclass type,jint from,jint to)
{
	editorScene->animMan->copyPropsOfNode(from, (to == 0) ?(int)editorScene->nodes.size()-1 : to);
	editorScene->updater->setDataForFrame(editorScene->currentFrame);

}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env, jclass type)
{
	return editorScene->nodes[editorScene->selectedNodeId]->assetId;
}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetIdWithNodeId(JNIEnv *env, jclass type,jint nodeId){
	return editorScene->nodes[nodeId]->assetId;
}

JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getTexture(JNIEnv *env, jclass type,jint selectedNode)
{
	return env->NewStringUTF(editorScene->nodes[selectedNode]->textureName.c_str());
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorX(JNIEnv *env, jclass type)
{
	return editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x;
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorY(JNIEnv *env, jclass type)
{
	return editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZ(JNIEnv *env, jclass type)
{
	return editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.z;
}
JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePath(JNIEnv *env, jclass type)
{
	return env->NewStringUTF(editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str());
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloat(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.nodeSpecificFloat;
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSize(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.fontSize;
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_refractionValue(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.refraction;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_reflectionValue(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.reflection;
}
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isLightning(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.isLighting;
}
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isVisible(JNIEnv *env, jclass type){
	return editorScene->nodes[editorScene->selectedNodeId]->props.isVisible;
}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_jointSize(JNIEnv *env, jclass type,jint selectedNode){
	return editorScene->nodes[selectedNode]->joints.size();
}
JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_mirrorState(JNIEnv *env, jclass type){
	return editorScene->getMirrorState();
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraFov(JNIEnv *env, jclass type){
	return editorScene->cameraFOV;
}
JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resolutionType(JNIEnv *env, jclass type){
	return editorScene->cameraResolutionType;
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightx(JNIEnv *env, jclass type) {
	return getLightProps().x;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lighty(JNIEnv *env, jclass type) {
	return getLightProps().y;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightz(JNIEnv *env, jclass type) {
	return getLightProps().z;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_shadowDensity(JNIEnv *env, jclass type) {
	return ShaderManager::shadowDensity;
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeMeshProperty(JNIEnv *env, jclass type,jfloat refraction,jfloat reflection,jboolean light,jboolean visible,jboolean storeAction){
	editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, storeAction);
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchMirror(JNIEnv *env, jclass type){
if(editorScene && editorScene->isRigMode)
	editorScene->rigMan->switchMirrorState();
else
	editorScene->switchMirrorState();
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env, jclass type,jfloat x, jfloat y, jfloat z, jfloat w,jfloat distance,jboolean storeAction){
	editorScene->actionMan->changeLightProperty(x, y, z, w,distance,storeAction);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_previewPosition(JNIEnv *env, jclass type,jint position,jint previewSize,jfloat topHeight,jfloat toolbarWidth){
	Logger::log(INFO, "IYAN3D", "Camera Preview: " + to_string(position) + " " + to_string(previewSize));
	cameraPreviewPosition(position,previewSize,topHeight,toolbarWidth);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchFrame(JNIEnv *env, jclass type){
editorScene->actionMan->switchFrame(editorScene->currentFrame);
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setCurrentFrame(JNIEnv *env, jclass type,jint currentFrame)
{
editorScene->previousFrame = editorScene->currentFrame;
editorScene->currentFrame = currentFrame;
editorScene->actionMan->switchFrame(editorScene->currentFrame);
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_stopPlaying(JNIEnv *env, jclass type)
{
editorScene->setLightingOn();
editorScene->isPlaying = false;
editorScene->actionMan->switchFrame(editorScene->currentFrame);
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_totalFrame(JNIEnv *env, jclass type)
{
	return (editorScene) ? editorScene->totalFrames : 24;
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addFrame(JNIEnv *env, jclass type,jint frame){
	editorScene->totalFrames += frame;
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTotalFrame(JNIEnv *env, jclass type,jint frame){
editorScene->totalFrames = frame;
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_play(JNIEnv *env, jclass type)
{

	if (editorScene->selectedNodeId != NOT_SELECTED)
	{
		editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
	}
	if(editorScene->selectedNodeIds.size() > 0)
		editorScene->selectMan->unselectObjects();

	Logger::log(INFO,"IYAN3D.CPP","Current Frame " + to_string(editorScene->currentFrame));

if(editorScene->isPlaying){
	if (editorScene->currentFrame + 1 < editorScene->totalFrames)
	{
		editorScene->isPlaying = true;
		editorScene->currentFrame++;
		editorScene->setLightingOff();
		editorScene->actionMan->switchFrame(editorScene->currentFrame);
	}
	else if (editorScene->currentFrame + 1 >= editorScene->totalFrames)
	{
		editorScene->setLightingOn();
		editorScene->isPlaying = false;
		editorScene->actionMan->switchFrame(editorScene->currentFrame);
	}
	else if (editorScene->currentFrame == editorScene->totalFrames) {
		editorScene->setLightingOn();
		editorScene->isPlaying = false;
		editorScene->actionMan->switchFrame(editorScene->currentFrame);
	}
		//editorScene->updater->setDataForFrame(editorScene->currentFrame);
}

}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_currentFrame(JNIEnv *env, jclass type){
	return (editorScene) ? editorScene->currentFrame : 0;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPlaying(JNIEnv *env, jclass type){
	return editorScene->isPlaying;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setIsPlaying(JNIEnv *env, jclass type,jboolean isPlaying){
	editorScene->isPlaying = isPlaying;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPositionViaToolBarPosition(JNIEnv *env, jclass type,jint selectedIndex,jfloat rightWidth,jfloat topHeight){
	cameraPositionViaToolBarPosition(selectedIndex,rightWidth,topHeight);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(JNIEnv *env, jclass type,jint fov,jint resolution,jboolean storeAction){
	editorScene->actionMan->changeCameraProperty(fov, (int)resolution, storeAction);
}


JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadText(JNIEnv *env, jclass type,
jfloat red,jfloat green,jfloat blue,jint typeOfNode,jstring textureName,jstring assetName, jint fontSize,jint bevalValue,jint assetAddType,jstring filePath,jboolean isTempNode)
{
	editorScene->loader->removeTempNodeIfExists();
	const char *name = env->GetStringUTFChars(assetName, 0);
wstring assetNameW = ConversionHelper::getWStringForString(name);
const char *nativeString = env->GetStringUTFChars(textureName, 0);
string texture = nativeString;
const char *fontName = env->GetStringUTFChars(filePath, 0);
string font = fontName;

Vector4 textColor = Vector4(red,green,blue,1.0);
NODE_TYPE nodeType = (typeOfNode == ASSET_TEXT) ? NODE_TEXT : NODE_TEXT_SKIN;
SGNode* textNode = editorScene->loader->loadNode(nodeType, 0,texture,assetNameW, fontSize, bevalValue, IMPORT_ASSET_ACTION, textColor, font,isTempNode);
	env->ReleaseStringUTFChars(assetName, name);
	env->ReleaseStringUTFChars(textureName, nativeString);
	env->ReleaseStringUTFChars(filePath, fontName);

if (textNode == NULL) {
if(editorScene && editorScene->loader)
	editorScene->loader->removeTempNodeIfExists();
	return false;
}
if(textNode)
	textNode->isTempNode = isTempNode;
	return true;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importImageOrVideo(JNIEnv *env, jclass type,jint nodeType,jstring assetName,jint imgWidth,jint imgHeight
,jint assetAddType,jboolean isTempNode)
{
	editorScene->loader->removeTempNodeIfExists();
	const char *name = env->GetStringUTFChars(assetName, 0);
	wstring assetNameW = ConversionHelper::getWStringForString(name);
	NODE_TYPE nType = (nodeType == ASSET_VIDEO) ? NODE_VIDEO : NODE_IMAGE;
	SGNode* sgNode = editorScene->loader->loadNode(nType, 0,"",assetNameW, imgWidth, imgHeight, IMPORT_ASSET_ACTION,Vector4(imgWidth,imgHeight,0,0),"",isTempNode);
	if(sgNode) {
		sgNode->isTempNode = isTempNode;
		return true;
	}
	editorScene->loader->removeTempNodeIfExists();
	env->ReleaseStringUTFChars(assetName, name);

	return false;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAdditionalLight(JNIEnv *env, jclass type,jint lightCount)
{
	if(ShaderManager::lightPosition.size() < 5) {
		int assetId = ASSET_ADDITIONAL_LIGHT + lightCount;
		wstring assetNameW = ConversionHelper::getWStringForString("Light " + to_string(lightCount));
		editorScene->loader->loadNode(NODE_ADDITIONAL_LIGHT, assetId ,"",assetNameW, 20 , 50 , IMPORT_ASSET_ACTION , Vector4(1.0),"",false);
	}
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightCount(JNIEnv *env, jclass type){
	return ShaderManager::lightPosition.size();
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAsSGM(JNIEnv *env, jclass type,jstring fileName,jstring textureName,
		jint assetId,jboolean haveTexture,jfloat x,jfloat y, jfloat z){
const char *objStr = env->GetStringUTFChars(fileName, 0);
const char *textureStr = env->GetStringUTFChars(textureName, 0);
editorScene->objMan->loadAndSaveAsSGM(objStr, textureStr, assetId,!haveTexture,Vector3(x,y,z));
env->ReleaseStringUTFChars(fileName, objStr);
env->ReleaseStringUTFChars(textureName, textureStr);


}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeTexture(JNIEnv *env, jclass type,jint selectedNodeId,jstring textureName,jfloat x,jfloat y,jfloat z,jboolean isTemp){

const char *texture = env->GetStringUTFChars(textureName, 0);
editorScene->selectMan->selectObject(selectedNodeId,false);
if(!(editorScene->selectedNodeIds.size() > 0) && editorScene->hasNodeSelected()){
editorScene->changeTexture(texture, Vector3(x,y,z),isTemp,false);
editorScene->selectMan->unselectObject(selectedNodeId);
editorScene->setLightingOn();
}
env->ReleaseStringUTFChars(textureName, texture);

}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempTexture(JNIEnv *env, jclass type){
editorScene->removeTempTextureAndVertex(selectedNodeId);
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_canEditRigBones(JNIEnv *env, jclass type){
	return (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && !editorScene->canEditRigBones(editorScene->nodes[editorScene->selectedNodeId]));
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_beginRigging(JNIEnv *env, jclass type){
	editorScene->riggingNodeId = editorScene->selectedNodeId;
	editorScene->enterOrExitAutoRigMode(true);
	editorScene->rigMan->sgmForRig(editorScene->nodes[selectedNodeId]);
	editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_OBJVIEW));
	editorScene->rigMan->boneLimitsCallBack = &boneLimitsCallBack;
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cancelRig(JNIEnv *env, jclass type,jboolean completed){
	if(editorScene->rigMan->deallocAutoRig(completed)) {
		editorScene->enterOrExitAutoRigMode(false);
	}
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addJoint(JNIEnv *env, jclass type,jobject obj){

if(editorScene->rigMan->isSkeletonJointSelected){
	if(editorScene->rigMan->rigKeys.size() >= RIG_MAX_BONES){
		jclass dataClass = env->FindClass("com/smackall/animator/Rig");
		jmethodID javaRigMethodRef = env->GetMethodID(dataClass, "boneLimitCallBack", "()V");
		env->CallVoidMethod(obj, javaRigMethodRef);
	}
	editorScene->rigMan->addNewJoint();
}
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchRigSceneMode(JNIEnv *env, jclass type,jobject obj,jint scene){
if(editorScene && editorScene->rigMan){
		jboolean completed = false;
		jclass dataClass = env->FindClass("com/smackall/animator/Rig");
		jmethodID javaRigMethodRef = env->GetMethodID(dataClass, "rigCompletedCallBack", "(Z)V");
		if(editorScene->rigMan->sceneMode + scene == RIG_MODE_PREVIEW){
			string path = constants::DocumentsStoragePath+"/mesh/123456.sgr";
			editorScene->rigMan->exportSGR(path);
			completed = true;
			env->CallVoidMethod(obj, javaRigMethodRef,completed);
		}
		editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode + scene));
		env->CallVoidMethod(obj, javaRigMethodRef,completed);
	}
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setSkeletonType(JNIEnv *env, jclass type,jint skeletonType){
	if(skeletonType == HUMAN_RIGGING) {
		editorScene->rigMan->skeletonType = SKELETON_HUMAN;
	}else if(skeletonType == OWN_RIGGING) {
		editorScene->rigMan->skeletonType = SKELETON_OWN;
	}
}
JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObjects(JNIEnv *env, jclass type){
	editorScene->selectMan->unselectObjects();
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_hideNode(JNIEnv *env, jclass type,jint nodeId,jboolean hide){
editorScene->nodes[nodeId]->node->setVisible(!hide);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObject(JNIEnv *env, jclass type,jint nodeId){
editorScene->selectMan->unselectObject(nodeId);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyKeysOfNode(JNIEnv *env, jclass type,jint fromNodeId,jint toNodeId){
	editorScene->animMan->copyKeysOfNode(fromNodeId, toNodeId);
	editorScene->updater->setDataForFrame(editorScene->currentFrame);
}

JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getBoneCount(JNIEnv *env, jclass type,jint nodeId){
	return (int)editorScene->nodes[nodeId]->joints.size();
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_applyAnimation(JNIEnv *env, jclass type,jint fromNodeId,jint applyedNodeId,jstring path){
	const char *filePathStr = env->GetStringUTFChars(path, 0);
	string filepath = filePathStr;
		editorScene->animMan->applyAnimations(filePathStr, applyedNodeId);
	env->ReleaseStringUTFChars(path, filePathStr);

}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scale(JNIEnv *env, jclass type){
	bool status = false;
	if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS))){
		if((editorScene->selectedNodeIds.size() > 0) && (editorScene->allObjectsScalable())){
			status = true;
		}
		else if(!(editorScene->selectedNodeIds.size() > 0) && (editorScene->isRigMode ||(editorScene->hasNodeSelected() && (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT)))){
			status = true;
		}
	}
	else if(editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_EDIT_ENVELOPES) && editorScene->rigMan->isSkeletonJointSelected)
		status = true;
	if(status){
		editorScene->controlType = SCALE;
		editorScene->updater->updateControlsOrientaion();
		editorScene->renHelper->setControlsVisibility(false);
	}
	return status;
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueX(JNIEnv *env, jclass type){
	Vector3 currentScale;
	if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
		currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
	return currentScale.x;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueY(JNIEnv *env, jclass type){
	Vector3 currentScale;
	if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
		currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
	return currentScale.y;
}
JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueZ(JNIEnv *env, jclass type){
	Vector3 currentScale;
	if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
		currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
	return currentScale.z;
}

JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_envelopScale(JNIEnv *env, jclass type){
	return  editorScene->rigMan->getSelectedJointScale();
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setScaleValue(JNIEnv *env, jclass type,jfloat x,jfloat y, jfloat z){
	if((editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size()) && editorScene->selectedNodeIds.size() <= 0)
	return;
	editorScene->actionMan->changeObjectScale(Vector3(x, y, z), true);
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_move(JNIEnv *env, jclass type){
	if(editorScene->hasNodeSelected()){
		editorScene->controlType = MOVE;
		editorScene->updater->updateControlsOrientaion();
	}
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rotate(JNIEnv *env, jclass type){
	if(editorScene->hasNodeSelected() || editorScene->allObjectsScalable())
	{
		editorScene->controlType = ROTATE;
		editorScene->updater->updateControlsOrientaion();
	}
}

JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rigNodeScale(JNIEnv *env, jclass type,float x, float y, float z){
	editorScene->rigMan->changeNodeScale(Vector3(x,y,z));
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isRigMode(JNIEnv *env, jclass type){
	return editorScene->isRigMode;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraView(JNIEnv *env, jclass type,int indexValue){
	CAMERA_VIEW_MODE cameraView = (indexValue == FRONT_VIEW) ? VIEW_FRONT : (indexValue == TOP_VIEW) ? (CAMERA_VIEW_MODE)VIEW_TOP : (indexValue == LEFT_VIEW) ? VIEW_LEFT : (indexValue == BACK_VIEW) ? VIEW_BACK : (indexValue == RIGHT_VIEW) ? VIEW_RIGHT :VIEW_BOTTOM;
	editorScene->updater->changeCameraView(cameraView);
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isNodeSelected(JNIEnv *env, jclass type,int position){
	if(editorScene->selectedNodeIds.size() > 0){
		for (int i = 0; i < editorScene->selectedNodeIds.size(); ++i) {
			if(editorScene->selectedNodeIds[i] == position) {
				return true;
				break;
			}
		}
		return false;
	}
	else if(editorScene->selectedNodeId == position){ return true;}
	return false;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteAnimation(JNIEnv *env, jclass type){
	bool isKeySetForNode = editorScene->animMan->removeAnimationForSelectedNodeAtFrame(editorScene->currentFrame);
	return isKeySetForNode;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env, jclass type,jobject obj,jint assetId,jstring name, jint animType){
	bool status = editorScene->animMan->storeAnimations(assetId);

	jclass dataClass = env->FindClass("com/smackall/animator/Save");
	jmethodID animSaveMethod = env->GetMethodID(dataClass, "addToDatabase", "(ZLjava/lang/String;I)V");
	env->CallVoidMethod(obj, animSaveMethod,status,name,animType);
	return status;
}

JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env, jclass type){
	return editorScene->isJointSelected;
}
























JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_dealloc(JNIEnv *env, jclass type){
if(editorScene){
delete editorScene;
editorScene = NULL;
}

int oglWidth = 0;
int oglHeight = 0;
screenScale = 1.0;
loadFilepath="init";
checkControlSelection = false;
touchBegan = Vector2(0.0,0.0);
touchX = 0.0;
touchY = 0.0;
prevPos = Vector2(0.0,0.0);
currentPos = Vector2(0.0,0.0);
tapPosition = Vector2(0.0,0.0);;
checktapposition = false;
selectedNodeId = -1;
displayPrepared = false;
fileName = "";
sceneSaved = false;
}

}


