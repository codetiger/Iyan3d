package com.smackall.animator.AnimationEditorView;

import android.view.View;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 7/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class ButtonHandler {

    public static void buttonHanler() {
        Constant.animationEditor.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                System.out.println("Selected Node Id : " + Constant.animationEditor.selectedNodeId);
                System.out.println("Selected Node Type : " + Constant.animationEditor.selectedNodeType);
                //Constant.animationEditor.gridViewHorizontally(Constant.animationEditor.selectedGridItem);
                if(Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType ==4 && GL2JNILib.isJointSelected()){
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.VISIBLE);
                }
                else if(Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType ==3 && GL2JNILib.isJointSelected()) {
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.INVISIBLE);
                }
               else if (Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_CAMERA.getValue()) {
                    System.out.println("First Function Called");
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.INVISIBLE);

                } else if ((Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_LIGHT.getValue())) {
                    System.out.println("Second Function Called");
                   Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                   Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.INVISIBLE);
                   Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                   Constant.animationEditor.animation_editor_option_btn.setVisibility(View.VISIBLE);
                   Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.VISIBLE);
                   Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.INVISIBLE);

                } else if ((Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_ADDITIONAL_LIGHT.getValue())) {
                    System.out.println("Third Function Called");
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.INVISIBLE);

                } else if (Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType != AnimationEditor.NODE_TYPE.NODE_UNDEFINED.getValue()) {
                    System.out.println("Fourth Function Called");
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.VISIBLE);

                } else {
                    Constant.animationEditor.animation_editor_move_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_rotate_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_select_btn.setVisibility(View.VISIBLE);
                    Constant.animationEditor.animation_editor_option_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_delete_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.animation_editor_scale_btn.setVisibility(View.INVISIBLE);
                    Constant.animationEditor.mirror_btn.setVisibility(View.INVISIBLE);

                }
            }
        });
    }
}
