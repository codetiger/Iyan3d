#include <jni.h>
#include "Iyan3dEngineFiles/HeaderFiles/SGEditorScene.h"
#include "SGEngine2/SceneManager/SceneManager.h"
#include "SGEngine2/Loaders/SceneImporter.h"
#include "../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-x86_64/usr/include/string.h"
#include "PngFileManager.h"

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

int IMPORT_ASSET = 25;
int UNDO = 26;
int REDO = 27;
int TEXT_IMAGE_ADD = 18;

int HIDE = 0;
int SHOW = 1;

SGEditorScene *editorScene;
SceneManager *sceneManager;
GLuint _colorRenderBuffer;
GLuint _depthRenderBuffer;
GLuint _frameBuffer;
int oglWidth, oglHeight;
float screenScale = 1.0;
Vector2 touchBegan;
float touchX = 0.0;
float touchY = 0.0;
Vector2 prevPos = Vector2(0.0,0.0);
Vector2 currentPos = Vector2(0.0,0.0);
Vector2 tapPosition;
bool displayPrepared = false;
string fileName = "";
jobject videoManagerClass;
jmethodID getVideoAtFrameMethodId;
JavaVM * g_vm;

void addCameraLight()
{
    editorScene->loader->loadNode(NODE_CAMERA, 0,"","",L"CAMERA", 0, 0, IMPORT_ASSET_ACTION,Vector4(0),"",false);
    editorScene->loader->loadNode(NODE_LIGHT, 0,"","", L"LIGHT", 0, 0, IMPORT_ASSET_ACTION,Vector4(0),"",false);
    Logger::log(INFO,"IYAN3D","Camera Light Added");
}

void shaderCallBackForNode(int nodeID, string matName,int materialIndex, string callbackFuncName)
{
    if(!editorScene)
            return;

        if (callbackFuncName.compare("setUniforms") == 0)
            editorScene->shaderCallBackForNode(nodeID, matName, materialIndex);
        else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
            editorScene->setJointsUniforms(nodeID, matName);
        else if (callbackFuncName.compare("setCtrlUniforms") == 0)
            editorScene->setControlsUniforms(nodeID, matName);
        else if (callbackFuncName.compare("RotationCircle") == 0)
            editorScene->setRotationCircleUniforms(nodeID, matName);
        else if (callbackFuncName.compare("LightCircles") == 0 || callbackFuncName.compare("LightLine") == 0)
            editorScene->setLightLineUniforms(nodeID, matName);
        else if (callbackFuncName.compare("GreenLines") == 0)
            editorScene->setGridLinesUniforms(nodeID, 3, matName);
        else if (callbackFuncName.compare("BlueLines") == 0)
            editorScene->setGridLinesUniforms(nodeID, 2, matName);
        else if (callbackFuncName.compare("RedLines") == 0)
            editorScene->setGridLinesUniforms(nodeID, 1, matName);
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
    if(!editorScene)
            return false;

            if (callbackFuncName.compare("setUniforms") == 0)
                return editorScene->isNodeTransparent(nodeId);
            else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
                return editorScene->isJointTransparent(nodeId, callbackFuncName);
            else if (callbackFuncName.compare("setCtrlUniforms") == 0)
                return editorScene->isControlsTransparent(nodeId, callbackFuncName);
            else if (callbackFuncName.compare("setOBJUniforms") == 0)
                return false;
            else if (callbackFuncName.compare("RotationCircle") == 0 || callbackFuncName.compare("LightCircles") == 0)
                return false;
            else if (callbackFuncName.compare("GreenLines") == 0)
                return false;
            else if (callbackFuncName.compare("BlueLines") == 0)
                return false;
            else if (callbackFuncName.compare("RedLines") == 0)
                return false;
            else if (callbackFuncName.compare("LightLine") == 0)
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
}

void setCallBack()
{
    sceneManager->ShaderCallBackForNode = &shaderCallBackForNode;
    sceneManager->isTransparentCallBack = &isTransparentCallBack;
}

void saveScene()
{
    if(!editorScene) return;
    string savePath1 = constants::DocumentsStoragePath + "/scenes/" + fileName + ".png";
    char *imagePath = new char[savePath1.length() + 1];
    strcpy(imagePath, savePath1.c_str());
    editorScene->saveThumbnail(imagePath);
    string savePath = constants::DocumentsStoragePath + "/projects/" +fileName + ".sgb";
    editorScene->saveSceneData(&savePath);
    delete imagePath;
}

void callBackIsDisplayPrepared(JNIEnv *env,jclass type)
{
    jmethodID mid = env->GetStaticMethodID(type, "callBackIsDisplayPrepared", "()V");
    env->CallStaticVoidMethod(type, mid);
}

Quaternion getLightProps()
{
    Quaternion lightProps;
    if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
    {
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
    if(position==PREVIEW_LEFTBOTTOM)
    {
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0)
        {
            editorScene->camPreviewOrigin.x=0;
            editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

        }
        if(editorScene->camPreviewScale==2.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

        }
    }
    if(position==PREVIEW_LEFTTOP)
    {
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0)
        {
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
        }
    }

    if(position==PREVIEW_RIGHTBOTTOM)
    {
        if(editorScene->camPreviewScale==1.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;


        }
        if(editorScene->camPreviewScale==2.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=oglHeight*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;

        }
    }
    if(position==PREVIEW_RIGHTTOP)
    {

        if(editorScene->camPreviewScale==1.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0)
        {
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=oglWidth*editorScene->screenScale-toolBarWidth*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=topHeight*editorScene->screenScale;
        }
    }
}

void cameraPositionViaToolBarPosition(int selectedIndex, float rightWidth,float topViewHeight)
{

    if(editorScene)
    {
        editorScene->topLeft = Vector2((selectedIndex==TOOLBAR_LEFT) ? rightWidth : 0.0 , topViewHeight) * editorScene->screenScale;
        editorScene->topRight = Vector2((selectedIndex==TOOLBAR_LEFT) ? oglWidth : oglWidth-rightWidth, topViewHeight) * editorScene->screenScale;
        editorScene->bottomLeft = Vector2((selectedIndex==TOOLBAR_LEFT) ? rightWidth : 0.0, oglHeight) * editorScene->screenScale;
        editorScene->bottomRight = Vector2((selectedIndex==TOOLBAR_LEFT) ?  oglWidth : oglWidth-rightWidth, oglHeight) * editorScene->screenScale;
        editorScene->renHelper->movePreviewToCorner();
    }
}

void boneLimitsCallBack() {}

bool downloadMissingAssetsCallBack(jobject object,string fileName, NODE_TYPE nodeType,bool hasTexture ,JNIEnv *env, jclass type)
{
    jstring fileNameStr = env->NewStringUTF(fileName.c_str());
    int nType = (int)nodeType;
    jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
    jmethodID javaRigMethodRef = env->GetMethodID(dataClass, "checkAssets","(Ljava/lang/String;I)Z");
    return env->CallBooleanMethod(object, javaRigMethodRef,fileNameStr,nType);
}

void updateXYZValuesCallBack(JNIEnv *env, jclass type,jobject object,bool hide,float x, float y, float z)
{
    jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
    jmethodID updateXYZMethod = env->GetMethodID(dataClass, "updateXYZValue","(ZFFF)V");
    return env->CallVoidMethod(object, updateXYZMethod,hide,x,y,z);
}

void updateRenderer(JNIEnv *env, jclass type)
{
    displayPrepared = sceneManager->PrepareDisplay(oglWidth, oglHeight, true, true, false,
                      Vector4(0.1, 0.1, 0.1, 1.0));
    editorScene->renderAll();
}

bool objectsScalable()
{
    bool status = true;
    if(editorScene->selectedNodeIds.size() > 0)
    {
        for(int i = 0; i < editorScene->selectedNodeIds.size(); i++)
        {
            NODE_TYPE nType = editorScene->nodes[editorScene->selectedNodeIds[i]]->getType();
            if(nType == NODE_CAMERA || nType == NODE_LIGHT || nType == NODE_ADDITIONAL_LIGHT || nType == NODE_PARTICLES)
            {
                status = false;
                break;
            }
        }
    }
    else if(editorScene->selectedNodeId != -1)
    {
        NODE_TYPE nType = editorScene->nodes[editorScene->selectedNodeId]->getType();
        if(nType == NODE_CAMERA || nType == NODE_LIGHT || nType == NODE_ADDITIONAL_LIGHT || nType == NODE_PARTICLES)
        {
            status = false;
        }
    }
    return status;
}

int getMaxUniformsForOpenGL()
{
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = OPENGLES2;

    int lowerLimit = 0;
    int upperLimit = 512;
    while ((upperLimit - lowerLimit) != 1)
    {
        int mid = (lowerLimit + upperLimit) / 2;
        if(ShaderManager::LoadShader(sceneManager, OPENGLES2, "SHADER_MESH", "mesh.vsh", "common.fsh", ShaderManager::getShaderStringsToReplace(mid), true))
            lowerLimit = mid;
        else
            upperLimit = mid;

    }
    printf("\n Max Uniforms %d ", lowerLimit);

    sceneManager->clearMaterials();
    return lowerLimit;
}

