package com.smackall.animator;

import android.app.Activity;
import android.content.Context;

import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.opengl.GL2JNILib;

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
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                doUndo(actionType,returnValue);
            }
        });
    }

    public void doUndo(int actionType , int returnValue)
    {
        switch (actionType) {
            case Constants.DO_NOTHING: {
                break;
            }
            case Constants.DELETE_ASSET: {
                if (returnValue < GL2JNILib.getNodeCount()) {
                    ((EditorView)((Activity)mContext)).renderManager.removeObject(returnValue,true);
                }
                break;
            }
            case Constants.ADD_MULTI_ASSET_BACK:{
                undoMultiAssetDeleted(returnValue);
                break;
            }
            case Constants.ADD_TEXT_IMAGE_BACK: {
                break;
            }
            case Constants.ADD_ASSET_BACK: {
                if(returnValue >= 900 && returnValue < 1000) {
                    ((EditorView)(Activity)mContext).renderManager.importLight(returnValue,Constants.UNDO_ACTION);
                }
                else if(returnValue >= 10000 && returnValue < 20000){
                    AssetsDB assetsDB = db.getModelWithAssetId(returnValue).get(0);
                    assetsDB.setIsTempNode(false);
                    assetsDB.setType(Constants.NODE_PARTICLES);
                }
                else {
                    AssetsDB assetsDB = db.getModelWithAssetId(returnValue).get(0);
                    if(assetsDB == null) {
                        assetsDB = db.getMyModelWithAssetId(returnValue).get(0);
                        if (assetsDB == null) return;
                    }
                    System.out.println("Asset Id : " + assetsDB.getAssetsId());
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.UNDO_ACTION);
                    assetsDB.setTexture(returnValue+"-cm");
                    ((EditorView)(Activity)mContext).renderManager.importAssets(assetsDB,false);
                }
                break;
            }
            case Constants.SWITCH_FRAME: {
                break;
            }
            case Constants.RELOAD_FRAMES: {
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
        System.out.println("Return Value : " + returnValue);
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                doRedo(returnValue);
            }
        });
    }

    public void doRedo(int returnValue)
    {
        if (returnValue == Constants.DO_NOTHING) {
            //DO NOTHING
        }
        else if (returnValue == Constants.DELETE_ASSET) {
            if (GL2JNILib.getSelectedNodeId() < GL2JNILib.getNodeCount()) {
                ((EditorView)((Activity)mContext)).renderManager.removeObject(GL2JNILib.getSelectedNodeId(),true);
            }
        }
        else if (returnValue == Constants.ADD_TEXT_IMAGE_BACK) {
        }
        else if (returnValue == Constants.SWITCH_MIRROR) {
            //self.mirrorSwitch.on = animationScene->getMirrorState();
        }
        else if(returnValue == Constants.ACTION_MULTI_NODE_DELETED_BEFORE){
            redoMultiAssetDeleted();
        }
        else {
            if (returnValue != Constants.DEACTIVATE_UNDO && returnValue != Constants.DEACTIVATE_REDO && returnValue != Constants.DEACTIVATE_BOTH) {
                //importPressed = NO;
                int assetId = returnValue;
                if(assetId > 900 && assetId < 1000) {
                    int numberOfLight = assetId - 900;
                    ((EditorView)(Activity)mContext).renderManager.importLight(returnValue,Constants.REDO_ACTION);
                } else {
                    AssetsDB assetsDB = db.getModelWithAssetId(returnValue).get(0);
                    if(assetsDB == null) {
                        assetsDB = db.getMyModelWithAssetId(returnValue).get(0);
                        if (assetsDB == null) return;
                    }
                    System.out.println("Asset Id : " + assetsDB.getAssetsId());
                    assetsDB.setIsTempNode(false);
                    assetsDB.setActionType(Constants.REDO_ACTION);
                    assetsDB.setTexture(returnValue+"-cm");
                    ((EditorView)(Activity)mContext).renderManager.importAssets(assetsDB,false);
                }
            }
        }
    }

    private void undoMultiAssetDeleted(int size)
    {
        for (int i = 0; i < size; i++)
            GL2JNILib.undo(UndoRedo.this);
        GL2JNILib.decreaseCurrentAction();
    }

    private void redoMultiAssetDeleted()
    {
        for (int i = 0; i < GL2JNILib.objectIndex(); i++)
                    GL2JNILib.redo(UndoRedo.this);
        GL2JNILib.increaseCurrentAction();
    }
}
