package com.smackall.animator.opengl;


import android.os.Environment;
import android.util.Log;
import android.view.View;

import com.smackall.animator.AnimationEditorView.ButtonHandler;
import com.smackall.animator.AnimationEditorView.UiUpdater;
import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.AssetsViewController.AssetsViewDatabaseHandler;
import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.common.Constant;

import java.io.File;
import java.util.List;

public class GL2JNILib {

    public static int selectedNodeId;
    public static int redoValue;
    public static int actiontype;
    public static int undoValue;
//    public static AssetsViewDatabaseHandler assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(Constant.animationEditor.context);



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



    public static void updateSelectednodeType(String s){
        Log.d("Native Type", String.valueOf(s));
        Constant.animationEditor.selectedNodeType = Integer.parseInt(String.valueOf(s));
    }

    public static void updateSelectednodeId(String s){
        Log.d("Native ID", String.valueOf(s));
        Constant.animationEditor.selectedNodeId = Integer.parseInt(String.valueOf(s));
    }

    public static void ButtonHandler(String s){
        Log.d("Native Type", String.valueOf(s));
        Constant.animationEditor.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ButtonHandler.buttonHanler();
            }
        });
    }

    public static void redoValue(String s){
        redoValue = Integer.parseInt(s);
        System.out.println("Redo Value : " + redoValue);
        if(redoValue==0){
            System.out.println("Action Type: DO_NOTHING ");
        }
        else if (redoValue == 1){
            System.out.println("Action Type: DELETE_ASSET :");
            GL2JNILib.undoRedoDeleteObject();
        }
        else if(redoValue==7){
            System.out.println("Action Type: ADD_TEXT_IMAGE_BACK, " );
        }
        else if (redoValue == 10){
            System.out.println("Action Type: SWITCH_MIRROR");
        }
        else if(redoValue > 900 && redoValue < 1000){
            System.out.println("Light Redo");
            GL2JNILib.addAssetToScenenAnimationEditor(redoValue, 7, 2,"light");
        }
        else if(redoValue == 4 ){

        }
        else if(redoValue == 8){
            System.out.println("SWITCH_FRAME " + GL2JNILib.getCurrentFrame());
            switchFrameUI(GL2JNILib.getCurrentFrame());
        }
        else{
            List<AssetsDB> assetsDBs = null;
            if (redoValue >= 20000 && redoValue < 30000 || redoValue >= 40000 && redoValue <50000 ) {
                assetsDBs = Constant.getAssetsDetail(Constant.ASSETS_DB.KEY_ASSETSID.getValue(),
                        Integer.toString(redoValue),
                        Constant.ASSETS_DB.DATABASE_NAME.getValue(),
                        Constant.ASSETS_DB.MY_LIBRARY_TABLE.getValue());
            }
            else {
                assetsDBs = Constant.getAssetsDetail(Constant.ASSETS_DB.KEY_ASSETSID.getValue(),
                        Integer.toString(redoValue),
                        Constant.ASSETS_DB.DATABASE_NAME.getValue(),
                        Constant.ASSETS_DB.ASSETS_TABLE.getValue());
            }
            String selectedAssetType=assetsDBs.get(0).getType();
            String selectedAssetName=assetsDBs.get(0).getAssetName();
            GL2JNILib.addAssetToScenenAnimationEditor(redoValue, Integer.parseInt(selectedAssetType), 2,selectedAssetName);
        }
    }

    public static void updateFrames(String framesStr) {
        System.out.println("Update Frames : " + framesStr);
        int totalFrames = Integer.parseInt(framesStr);
        Constant.animationEditor.frameCount = totalFrames;
        Constant.animationEditor.animationFrameCount = totalFrames;
        Constant.animationEditor.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.animationEditor.gridViewHorizontally(0);
            }
        });
    }

    public static void undoValue(String s1,String s2){
        undoValue=Integer.parseInt(s1);
        actiontype=Integer.parseInt(s2);
        switch (actiontype){
            case 0:
                System.out.println("Action Type: DO_NOTHING");
                break;
            case 1:
                System.out.println("Action Type: DELETE_ASSET :" + GL2JNILib.undoValue);
                int nodeIndex=GL2JNILib.undoValue;
                if(nodeIndex==-1|| nodeIndex==0||nodeIndex==1){
                    System.out.println("Selected node id problem");
                    break;
                }
                GL2JNILib.deleObejectBtNodeId(nodeIndex);
                break;
            case 2:
                System.out.println("Action Type:  ADD_ASSET_BACK " + GL2JNILib.getReturnValue());
                System.out.println("Undo Value : " + undoValue);

                List<AssetsDB> assetsDBs = null;
                if(undoValue > 900 && undoValue < 1000){
                    System.out.println("Light Redo");
                    GL2JNILib.addAssetToScenenAnimationEditor(undoValue, 7, 1,"light");
                }
                else if (undoValue >= 20000 && undoValue < 30000 || undoValue >= 40000 && undoValue <50000 ) {
                    assetsDBs = Constant.getAssetsDetail(Constant.ASSETS_DB.KEY_ASSETSID.getValue(),
                            Integer.toString(undoValue),
                            Constant.ASSETS_DB.DATABASE_NAME.getValue(),
                            Constant.ASSETS_DB.MY_LIBRARY_TABLE.getValue());
                    String selectedAssetType=assetsDBs.get(0).getType();
                    GL2JNILib.addAssetToScenenAnimationEditor(GL2JNILib.getReturnValue(), Integer.parseInt(selectedAssetType), 1,assetsDBs.get(0).getAssetName());
                }
                else {
                    assetsDBs = Constant.getAssetsDetail(Constant.ASSETS_DB.KEY_ASSETSID.getValue(),
                            Integer.toString(undoValue),
                            Constant.ASSETS_DB.DATABASE_NAME.getValue(),
                            Constant.ASSETS_DB.ASSETS_TABLE.getValue());
                    String selectedAssetType=assetsDBs.get(0).getType();
                    GL2JNILib.addAssetToScenenAnimationEditor(GL2JNILib.getReturnValue(), Integer.parseInt(selectedAssetType), 1,assetsDBs.get(0).getAssetName());
                }

                break;
            case 3:
                System.out.println("Action Type: DEACTIVATE_UNDO ");
                break;
            case 4:
                System.out.println("Action Type:DEACTIVATE_REDO ");

                break;
            case 5:
                System.out.println("Action Type:DEACTIVATE_BOTH");
                break;
            case 6:
                System.out.println("Action Type: ACTIVATE_BOTH ");
                break;
            case 7:
                System.out.println("Action Type: ADD_TEXT_IMAGE_BACK, "+GL2JNILib.addTextImageBack() );
                break;
            case 8:
                System.out.println("Action Type: SWITCH_FRAME :" + GL2JNILib.getCurrentFrame());
                 switchFrameUI(GL2JNILib.getCurrentFrame());
                break;
            case 9:
                System.out.println("Action Type: RELOAD_FRAMES ");

                break;
            case 10:
                System.out.println("Action Type: SWITCH_MIRROR");

                break;
        }
    }


    public static void addAssetBack(String s){
        int assetid=Integer.parseInt(s);
        List<AssetsDB> assetsDBs = Constant.getAssetsDetail("assetsId", Integer.toString(assetid), "assets", "Assets");
        String selectedAssetType=assetsDBs.get(0).getType();
        String selectedAssetName=assetsDBs.get(0).getAssetName();
        GL2JNILib.addAssetToScenenAnimationEditor(assetid, Integer.parseInt(selectedAssetType), 2,selectedAssetName);
        Log.d("Native Type", String.valueOf(s));
    }

    public static void undoRedoButtonUpdate(int state){
        Constant.gl2JNILib.undoRedoButtonState(state);
    }

    public static void mirrorStateUpdater(int state){
        Constant.gl2JNILib.mirrorButtonState(state);
    }

    public static void saveFinish(){
        Constant.animationEditor.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.animationEditor.showOrHideSaveProgress("saveFinish");
            }
        });
    }

        public void undoRedoButtonState(final int state){
            Constant.animationEditor.activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Constant.animationEditor.changeUndoRedoButtonState(state);
                }
            });
        }

    public void mirrorButtonState(final int jointSize){
        Constant.animationEditor.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Constant.animationEditor.jointSize=jointSize;
                if(jointSize == 54)
                    Constant.animationEditor.mirror_btn.setVisibility(View.VISIBLE);
                else
                    Constant.animationEditor.mirror_btn.setVisibility(View.INVISIBLE);
            }
        });
    }

    public static void switchFrameUI(final int currentFrame){
        Constant.animationEditor.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                UiUpdater.switchFrameUI(currentFrame, Constant.animationEditor.context);
            }
        });
    }

    public static void callBackIsRenderedUpdater(){
        GL2JNIView.callBackSurfaceRendered(true);
    }

    public static void downloadCallBack(String filename, String nodeType){

        int nodeTypeInt = Integer.parseInt(nodeType);
        System.out.println("File Name : " + filename);
        File cacheDir = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/.cache/");
        Constant.deleteDir(cacheDir);
        Constant.mkDir(cacheDir);
        if(nodeTypeInt == Constant.NODE_TYPE.NODE_SGM.getValue()){
            System.out.println("Sgm File Name : " + filename);
            String CheckFile = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/mesh/";
            if((new File(CheckFile+filename+".sgm").exists() && new File(CheckFile+filename+"-cm.png").exists()))
                return;
            String urlMesh = "http://iyan3dapp.com/appapi/mesh/"+filename+".sgm";
            String urlTexture = "http://iyan3dapp.com/appapi/meshtexture/"+filename+".png";
            String downloadPath =Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/.cache/";
            Constant.downloadCallBack.downloadCallBack(Constant.animationEditor.context,urlMesh,filename+".sgm",downloadPath,nodeTypeInt);
            Constant.downloadCallBack.downloadCallBack(Constant.animationEditor.context,urlTexture,filename+"-cm.png",downloadPath,nodeTypeInt);
        }
        else if(nodeTypeInt == Constant.NODE_TYPE.NODE_RIG.getValue()){
            System.out.println("Rig File Name : " + filename);
            String CheckFile = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/mesh/";
            if((new File(CheckFile+filename+".sgr").exists() && new File(CheckFile+filename+"-cm.png").exists()))
                return;
            String urlMesh = "http://iyan3dapp.com/appapi/mesh/"+filename+".sgr";
            String urlTexture = "http://iyan3dapp.com/appapi/meshtexture/"+filename+".png";
            String downloadPath =Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/.cache/";
            Constant.downloadCallBack.downloadCallBack(Constant.animationEditor.context,urlMesh,filename+".sgr",downloadPath,nodeTypeInt);
            Constant.downloadCallBack.downloadCallBack(Constant.animationEditor.context,urlTexture,filename+"-cm.png",downloadPath,nodeTypeInt);
        }
        else if(nodeTypeInt == Constant.NODE_TYPE.NODE_OBJ.getValue()){
            System.out.println("Obj File Name : " + filename);
            String CheckFile = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/mesh/";
            if((new File(CheckFile+filename+".obj").exists() && new File(CheckFile+filename+"-cm.png").exists()))
                return;
            Constant.animationEditor.activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    if(Constant.informDialog == null)
                    Constant.informDialog(Constant.animationEditor.context, "Some resources files are missing.");
                }
            });
            return;
        }
        else if(nodeTypeInt == Constant.NODE_TYPE.NODE_TEXT.getValue()){
            System.out.println("Font File Name : " + filename);
            AssetsViewDatabaseHandler assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(Constant.animationEditor.context);
            System.out.println("Database Size : " + assetsViewDatabaseHandler.getAssetsCount(Constant.ASSETS_DB.ASSETS_TABLE.getValue()));
            List<AssetsDB> assetsDBs =  Constant.getAssetsDetail(Constant.ASSETS_DB.KEY_ASSET_NAME.getValue(), filename, Constant.ASSETS_DB.DATABASE_NAME.getValue(), Constant.ASSETS_DB.ASSETS_TABLE.getValue());
            System.out.println("Database After Size : " + assetsDBs.size());

            if(assetsDBs.size() <= 0){
                Constant.animationEditor.activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(Constant.informDialog == null)
                        Constant.informDialog(Constant.animationEditor.context,"Please connect with internet some resources files are missing need to download it.");
                    }
                });
            assetsViewDatabaseHandler = null;
                return;
            }

            String fontNameWithExec = assetsDBs.get(0).getAssetName().substring(assetsDBs.get(0).getAssetName().length() - 4);
            System.out.println("Font Id : " + assetsDBs.get(0).getAssetsId()+fontNameWithExec);

            String CheckFile = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/fonts/";
            if((new File(CheckFile+filename).exists())) {
                assetsViewDatabaseHandler = null;
                return;
            }
            String CheckMyFontFile = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/fonts/userFonts/";
            if((new File(CheckMyFontFile+filename).exists())) {
                assetsViewDatabaseHandler = null;
                return;
            }
            String url = "http://iyan3dapp.com/appapi/font/"+assetsDBs.get(0).getAssetsId()+fontNameWithExec;
            String downloadPath =Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/.cache/";
            File fontFile = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/fonts/"+filename);
            if(fontFile.exists()) {
                assetsViewDatabaseHandler = null;
                return;
            }
            System.out.println("File Name Before Download : " + filename);
            Constant.downloadCallBack.downloadCallBack(Constant.animationEditor.context,url,filename,downloadPath,nodeTypeInt);
            if(assetsViewDatabaseHandler != null)
            assetsViewDatabaseHandler = null;
        }
        else if(nodeTypeInt == Constant.NODE_TYPE.NODE_IMAGE.getValue()){
            System.out.println("Image File Name : " + filename);
            String userData = Environment.getExternalStorageDirectory()+"/Android/data/"+Constant.animationEditor.context.getPackageName()+"/importedImages/";
            if((new File(userData+filename).exists()))
                return;
            Constant.animationEditor.activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(Constant.informDialog == null)
                    Constant.informDialog(Constant.animationEditor.context, "Some resources files are missing.");
                }
            });
            return;
        }
    }
}
