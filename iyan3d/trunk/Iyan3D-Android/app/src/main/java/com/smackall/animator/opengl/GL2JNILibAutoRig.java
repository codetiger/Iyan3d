package com.smackall.animator.opengl;

import com.smackall.animator.ImportAndRig.RigModel;
import com.smackall.animator.common.Constant;

/**
 * Created by shankarganesh on 20/10/15.
 */
public class GL2JNILibAutoRig {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("iyan3d");
    }
    public static native void initAutorig(int width, int height);
    public static native void stepAutoRig();
    public static native void addObjToScene(String objPath, String texturePath);
    public static native void panBeginAutorig(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void panProgress(float cordX1, float cordY1,float cordX2, float cordY2);
    public static native void swipeAutorig(float velocityX, float velocityY);
    public static native void tapAutorig(float x, float y);
    public static native void tapMoveAutorig(float x, float y);
    public static native void tapEndAutorig(float x, float y);
    public static native void rotateAction();
    public static native void moveAction();
    public static native void singleBone();
    public static native void humanRigging();
    public static native void controlSelectionAutoRig(float x ,float y);
    public static native void nextBtnAurorig(String filePath);
    public static native void scaleAutoRig(float x,boolean state);
    public static native void prevBtnAction();
    public static native void unodBtnAction();
    public static native void redoBtnAction();
    public static native float getSelectedNodeOrJointScaleAutoRig();
    public static native float scaleJointAutorig(float x, float y ,float z,boolean state);
    public static native void addNewJoint();
    public static native void deleteAutoRigScene();
    public static native boolean isAutoRigJointSelected();
    public static native boolean updateMirror();
    public static native float getSelectedJointScale();
    public static native float getAutoRigScalevalueX();
    public static native float getAutoRigScalevalueY();
    public static native float getAutoRigScalevalueZ();
    public static native int selectedNodeId();
    public static native int skeletonType();




    public static void sgrCreated()
    {
        Constant.rigModel.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.rigModel.progressUpdater();
            }
        });
    }

    public static void addJointBtnHandler(final String state)
    {
        Constant.rigModel.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //Constant.rigModel.addJointBtnHandler(Integer.parseInt(state));
            }
        });
    }

    public static void boneLimitCallback()
    {
        System.out.println("Bone Limit Called");
        Constant.rigModel.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.informDialog(Constant.rigModel, "The Maximum bones per object cannot exceed 57.");
            }
        });
    }

    public static void callBackGetSceneMode(final int sceneMode){
        Constant.rigModel.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.rigModel.addJointBtnHandler(sceneMode);
            }
        });
    }
    public static void undoOrRedoBtnUpdater(final String state){
        Constant.rigModel.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.rigModel.undoOrRedoBtnUpdater(state);
            }
        });
    }
}