int getMaximumJointsSize()
{
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = OPENGLES2;

    int lowerLimit = 0;
    int upperLimit = 512;
    while ((upperLimit - lowerLimit) != 1)
    {
        int mid = (lowerLimit + upperLimit) / 2;
        if(ShaderManager::LoadShader(sceneManager, OPENGLES2, "SHADER_SKIN", "skin.vsh", "common.fsh", ShaderManager::getStringsForRiggedObjects(mid), true))
            lowerLimit = mid;
        else
            upperLimit = mid;

    }
    Logger::log(INFO,"Iyan3d", "Max Joints : " +  to_string(lowerLimit));

    sceneManager->clearMaterials();
    return lowerLimit;
}

void syncSceneWithPhysicsWorld()
{
    if(editorScene)
        editorScene->syncSceneWithPhysicsWorld();
}

void updatePhysics(int frame)
{
    if(editorScene)
        editorScene->updatePhysics(frame);
}

unsigned char* getVideoFrameCallBack(string filename,int frame,int width,int height)
{
    /*
    Logger::log(INFO,"IYAN3D","Working");
    JNIEnv *env;
    g_vm->AttachCurrentThread(&env, 0);
    jclass dataClass = env->FindClass("com/smackall/animator/Helper/VideoManager");
    int len = 0;

    jbyteArray j_value =  (jbyteArray)env->CallObjectMethod(videoManagerClass,getVideoAtFrameMethodId,env->NewStringUTF(filename.c_str()), frame, width, height);
    if(j_value != NULL)
    {
    	Logger::log(INFO,"IYAN3D","BytenotNull");
    	len = env->GetArrayLength(j_value);
    	unsigned char* value = new unsigned char[len];
    	env->GetByteArrayRegion (j_value, 0, len, reinterpret_cast<jbyte*>(value));
    	return  value;
    }
    else{
    		Logger::log(INFO,"IYAN3D","Byte is null");
    }
    */
    return NULL;
}

void changeLightPropertyAtIndex(JNIEnv *env, jclass type, jobject callback,PROP_INDEX pIndex, Vector4 value,bool status)
{
    if(!editorScene || editorScene->selectedNodeId == NOT_SELECTED)
        return;

    SGNode* selectedNode = editorScene->nodes[editorScene->selectedNodeId];
    Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, selectedNode->scaleKeys);
    Vector4 lightProps = (pIndex == VERTEX_COLOR) ? value : Vector4(scale, ShaderManager::shadowDensity);
    float dist = (pIndex == SPECIFIC_FLOAT) ? value.x : selectedNode->getProperty(SPECIFIC_FLOAT).value.x;
    int lightType = (selectedNode->getProperty(pIndex).groupName == "LIGHT TYPE") ? pIndex - 23 : selectedNode->getProperty(LIGHT_TYPE).value.x;
    lightProps.w = (pIndex == SHADOW_DARKNESS) ? value.x : ShaderManager::shadowDensity;
    /* Number 22 because Point light's value is 0 in previous version , now it is 22 */
    if(!status)
        editorScene->actionMan->changeLightProperty(lightProps.x, lightProps.y, lightProps.z, lightProps.w, dist, lightType, true);
    else if(status)
        editorScene->actionMan->storeLightPropertyChangeAction(lightProps.x, lightProps.y, lightProps.z, lightProps.w, dist, lightType);

    if(status) {
        editorScene->updateDirectionLine();
    }
}

void setProperties(JNIEnv *env, jclass type, jobject callback, std::map< PROP_INDEX, Property > options)
{
    std::map< PROP_INDEX, Property >::iterator optIt;
    jclass propertyClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
    jmethodID setPropertToMap = env->GetMethodID(propertyClass, "addSGNodeProperty", "(IIILjava/lang/String;Ljava/lang/String;ILjava/lang/String;FFFFZ)V");

    for(optIt = options.begin(); optIt != options.end(); optIt++)
    {
        Property prop = optIt->second;
        jstring title = env->NewStringUTF(prop.title.c_str());
        jstring fileName = env->NewStringUTF(prop.fileName.c_str());
        jstring groupName = env->NewStringUTF(prop.groupName.c_str());
        Vector4 value = prop.value;
        Logger::log(INFO,"IYAN3D","Index : " + to_string(prop.index));
        env->CallVoidMethod(callback, setPropertToMap,prop.index,prop.parentIndex,prop.type,title,fileName,prop.iconId,groupName,value.x,value.y,value.z,value.w,
                            prop.isEnabled);
        if(prop.subProps.size() > 0)
        {
            setProperties(env, type, callback, prop.subProps);
        }
    }
}

void setDirection()
{
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->enableDirectionIndicator();
        editorScene->updater->updateControlsOrientaion();
    }
}

void createDuplicateAssets(JNIEnv *env, jclass type, jobject callback)
{
        int selectedAssetId  = NOT_EXISTS;
        int selectedNode = NOT_EXISTS;
        NODE_TYPE selectedNodeType = NODE_UNDEFINED;

        if(editorScene && editorScene->selectedNodeIds.size() > 0)
        {
            editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_ADDED, 0);
            vector< int > addedNodeIds;
            vector< int > fromNodeIds(editorScene->selectedNodeIds);

            for(int i = 0; i < fromNodeIds.size(); i++)
            {
                selectedAssetId = editorScene->nodes[fromNodeIds[i]]->assetId;
                selectedNodeType = editorScene->nodes[fromNodeIds[i]]->getType();
                selectedNode = fromNodeIds[i];

                jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
                jmethodID saveCompleteCallBack = env->GetMethodID(dataClass, "cloneSelectedAssetWithId", "(III)V");
                env->CallVoidMethod(callback, saveCompleteCallBack,selectedAssetId,(int)selectedNodeType,selectedNode);
                addedNodeIds.push_back(editorScene->nodes.size() - 1);
            }

            editorScene->selectMan->unselectObjects();
            for(int i = 0; i < addedNodeIds.size(); i++)
            {
                editorScene->selectMan->selectObject(addedNodeIds[i], true);
            }
            editorScene->updater->updateControlsOrientaion();
            editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_ADDED, 0);

        }
        else if(editorScene && editorScene->selectedNodeId != NOT_SELECTED)
        {
            selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
            selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
            selectedNode = editorScene->selectedNodeId;

            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID saveCompleteCallBack = env->GetMethodID(dataClass, "cloneSelectedAssetWithId", "(III)V");
            env->CallVoidMethod(callback, saveCompleteCallBack,selectedAssetId,(int)selectedNodeType,selectedNode);

            editorScene->selectMan->selectObject(editorScene->nodes.size()-1 , false);
            editorScene->updater->setDataForFrame(editorScene->currentFrame);
        }

        syncSceneWithPhysicsWorld();
}

void changedPropertyAtIndex(JNIEnv *env, jclass type, jobject callback,PROP_INDEX index, Vector4 value, bool status)
{

    SGNode* selectedNode= NULL;

    jclass callBackClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");

    if(editorScene->selectedNodeId >= 0 || editorScene->selectedNodeId < editorScene->nodes.size())
        selectedNode = editorScene->nodes[editorScene->selectedNodeId];

    switch (index) {
        case VISIBILITY:
        case LIGHTING:
        case REFRACTION:
        case REFLECTION: {
            float prevRefraction = selectedNode->getProperty(REFRACTION).value.x;
            float prevReflection = selectedNode->getProperty(REFLECTION).value.x;
            bool prevLighting = selectedNode->getProperty(LIGHTING).value.x;
            bool prevVisibility = selectedNode->getProperty(VISIBILITY).value.x;

            editorScene->actionMan->changeMeshProperty((index == REFRACTION) ? value.x : prevRefraction, (index == REFLECTION) ? value.x : prevReflection, (index == LIGHTING) ? value.x : prevLighting, (index == VISIBILITY) ? value.x : prevVisibility, status);
            break;
        }
        case TEXTURE_SMOOTH: {
            if(editorScene && editorScene->selectedNodeId != NOT_SELECTED && editorScene->nodes[editorScene->selectedNodeId]) {
                editorScene->nodes[editorScene->selectedNodeId]->smoothTexture = status;
                editorScene->nodes[editorScene->selectedNodeId]->getProperty(TEXTURE_SMOOTH).value.x = value.x;
                if(sceneManager->device == OPENGLES2)
                        editorScene->changeTexture(editorScene->nodes[editorScene->selectedNodeId]->getProperty(TEXTURE).fileName, Vector3(1.0), false, false);
            }
            break;
        }
        case TEXTURE: {
            jmethodID deleteObjectOrAnimation = env->GetMethodID(callBackClass, "changeTextureForAsset", "()V");
            env->CallVoidMethod(callback, deleteObjectOrAnimation);
            break;
        }
        case TEXTURE_SCALE: {
            editorScene->selectMan->unHightlightSelectedNode();
            editorScene->actionMan->changeUVScale(editorScene->selectedNodeId, value.x);
            break;
        }
        case DELETE: {
            if(editorScene->selectedNodeIds.size() > 0){
                editorScene->loader->removeSelectedObjects();
            }
            else{
                jmethodID deleteObjectOrAnimation = env->GetMethodID(callBackClass, "deleteObjectOrAnimation", "()V");
                env->CallVoidMethod(callback, deleteObjectOrAnimation);
            }
            break;
        }
        case CLONE: {
            createDuplicateAssets(env,type,callback);
            break;
        }
        case VERTEX_COLOR: {
            if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
                changeLightPropertyAtIndex(env,type,callback,index,value,status);
            }
            break;
        }

        case SPECIFIC_FLOAT: {
            if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
                changeLightPropertyAtIndex(env,type,callback,index,value,status);
            }
            break;
        }
        case SHADOW_DARKNESS:
        case LIGHT_DIRECTIONAL:
        case LIGHT_POINT: {
            changeLightPropertyAtIndex(env,type,callback,index,value,status);
            break;
        }
        case FORCE_DIRECTION: {
            setDirection();
            break;
        }
        case FOV:
        case CAM_RESOLUTION: {
            float cameraFOV = (index == FOV) ? value.x : editorScene->nodes[editorScene->selectedNodeId]->getProperty(FOV).value.x;
            int resolnType = (index == CAM_RESOLUTION) ? value.x : editorScene->nodes[editorScene->selectedNodeId]->getProperty(CAM_RESOLUTION).value.x;
            editorScene->actionMan->changeCameraProperty(cameraFOV , resolnType, status);
            break;
        }
        case AMBIENT_LIGHT: {
            editorScene->shaderMGR->getProperty(AMBIENT_LIGHT).value.x = value.x;
            break;
        }
        default:
            break;
    }
}

