package com.smackall.animator.opengl;

public class GL2JNILib {

     static {

        System.loadLibrary("c++_shared");
        System.loadLibrary("iyan3d");
    }

    /**
     * @param width the current view width
     * @param height the current view height
     */

    public static native void init(int width, int height);
    public static native void step();
    public static native void swipe(float velocityX, float velocityY);
    public static native void paBegin(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void panProgress(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void tap(float x, float y);
    public static native void controlSelection(float x ,float y);
    public static native void setAssetspath(String assetPath,String setMeshpath,String Imagepath);
    public static native void moveBtn();
    public static native void rotateBtn();
    public static native void tapMove(float x, float y);
    public static native void  tapEnd(float x, float y);
    public static native int getSelectedNodeId();
    public static native int getAssetId();
    public static native int getAssetType();
    public static native float getScaleXval();
    public static native float getScaleYval();
    public static native float getScaleZval();
    public static native void scaleAction(float x ,float y,float z,boolean state );
    public static native void deleteObject();
    public static native void cameraTopView();
    public static native void cameraLeftView();
    public static native void cameraRightView();
    public static native void cameraBottomView();
    public static native void cameraFrontView();
    public static native void cameraBackView();
    public static native float getMeshBrightness();
    public static native float getMeshSpecular();
    public static native boolean getLightingSwitch();
    public static native boolean getVisibilitySwitch();
    public static native void meshPropertyChanged(float brightness,float specular,boolean lighting,boolean visibility);
    public static native float getCameraFov();
    public static native int getCameraResolution();
    public static native void cameraPropertyChanged(float fov , int resolution);
    public static native void frameClicked(int position);
    public static native void playAnimation();
    public static native void playTimer();
    public static native void stopAnimation();
    public static native int getAnimationFrameCount();
    public static native void selectObject(int position);
    public static native void saveAnimation(String projectFileName);
    public static native void saveThumbnail(String projectFileName);
    public static native int undoFunction();
    public static native int redoFunction();
    public static native int getReturnValue();
    public static native void deleObejectBtNodeId(int nodeid);
    public static native int getCurrentFrame();
    public static native void addFramebtn();
    public static native void addAssetToScenenAnimationEditor( int assetId , int nodeType,int assetAddType, String assetName);
    public static native String addTextImageBack();
    public static native void loadFile(String filePath);
    public static native int getAssetCount();
    public static native String getAssetList(int i);
    public static native void importImageAction(String imageName,int imageWidth,int imageHeight);
    public static native void renderFrame(int frame,int shaderType,boolean watermark,String path,int resolution);
    public static native boolean saveTemplate(String assetId);
    public static native void deleteScene();
    public static native int addExtraLight();
    public static native void changeLightProperty(float R, float G, float B, float shadow);
    public static native float getShadowDensity();
    public static native float getLightPropertiesRed();
    public static native float getLightPropertiesGreen();
    public static native float getLightPropertiesBlue();
    public static native void setLightingOff();
    public static native void setLightingOn();
    public static native void resetClickedAssetId();
    public static native void exportingCompleted();
    public static native void undoRedoDeleteObject();
    public static native void switchMirrorState();
    public static native boolean getMirrorState();
    public static native int getJointSize();
    public static native int getLightcount();
    public static native boolean deleteanimation();
    public static native boolean isKeySetForNode(int currentFrame);
    public static native boolean isJointSelected();
    public static native void storeShaderProps();
    public static native void resetContext();
}
