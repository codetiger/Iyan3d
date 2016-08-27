package com.smackall.animator.NativeCallBackClasses;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

/**
 * Created by Sabish.M on 4/5/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NativeCallBacks {

    EditorView editorView;
    boolean resourceMissingDialogShowing = false;

    public NativeCallBacks(EditorView editorView) {
        this.editorView = editorView;
    }

    public void saveCompletedCallBack(boolean isCloudRender) {
        if (isCloudRender) {
            this.editorView.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    editorView.export.packCloudRender();
                }
            });
        } else
            this.editorView.saveCompletedCallBack();
    }

    public boolean checkAssets(String filename, int nodeType) {
        boolean state = this.editorView.missingAssetHandler.checkAssets(filename, nodeType);
        if (!state && !resourceMissingDialogShowing) {
            resourceMissingDialogShowing = true;
            UIHelper.informDialog(this.editorView, editorView.getString(R.string.some_resources_are_missing));
        }
        return state;
    }

    public void boneLimitCallBack() {
        this.editorView.rig.boneLimitCallBack();
    }

    public void rigCompletedCallBack(boolean completed) {
        this.editorView.rig.rigCompletedCallBack(completed);
    }

    public void addToDatabase(boolean status, String name, int type) {
        this.editorView.save.addToDatabase(status, name, type);
    }

    public void undo(int actionType, int returnValue) {
        this.editorView.undoRedo.undo(actionType, returnValue);
    }

    public void redo(int returnValue) {
        this.editorView.undoRedo.redo(returnValue);
    }

    public void updatePreview(int frame) {
        this.editorView.export.updatePreview(frame);
    }

    public void updateXYZValue(boolean hide, float x, float y, float z) {
        this.editorView.updateXYZValues.updateXyzValue(hide, x, y, z);
    }

    public void cloneSelectedAssetWithId(int selectedAssetId, int selectedNodeType, int selectedNode) {
        this.editorView.renderManager.cloneSelectedAsset(selectedAssetId, selectedNodeType, selectedNode);
    }

    public void showOrHideLoading(int state) {
        editorView.showOrHideLoading(state);
    }

    public void clearMap() {
        this.editorView.sgNodeOptionsMap.clearNodeProps();
    }

    public void addSGNodeProperty(int index, int parentIndex, int type, String title, String fileName, int iconId, String groupName,
                                  float valueX, float valueY, float valueZ, float valueW, boolean isEnabled) {
        this.editorView.sgNodeOptionsMap.addSGNodeProperty(index, parentIndex, type, title, fileName, iconId, groupName,
                valueX, valueY, valueZ, valueW, isEnabled);
    }

    public void deleteObjectOrAnimation()
    {
        editorView.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                editorView.delete.showDelete();
            }
        });
    }
    public void changeTextureForAsset()
    {
        editorView.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                editorView.textureSelection.showChangeTexture();
            }
        });
    }
}
