//
// Created by shankarganesh on 22/9/15.
//

#include "preview_scene.h"
#include "Iyan3dEngineFiles/HeaderFiles/SGPreviewScene.h"
#include "opengl.h"
#include <ctime>

GLuint _colorRenderBufferAsset;
GLuint _depthRenderBufferAsset;
GLuint _frameBufferAsset;
int oglWidthAsset , oglHeightAsset;
float screenScalepreview = 1.0;
SceneManager *previewSceneManager;
SGPreviewScene *previewScene;
VIEW_TYPE currentViewType;
NODE_TYPE animationSelectionnodeType;
int animationSelectionAsseitId,animationSelectionfontSize,animationSelectionBevelValue;
string animationSelectionAssetName,animationSelectionFontFilepath;
Vector4 animationSelectiontextColor;
bool applyAnimation;
int animationAssetId;
void addCameraLightInPreviewScene()
{
    previewScene->loadNode(NODE_CAMERA,0,L"CAMERA");
    previewScene->loadNode(NODE_LIGHT,0,L"LIGHT");

    if(currentViewType==ALL_ANIMATION_VIEW && animationSelectionnodeType==NODE_RIG){
        previewScene->loadNode(animationSelectionnodeType,animationSelectionAsseitId,ConversionHelper::getWStringForString(animationSelectionAssetName));
    }
    if(currentViewType==ALL_ANIMATION_VIEW && animationSelectionnodeType==NODE_TEXT){
        previewScene->loadNode(animationSelectionnodeType,0,ConversionHelper::getWStringForString(animationSelectionAssetName),animationSelectionfontSize,animationSelectionBevelValue,animationSelectiontextColor,animationSelectionFontFilepath);
    }
}
void nodeShaderCallBack(int nodeID,string matName,string callbackFuncName)
{
    if(callbackFuncName.compare("setUniforms") == 0 && previewScene)
        previewScene->shaderCallBackForNode(nodeID,matName);
}
bool transparentCallBack(int nodeId,string callbackFuncName)
{
    if(callbackFuncName.compare("setUniforms") == 0 && previewScene)
        return previewScene->isNodeTransparent(nodeId);
    else if(callbackFuncName.compare("setOBJUniforms") == 0)
        return  false;
    return false;
}

void setupRenderBufferPreview()
{
    glGenRenderbuffers(1, &_colorRenderBufferAsset);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBufferAsset);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA4,oglWidthAsset,oglHeightAsset);
//    checkGlError("setup render buffer ");
//[_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
void setupDepthBufferPreview()
{
    glGenRenderbuffers(1, &_depthRenderBufferAsset);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBufferAsset);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, oglWidthAsset * screenScalepreview, oglHeightAsset * screenScalepreview);
    __android_log_print(ANDROID_LOG_VERBOSE, "IYAN3D", "Depth buffer setup successfull");
//    checkGlError("setup depth buffer");
}
void setupFrameBufferPreview()
{
    glGenFramebuffers(1, &_frameBufferAsset);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBufferAsset);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufferAsset);
//    checkGlError("setup frame buffer");
}
void initpreviewScene(int width, int height){
    oglWidthAsset=width;
    oglHeightAsset=height;

    LOGI("Screen width %d height %d ",width,height);
       previewSceneManager= new SceneManager(width,height,1,OPENGLES2,"bgImageforall.png");
    previewScene = new SGPreviewScene(OPENGLES2,previewSceneManager,width,height,currentViewType);
    previewScene->screenScale = screenScalepreview;
    setupRenderBufferPreview();
    setupDepthBufferPreview();
    setupFrameBufferPreview();
    previewSceneManager->setFrameBufferObjects(0,_colorRenderBufferAsset,_depthRenderBufferAsset);
    previewSceneManager->ShaderCallBackForNode = &nodeShaderCallBack;
    previewSceneManager->isTransparentCallBack = &transparentCallBack;
    addCameraLightInPreviewScene();
}
void renderPreviewScene(){
    if(applyAnimation){
        previewScene->applyAnimations(animationAssetId);
        applyAnimation= false;
    }
    if(previewScene)
     previewScene->renderAll();


}
void removeCurrentObject(){
    LOGI("Node Size: %ld",previewScene->nodes.size());
    while (previewScene->nodes.size()>1){
        if(previewScene){
            previewScene->removeObject((u16)(previewScene->nodes.size()-1), false);

        }
    }
}
void DisplayAsset(int AssetId,string AssetName,int AssetType){
    NODE_TYPE nodeType;
    if(AssetType == 1)
        nodeType = NODE_RIG;
    else if(AssetType == 6)
        nodeType = NODE_OBJ;
    else
        nodeType = NODE_SGM;

        removeCurrentObject();
        LOGI("Asset Name : %s",AssetName.c_str());
        previewScene->loadNode(nodeType,AssetId,ConversionHelper::getWStringForString(AssetName));

    }

void swipeRotate(float x, float y){
    previewScene->swipeToRotate(-x/50.0,-y/50.0);
}

void swipeEnd(){
    previewScene->swipeEnd();
}
void setViewType(VIEW_TYPE viewType){

currentViewType=viewType;

}
void animationSelectionAssetItemToBeDisplayed(NODE_TYPE nodeType,int assetId,string assetName){


    animationSelectionAsseitId=assetId;
    animationSelectionAssetName=assetName;
    animationSelectionnodeType=nodeType;

}
void animationSelectionTextItemToBeDisplayed(string text,int fontSize, float bevelValue,Vector3 color,string filePath){

    animationSelectionAssetName=text;
    animationSelectionfontSize=fontSize;
    animationSelectionBevelValue=bevelValue;
    animationSelectiontextColor=Vector4(color.x,color.y,color.z,1.0);
    animationSelectionFontFilepath=filePath;
    animationSelectionnodeType=NODE_TEXT;
}

void playAnimation(){
    if(previewScene){
        if(previewScene->currentFrame==previewScene->totalFrames){
            previewScene->currentFrame=0;
        }
        if(previewScene->currentFrame<previewScene->totalFrames){
            previewScene->playAnimation();
        }
        previewScene->currentFrame++;
    }
}
void applyAnimationToNode(int assetid){

    LOGI("PREVIEWSCENE Apply animation %d " ,assetid);
    animationAssetId=assetid;
applyAnimation=true;
}
void stopAllAnimations(){
    if(previewScene)
        previewScene->stopAnimation();

}
void remove3DText(){


}
void load3DText(string displayText,int fontSize,int bevelValue,int R,int G, int B,string filenmame){

    LOGI("Font SIze: %d ",fontSize);
    
    previewScene->removeObject(previewScene->nodes.size()-1, false);
    previewScene->loadNode(NODE_TEXT,0,ConversionHelper::getWStringForString(displayText),fontSize,bevelValue,Vector4((float)R/255.0,(float)G/255.0,(float)B/255.0,1.0),filenmame);
}
void panBegin(float x1, float y1, float x2, float y2)
{
previewScene->pinchBegan(Vector2(x1,y1),Vector2(x2,y2));
}
void panprogress(float x1, float y1, float x2, float y2)
{
previewScene->pinchZoom(Vector2(x1,y1),Vector2(x2,y2));
}
void destroyScene(){
    if(previewScene){
        delete previewScene;
    }
}

bool isPreviewScene(){
    if(previewScene)
        return true;
    else
        return false;
}


