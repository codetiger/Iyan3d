package com.smackall.animator.NativeCallBackClasses;

import com.smackall.animator.EditorView;

/**
 * Created by Sabish.M on 4/5/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NativeCallBacks {

    EditorView editorView;

    public NativeCallBacks(EditorView editorView)
    {
        this.editorView = editorView;
    }

    public void saveCompletedCallBack(boolean isCloudRender)
    {
        if(isCloudRender) {
            this.editorView.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    editorView.export.packCloudRender();
                }
            });
        }
        else
            this.editorView.saveCompletedCallBack();
    }

    public boolean checkAssets(String filename , int nodeType){
        return this.editorView.missingAssetHandler.checkAssets(filename,nodeType);
    }

    public void boneLimitCallBack()
    {
        this.editorView.rig.boneLimitCallBack();
    }

    public void rigCompletedCallBack(boolean completed)
    {
        this.editorView.rig.rigCompletedCallBack(completed);
    }

    public void addToDatabase(boolean status, String name, int type){
        this.editorView.save.addToDatabase(status,name,type);
    }

    public void undo(int actionType , int returnValue){
        this.editorView.undoRedo.undo(actionType,returnValue);
    }

    public void redo(int returnValue){
        this.editorView.undoRedo.redo(returnValue);
    }

    public void updatePreview(int frame){
        this.editorView.export.updatePreview(frame);
    }

    public void updateXYZValue(boolean hide, float x, float y, float z){
        this.editorView.updateXYZValues.updateXyzValue(hide,x,y,z);
    }

    public void cloneSelectedAssetWithId(int selectedAssetId,int selectedNodeType,int selectedNode ){
        this.editorView.renderManager.cloneSelectedAsset(selectedAssetId,selectedNodeType,selectedNode);
    }

    public void showOrHideLoading(final int state){
                editorView.showOrHideLoading(state);
    }
}
