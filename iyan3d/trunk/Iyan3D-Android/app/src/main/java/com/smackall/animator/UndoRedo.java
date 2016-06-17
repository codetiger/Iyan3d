package com.smackall.animator;

import android.app.Activity;
import android.content.Context;

import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Sabish.M on 26/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UndoRedo {

    private Context mContext;
    private DatabaseHelper db;

    public UndoRedo(Context context,DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    public void undo(final int actionType , final int returnValue)
    {
       doUndo(actionType,returnValue);
    }

    public void doUndo(int actionType , int returnValue)
    {
        switch (actionType) {
            case Constants.DO_NOTHING: {
                break;
            }
            case Constants.DELETE_ASSET: {
                if (returnValue < GL2JNILib.getNodeCount()) {
                    GL2JNILib.removeNode(returnValue,true);
                }
                break;
            }
            case Constants.ADD_INSTANCE_BACK:
                ((EditorView)mContext).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.ADD_INSTANCE_BACK(1);
                    }
                });
                break;
            case Constants.DELETE_MULTI_ASSET:
            case Constants.ADD_MULTI_ASSET_BACK:{
                undoMultiAssetDeleted(returnValue);
                break;
            }
            case Constants.ADD_TEXT_IMAGE_BACK: {
                break;
            }
            case Constants.ADD_ASSET_BACK: {
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
                if(returnValue >= 900 && returnValue < 1000) {
                    GL2JNILib.importAdditionalLight(returnValue-900,Constants.UNDO_ACTION);
                }
                else if(returnValue >= 10000 && returnValue < 20000){
                    AssetsDB assetsDB = db.getModelWithAssetId(returnValue).get(0);
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.UNDO_ACTION);
                    assetsDB.setType(Constants.NODE_PARTICLES);
                }
                else {
                    AssetsDB assetsDB = (isStoreAsset(returnValue)) ? db.getModelWithAssetId(returnValue).get(0) : db.getMyModelWithAssetId(returnValue).get(0);
                    if(assetsDB == null) {
                        assetsDB = db.getMyModelWithAssetId(returnValue).get(0);
                        if (assetsDB == null) return;
                    }
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.UNDO_ACTION);
                    assetsDB.setTexture(returnValue+"-cm");
                    GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(),assetsDB.getX(),assetsDB.getY(),assetsDB.getZ() ,assetsDB.getActionType());
                    ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
                }
                break;
            }
            case Constants.SWITCH_FRAME: {
                ((EditorView)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((EditorView)(Activity)mContext).play.highLightFrame(GL2JNILib.currentFrame());
                        ((EditorView)(Activity)mContext).frameAdapter.notifyDataSetChanged();
                    }
                });
                break;
            }
            case Constants.RELOAD_FRAMES: {
                ((EditorView)(Activity)mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((EditorView)(Activity)mContext).frameAdapter.notifyDataSetChanged();
                    }
                });
                break;
            }
            case Constants.SWITCH_MIRROR:
                break;
            default:
                break;
        }
    }

    public void redo(final int returnValue)
    {
        doRedo(returnValue);
    }

    public void doRedo(int returnValue)
    {
        if (returnValue == Constants.DO_NOTHING) {
            //DO NOTHING
        }
        else if (returnValue == Constants.DELETE_ASSET) {
            if (GL2JNILib.getSelectedNodeId() < GL2JNILib.getNodeCount()) {
                GL2JNILib.removeNode(GL2JNILib.getSelectedNodeId(),true);
            }
        }
        else if (returnValue == Constants.ADD_TEXT_IMAGE_BACK) {
        }
        else if (returnValue == Constants.SWITCH_MIRROR) {
            //self.mirrorSwitch.on = animationScene->getMirrorState();
        }
        else if(returnValue == Constants.ACTION_MULTI_NODE_DELETED_BEFORE || returnValue == Constants.ADD_MULTI_ASSET_BACK){
            redoMultiAssetDeleted(GL2JNILib.objectIndex());
        }
        else if(returnValue == Constants.ADD_INSTANCE_BACK){
            GL2JNILib.ADD_INSTANCE_BACK(0);
        }
        else {
            if (returnValue != Constants.DEACTIVATE_UNDO && returnValue != Constants.DEACTIVATE_REDO && returnValue != Constants.DEACTIVATE_BOTH) {
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
                if(returnValue > 900 && returnValue < 1000) {
                    int numberOfLight = returnValue - 900;
                    ((EditorView)(Activity)mContext).renderManager.importLight(returnValue-900,Constants.REDO_ACTION);
                } else if(returnValue >= 10000 && returnValue < 20000){
                    AssetsDB assetsDB = db.getModelWithAssetId(returnValue).get(0);
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.REDO_ACTION);
                    assetsDB.setType(Constants.NODE_PARTICLES);
                } else {
                    AssetsDB assetsDB = (isStoreAsset(returnValue)) ? db.getModelWithAssetId(returnValue).get(0) : db.getMyModelWithAssetId(returnValue).get(0);
                    if(assetsDB == null) {
                        assetsDB = db.getMyModelWithAssetId(returnValue).get(0);
                        if (assetsDB == null) return;
                    }
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.REDO_ACTION);
                    assetsDB.setTexture(returnValue+"-cm");
                    GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(),assetsDB.getX(),assetsDB.getY(),assetsDB.getZ() ,assetsDB.getActionType());
                    ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
                }
            }
        }
    }

    private void undoMultiAssetDeleted(int size)
    {
        for (int i = 0; i < size; i++) {
            GL2JNILib.undo(((EditorView) mContext).nativeCallBacks);
        }
        GL2JNILib.decreaseCurrentAction();
    }

    private void redoMultiAssetDeleted(int size)
    {
        for (int i = 0; i < size; i++) {
            GL2JNILib.redo(((EditorView) mContext).nativeCallBacks);
        }
        GL2JNILib.increaseCurrentAction();
    }

    private boolean isStoreAsset(int assetId){
        return db.getModelWithAssetId(assetId) != null && db.getModelWithAssetId(assetId).size() > 0 && db.getModelWithAssetId(assetId).get(0).getAssetsId() == assetId;
    }
}
