package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

import smackall.animator.R;


/**
 * Created by Sabish.M on 7/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class CameraSettings {

    LinearLayout full_hd_btn;
    LinearLayout hd_btn;
    LinearLayout dvd_btn;
    public int resolution = 0;
    float field_of_view;

    public void cameraPropertiesDialog(final Context context, Activity activity, final int x , int y,final int resolution1, final float field_of_view_passed) {
        final Dialog camera_properties_dialog = new Dialog(context);
        camera_properties_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        camera_properties_dialog.setContentView(R.layout.animation_editor_camera_properties);
        camera_properties_dialog.setCancelable(true);
        camera_properties_dialog.setCanceledOnTouchOutside(true);
        this.resolution = resolution1;
        this.field_of_view = field_of_view_passed;

        switch (Constant.getScreenCategory(context)){
            case "normal":
                camera_properties_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 1.5),(int) (Constant.animationEditor.height / 1.5));
                break;
            case "large":
                camera_properties_dialog.getWindow().setLayout(Constant.animationEditor.width / 2, Constant.animationEditor.height / 2);
                break;
            case "xlarge":
                camera_properties_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 2.5),(int) (Constant.animationEditor.height / 2.5));
                break;
            case "undefined":
                camera_properties_dialog.getWindow().setLayout(Constant.animationEditor.width / 3, Constant.animationEditor.height / 3);
                break;
        }


        Window window = camera_properties_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.CENTER | Gravity.RIGHT;
        wlp.dimAmount = 0.0f;
        wlp.x = 0;
        wlp.y = y - (Constant.animationEditor.height / 7);
        System.out.println("X Y Coordinate : " + x + " " + y);
        window.setAttributes(wlp);

        final TextView field_of_view_value_text = (TextView) camera_properties_dialog.findViewById(R.id.field_of_view_value_text);

        full_hd_btn = (LinearLayout) camera_properties_dialog.findViewById(R.id.full_hd_btn);
        hd_btn = (LinearLayout) camera_properties_dialog.findViewById(R.id.hd_btn);
        dvd_btn = (LinearLayout) camera_properties_dialog.findViewById(R.id.dvd_btn);

        SeekBar field_of_view_seekbar = (SeekBar) camera_properties_dialog.findViewById(R.id.field_of_view_seekbar);


        field_of_view_seekbar.setMax(1500);
        int fovValue = (int) (field_of_view *10);
        field_of_view_seekbar.setProgress(fovValue);
        field_of_view_value_text.setText("0.0");

        camera_properties_dialog.show();

        full_hd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution = 0;
                buttonHandler(context);
                cameraProperties(field_of_view, resolution);
            }
        });

        hd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution =1;
                buttonHandler(context);
                cameraProperties(field_of_view, resolution);
            }
        });

        dvd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution =2;
                buttonHandler(context);
                cameraProperties(field_of_view, resolution);
            }
        });

        field_of_view_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                field_of_view = (float) progress / 10.0f;
                field_of_view_value_text.setText(String.valueOf(field_of_view));
                cameraProperties(field_of_view, resolution);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        field_of_view_value_text.setText(String.valueOf(field_of_view));
        buttonHandler(context);
        camera_properties_dialog.show();
    }

    private void buttonHandler(Context context){

        if(resolution == 0){
            full_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_pressed));
            hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border));
            dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_radi));
        }
        if(resolution == 1){
            full_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_radi));
            hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_pressed));
            dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_radi));
        }
        if(resolution == 2){
            full_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_radi));
            hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border));
            dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_pressed));
        }
    }

    private void cameraProperties(float fov , int resolution){

        System.out.println("Field Of View : " + fov);
        System.out.println("Resolution Value : " + resolution);
        GL2JNILib.cameraPropertyChanged(fov,resolution);


    }
}