extern "C" {
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMaxJoint(JNIEnv *env, jclass type);
    JNIEXPORT jintArray JNICALL Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type, jint width,jint height,jboolean addVAOSupport,jint uniformValue,jint maxJoints);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadScene(JNIEnv *env,jclass type,jobject object,jstring filePath);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env,jclass type,jstring assetPath_,jstring setMeshpath_,jstring setImagepath_);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapEnd(JNIEnv *env, jclass type,jfloat x, jfloat y);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env, jclass type, jfloat x,jfloat y,jboolean isMultiSelect);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type,jobject object,jfloat x, jfloat y);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_touchBegan(JNIEnv *env, jclass type,jfloat x, jfloat y);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type,jfloat velocityX,jfloat velocityY);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_panBegin(JNIEnv *env, jclass type,jfloat cordX1,jfloat cordY1,jfloat cordX2,jfloat cordY2);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env,jclass type,jobject object,jfloat cordX1,jfloat cordY1,jfloat cordX2,jfloat cordY2);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type, jfloat x, jfloat y,jboolean isMultiSelect);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env, jclass type, jstring filePath_);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_save(JNIEnv *env, jclass type,jobject object ,jboolean isCloudRender,jstring filePath,jboolean isAutoSave);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_dealloc(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempNode(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeCount(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeType(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodeName(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectNode(JNIEnv *env, jclass type,jint nodeId,jboolean isMultiSelect);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeNode(JNIEnv *env, jclass type,jint nodeId,jboolean isUndoOrRedo);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyPropsOfNode(JNIEnv *env, jclass type,jint from,jint to,jboolean excludeKeys);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetIdWithNodeId(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getTexture(JNIEnv *env, jclass type,jint selectedNode);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorX(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorY(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZ(JNIEnv *env, jclass type);

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorXWithId(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorYWithId(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZWithId(JNIEnv *env, jclass type,jint nodeId);

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePath(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloat(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloatWithId(JNIEnv *env, jclass type,jint nodeId);

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSize(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSizeWithId(JNIEnv *env, jclass type,jint nodeId);
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
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getRigMirrorState(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env, jclass type,jfloat x, jfloat y, jfloat z, jfloat w,jfloat distance,jint lightType,jboolean storeAction);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_previewPosition(JNIEnv *env, jclass type,jint position,jint previewSize,jfloat topHeight,jfloat toolbarWidth);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchFrame(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setCurrentFrame(JNIEnv *env, jclass type,jint currentFrame,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_stopPlaying(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_play(JNIEnv *env, jclass type,jobject object);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_totalFrame(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addFrame(JNIEnv *env, jclass type,jint frame);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_currentFrame(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPlaying(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setIsPlaying(JNIEnv *env, jclass type,jboolean isPlaying,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPositionViaToolBarPosition(JNIEnv *env, jclass type,jint selectedIndex, jfloat rightWidth,jfloat topHeight);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAsset(JNIEnv *env,jclass type,jint assetType,jint assetId,jstring assetName,jstring textureName,
            jint width,jint height,jboolean isTempNode,jfloat x,jfloat y, jfloat z,int assetActionType);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importModel(JNIEnv *env,jclass type,jint assetId,jstring assetName,jstring meshPath,jstring texturePath,
       jfloat x,jfloat y, jfloat z,int assetAddType,jboolean isTempNode);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(JNIEnv *env, jclass type,jint fov,jint resolution,jboolean storeAction);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadText(JNIEnv *env, jclass type,jfloat red,jfloat green,jfloat blue,jint typeOfNode,jstring textureName,jstring assetName,jint fontSize,jint bevalValue,jint assetAddType,jstring filePath,jboolean isTempNode);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importImageOrVideo(JNIEnv *env, jclass type,jint nodeType,jstring assetName,jint imgWidth,jint imgHeight
            ,jint assetAddType,jboolean isTempNode);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAdditionalLight(JNIEnv *env, jclass type,jint lightCount,jint action);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightCount(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAsSGM(JNIEnv *env, jclass type,jstring fileName,jstring textureName,jint assetId,jboolean haveTexture,jfloat x,jfloat y, jfloat z);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeTexture(JNIEnv *env, jclass type,jint selectedNodeId,jstring textureName,jfloat x,jfloat y,jfloat z,jboolean isTemp);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempTexture(JNIEnv *env, jclass type,jint selectedNodeId);
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
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scale(JNIEnv *env, jclass type,jobject object);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueX(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueY(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueZ(JNIEnv *env, jclass type);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_envelopScale(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeEnvelopScale(JNIEnv *env, jclass type, jfloat x);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setScaleValue(JNIEnv *env, jclass type,jfloat x,jfloat y, jfloat z,jboolean store);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_move(JNIEnv *env, jclass type,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rotate(JNIEnv *env, jclass type,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rigNodeScale(JNIEnv *env, jclass type,float x, float y, float z);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isRigMode(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isVAOSupported(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraView(JNIEnv *env, jclass type,int indexValue);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isNodeSelected(JNIEnv *env, jclass type,int position);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteAnimation(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env, jclass type,jobject obj,jint assetId,jstring name, jint animType);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env, jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isHaveKey(JNIEnv *env,jclass type, jint currentFrame);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_hasNodeSelected(JNIEnv *env,jclass type);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_editorScene(JNIEnv *env,jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_undo(JNIEnv *env,jclass type,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_redo(JNIEnv *env,jclass type,jobject object);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_decreaseCurrentAction(JNIEnv *env,jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_objectIndex(JNIEnv *env,jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_increaseCurrentAction(JNIEnv *env,jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_renderFrame(JNIEnv *env,jclass type,jobject object,jint frame,jint shader,jboolean isImage,jboolean waterMark
            ,jfloat x, jfloat y, jfloat z);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePathWithId(JNIEnv *env, jclass type,jint id);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_perVertexColor(JNIEnv *env,jclass type,jint id);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_initVideoManagerClass(JNIEnv *env,jclass type,jobject object);

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Mesh(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Texture(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_MeshThumbnail(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Font(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_AnimationThumbnail(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Particle(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Animation(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Credits(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_UseOrRecharge(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_CheckProgress(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_TaskDownload(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_verifyRestorePurchase(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_RenderTask(JNIEnv *env, jclass type);
    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_PublishAnimation(JNIEnv *env, jclass type);

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setNodeLighting(JNIEnv *env, jclass type,jint nodeId,jboolean state);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setNodeVisiblity(JNIEnv *env, jclass type,jint nodeId,jboolean state);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setfreezeRendering(JNIEnv *env, jclass type,jboolean state);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTransparency(JNIEnv *env, jclass type,jfloat transparency,jint nodeId);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cloneSelectedAsset(JNIEnv *env, jclass type,jint selectedAssetId,jint selectedNodeType,jint selectedNodeIndex,jint assetAddType);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectedNodeIdsSize(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeIdAtIndex(JNIEnv *env, jclass type,jint index);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_createDuplicateAssets(JNIEnv *env, jclass type,jobject object);
    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPhysicsEnabled(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_physicsType(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_velocity(JNIEnv *env, jclass type,jint nodeId);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_enablePhysics(JNIEnv *env, jclass type,jboolean status);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setPhysicsType(JNIEnv *env, jclass type,jint physicsType);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_velocityChanged(JNIEnv *env, jclass type,jint value);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setDirection(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeSelectedObjects(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightType(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_ADD_INSTANCE_BACK(JNIEnv *env, jclass type,jint actionType);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_syncPhysicsWithWorld(JNIEnv *env, jclass type,jint from, jint to,jboolean doUpdatePhysics);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_updatePhysics(JNIEnv *env, jclass type,jint frame);
    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedJointScale(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_smoothTexState(JNIEnv *env, jclass type);
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_smoothTexStateWithId(JNIEnv *env, jclass type, jint nodeId);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTextureSmoothStatus(JNIEnv *env, jclass type, jboolean status);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodePropsMapWithNodeId(JNIEnv *env, jclass type,jobject callback,jint nodeId);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSceneProps(JNIEnv *env, jclass type, jobject callback);
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changedPropertyAtIndex(JNIEnv *env, jclass type, jobject callback,jint index, jfloat x,
         jfloat y,jfloat z,jfloat w, jboolean status);

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Mesh(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://iyan3dapp.com/appapi/mesh/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Texture(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://iyan3dapp.com/appapi/meshtexture/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_MeshThumbnail(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/128images/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Font(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://iyan3dapp.com/appapi/font/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_AnimationThumbnail(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://iyan3dapp.com/appapi/animationImage/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Particle(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://iyan3dapp.com/appapi/particles/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Animation(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("http://iyan3dapp.com/appapi/animationFile/");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_Credits(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/login.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_UseOrRecharge(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/credits-and.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_CheckProgress(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/checkprogress.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_TaskDownload(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/download.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_verifyRestorePurchase(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/restore-and.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_RenderTask(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/rendertask.php");
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_PublishAnimation(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF("https://www.iyan3dapp.com/appapi/publish.php");
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getMaxJoint(JNIEnv *env, jclass type)
    {
        return getMaximumJointsSize();
    }

    JNIEXPORT jintArray JNICALL Java_com_smackall_animator_opengl_GL2JNILib_init(JNIEnv *env, jclass type, jint width,jint height,jboolean addVAOSupport,jint uniformValue,jint maxJoints)
    {

        if (editorScene)
            return 0;

        oglWidth = width;
        oglHeight = height;
        sceneManager = new SceneManager(width, height, 1, OPENGLES2, constants::BundlePath);
        sceneManager->setVAOSupport(addVAOSupport);
        int maxUniform = uniformValue;
        int maxJoint = maxJoints;
        if(maxUniform == 0)
            maxUniform = getMaxUniformsForOpenGL();
        if(maxJoint ==0)
            maxJoint = getMaximumJointsSize();
        editorScene = new SGEditorScene(OPENGLES2, sceneManager, width, height,maxUniform,maxJoint);
        editorScene->addVAOSupport = addVAOSupport;
        editorScene->screenScale = screenScale;
        sceneManager->setFrameBufferObjects(0, _colorRenderBuffer, _depthRenderBuffer);

        setCallBack();
        editorScene->downloadMissingAssetsCallBack = &downloadMissingAssetsCallBack;
        editorScene->getVideoFrameCallBack = &getVideoFrameCallBack;

        jintArray intArray = env->NewIntArray(2);
        jint values[2];
        values[0] = maxUniform;
        values[1] = maxJoint;

        env->SetIntArrayRegion(intArray,0,2,values);

        return intArray;
    }


    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_step(JNIEnv *env, jclass type)
    {
        if (editorScene)
        {
            updateRenderer(env,type);
        }
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_loadScene(JNIEnv *env,jclass type,jobject object,jstring filePath)
    {

        const char *loadFilepath = env->GetStringUTFChars(filePath, 0);
        string file = loadFilepath;
        if (file == "init")
        {
            addCameraLight();
            loadFilepath = "";
            editorScene->updater->setCameraProperty(editorScene->cameraFOV, editorScene->cameraResolutionType);
            editorScene->updater->setDataForFrame(editorScene->currentFrame);
            callBackIsDisplayPrepared(env, type);
        }
        else if (file.size() > 0)
        {
            editorScene->loader->loadSceneData(&file,env,type,object);
            loadFilepath = "";
            callBackIsDisplayPrepared(env, type);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_setAssetspath(JNIEnv *env, jclass type, jstring assetPath_, jstring setMeshpath_, jstring setImagepath_)
    {
        const char *assetPath = env->GetStringUTFChars(assetPath_, 0);
        const char *setMeshpath = env->GetStringUTFChars(setMeshpath_, 0);
        const char *setImagepath = env->GetStringUTFChars(setImagepath_, 0);
        string assetStoragePath = assetPath;
        string meshStoragePath = setMeshpath;
        string importedImagesPath = setImagepath;
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
        //editorScene->setLightingOn();
    }
    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_controlSelection(JNIEnv *env, jclass type, jfloat x,jfloat y,jboolean isMultiSelect)
    {
        touchBegan = Vector2(x, y);
        editorScene->selectMan->checkCtrlSelection(Vector2(x, y),isMultiSelect,displayPrepared);
    }

    JNIEXPORT void JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_tapMove(JNIEnv *env, jclass type,jobject object ,jfloat x, jfloat y)
    {

        if(editorScene)
        {
            if(editorScene->isRigMode)
                editorScene->moveMan->touchMoveRig(Vector2(x, y), touchBegan, oglWidth * screenScale, oglHeight * screenScale);
            else
            {
                editorScene->moveMan->touchMove(Vector2(x, y), touchBegan, oglWidth, oglHeight);
                editorScene->setLightingOff();
            }

            if (!editorScene->isPlaying)
            {
                Vector3 trans = editorScene->getTransformValue();
                updateXYZValuesCallBack(env,type,object,false,trans.x,trans.y,trans.z);
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_touchBegan(JNIEnv *env, jclass type,jfloat x, jfloat y)
    {
        if(editorScene->isRigMode)
            editorScene->moveMan->touchBeganRig(Vector2(x,y)*screenScale);
        else
            editorScene->moveMan->touchBegan(Vector2(x,y) * screenScale);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_swipe(JNIEnv *env, jclass type, jfloat velocityX, jfloat velocityY)
    {

        if(editorScene)
        {
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
            editorScene->updater->updateControlsOrientaion();
            editorScene->updater->updateLightCamera();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_panProgress(JNIEnv *env, jclass type,jobject object, jfloat cordX1, jfloat cordY1, jfloat cordX2,jfloat cordY2)
    {
        editorScene->setLightingOff();
        editorScene->moveMan->panProgress(Vector2(cordX1, cordY1),Vector2(cordX2, cordY2));
        editorScene->updater->updateControlsOrientaion();
        editorScene->updater->updateLightCamera();
        if (!editorScene->isPlaying)
        {
            Vector3 trans = editorScene->getTransformValue();
            updateXYZValuesCallBack(env,type,object,false,trans.x,trans.y,trans.z);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_tap(JNIEnv *env, jclass type, jfloat x, jfloat y,jboolean isMultiSelect)
    {
        tapPosition = Vector2(x, y) * screenScale;

        if(editorScene->renHelper->isMovingCameraPreview(tapPosition))
        {
            editorScene->camPreviewScale = (editorScene->camPreviewScale == 1.0) ? 2.0 : 1.0;
            return;
        }

        editorScene->isRTTCompleted = true;
        if(editorScene->isRigMode)
            editorScene->selectMan->checkSelectionForAutoRig(Vector2(x, y));
        else
            editorScene->selectMan->checkSelection(tapPosition,isMultiSelect, displayPrepared);
        if((editorScene->controlType == SCALE || editorScene->controlType == ROTATE)&& !objectsScalable())
        {
            editorScene->controlType = MOVE;
            editorScene->updater->updateControlsOrientaion();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_loadFile(JNIEnv *env, jclass type, jstring filePath_)
    {
        const char *filePath = env->GetStringUTFChars(filePath_, 0);
//	loadFilepath = filePath;
//    Logger::log(INFO,"IYAN3D","file path loa: "+loadFilepath);
        env->ReleaseStringUTFChars(filePath_, filePath);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_save(JNIEnv *env, jclass type,jobject object ,jboolean isCloudRender,jstring filePath,jboolean isAutoSave)
    {
        const char *nativeString = env->GetStringUTFChars(filePath, 0);
        fileName = nativeString;
        saveScene();
        if(!isAutoSave)
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID saveCompleteCallBack = env->GetMethodID(dataClass, "saveCompletedCallBack", "(Z)V");
            env->CallVoidMethod(object, saveCompleteCallBack,isCloudRender);
        }
        env->ReleaseStringUTFChars(filePath, nativeString);
    }

    JNIEXPORT jint JNICALL
    Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeId(JNIEnv *env, jclass type)
    {

        return (jint) editorScene->selectedNodeId;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importModel(JNIEnv *env,jclass type,jint assetId,jstring assetName,jstring meshPath,jstring texturePath,
    jfloat x,jfloat y, jfloat z,int assetAddType,jboolean isTempNode){

        const char *assetName_ = env->GetStringUTFChars(assetName, 0);
        string assetNameStr = assetName_;
        const char *nativeStringForTexture = env->GetStringUTFChars(texturePath, 0);
        string texture = nativeStringForTexture;
        const char *nativeStringForMesh = env->GetStringUTFChars(meshPath, 0);
        string mesh = nativeStringForMesh;
        Vector3 color = Vector3(x,y,z);

        editorScene->loader->removeTempNodeIfExists();

        SceneImporter *loader = new SceneImporter();
        loader->importNodesFromFile(editorScene, assetNameStr, mesh, texture, false, color, isTempNode);

        if(!isTempNode){
            if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
               editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
        }

        env->ReleaseStringUTFChars(texturePath, nativeStringForTexture);
        env->ReleaseStringUTFChars(meshPath, nativeStringForMesh);
        env->ReleaseStringUTFChars(assetName, assetName_);
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
        if(assetType == 1)
            nodeType = NODE_RIG;
        else
            nodeType = NODE_SGM;

        if(assetId >= 50000 && assetId < 60000)
            nodeType = NODE_PARTICLES;

        ActionType actionType = IMPORT_ASSET_ACTION;
        if(assetActionType == IMPORT_ASSET)
            actionType = IMPORT_ASSET_ACTION;
        else if(assetActionType == UNDO)
            actionType = UNDO_ACTION;
        else if (assetActionType == REDO)
            actionType = REDO_ACTION;

        editorScene->loader->removeTempNodeIfExists();

        //SGNode *sgnode = editorScene->loader->loadNode(nodeType, assetId, texture,assetNameW,0, 0,actionType,color,"",isTempNode);
        //if(sgnode)
        //    sgnode->isTempNode = isTempNode;
        //if(isTempNode)
        //    editorScene->isPreviewMode = false;
        //if(!isTempNode)
        //{
        //    if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
        //        editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
        //}

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
        if(editorScene->nodes.size()>nodeId)
        {
            if(editorScene->isNodeInSelection(editorScene->nodes[nodeId]))
                editorScene->selectMan->multipleSelections(nodeId);
            if(editorScene->isNodeSelected && editorScene->selectedNodeId != nodeId)
                editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
            editorScene->selectedNodeId = nodeId;
            if(!isUndoOrRedo)
            {
                if(editorScene->nodes[nodeId]->getType() == NODE_TEXT_SKIN || editorScene->nodes[nodeId]->getType() == NODE_TEXT || editorScene->nodes[nodeId]->getType() == NODE_IMAGE || editorScene->nodes[nodeId]->getType() == NODE_VIDEO)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
                else if (editorScene->nodes[nodeId]->getType() == NODE_OBJ || editorScene->nodes[nodeId]->getType() == NODE_SGM || editorScene->nodes[nodeId]->getType() == NODE_RIG || editorScene->nodes[nodeId]->getType() == NODE_ADDITIONAL_LIGHT|| editorScene->nodes[nodeId]->getType() == NODE_PARTICLES)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
            }
            editorScene->loader->removeObject(nodeId);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyPropsOfNode(JNIEnv *env, jclass type,jint from,jint to,jboolean excludeKeys)
    {
        editorScene->animMan->copyPropsOfNode(from, (to == 0) ?(int)editorScene->nodes.size()-1 : to,excludeKeys);
        editorScene->updater->setDataForFrame(editorScene->currentFrame);
    }
    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetId(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->assetId;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getAssetIdWithNodeId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->assetId;
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getTexture(JNIEnv *env, jclass type,jint selectedNode)
    {
        return jstring("null");//TODO env->NewStringUTF(editorScene->nodes[selectedNode]->textureName.c_str());
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorX(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorY(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.y;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZ(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.z;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorXWithId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->getProperty(VERTEX_COLOR).value.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorYWithId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->getProperty(VERTEX_COLOR).value.y;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getVertexColorZWithId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->getProperty(VERTEX_COLOR).value.z;
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePath(JNIEnv *env, jclass type)
    {
        return env->NewStringUTF(editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str());
    }

    JNIEXPORT jstring JNICALL Java_com_smackall_animator_opengl_GL2JNILib_optionalFilePathWithId(JNIEnv *env, jclass type,jint id)
    {
        return env->NewStringUTF(editorScene->nodes[id]->optionalFilePath.c_str());
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloat(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(SPECIFIC_FLOAT).value.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_nodeSpecificFloatWithId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->getProperty(SPECIFIC_FLOAT).value.x;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSize(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(FONT_SIZE).value.x;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getFontSizeWithId(JNIEnv *env, jclass type,jint nodeId)
    {
        return editorScene->nodes[nodeId]->getProperty(FONT_SIZE).value.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_refractionValue(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(REFRACTION).value.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_reflectionValue(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(REFLECTION).value.x;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isLightning(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(LIGHTING).value.x;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isVisible(JNIEnv *env, jclass type)
    {
        return editorScene->nodes[editorScene->selectedNodeId]->getProperty(VISIBILITY).value.x;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_jointSize(JNIEnv *env, jclass type,jint selectedNode)
    {
        return editorScene->nodes[selectedNode]->joints.size();
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_mirrorState(JNIEnv *env, jclass type)
    {
        return editorScene->getMirrorState();
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraFov(JNIEnv *env, jclass type)
    {
        return editorScene->cameraFOV;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_resolutionType(JNIEnv *env, jclass type)
    {
        return editorScene->cameraResolutionType;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightx(JNIEnv *env, jclass type)
    {
        return getLightProps().x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lighty(JNIEnv *env, jclass type)
    {
        return getLightProps().y;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightz(JNIEnv *env, jclass type)
    {
        return getLightProps().z;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_shadowDensity(JNIEnv *env, jclass type)
    {
        return ShaderManager::shadowDensity;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeMeshProperty(JNIEnv *env, jclass type,jfloat refraction,jfloat reflection,jboolean light,jboolean visible,jboolean storeAction)
    {
        editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, storeAction);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchMirror(JNIEnv *env, jclass type)
    {
        if(editorScene && editorScene->isRigMode)
            editorScene->rigMan->switchMirrorState();
        else
            editorScene->switchMirrorState();
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getRigMirrorState(JNIEnv *env, jclass type)
    {
        return editorScene->actionMan->getMirrorState();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeLightProperty(JNIEnv *env, jclass type,jfloat x, jfloat y, jfloat z, jfloat w,jfloat distance,jint lightType,jboolean storeAction)
    {
        editorScene->shadowsOff = false;
        editorScene->actionMan->changeLightProperty(x, y, z, w,distance,lightType,storeAction);
        if(storeAction)
        {
            editorScene->updateDirectionLine();
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_previewPosition(JNIEnv *env, jclass type,jint position,jint previewSize,jfloat topHeight,jfloat toolbarWidth)
    {
        cameraPreviewPosition(position,previewSize,topHeight,toolbarWidth);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchFrame(JNIEnv *env, jclass type)
    {
        editorScene->actionMan->switchFrame(editorScene->currentFrame);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setCurrentFrame(JNIEnv *env, jclass type,jint currentFrame,jobject object)
    {
        if(object != NULL)
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID loadingMethod = env->GetMethodID(dataClass, "showOrHideLoading", "(I)V");
            env->CallVoidMethod(object, loadingMethod,SHOW);
        }
        editorScene->previousFrame = editorScene->currentFrame;
        editorScene->currentFrame = currentFrame;
        editorScene->actionMan->switchFrame((float)editorScene->currentFrame);
        if(object != NULL)
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID loadingMethod = env->GetMethodID(dataClass, "showOrHideLoading", "(I)V");
            env->CallVoidMethod(object, loadingMethod,HIDE);
        }
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

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addFrame(JNIEnv *env, jclass type,jint frame)
    {
        editorScene->totalFrames += frame;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTotalFrame(JNIEnv *env, jclass type,jint frame)
    {
        editorScene->totalFrames = frame;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_play(JNIEnv *env, jclass type,jobject object)
    {
        if (editorScene->selectedNodeId != NOT_SELECTED)
        {
            editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
        }
        if(editorScene->selectedNodeIds.size() > 0)
            editorScene->selectMan->unselectObjects();
        if(editorScene->isPlaying)
        {
            if (editorScene->currentFrame +1< editorScene->totalFrames)
            {
                editorScene->setLightingOff();
                editorScene->isPlaying = true;
                editorScene->currentFrame++;
                editorScene->actionMan->switchFrame(editorScene->currentFrame);
            }
            else if (editorScene->currentFrame + 1 >= editorScene->totalFrames)
            {
                editorScene->setLightingOn();
                editorScene->actionMan->switchFrame(editorScene->currentFrame);
                editorScene->isPlaying = false;

            }
            else if (editorScene->currentFrame == editorScene->totalFrames)
            {
                editorScene->setLightingOn();
                editorScene->actionMan->switchFrame(editorScene->currentFrame);
                editorScene->isPlaying = false;
            }
            //editorScene->updater->setDataForFrame(editorScene->currentFrame);
        }
        else
            editorScene->setLightingOn();
        return true;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_currentFrame(JNIEnv *env, jclass type)
    {
        return (editorScene) ? editorScene->currentFrame : 0;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPlaying(JNIEnv *env, jclass type)
    {
        return (editorScene) ? editorScene->isPlaying : false;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setIsPlaying(JNIEnv *env, jclass type,jboolean isPlaying,jobject object)
    {
        if(isPlaying)
        {
            if(object != NULL)
            {
                jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
                jmethodID loadingMethod = env->GetMethodID(dataClass, "showOrHideLoading", "(I)V");
                env->CallVoidMethod(object, loadingMethod,SHOW);
            }
            syncSceneWithPhysicsWorld();
            for(int i = editorScene->currentFrame; i < editorScene->totalFrames; i++)
                editorScene->updatePhysics(i);
        }
        editorScene->isPlaying = isPlaying;
        if(object != NULL)
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID loadingMethod = env->GetMethodID(dataClass, "showOrHideLoading", "(I)V");
            env->CallVoidMethod(object, loadingMethod,HIDE);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPositionViaToolBarPosition(JNIEnv *env, jclass type,jint selectedIndex,jfloat rightWidth,jfloat topHeight)
    {
        cameraPositionViaToolBarPosition(selectedIndex,rightWidth,topHeight);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraPropertyChanged(JNIEnv *env, jclass type,jint fov,jint resolution,jboolean storeAction)
    {
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
        ActionType actionType = IMPORT_ASSET_ACTION;
        if(assetAddType == TEXT_IMAGE_ADD)
            actionType = IMPORT_ASSET_ACTION;
        else if(assetAddType == UNDO)
            actionType = UNDO_ACTION;
        else if (assetAddType == REDO)
            actionType = REDO_ACTION;

         SGNode* textNode = editorScene->loader->loadNode(nodeType, 0,"","-1",assetNameW, fontSize, bevalValue, actionType, textColor, font,isTempNode);


        //TODO SGNode* textNode = editorScene->loader->loadNode(nodeType, 0,texture,assetNameW, fontSize, bevalValue, actionType, textColor, font,isTempNode);

        env->ReleaseStringUTFChars(assetName, name);
        env->ReleaseStringUTFChars(textureName, nativeString);
        env->ReleaseStringUTFChars(filePath, fontName);

        if (textNode == NULL)
        {
            if(editorScene && editorScene->loader)
                editorScene->loader->removeTempNodeIfExists();
            return false;
        }
        if(textNode)
            textNode->isTempNode = isTempNode;
        if(isTempNode)
            editorScene->isPreviewMode = false;
        if(!isTempNode)
        {
            if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0);
        }

        return true;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importImageOrVideo(JNIEnv *env, jclass type,jint nodeType,jstring assetName,jint imgWidth,jint imgHeight
            ,jint assetAddType,jboolean isTempNode)
    {
        editorScene->loader->removeTempNodeIfExists();
        const char *name = env->GetStringUTFChars(assetName, 0);
        wstring assetNameW = ConversionHelper::getWStringForString(name);
        NODE_TYPE nType = (nodeType == NODE_VIDEO) ? NODE_VIDEO : NODE_IMAGE;

        ActionType actionType = IMPORT_ASSET_ACTION;
        if(assetAddType == TEXT_IMAGE_ADD)
            actionType = IMPORT_ASSET_ACTION;
        else if(assetAddType == UNDO)
            actionType = UNDO_ACTION;
        else if (assetAddType == REDO)
            actionType = REDO_ACTION;
      //TODO  SGNode* sgNode = editorScene->loader->loadNode(nType, 0,"",assetNameW, imgWidth, imgHeight,(int)actionType ,Vector4(imgWidth,imgHeight,0,0),"",isTempNode);
       SGNode* sgNode = editorScene->loader->loadNode(nodeType, 0,"","-1",assetNameW,imgWidth, imgHeight,(int)actionType ,Vector4(imgWidth,imgHeight,0,0),"",isTempNode);

        if(sgNode)
            sgNode->isTempNode = isTempNode;
        if(isTempNode)
            editorScene->isPreviewMode = false;
        if(!isTempNode)
        {
            if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0);
        }
        env->ReleaseStringUTFChars(assetName, name);

        return false;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_importAdditionalLight(JNIEnv *env, jclass type,jint lightCount,jint action)
    {
        ActionType actionType = IMPORT_ASSET_ACTION;
        if(action == IMPORT_ASSET)
            actionType = IMPORT_ASSET_ACTION;
        else if(action == UNDO)
            actionType = UNDO_ACTION;
        else if (action == REDO)
            actionType = REDO_ACTION;

        if(ShaderManager::lightPosition.size() < 5)
        {
            int assetId = ASSET_ADDITIONAL_LIGHT + lightCount;
            wstring assetNameW = ConversionHelper::getWStringForString("Light " + to_string(lightCount));
            //TODO editorScene->loader->loadNode(NODE_ADDITIONAL_LIGHT, assetId ,"",assetNameW, 20 , 50 , actionType , Vector4(1.0),"",false);
            if(actionType != UNDO_ACTION && actionType != REDO_ACTION)
            {
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId , "Light"+ to_string(lightCount));
            }
        }
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_lightCount(JNIEnv *env, jclass type)
    {
        return ShaderManager::lightPosition.size();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAsSGM(JNIEnv *env, jclass type,jstring fileName,jstring textureName,
            jint assetId,jboolean haveTexture,jfloat x,jfloat y, jfloat z)
    {
        const char *objStr = env->GetStringUTFChars(fileName, 0);
        const char *textureStr = env->GetStringUTFChars(textureName, 0);
        //TODO editorScene->objMan->loadAndSaveAsSGM(objStr, textureStr, assetId,!haveTexture,Vector3(x,y,z));
        env->ReleaseStringUTFChars(fileName, objStr);
        env->ReleaseStringUTFChars(textureName, textureStr);


    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeTexture(JNIEnv *env, jclass type,jint selectedNodeId,jstring textureName,jfloat x,jfloat y,jfloat z,jboolean isTemp)
    {

        const char *texture = env->GetStringUTFChars(textureName, 0);
        editorScene->selectMan->selectObject(selectedNodeId,false);
        if(!(editorScene->selectedNodeIds.size() > 0) && editorScene->hasNodeSelected())
        {
            editorScene->changeTexture(texture, Vector3(x,y,z),isTemp,false);
            editorScene->selectMan->unselectObject(selectedNodeId);
            editorScene->setLightingOn();
        }
        env->ReleaseStringUTFChars(textureName, texture);

    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeTempTexture(JNIEnv *env, jclass type,jint selectedNodeId)
    {
        editorScene->removeTempTextureAndVertex(selectedNodeId);
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_canEditRigBones(JNIEnv *env, jclass type)
    {
        return (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && !editorScene->canEditRigBones(editorScene->nodes[editorScene->selectedNodeId]));
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_beginRigging(JNIEnv *env, jclass type)
    {
        int selectedNodeId = editorScene->selectedNodeId;
        editorScene->riggingNodeId = selectedNodeId;
        editorScene->enterOrExitAutoRigMode(true);
        editorScene->rigMan->sgmForRig(editorScene->nodes[selectedNodeId]);
        editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_OBJVIEW));
        //editorScene->rigMan->boneLimitsCallBack = &boneLimitsCallBack;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cancelRig(JNIEnv *env, jclass type,jboolean completed)
    {
        if(editorScene->rigMan->deallocAutoRig(completed))
        {
            editorScene->enterOrExitAutoRigMode(false);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_addJoint(JNIEnv *env, jclass type,jobject obj)
    {

        if(editorScene->rigMan->isSkeletonJointSelected)
        {
            /*
            	if(editorScene->rigMan->rigKeys.size() >= RIG_MAX_BONES){
            		jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            		jmethodID javaRigMethodRef = env->GetMethodID(dataClass, "boneLimitCallBack", "()V");
            		env->CallVoidMethod(obj, javaRigMethodRef);
            	}
            	*/
            editorScene->rigMan->addNewJoint();
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_switchRigSceneMode(JNIEnv *env, jclass type,jobject obj,jint scene)
    {
        if(editorScene && editorScene->rigMan)
        {
            jboolean completed = false;
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID javaRigMethodRef = env->GetMethodID(dataClass, "rigCompletedCallBack", "(Z)V");
            if(editorScene->rigMan->sceneMode + scene == RIG_MODE_PREVIEW)
            {
                string path = constants::DocumentsStoragePath+"/mesh/123456.sgr";
                editorScene->rigMan->exportSGR(path);
                completed = true;
                env->CallVoidMethod(obj, javaRigMethodRef,completed);
            }
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode + scene));
            env->CallVoidMethod(obj, javaRigMethodRef,completed);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setSkeletonType(JNIEnv *env, jclass type,jint skeletonType)
    {
        if(skeletonType == HUMAN_RIGGING)
        {
            editorScene->rigMan->skeletonType = SKELETON_HUMAN;
        }
        else if(skeletonType == OWN_RIGGING)
        {
            editorScene->rigMan->skeletonType = SKELETON_OWN;
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObjects(JNIEnv *env, jclass type)
    {
        editorScene->selectMan->unselectObjects();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTransparency(JNIEnv *env, jclass type,jfloat transparency,jint nodeId)
    {
        editorScene->nodes[nodeId]->getProperty(TRANSPARENCY,0).value.x = transparency;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_hideNode(JNIEnv *env, jclass type,jint nodeId,jboolean hide)
    {
        editorScene->nodes[nodeId]->node->setVisible(!hide);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_unselectObject(JNIEnv *env, jclass type,jint nodeId)
    {
        editorScene->selectMan->unselectObject(nodeId);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_copyKeysOfNode(JNIEnv *env, jclass type,jint fromNodeId,jint toNodeId)
    {
        editorScene->animMan->copyKeysOfNode(fromNodeId, toNodeId);
        editorScene->updater->setDataForFrame(editorScene->currentFrame);
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getBoneCount(JNIEnv *env, jclass type,jint nodeId)
    {
        if(editorScene->isRigMode)
        {
            return (int)editorScene->rigMan->rigKeys.size();
        }
        else
            return (int)editorScene->nodes[nodeId]->joints.size();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_applyAnimation(JNIEnv *env, jclass type,jint fromNodeId,jint applyedNodeId,jstring path)
    {
        const char *filePathStr = env->GetStringUTFChars(path, 0);
        string filepath = filePathStr;
        editorScene->animMan->applyAnimations(filePathStr, applyedNodeId);
        env->ReleaseStringUTFChars(path, filePathStr);
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scale(JNIEnv *env, jclass type,jobject object)
    {
        bool status = false;
        if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
        {
            if((editorScene->selectedNodeIds.size() > 0) && (editorScene->allObjectsScalable()))
            {
                status = true;
            }
            else if(!(editorScene->selectedNodeIds.size() > 0) && (editorScene->isRigMode ||(editorScene->hasNodeSelected() && (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_PARTICLES && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT))))
            {
                status = true;
            }
        }
        else if(editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_EDIT_ENVELOPES) && editorScene->rigMan->isSkeletonJointSelected)
            status = true;
        if(status)
        {
            editorScene->controlType = SCALE;
            editorScene->updater->updateControlsOrientaion();
            editorScene->renHelper->setControlsVisibility(false);
            if (!editorScene->isPlaying)
            {
                Vector3 trans = editorScene->getTransformValue();
                updateXYZValuesCallBack(env,type,object,false,trans.x,trans.y,trans.z);
            }
        }
        return status;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueX(JNIEnv *env, jclass type)
    {
        Vector3 currentScale;
        if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
            currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
        return currentScale.x;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueY(JNIEnv *env, jclass type)
    {
        Vector3 currentScale;
        if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
            currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
        return currentScale.y;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_scaleValueZ(JNIEnv *env, jclass type)
    {
        Vector3 currentScale;
        if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)))
            currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
        return currentScale.z;
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_envelopScale(JNIEnv *env, jclass type)
    {
        return  editorScene->rigMan->getSelectedJointScale();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setScaleValue(JNIEnv *env, jclass type,jfloat x,jfloat y, jfloat z,jboolean store)
    {
        if((editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size()) && editorScene->selectedNodeIds.size() <= 0)
            return;
        editorScene->actionMan->changeObjectScale(Vector3(x, y, z), store);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_move(JNIEnv *env, jclass type,jobject object)
    {
        if(editorScene->hasNodeSelected() || editorScene->selectedNodeIds.size() > 0)
        {
            editorScene->controlType = MOVE;
            editorScene->updater->updateControlsOrientaion();
            if (!editorScene->isPlaying)
            {
                Vector3 trans = editorScene->getTransformValue();
                updateXYZValuesCallBack(env,type,object,false,trans.x,trans.y,trans.z);
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rotate(JNIEnv *env, jclass type,jobject object)
    {
        if(editorScene->hasNodeSelected() || editorScene->allObjectsScalable())
        {
            editorScene->controlType = ROTATE;
            editorScene->updater->updateControlsOrientaion();
            if (!editorScene->isPlaying)
            {
                Vector3 trans = editorScene->getTransformValue();
                updateXYZValuesCallBack(env,type,object,false,trans.x,trans.y,trans.z);
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_rigNodeScale(JNIEnv *env, jclass type,float x, float y, float z)
    {
        editorScene->rigMan->changeNodeScale(Vector3(x,y,z));
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changeEnvelopScale(JNIEnv *env, jclass type, jfloat x)
    {
        if((editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_EDIT_ENVELOPES)) &&  editorScene->rigMan->isSkeletonJointSelected)
        {
            editorScene->rigMan->changeEnvelopeScale(Vector3(x), false);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isRigMode(JNIEnv *env, jclass type)
    {
        return editorScene->isRigMode;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isVAOSupported(JNIEnv *env, jclass type)
    {
        return editorScene->renHelper->supportsVAO();
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cameraView(JNIEnv *env, jclass type,int indexValue)
    {
        CAMERA_VIEW_MODE cameraView = (indexValue == FRONT_VIEW) ? VIEW_FRONT : (indexValue == TOP_VIEW) ? (CAMERA_VIEW_MODE)VIEW_TOP : (indexValue == LEFT_VIEW) ? VIEW_LEFT : (indexValue == BACK_VIEW) ? VIEW_BACK : (indexValue == RIGHT_VIEW) ? VIEW_RIGHT :VIEW_BOTTOM;
        editorScene->updater->changeCameraView(cameraView);
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isNodeSelected(JNIEnv *env, jclass type,int position)
    {
        if(editorScene->selectedNodeIds.size() > 0)
        {
            for (int i = 0; i < editorScene->selectedNodeIds.size(); ++i)
            {
                if(editorScene->selectedNodeIds[i] == position)
                {
                    return true;
                    break;
                }
            }
            return false;
        }
        else if(editorScene->selectedNodeId == position)
        {
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_deleteAnimation(JNIEnv *env, jclass type)
    {
        bool isKeySetForNode = editorScene->animMan->removeAnimationForSelectedNodeAtFrame(editorScene->currentFrame);
        return isKeySetForNode;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_saveAnimation(JNIEnv *env, jclass type,jobject obj,jint assetId,jstring name, jint animType)
    {
        bool status = editorScene->animMan->storeAnimations(assetId);

        jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
        jmethodID animSaveMethod = env->GetMethodID(dataClass, "addToDatabase", "(ZLjava/lang/String;I)V");
        env->CallVoidMethod(obj, animSaveMethod,status,name,animType);
        return status;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isJointSelected(JNIEnv *env, jclass type)
    {
        return editorScene->isJointSelected;
    }


    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isHaveKey(JNIEnv *env,jclass type, jint currentFrame)
    {
        if(editorScene)
            return editorScene->isKeySetForFrame.find(currentFrame) != editorScene->isKeySetForFrame.end();
        else
            return false;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_hasNodeSelected(JNIEnv *env,jclass type)
    {
        return editorScene->hasNodeSelected();
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_editorScene(JNIEnv *env,jclass type)
    {
        if(editorScene) return true;
        return false;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_undo(JNIEnv *env,jclass type,jobject object)
    {
        int returnValue = NOT_SELECTED;
        ACTION_TYPE actionType = (ACTION_TYPE)editorScene->undo(returnValue);

        switch(actionType)
        {
        case ADD_INSTANCE_BACK:
        {
            SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction - 1];

            int actionId = recentAction.actionSpecificIntegers[1];
            int nodeIndex = -1;
            for (int i = 0; i < editorScene->nodes.size(); i++)
            {
                if(actionId == editorScene->nodes[i]->actionId)
                    nodeIndex = i;
            }

            if(nodeIndex != NOT_EXISTS)
            {
                editorScene->loader->createInstance(editorScene->nodes[nodeIndex], editorScene->nodes[nodeIndex]->getType(), UNDO_ACTION);
            }
            break;
        }
        default:
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID undo = env->GetMethodID(dataClass, "undo", "(II)V");
            env->CallVoidMethod(object, undo,(int)actionType,returnValue);
            break;
        }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_redo(JNIEnv *env,jclass type,jobject object)
    {
        int returnValue = editorScene->redo();
        if (returnValue == ADD_INSTANCE_BACK)
        {
            SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction];

            int actionId = recentAction.actionSpecificIntegers[1];
            int nodeIndex = -1;
            for (int i = 0; i < editorScene->nodes.size(); i++)
            {
                if(actionId == editorScene->nodes[i]->actionId)
                    nodeIndex = i;
            }

            if(nodeIndex != NOT_EXISTS)
            {
                editorScene->loader->createInstance(editorScene->nodes[nodeIndex], editorScene->nodes[nodeIndex]->getType(), REDO_ACTION);
            }

        }
        else
        {
            jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
            jmethodID redo = env->GetMethodID(dataClass, "redo", "(I)V");
            env->CallVoidMethod(object, redo,returnValue);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_decreaseCurrentAction(JNIEnv *env,jclass type)
    {
        editorScene->actionMan->currentAction--;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_increaseCurrentAction(JNIEnv *env,jclass type)
    {
        editorScene->actionMan->currentAction++;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_objectIndex(JNIEnv *env,jclass type)
    {
        SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction-1];
        return recentAction.objectIndex;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_renderFrame(JNIEnv *env,jclass type,jobject object,jint frame,jint shader,jboolean isImage,jboolean waterMark
            ,jfloat x, jfloat y, jfloat z)
    {
        editorScene->renHelper->isExportingImages = true;
        editorScene->updater->setDataForFrame(frame-1);
        string path = constants::DocumentsStoragePath+"/.cache/"+to_string(frame)+".png";
        editorScene->renHelper->renderAndSaveImage(path.c_str(), shader, false, waterMark,(isImage) ? -1 : frame-1, Vector4(x,y,z,1.0));
        jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/NativeCallBacks");
        jmethodID preview = env->GetMethodID(dataClass, "updatePreview", "(I)V");
        env->CallVoidMethod(object, preview,frame);
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_perVertexColor(JNIEnv *env,jclass type,jint id)
    {
        return editorScene->nodes[id]->getProperty(IS_VERTEX_COLOR).value.x;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_initVideoManagerClass(JNIEnv *env,jclass type,jobject object)
    {
        env->GetJavaVM(&g_vm);
        videoManagerClass = env->NewGlobalRef(object); //object;
        jclass dataClass = env->FindClass("com/smackall/animator/NativeCallBackClasses/VideoManager");
        getVideoAtFrameMethodId = env->GetMethodID(dataClass,"getImageAtTime","(Ljava/lang/String;III)[B");
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setNodeLighting(JNIEnv *env, jclass type,jint nodeId,jboolean state)
    {
        if(editorScene)
        {
            if(nodeId != -1)
                editorScene->nodes[nodeId]->getProperty(LIGHTING,0).value.x = state;
            else if(editorScene->selectedNodeId != -1)
                editorScene->nodes[editorScene->selectedNodeId]->getProperty(LIGHTING,0).value.x = state;
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setNodeVisiblity(JNIEnv *env, jclass type,jint nodeId,jboolean state)
    {
        if(editorScene)
        {
            if(nodeId != -1)
                editorScene->nodes[nodeId]->getProperty(VISIBILITY).value.x = state;
            else if(editorScene->selectedNodeId != -1)
                editorScene->nodes[editorScene->selectedNodeId]->getProperty(VISIBILITY).value.x = state;
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setfreezeRendering(JNIEnv *env, jclass type,jboolean state)
    {
        if(editorScene)
            editorScene->freezeRendering = state;
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_cloneSelectedAsset(JNIEnv *env, jclass type,jint selectedAssetId,jint selectedNodeType,jint selectedNodeIndex,jint assetAddType)
    {

        SGNode *sgNode = NULL;
        if(selectedNodeIndex != NOT_EXISTS)
            sgNode = editorScene->nodes[selectedNodeIndex];

        if (selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ)
        {

            if(sgNode->node->original && sgNode->node->original->instancedNodes.size() >= 8000)
            {
                return false;
            }

            editorScene->loader->createInstance(sgNode, sgNode->getType(), assetAddType);

            if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, sgNode->assetId);
            editorScene->animMan->copyPropsOfNode(selectedNodeIndex, (int)editorScene->nodes.size()-1);
            editorScene->selectMan->selectObject(editorScene->nodes.size()-1 , false);
            editorScene->updater->setDataForFrame(editorScene->currentFrame);
        }
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_selectedNodeIdsSize(JNIEnv *env, jclass type)
    {
        return (int)editorScene->selectedNodeIds.size();
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedNodeIdAtIndex(JNIEnv *env, jclass type,jint index)
    {
        return editorScene->selectedNodeIds[index];
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_createDuplicateAssets(JNIEnv *env, jclass type,jobject object)
    {

       createDuplicateAssets(env,type,object);
    }

    JNIEXPORT jboolean JNICALL Java_com_smackall_animator_opengl_GL2JNILib_isPhysicsEnabled(JNIEnv *env, jclass type,jint nodeId)
    {
        bool isPhysicsEnable = false;
        if(nodeId != -1)
            isPhysicsEnable = editorScene->nodes[nodeId]->getProperty(HAS_PHYSICS).value.x;
        return isPhysicsEnable;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_physicsType(JNIEnv *env, jclass type,jint nodeId)
    {
        int physicsType = 0;
        if(nodeId != -1)
            physicsType = editorScene->nodes[nodeId]->getProperty(PHYSICS_KIND).value.x;
        return physicsType;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_velocity(JNIEnv *env, jclass type,jint nodeId)
    {
        int velocity = 0;
        if(nodeId != -1)
            velocity = editorScene->nodes[nodeId]->getProperty(FORCE_MAGNITUDE).value.x;
        return velocity;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_enablePhysics(JNIEnv *env, jclass type,jboolean status)
    {
        if(editorScene && editorScene->selectedNodeId != NOT_SELECTED)
        {
            editorScene->nodes[editorScene->selectedNodeId]->getProperty(HAS_PHYSICS).value.x = status;
            if(!status)
            {
                syncSceneWithPhysicsWorld();
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setPhysicsType(JNIEnv *env, jclass type,jint physicsType)
    {
        if(editorScene && editorScene->selectedNodeId != NOT_SELECTED)
        {
            editorScene->setPropsOfObject(editorScene->nodes[editorScene->selectedNodeId], physicsType);
            syncSceneWithPhysicsWorld();
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_velocityChanged(JNIEnv *env, jclass type,jint value)
    {
        if(editorScene && editorScene->selectedNodeId != NOT_SELECTED)
        {
            editorScene->nodes[editorScene->selectedNodeId]->getProperty(HAS_PHYSICS,0).subProps[FORCE_MAGNITUDE].value.x = value;
            syncSceneWithPhysicsWorld();
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setDirection(JNIEnv *env, jclass type)
    {
         setDirection();
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_removeSelectedObjects(JNIEnv *env, jclass type)
    {
        if(editorScene->selectedNodeIds.size() > 0)
        {
            editorScene->loader->removeSelectedObjects();
        }
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getLightType(JNIEnv *env, jclass type)
    {
        return editorScene->getSelectedNode()->getProperty(LIGHT_TYPE).value.x;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_ADD_INSTANCE_BACK(JNIEnv *env, jclass type,jint actionType)
    {
        SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction - (actionType == 1) ? 1 : 0];

        int actionId = recentAction.actionSpecificIntegers[1];
        int nodeIndex = -1;
        for (int i = 0; i < editorScene->nodes.size(); i++)
        {
            if(actionId == editorScene->nodes[i]->actionId)
                nodeIndex = i;
        }

        if(nodeIndex != NOT_EXISTS)
        {
            editorScene->loader->createInstance(editorScene->nodes[nodeIndex], editorScene->nodes[nodeIndex]->getType(), UNDO_ACTION);
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_syncPhysicsWithWorld(JNIEnv *env, jclass type,jint from, jint to,jboolean doUpdatePhysics)
    {
        syncSceneWithPhysicsWorld();
        for(int i = from; i < to && doUpdatePhysics; i++)
            updatePhysics(i);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_updatePhysics(JNIEnv *env, jclass type,jint frame)
    {
        updatePhysics(frame);
    }

    JNIEXPORT jfloat JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSelectedJointScale(JNIEnv *env, jclass type)
    {
        return editorScene->rigMan->getSelectedJointScale();
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_smoothTexState(JNIEnv *env, jclass type)
    {
        int smoothTex = (editorScene->nodes[editorScene->selectedNodeId]->getProperty(IS_VERTEX_COLOR).value.x) ? -1 : (int)editorScene->nodes[editorScene->selectedNodeId]->smoothTexture;
        return smoothTex;
    }

    JNIEXPORT jint JNICALL Java_com_smackall_animator_opengl_GL2JNILib_smoothTexStateWithId(JNIEnv *env, jclass type, jint nodeId)
    {
        int smoothTex = (editorScene->nodes[nodeId]->getProperty(IS_VERTEX_COLOR).value.x) ? -1 : (int)editorScene->nodes[nodeId]->smoothTexture;
        return smoothTex;
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_setTextureSmoothStatus(JNIEnv *env, jclass type, jboolean status)
    {
        if(editorScene && editorScene->selectedNodeId != NOT_SELECTED && editorScene->nodes[editorScene->selectedNodeId])
        {
            editorScene->nodes[editorScene->selectedNodeId]->smoothTexture = status;
            if(sceneManager->device == OPENGLES2){
             //TODO  editorScene->changeTexture(editorScene->nodes[editorScene->selectedNodeId]->textureName, Vector3(1.0), false, false);
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getNodePropsMapWithNodeId(JNIEnv *env, jclass type, jobject callback,jint nodeId)
    {
        std::map< PROP_INDEX, Property > options = editorScene->nodes[nodeId]->getAllProperties(0); //TODO
        setProperties(env, type, callback, options);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_getSceneProps(JNIEnv *env, jclass type, jobject callback)
    {
        std::map< PROP_INDEX, Property > sceneProps = editorScene->shaderMGR->sceneProps;
        setProperties(env, type, callback, sceneProps);
    }

    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_changedPropertyAtIndex(JNIEnv *env, jclass type, jobject callback,jint index, jfloat x,
     jfloat y,jfloat z,jfloat w, jboolean status){
        changedPropertyAtIndex(env,type,callback,(PROP_INDEX)index,Vector4(x,y,z,w),status);
    }


    JNIEXPORT void JNICALL Java_com_smackall_animator_opengl_GL2JNILib_dealloc(JNIEnv *env, jclass type)
    {

        if(editorScene)
        {
            delete editorScene;
            editorScene = NULL;
        }

        int oglWidth = 0;
        int oglHeight = 0;
        screenScale = 1.0;
        touchBegan = Vector2(0.0,0.0);
        touchX = 0.0;
        touchY = 0.0;
        prevPos = Vector2(0.0,0.0);
        currentPos = Vector2(0.0,0.0);
        tapPosition = Vector2(0.0,0.0);;
        displayPrepared = false;
        fileName = "";
    }
}


