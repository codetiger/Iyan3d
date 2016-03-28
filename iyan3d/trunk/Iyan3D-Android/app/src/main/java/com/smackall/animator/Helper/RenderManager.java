package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.ViewGroup;

import com.smackall.animator.EditorView;
import com.smackall.animator.R;
import com.smackall.animator.Rig;
import com.smackall.animator.opengl.GL2JNILib;

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
                if (event.getPointerCount() > 1)
                    GL2JNILib.panProgress(event.getX(0), event.getY(0), event.getX(1), event.getY(1));
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
                GL2JNILib.tapMove(event.getRawX(), event.getRawY());
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
                if(event.getSource() == Constants.LONG_PRESS)
                    ((Activity)mContext).runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ((EditorView) ((Activity) mContext)).popUpManager.initPopUpManager(GL2JNILib.getSelectedNodeId(),null, event);
                        }
                    });
                ((Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((EditorView)((Activity)mContext)).frameAdapter.notifyDataSetChanged();
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
            }
        });
    }

    public void importImage(final  ImageDB imageDB){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.importImageOrVideo(imageDB.getNodeType(), imageDB.getName(), imageDB.getWidth(), imageDB.getHeight(), imageDB.getActionType(), imageDB.getIsTempNode());
            }
        });
    }

    public void importLight(final int lightId, final int action){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.importAdditionalLight(lightId,action);
            }
        });
    }

    public void changeTexture(final int selectedNodeId, final String textureName, final float x, final float y, final float z, final boolean isTemp){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeTexture(selectedNodeId, textureName, x, y, z, isTemp);
            }
        });
    }

    public void removeTempTexture(){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.removeTempTexture();
            }
        });
    }

    public void setCurrentFrame(final int frame){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setCurrentFrame(frame);
            }
        });
    }

    public void cameraPosition()
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
                    GL2JNILib.cameraPositionViaToolBarPosition((sp.getInt(mContext, "toolbarPosition")), insertPoint.getWidth(), ((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight());
                    GL2JNILib.previewPosition(sp.getInt(mContext, "previewPosition"), (sp.getInt(mContext, "previewSize") == 0) ? 1 : 2,((Activity) mContext).findViewById(R.id.recycleViewHolder).getHeight(),insertPoint.getWidth());
                }
            }
        });
    }

    public void saveAsSGM(final String FileName, final String textureName, final int tempName, final boolean haveTexture, final float x, final float y, final float z){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.saveAsSGM(FileName, textureName, tempName, haveTexture, x, y, z);
//                AssetsDB assetsDB = new AssetsDB();
//                assetsDB.setIsTempNode(true);
//                assetsDB.setAssetName(FileName);
//                assetsDB.setAssetsId(tempName);
//                assetsDB.setType(Constants.NODE_SGM);
//                assetsDB.setTexture(textureName);
//                assetsDB.setx(x);
//                assetsDB.setY(y);
//                assetsDB.setZ(z);
//                importAssets(assetsDB);
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
                ((EditorView)((Activity)mContext)).rig.switchSceneMode(1);
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
                GL2JNILib.switchRigSceneMode(rig, sceneMode);
            }
        });
    }
    public void addJoint(final Rig rig){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.addJoint(rig);
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

    public void setScale(final float x, final float y, final float z){
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setScaleValue(x, y, z);
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
    public void setMove()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.move();
            }
        });
    }
    public void setRotate()
    {
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.rotate();
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

    public void createDuplicate()
    {
        int selectedAssetId  = -1;
        int selectedNodeType = Constants.NODE_UNDEFINED;
        if(GL2JNILib.getSelectedNodeId() != -1) {
            selectedAssetId = GL2JNILib.getAssetId();
            selectedNodeType = GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId());
        }

        if((selectedNodeType == Constants.NODE_RIG || selectedNodeType ==  Constants.NODE_SGM || selectedNodeType ==
                Constants.NODE_OBJ || selectedNodeType == Constants.NODE_PARTICLES) && selectedAssetId != -1)
        {
            AssetsDB assetsDB;
            if(selectedAssetId > 20000 && selectedAssetId <=30000 || selectedAssetId > 40000 && selectedAssetId <=50000)
                assetsDB = ((EditorView)(Activity)mContext).db.getMyModelWithAssetId(selectedAssetId).get(0);
            else
                assetsDB = ((EditorView)(Activity)mContext).db.getModelWithAssetId(selectedAssetId).get(0);
            assetsDB.texture = GL2JNILib.getTexture(GL2JNILib.getSelectedNodeId());
            assetsDB.isTempNode = false;
            importAssets(assetsDB,false);
        }
        else if((selectedNodeType == Constants.NODE_IMAGE || selectedNodeType == Constants.NODE_VIDEO) && selectedAssetId != -1){
            String name = GL2JNILib.getNodeName(GL2JNILib.getSelectedNodeId());
            int imgW = (int) GL2JNILib.getVertexColorX();
            int imgH = (int) GL2JNILib.getVertexColorY();
            ImageDB imageDB = new ImageDB();
            imageDB.setNodeType((GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_IMAGE) ? Constants.NODE_IMAGE : Constants.NODE_VIDEO);
            imageDB.setName(name);
            imageDB.setWidth(imgW);
            imageDB.setHeight(imgH);
            imageDB.setAssetAddType(0);
            imageDB.setTempNode(false);
            importImage(imageDB);
        }
        else if((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedAssetId != -1){
            TextDB textDB = new TextDB();
            textDB.setAssetName(GL2JNILib.getNodeName(GL2JNILib.getSelectedNodeId()));
            textDB.setFilePath(GL2JNILib.optionalFilePath());
            textDB.setRed(GL2JNILib.getVertexColorX());
            textDB.setGreen(GL2JNILib.getVertexColorY());
            textDB.setBlue(GL2JNILib.getVertexColorZ());
            textDB.setBevalValue((int) GL2JNILib.nodeSpecificFloat());
            textDB.setFontSize(GL2JNILib.getFontSize());
            textDB.setTextureName(GL2JNILib.getTexture(GL2JNILib.getSelectedNodeId()));
            textDB.setTypeOfNode((GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_TEXT) ? Constants.ASSET_TEXT : Constants.ASSET_TEXT_RIG);
            textDB.setTempNode(false);
            importText(textDB);
        }
    }
}
