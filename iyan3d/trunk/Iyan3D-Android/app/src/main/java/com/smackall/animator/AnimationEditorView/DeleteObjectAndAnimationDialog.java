package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.view.Window;
import android.widget.Button;

import com.smackall.animator.common.Constant;

import smackall.animator.R;


/**
 * Created by Sabish.M on 8/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DeleteObjectAndAnimationDialog {

    public static void deleteObjectAndAnimationDialog(final Context context, Activity activity){
        final Dialog delete_dialog = new Dialog(context);
        delete_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);

        if ((Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_CAMERA.getValue())
                || (Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_LIGHT.getValue())) {
            delete_dialog.setContentView(R.layout.delete_animation_dialog);
            System.out.println("Delete Animation Content Set");
        }
        else{
            System.out.println("Delete Object Content Set");
            delete_dialog.setContentView(R.layout.delete_object_animation);
        }


        delete_dialog.setCancelable(true);
        delete_dialog.setCanceledOnTouchOutside(true);
        switch (Constant.getScreenCategory(context)){
            case "normal":
                delete_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 1.5), (int)(Constant.animationEditor.height / 1.5));
                break;
            case "large":
                delete_dialog.getWindow().setLayout(Constant.animationEditor.width / 2, Constant.animationEditor.height / 2);
                break;
            case "xlarge":
                delete_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 2.5), (int)(Constant.animationEditor.height / 2.5));
                break;
            case "undefined":
                delete_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 2.5), (int)(Constant.animationEditor.height / 2.5));
                break;
        }


        Button delete_animation_btn = (Button) delete_dialog.findViewById(R.id.delete_animation_btn);
        Button cancel_btn = (Button) delete_dialog.findViewById(R.id.cancel_btn);


        if ((Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_CAMERA.getValue())
                || (Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_LIGHT.getValue())) {
            System.out.println("Delete Object Rejected From Menu");
        }
        else{
            System.out.println("Delete Object Enabled From Menu");
            Button delete_object_btn = (Button) delete_dialog.findViewById(R.id.delete_object_btn);
            delete_object_btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    ConfirmDialog.deleteConfirmDialog(context, "object");
                    delete_dialog.dismiss();
                }
            });
        }

        delete_dialog.show();

        delete_animation_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ConfirmDialog.deleteConfirmDialog(context, "animation");
                delete_dialog.dismiss();
            }
        });
        cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                delete_dialog.dismiss();
            }
        });

    }
}
