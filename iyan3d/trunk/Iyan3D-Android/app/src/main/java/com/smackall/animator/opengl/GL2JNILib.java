package com.smackall.animator.opengl;

import android.content.Context;

import com.smackall.animator.NativeCallBackClasses.NativeCallBacks;
import com.smackall.animator.NativeCallBackClasses.VideoManager;

public class GL2JNILib {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("iyan3d");
    }

    /**
     * @param width the current view width
     * @param height the current view height
     */

    Context mContext;
    public void setGl2JNIView(Context context)
    {
        this.mContext= context;
    }

    public static native String Mesh();
    public static native String Texture();
    public static native String MeshThumbnail();
    public static native String Font();
    public static native String AnimationThumbnail();
    public static native String Particle();
    public static native String Animation();
    public static native String Credits();
    public static native String UseOrRecharge();
    public static native String CheckProgress();
    public static native String TaskDownload();
    public static native String verifyRestorePurchase();
    public static native String RenderTask();
    public static native String PublishAnimation();


    public static native int init(int width, int height,boolean addVAOSupport,int uniformValue);
    public static native void step();
    public static native void loadScene(NativeCallBacks nativeCallBacks,String filepath);
    public static native void swipe(float velocityX, float velocityY);
    public static native void panBegin(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void panProgress(NativeCallBacks nativeCallBacks,float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void tap(float x, float y,boolean isMultiSelect);
    public static native void controlSelection(float x ,float y,boolean isMultiSelect);
    public static native boolean save(NativeCallBacks nativeCallBacks,boolean forCloudRendering, String filepath);
    public static native void tapMove(NativeCallBacks nativeCallBacks,float x, float y);
    public static native void  tapEnd(float x, float y);
    public static native void touchBegan(float x,float y);
    public static native void loadFile(String filePath);
    public static native void dealloc();
    public static native void setAssetspath(String assetPath,String setMeshpath,String Imagepath);
    public static native int getSelectedNodeId();
    public static native void importAsset(int assetAddType,int assetId,String assetName, String textureName,int width,int height,boolean isTempNode,float x,float y, float z,int assetActionType);
    public static native void removeTempNode();
    public static native int getNodeCount();
    public static native int getNodeType(int nodeId);
    public static native String getNodeName(int nodeId);
    public static native void selectNode(int nodeId, boolean isMultiSelect);
    public static native void removeNode(int nodeId, boolean isUndoRedo);
    public static native void copyPropsOfNode(int from,int to,boolean excludeKeys);
    public static native int getAssetId();
    public static native int getAssetIdWithNodeId(int nodeId);
    public static native String getTexture(int nodeId);
    public static native float getVertexColorX();
    public static native float getVertexColorY();
    public static native float getVertexColorZ();
    public static native float getVertexColorXWithId(int nodeId);
    public static native float getVertexColorYWithId(int nodeId);
    public static native float getVertexColorZWithId(int nodeId);
    public static native String optionalFilePath();
    public static native String optionalFilePathWithId(int id);
    public static native float nodeSpecificFloat();
    public static native float nodeSpecificFloatWithId(int nodeId);
    public static native int getFontSize();
    public static native int getFontSizeWithId(int nodeId);
    public static native float refractionValue();
    public static native float reflectionValue();
    public static native boolean isLightning();
    public static native boolean isVisible();
    public static native int jointSize(int selectedNode);
    public static native boolean mirrorState();
    public static native float cameraFov();
    public static native int resolutionType();
    public static native float lightx();
    public static native float lighty();
    public static native float lightz();
    public static native float shadowDensity();
    public static native void changeMeshProperty(float refraction,float reflection, boolean light,boolean visible,boolean storeAction);
    public static native void switchMirror();
    public static native boolean getRigMirrorState();
    public static native void changeLightProperty(float x, float y, float z, float w,float distance,int lightType,boolean storeAction);
    public static native void previewPosition(int position,int previewSize,float topHeight,float toolBarWidth);
    public static native void switchFrame();
    public static native void setCurrentFrame(int currentFrame,NativeCallBacks nativeCallBacks);
    public static native void stopPlaying();
    public static native boolean play(NativeCallBacks nativeCallBacks);
    public static native int totalFrame();
    public static native void addFrame(int frame);
    public static native int currentFrame();
    public static native boolean isPlaying();
    public static native void setIsPlaying(boolean isPlaying,NativeCallBacks nativeCallBacks);
    public static native void cameraPositionViaToolBarPosition(int selectedIndex,float rightWidth,float topHeight);
    public static native void cameraPropertyChanged(int fov,int resolution,boolean storeAction);
    public static native boolean loadText(float red,float green,float blue,int typeOfNode,String textureName,String assetName,int fontSize,int bevalValue,int assetAddType,String filePath,boolean isTempNode);
    public static native boolean importImageOrVideo(int nodeType,String name,int width,int height,int assetAddType,boolean isTempNode);
    public static native void importAdditionalLight(int lightCount,int action);
    public static native int lightCount();
    public static native void saveAsSGM(String FileName,String textureName,int assetId,boolean haveTexture,float x,float y,float z);
    public static native void changeTexture(int selectedNodeId,String textureName,float x,float y,float z,boolean isTemp);
    public static native void removeTempTexture(int selectedNodeId);
    public static native boolean canEditRigBones();
    public static native void beginRigging();
    public static native void cancelRig(boolean completed);
    public static native boolean changeEnvelopScale(float x);
    public static native void addJoint(NativeCallBacks nativeCallBacks);
    public static native void switchRigSceneMode(NativeCallBacks nativeCallBacks,int scene);
    public static native void setSkeletonType(int type);
    public static native void exportSgr();
    public static native void unselectObjects();
    public static native void unselectObject(int nodeId);
    public static native void hideNode(int nodeId,boolean hide);
    public static native void copyKeysOfNode(int fromNodeId,int toNodeId);
    public static native int getBoneCount(int nodeId);
    public static native void setTotalFrame(int frame);
    public static native void applyAnimation(int fromNodeI,int applyedNodeId ,String path);
    public static native boolean scale(NativeCallBacks nativeCallBacks);
    public static native float scaleValueX();
    public static native float scaleValueY();
    public static native float scaleValueZ();
    public static native float envelopScale();
    public static native void setScaleValue(float x, float y, float z,boolean store);
    public static native void move(NativeCallBacks nativeCallBacks);
    public static native void rotate(NativeCallBacks nativeCallBacks);
    public static native void rigNodeScale(float x, float y, float z);
    public static native boolean isRigMode();
    public static native boolean isVAOSupported();
    public static native boolean cameraView(int cameraView);
    public static native boolean isNodeSelected(int position);
    public static native boolean deleteAnimation();
    public static native boolean saveAnimation(NativeCallBacks nativeCallBacks,int assetId,String name,int type);
    public static native boolean isJointSelected();
    public static native boolean isHaveKey(int currentFrame);
    public static native boolean hasNodeSelected();
    public static native boolean editorScene();
    public static native int undo(NativeCallBacks nativeCallBacks);
    public static native int redo(NativeCallBacks nativeCallBacks);
    public static native void decreaseCurrentAction();
    public static native int objectIndex();
    public static native void increaseCurrentAction();
    public static native void renderFrame(NativeCallBacks nativeCallBacks,int frame,int shader,boolean isImage,boolean waterMark, float x, float y, float z);
    public static native boolean perVertexColor(int id);
    public static native void initVideoManagerClass(VideoManager videoManager);
    public static native void setNodeLighting(int nodeId, boolean state);
    public static native void setNodeVisiblity(int nodeId, boolean state);
    public static native void setfreezeRendering(boolean state);
    public static native void setTransparency(float value,int nodeId);
    public static native boolean cloneSelectedAsset(int assetid,int nodeType, int nodeId,int assetAddType);
    public static native int selectedNodeIdsSize();
    public static native int getSelectedNodeIdAtIndex(int index);
    public static native void createDuplicateAssets(NativeCallBacks nativeCallBacks);
    public static native boolean isPhysicsEnabled(int nodeId);
    public static native int physicsType(int nodeId);
    public static native int velocity(int nodeId);
    public static native void enablePhysics(boolean status);
    public static native void setPhysicsType(int type);
    public static native void velocityChanged(int value);
    public static native void setDirection();
    public static native void removeSelectedObjects();
    public static native int getLightType();
    public static native void ADD_INSTANCE_BACK(int actionType);
    public static native void syncPhysicsWithWorld(int from, int to,boolean updatePhysics);
    public static native void updatePhysics(int frame);
    public static native float getSelectedJointScale();
    public static native int smoothTexState();
    public static native int smoothTexStateWithId(int nodeId);
    public static native void setTextureSmoothStatus(boolean state);

    public static void callBackIsDisplayPrepared(){
        GL2JNIView.callBackSurfaceRendered();
    }
}
