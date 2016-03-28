package com.smackall.animator.opengl;

import android.content.Context;

import com.smackall.animator.Export;
import com.smackall.animator.Helper.MissingAssetHandler;
import com.smackall.animator.Rig;
import com.smackall.animator.Save;
import com.smackall.animator.UndoRedo;

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

    public static native void init(int width, int height);
    public static native void step();
    public static native void loadScene(MissingAssetHandler missingAssetHandler,String filepath);
    public static native void swipe(float velocityX, float velocityY);
    public static native void panBegin(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void panProgress(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void tap(float x, float y,boolean isMultiSelect);
    public static native void controlSelection(float x ,float y,boolean isMultiSelect);
    public static native boolean save(String filepath);
    public static native void tapMove(float x, float y);
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
    public static native void copyPropsOfNode(int from,int to);
    public static native int getAssetId();
    public static native int getAssetIdWithNodeId(int nodeId);
    public static native String getTexture(int nodeId);
    public static native float getVertexColorX();
    public static native float getVertexColorY();
    public static native float getVertexColorZ();
    public static native String optionalFilePath();
    public static native float nodeSpecificFloat();
    public static native int getFontSize();
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
    public static native void changeLightProperty(float x, float y, float z, float w,float distance,boolean storeAction);
    public static native void previewPosition(int position,int previewSize,float topHeight,float toolBarWidth);
    public static native void switchFrame();
    public static native void setCurrentFrame(int currentFrame);
    public static native void stopPlaying();
    public static native void play();
    public static native int totalFrame();
    public static native void addFrame(int frame);
    public static native int currentFrame();
    public static native boolean isPlaying();
    public static native void setIsPlaying(boolean isPlaying);
    public static native void cameraPositionViaToolBarPosition(int selectedIndex,float rightWidth,float topHeight);
    public static native void cameraPropertyChanged(int fov,int resolution,boolean storeAction);
    public static native boolean loadText(float red,float green,float blue,int typeOfNode,String textureName,String assetName,int fontSize,int bevalValue,int assetAddType,String filePath,boolean isTempNode);
    public static native boolean importImageOrVideo(int nodeType,String name,int width,int height,int assetAddType,boolean isTempNode);
    public static native void importAdditionalLight(int lightCount,int action);
    public static native int lightCount();
    public static native void saveAsSGM(String FileName,String textureName,int assetId,boolean haveTexture,float x,float y,float z);
    public static native void changeTexture(int selectedNodeId,String textureName,float x,float y,float z,boolean isTemp);
    public static native void removeTempTexture();
    public static native boolean canEditRigBones();
    public static native void beginRigging();
    public static native void cancelRig(boolean completed);
    public static native void addJoint(Rig rig);
    public static native void switchRigSceneMode(Rig rig,int scene);
    public static native void setSkeletonType(int type);
    public static native void exportSgr();
    public static native void unselectObjects();
    public static native void unselectObject(int nodeId);
    public static native void hideNode(int nodeId,boolean hide);
    public static native void copyKeysOfNode(int fromNodeId,int toNodeId);
    public static native int getBoneCount(int nodeId);
    public static native void setTotalFrame(int frame);
    public static native void applyAnimation(int fromNodeI,int applyedNodeId ,String path);
    public static native boolean scale();
    public static native float scaleValueX();
    public static native float scaleValueY();
    public static native float scaleValueZ();
    public static native float envelopScale();
    public static native void setScaleValue(float x, float y, float z);
    public static native void move();
    public static native void rotate();
    public static native void rigNodeScale(float x, float y, float z);
    public static native boolean isRigMode();
    public static native boolean cameraView(int cameraView);
    public static native boolean isNodeSelected(int position);
    public static native boolean deleteAnimation();
    public static native boolean saveAnimation(Save save,int assetId,String name,int type);
    public static native boolean isJointSelected();
    public static native boolean isHaveKey(int currentFrame);
    public static native boolean hasNodeSelected();
    public static native boolean editorScene();
    public static native int undo(UndoRedo undoRedo);
    public static native int redo(UndoRedo undoRedo);
    public static native void decreaseCurrentAction();
    public static native int objectIndex();
    public static native void increaseCurrentAction();
    public static native void renderFrame(Export export,int frame,int shader,boolean isImage,boolean waterMark, float x, float y, float z);


    public static void callBackIsDisplayPrepared(){
        GL2JNIView.callBackSurfaceRendered();
    }
}
