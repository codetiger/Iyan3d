package com.smackall.animator.Helper;

import android.content.Context;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;

import com.smackall.animator.EditorView;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 23/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Delete {

    Context mContext;

    public Delete(Context context) {
        this.mContext = context;
    }

    public void showDelete() {
        final AlertDialog builder;
        LayoutInflater inflater = LayoutInflater.from(mContext);
        final View dialog = inflater
                .inflate(R.layout.delete, null);
        builder = new AlertDialog.Builder(mContext).create();
        builder.setTitle(mContext.getString(R.string.delete));
        Button deleteObj = (Button) dialog.findViewById(R.id.deleteObj);
        dialog.findViewById(R.id.deleteObj).setVisibility((GL2JNILib.getSelectedNodeId() < 2) ? View.GONE : View.VISIBLE);
        Button deleteAnim = (Button) dialog.findViewById(R.id.deleteAnim);
        Button cancel = (Button) dialog.findViewById(R.id.cancel);
        deleteObj.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((EditorView) mContext).renderManager.removeObject(GL2JNILib.getSelectedNodeId(), false);
                builder.cancel();
            }
        });

        deleteAnim.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!GL2JNILib.deleteAnimation()) {
                    UIHelper.informDialog(mContext, mContext.getString(R.string.no_aniamtion_in_frame_msg));
                }
                builder.cancel();
            }
        });
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                builder.cancel();
            }
        });

        builder.setView(dialog);
        builder.setCanceledOnTouchOutside(false);
        builder.show();
    }
}
