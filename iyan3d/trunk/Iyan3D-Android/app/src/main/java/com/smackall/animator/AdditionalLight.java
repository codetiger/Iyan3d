package com.smackall.animator;

import android.app.Activity;
import android.content.Context;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AdditionalLight {

    private Context mContext;

    public AdditionalLight(Context mContext) {
        this.mContext = mContext;
    }

    public void addLight() {
        if(GL2JNILib.lightCount() > 4) return;
        int lightCount = 1;
        int lightId = 0;
        for (int i = 2; i < GL2JNILib.getNodeCount(); i++) {
            if (GL2JNILib.getNodeType(i) == Constants.NODE_ADDITIONAL_LIGHT)
                lightId = Math.max(GL2JNILib.getAssetIdWithNodeId(i), lightId);
        }
        lightCount += lightId - ((lightId != 0) ? 900 : 0);

        ((EditorView)(Activity)mContext).renderManager.importLight(lightCount);
    }
}
