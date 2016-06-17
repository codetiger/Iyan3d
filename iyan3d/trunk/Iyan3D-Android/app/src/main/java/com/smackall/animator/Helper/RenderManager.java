package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.ViewGroup;

import com.smackall.animator.EditorView;
import com.smackall.animator.NativeCallBackClasses.NativeCallBacks;
import com.smackall.animator.R;
import com.smackall.animator.Rig;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.List;

/**
 * Created by Sabish.M on 15/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class RenderManager {
    Context mContext;
    private SharedPreferenceManager sp;
    public GLSurfaceView glView;
    boolean wait;
    public RenderManager(Context mContext,SharedPreferenceManager sp){
        this.mContext = mContext;
        this.sp = sp;
        this.glView = ((EditorView)((Activity)mContext)).glView;
    }

    public void selectObject(final int nodeId)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.selectNode(nodeId, (sp.getInt(mContext, "multiSelect") == 1));
            }
        });
        ((EditorView)((Activity)mContext)).reloadMyObjectList();
    }
    public void removeObject(final int nodeId, final boolean isUndoRedo)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeNode(nodeId, isUndoRedo);
            }
        });
        ((EditorView)((Activity)mContext)).reloadMyObjectList();
    }

    public void renderAll() {GL2JNILib.step();}

    public void panProgress(final MotionEvent event){
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (event.getPointerCount() > 1)
                        GL2JNILib.panProgress(((EditorView) (Activity) mContext).nativeCallBacks,event.getX(0), event.getY(0), event.getX(1), event.getY(1));
                }
                catch (IllegalArgumentException e){
                    e.printStackTrace();
                }
            }
        });
    }

    public void panBegin(final MotionEvent event){
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if(event.getPointerCount() > 1)
                    GL2JNILib.panBegin(event.getX(0), event.getY(0), event.getX(1), event.getY(1));
            }
        });
    }

    public void touchBegan(final MotionEvent event){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if(event.getPointerCount() == 1)
                    GL2JNILib.touchBegan(event.getX(), event.getY());
            }
        });
    }

    public void swipe(final VelocityTracker velocityTracker){
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.swipe(velocityTracker.getXVelocity(), velocityTracker.getYVelocity());
            }
        });
    }
    public void tapMove(final MotionEvent event)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.tapMove(((EditorView)mContext).nativeCallBacks,event.getX(), event.getY());
            }
        });
    }

    public void tapEnd(final MotionEvent event)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.tapEnd(event.getX(), event.getY());
                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(mContext != null && ((EditorView) ((Activity) mContext)) != null && ((EditorView) ((Activity) mContext)).frameAdapter != null)
                            ((EditorView) ((Activity) mContext)).frameAdapter.notifyDataSetChanged();
                    }
                });
            }
        });
    }
    public void checkControlSelection(final MotionEvent event)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.controlSelection(event.getX(), event.getY(), (sp.getInt(mContext, "multiSelect") == 1));
            }
        });
    }

    public void checktapposition(final MotionEvent event)
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                    GL2JNILib.tap(event.getX(), event.getY(), (sp.getInt(mContext, "multiSelect") == 1));

                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(((EditorView) ((Activity) mContext)) != null && ((EditorView) ((Activity) mContext)).frameAdapter!= null)
                            ((EditorView) ((Activity) mContext)).frameAdapter.notifyDataSetChanged();
                    }
                });
            }
        });
    }

    public void removeTempNode()
    {
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeTempNode();
            }
        });
    }
    public void importAssets(final AssetsDB assetsDB, final boolean waitUntilCompleted)
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(),assetsDB.getX(),assetsDB.getY(),assetsDB.getZ() ,assetsDB.getActionType());
                ((EditorView)((Activity)mContext)).showOrHideLoading(Constants.HIDE);
                wait = false;
            }
        });
    }

    public void importText(final TextDB textDB){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.loadText(textDB.getRed(), textDB.getGreen(), textDB.getBlue(), textDB.getTypeOfNode(), textDB.getTextureName(), textDB.getAssetName()
                        , textDB.getFontSize(), textDB.getBevalValue(), textDB.getActionType(), textDB.getFilePath(), textDB.getTempNode());
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void importImage(final  ImageDB imageDB){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.importImageOrVideo(imageDB.getNodeType(), imageDB.getName(), imageDB.getWidth(), imageDB.getHeight(), imageDB.getActionType(), imageDB.getIsTempNode());
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);

            }
        });
    }

    public void importLight(final int lightId, final int action){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.importAdditionalLight(lightId, action);
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);

            }
        });
    }

    public void changeTexture(final int selectedNodeId, final String textureName, final float x, final float y, final float z, final boolean isTemp){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeTexture(selectedNodeId, textureName, x, y, z, isTemp);
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void removeTempTexture(final int selectedNodeId){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeTempTexture(selectedNodeId);
            }
        });
    }

    public void setCurrentFrame(final int frame, final NativeCallBacks nativeCallBacks){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setCurrentFrame(frame,nativeCallBacks);
            }
        });
    }

    public void cameraPosition(final float statusBarHeight)
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ViewGroup insertPointParent = (sp.getInt(mContext,"toolbarPosition") == 1 ) ? (ViewGroup)((ViewGroup) ((Activity)mContext).findViewById(R.id.rightView)).getParent()
                        : (ViewGroup)((ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)).getParent();
                ViewGroup insertPoint = null;
                for(int i = 0; i < insertPointParent.getChildCount(); i++){
                    if(insertPointParent.getChildAt(i).getTag() != null && insertPointParent.getChildAt(i).getTag().toString().equals("-1")) {
                        insertPoint = (ViewGroup) insertPointParent.getChildAt(i);
                    }
                }
                if(insertPoint != null) {
                    GL2JNILib.cameraPositionViaToolBarPosition((sp.getInt(mContext, "toolbarPosition")), insertPoint.getWidth(), ((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight()+statusBarHeight);
                    GL2JNILib.previewPosition(sp.getInt(mContext, "previewPosition"), (sp.getInt(mContext, "previewSize") == 0) ? 1 : 2,((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight()+statusBarHeight,insertPoint.getWidth());
                }
            }
        });
    }

    public void saveAsSGM(final String FileName, final String textureName, final int tempName, final boolean haveTexture, final float x, final float y, final float z){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.saveAsSGM(FileName, textureName, tempName, haveTexture, x, y, z);
            }
        });
    }

    public void cameraProperty(final int fov, final int resolution, final boolean storeAction){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraPropertyChanged(fov, resolution, storeAction);
            }
        });
    }

    public void beginRigging()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.beginRigging();
                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((EditorView) ((Activity) mContext)).rig.switchSceneMode(1);
                        ((EditorView) ((Activity) mContext)).rig.currentSceneMode += 1;
                        ((EditorView) ((Activity) mContext)).rig.updateText();
                    }
                });

            }
        });
    }

    public void cancelRig(final boolean completed){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cancelRig(completed);
            }
        });
    }
    public void switchSceneMode(final Rig rig,final int sceneMode){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.switchRigSceneMode(((EditorView)mContext).nativeCallBacks, sceneMode);
            }
        });
    }
    public void addJoint(final Rig rig){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.addJoint(((EditorView)mContext).nativeCallBacks);
            }
        });
    }
    public void setSkeletonType(final int type){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setSkeletonType(type);
            }
        });
    }
    public void unselectObjects()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.unselectObjects();
            }
        });
    }
    public void hideNode(final int nodeId, final boolean hide){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.hideNode(nodeId, hide);
            }
        });
    }
    public void unselectObject(final int nodeId){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.unselectObject(nodeId);
            }
        });
    }

    public void setScale(final float x, final float y, final float z, final boolean store){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setScaleValue(x, y, z,store);
            }
        });
    }
    public void setRigScale(final float x, final float y, final float z){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.rigNodeScale(x, y, z);
            }
        });
    }
    public void changeEnvelopScale(final float x){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeEnvelopScale(x);
            }
        });
    }
    public void setMove()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.move(((EditorView) (Activity) mContext).nativeCallBacks);
            }
        });
    }
    public void setRotate()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.rotate(((EditorView) (Activity) mContext).nativeCallBacks);
            }
        });
    }
    public void cameraView(final int cameraView){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.cameraView(cameraView);
            }
        });
    }

    public void changeMeshProps(final float refraction, final float reflection, final boolean light, final boolean visible, final boolean storeAction){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeMeshProperty(refraction,reflection,light,visible,storeAction);
                ((EditorView)mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }

    public void createDuplicate()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.createDuplicateAssets(((EditorView)mContext).nativeCallBacks);
                ((EditorView) (Activity) mContext).showOrHideLoading(Constants.HIDE);
            }
        });
    }


    public void cloneSelectedAsset(int selectedAssetId, int selectedNodeType, final int selectedNodeId)
    {
        if (selectedNodeType ==  Constants.NODE_SGM || selectedNodeType ==  Constants.NODE_OBJ) {
            boolean status = GL2JNILib.cloneSelectedAsset(selectedAssetId,selectedNodeType,selectedNodeId,Constants.IMPORT_ASSET_ACTION);
            if(!status)
                UIHelper.informDialog(mContext,"Copy limit exceeded. Import the object using 'Add' button on ToolBar.");

        } else if((selectedNodeType == Constants.NODE_RIG || selectedNodeType == Constants.NODE_PARTICLES) && selectedAssetId != Constants.NOT_SELECTED)
        {
            List<AssetsDB> assetsDBs = null;
            assetsDBs = ((EditorView)mContext).db.getModelWithAssetId(selectedAssetId);
            if(assetsDBs == null || assetsDBs.size() <= 0)
                assetsDBs = ((EditorView)mContext).db.getMyModelWithAssetId(selectedAssetId);
            AssetsDB assetsDB = null;
            if(assetsDBs != null && assetsDBs.size() > 0) {
                assetsDB = assetsDBs.get(0);
                assetsDB.setTexture(GL2JNILib.getTexture(selectedNodeId));
                assetsDB.setx(GL2JNILib.getVertexColorXWithId(selectedNodeId));
                assetsDB.setY(GL2JNILib.getVertexColorYWithId(selectedNodeId));
                assetsDB.setZ(GL2JNILib.getVertexColorZWithId(selectedNodeId));
                assetsDB.setIsTempNode(false);
                importAssets(assetsDB,false);
                GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount()-1, true);
            }
        }
        else if((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedNodeId != Constants.NOT_SELECTED){
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
            TextDB textDB = new TextDB();
            textDB.setAssetName(GL2JNILib.getNodeName(selectedNodeId));
            textDB.setFilePath(GL2JNILib.optionalFilePathWithId(selectedNodeId));
            textDB.setRed(GL2JNILib.getVertexColorXWithId(selectedNodeId));
            textDB.setGreen(GL2JNILib.getVertexColorYWithId(selectedNodeId));
            textDB.setBlue(GL2JNILib.getVertexColorZWithId(selectedNodeId));
            textDB.setBevalValue((int) GL2JNILib.nodeSpecificFloatWithId(selectedNodeId));
            textDB.setFontSize(GL2JNILib.getFontSizeWithId(selectedNodeId));
            textDB.setTextureName(GL2JNILib.getTexture(selectedNodeId));
            textDB.setTypeOfNode(GL2JNILib.getNodeType(selectedNodeId));
            textDB.setTempNode(false);
            GL2JNILib.loadText(textDB.getRed(), textDB.getGreen(), textDB.getBlue(), textDB.getTypeOfNode(), textDB.getTextureName(), textDB.getAssetName()
                    , textDB.getFontSize(), textDB.getBevalValue(), textDB.getActionType(), textDB.getFilePath(), textDB.getTempNode());
            GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount()-1,false);
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);

        } else if ((selectedNodeType == Constants.NODE_IMAGE || selectedNodeType == Constants.NODE_VIDEO)) {
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
            String name = GL2JNILib.getNodeName(selectedNodeId);
            int imgW = (int) GL2JNILib.getVertexColorXWithId(selectedNodeId);
            int imgH = (int) GL2JNILib.getVertexColorYWithId(selectedNodeId);
            final ImageDB imageDB = new ImageDB();
            imageDB.setNodeType((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_IMAGE) ? Constants.NODE_IMAGE : Constants.NODE_VIDEO);
            imageDB.setName(name);
            imageDB.setWidth(imgW);
            imageDB.setHeight(imgH);
            imageDB.setAssetAddType(0);
            imageDB.setTempNode(false);
            GL2JNILib.importImageOrVideo(imageDB.getNodeType(), imageDB.getName(), imageDB.getWidth(), imageDB.getHeight(), imageDB.getActionType(), imageDB.getIsTempNode());
            GL2JNILib.copyPropsOfNode(selectedNodeId,GL2JNILib.getNodeCount()-1,false);
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
        }
    }
}
