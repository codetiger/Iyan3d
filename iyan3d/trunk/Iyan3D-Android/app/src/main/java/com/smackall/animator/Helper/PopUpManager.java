package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.view.MotionEvent;
import android.view.View;

import com.smackall.animator.EditorView;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 15/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class PopUpManager {

    private Context mContext;
    private EditorView editorView;

    public PopUpManager(Context context){
        this.mContext = context;
        editorView = ((EditorView)((Activity)mContext));
    }

    public void initPopUpManager(int selectedNodeId,View v,MotionEvent event)
    {
        if(selectedNodeId == -1) return;
        int nodeType = GL2JNILib.getNodeType(selectedNodeId);

        switch (nodeType){
            case Constants.NODE_CAMERA:
                editorView.cameraProps.showCameraProps(v,event);
                break;
            case Constants.NODE_ADDITIONAL_LIGHT:
                editorView.lightProps.showLightProps(v,event);
                break;
            case Constants.NODE_LIGHT:
                editorView.lightProps.showLightProps(v,event);
                break;
            case Constants.NODE_IMAGE:
            case Constants.NODE_VIDEO:
            case Constants.NODE_PARTICLES:
                editorView.meshProps.showMeshProps(v,event);
                break;
            case Constants.NODE_TEXT:
            case Constants.NODE_RIG:
            case Constants.NODE_TEXT_SKIN:
            case Constants.NODE_SGM:
            case Constants.NODE_OBJ:
                editorView.meshProps.showMeshProps(v,event);
                break;
        }
    }
}
