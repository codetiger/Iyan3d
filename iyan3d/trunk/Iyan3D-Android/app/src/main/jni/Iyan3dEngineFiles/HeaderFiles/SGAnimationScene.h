#ifndef __Iyan3D__SGAnimationScene__
#define __Iyan3D__SGAnimationScene__

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "../../SGEngine2/Utilities/Logger.h"
#include "Constants.h"
#include "ShaderManager.h"
#include "SGAnimationSceneHelper.h"
#include "SGScene.h"
#include "SGNode.h"

#ifdef ANDROID
#include "jni.h"
#endif

#include <map>
#include "Font/TextMesh3d.h"

using namespace std;

class SGAnimationScene : public SGScene
{
private:
    bool isMoved,exportImage,writeThumbnailTexture;
    string exportImagePath;
    Vector2 nodeJointPickerTouchPosition;
    Vector2 ctrlPickerTouchPosition;
    std::map<int,Vector3>             ikJointsPositionMap;
    std::map<int,Vector3>::iterator   ikJointsPositoinMapItr;
    std::map<int,vector<Vector3> > textJointsBasePos;
    void getNodeColorFromTouchTexture();
    bool selectNodeOrJointInPixel(Vector2 touchPixel);
    void rttDrawCall();
    void postRTTDrawCall();
    void rttShadowMap();
    void storeInitialKeyForUndo();
    void postNodeJointSelection();
    void rttControlSelectionAnim(Vector2 touchPosition);
    void getCtrlColorFromTouchTextureAnim(Vector2 touchPosition);
    bool checkNodeSize();
    
public:
    bool changeAction,isPlaying,isExportingImages,isRTTCompleted,isPremiumUnlocked,isExporting1stTime , pauseRendering;
    int currentFrame,totalFrames,previousFrame,cameraResolutionType,animStartFrame,animTotalFrames,actionObjectsSize;
    int assetIDCounter , renderingType;
    MATERIAL_TYPE commonType , commonSkinType , vertexColorTextType , toonType , toonSkinType , vertexColorTextToonType , colorType , colorSkinType , vertexColorType;
    
    float cameraRadius,cameraFOV;
    std::string animFilePath  , documentsDirectory;
    SGAction assetAction;
    shared_ptr<MeshNode> objNode;
    shared_ptr<CameraNode> renderCamera;
    Texture *previewTexture ,*thumbnailTexture,*shadowTexture;
    vector<SGNode*> nodes;
    std::map<int,int> isKeySetForFrame;
    std::map<int,int>::iterator keyFramesIterator;
    std::map<int,Texture*> renderingTextureMap;
    std::map<int,Texture*>::iterator textureMapIt;
    SGAnimationScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight);
    ~SGAnimationScene();
    virtual void renderAll();
    void (*notRespondingCallBack)();
    bool (*downloadMissingAssetCallBack)(std::string filePath, NODE_TYPE nodeType);
    void (*fileWriteCallBack)();
    void checkCtrlSelection(Vector2 curTouchPos,bool isDisplayPrepared = false);
    void initTextures();
    void initRenderCamera();
    //void shaderCallBackForNode(int nodeIndex,string matName);
    void checkSelection(Vector2 touchPosition,bool isDisplayPrepared = false);
    void changeObjectScale(Vector3 scale, bool isChanged);
    void rttNodeJointSelection(Vector2 touchPosition);
    bool updateNodeSelectionFromColor(Vector3 pixel);
    void highlightSelectedNode();
    void setDataForFrame(int frame);
    void setKeysForFrame(int frame);
    void applySGAOnNode(std::string *filePath);
    bool storeAnimations(int assetId);
    void applyAnimations(string filePath , int nodeIndex = -1);
    void applySGAPositionData(short sgaFrames,int currentFrame,ifstream* filePointer,float heightRatio);
    void applySGARotationData(short sgaFrames,ifstream* filePointer,int currentFrame);
    void touchBegan(Vector2 curTouchPos);
    void touchMove(Vector2 curTouchPos,Vector2 prevTouchPos,float width,float height);
    void touchEnd(Vector2 curTouchPos);
    void DrawMoveAxisLine();
    void setRenderCameraOrientation();
    void drawCameraPreview();
    void storeMovementAction();
    void switchFrame(int frame);
    void findAndInsertInIKPositionMap(int jointId);
    void getIKJointPosition();
    bool loadSceneData(std::string *filePath);

    #ifdef  ANDROID
        bool readScene(ifstream *filePointer, JNIEnv *env, jclass type);
        bool loadSceneData(std::string *filePath, JNIEnv *env, jclass type);
        bool (*downloadMissingAssetsCallBack)(std::string filePath, NODE_TYPE nodeType, JNIEnv *env, jclass type);
    #endif
    bool readScene(ifstream *filePointer);
    void saveSceneData(string *filePath);
    void writeScene(ofstream *filePointer);
    void reloadKeyFrameMap();
    void setCameraProperty(float fov , int resolutionType);
    void updateRenderCamera();
    void unselectObject(int objectId);
    void selectObject(int objectId);
    void setMirrorState(MIRROR_SWITCH_STATE flag);
    bool switchMirrorState();
    bool loadNode(SGNode *sgNode,int actionType = OPEN_SAVED_FILE);
    void performUndoRedoOnNodeLoad(SGNode* meshObject,int actionType);
    void removeObject(u16 nodeIndex,bool deAllocScene = false);
    void storeAddOrRemoveAssetAction(int actionType, int assetId , string optionalFilePath = "");
    void StoreDeleteObjectKeys(int nodeIndex);
    void changeMeshProperty(float brightness, float specular, bool isLighting, bool isVisible, bool isChanged);
    void changeCameraProperty(float fov , int resolutionType, bool isChanged);
    void changeLightProperty(float red , float green, float blue, float shadow, bool isChanged);
    void saveThumbnail(char* imagePath);
    void renderAndSaveImage(char *imageName , int shaderType,bool isDisplayPrepared = false, bool removeWaterMark = false);
    void storeLightPropertyChangeAction(float redValue , float greenValue, float blueValue , float shadowDensity);
    bool removeAnimationForSelectedNodeAtFrame(int selectedFrame);
    bool changeObjectOrientation(Vector3 outputValue);
    //bool isNodeTransparent(int nodeId);
    void addAction(SGAction& action);
    void updateLightCamera();
    void setDocumentsDirectory(string documentsPath);
    //void exportSceneAsFBX();
    vector<string> generateSGFDFiles(int startFrame , int endFrame);
    void updateLightProperties(int frameId);
    void addLight(SGNode * light);
    void popLightProps();
    void clearLightProps();
    void setLightingOff();
    void setLightingOn();
    void setTransparencyForIntrudingObjects();

    void resetMaterialTypes(bool isToonShader);

    NODE_TYPE getSelectedNodeType();

    SGNode* loadNode(NODE_TYPE type,int assetId,wstring imagePath = L" ",int imgWidth = 0,int imgHeight = 0,int actionType = OPEN_SAVED_FILE, Vector4 textColor = Vector4(0),string fontFilePath = "");
    //Have to be moved to SGAnimationSceneHelper
    Quaternion readRotationFromSGAFile(float rollValue,ifstream* filePointer);
    Vector3 getSelectedNodeScale();
    Vector4 getCameraPreviewLayout1();
    int undo(int &returnValue2);
    int redo();
};
#endif