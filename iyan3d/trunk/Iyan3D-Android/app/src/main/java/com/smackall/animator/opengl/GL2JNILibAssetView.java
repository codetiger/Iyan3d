package com.smackall.animator.opengl;


import com.smackall.animator.common.Constant;

/**
 * Created by shankarganesh on 22/9/15.
 */
public class GL2JNILibAssetView {
    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("iyan3d");
    }

    /**
     * @param width the current view width
     * @param height the current view height
     */
    public static native void initAssetView(int width, int height);
    public static native void stepAssetView();
    public static native void assetViewFinish();
    public static native void addAssetToScene( int assetId , int nodeType,String name);
    public static native void assetItemDisplay(int AssetId,String Assetname,int AssetType);
    public static native void assetSwipetoRotate(float x, float y);
    public static native void assetSwipetoEnd();
    public static native void setViewType(int viewtype);
    public static native void loadNodeAnimationSelection(int assetId,int assetType,String assetName);
    public static native void applyAnimationtoNode(int assetId);
    public static native int getBoneCount(int assetId);
    public static native void playAnimation();
    public static native void stopAllAnimations();
    public static native void remove3dText();
    public static native void load3DText(String displayText,int size,int bevelValue,int red,int blue,int green,String filename);
    public static native void add3DTextToScene();
    public static native void  applyAnimationToNodeAnimationEditor(String filePath);
    public static native void paBeginanim(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void panProgressanim(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void deletePreviewScene();
    public static native void deleteAssetView();
    public static native boolean isPreviewScene();

    public static void isRenderingFinishWithText(int isFinish){
        if(isFinish == 1)
            Constant.textAssetsSelectionView.isRederingFinishWithText = true;
    }

    public static void callBackAssetViewSurfaceCreated(){
            System.out.println("CallBack AssetView Rendered Working");
            GL2JNIViewAssetView.callBackSufaceCreated(true);
    }


}

