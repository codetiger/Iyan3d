package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

import smackall.animator.R;

/**
 * Created by Sabish.M on 13/11/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class LightPropertiesDialog {

        int red, green, blue, darkness;

        public void lightPropertiesDialog(Context context, Activity activity, final int x , int y){
            final Dialog scale_dialog = new Dialog(context);
            scale_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
            scale_dialog.setContentView(R.layout.light_properties_layout);
            scale_dialog.setCancelable(true);
            scale_dialog.setCanceledOnTouchOutside(true);

            switch (Constant.getScreenCategory(context)){
                case "normal":
                    scale_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 1.5),(Constant.animationEditor.height / 1));
                    break;
                case "large":
                    scale_dialog.getWindow().setLayout(Constant.animationEditor.width / 2, (int) (Constant.animationEditor.height / 1.5));
                    break;
                case "xlarge":
                    scale_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 2.5),(Constant.animationEditor.height / 2));
                    break;
                case "undefined":
                    scale_dialog.getWindow().setLayout(Constant.animationEditor.width / 3, (int) (Constant.animationEditor.height / 2.5));
                    break;
            }

            Window window = scale_dialog.getWindow();
            WindowManager.LayoutParams wlp = window.getAttributes();
            wlp.gravity= Gravity.CENTER | Gravity.RIGHT;
            wlp.dimAmount=0.0f;
            wlp.x = 0;
            wlp.y = y - (Constant.height/7);

            window.setAttributes(wlp);

            final SeekBar seekBar_red_value = (SeekBar) scale_dialog.findViewById(R.id.red_seekbar_value);
            final SeekBar seekBar_green_value = (SeekBar) scale_dialog.findViewById(R.id.green_seekbar_value);
            final SeekBar seekBar_blue_value = (SeekBar) scale_dialog.findViewById(R.id.blue_seekbar_value);
            final SeekBar seekBar_darkness_value = (SeekBar) scale_dialog.findViewById(R.id.darkness_value);
            final TextView shadow_text = (TextView) scale_dialog.findViewById(R.id.shadow_text);

            final TextView c_text = (TextView) scale_dialog.findViewById(R.id.c_text);
            c_text.setTextSize(TypedValue.COMPLEX_UNIT_SP, Constant.animationEditor.height / 10);

            seekBar_red_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.red));
            seekBar_green_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.green));
            seekBar_blue_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.blue));


            seekBar_red_value.setMax(255);
            seekBar_green_value.setMax(255);
            seekBar_blue_value.setMax(255);
            red = (int) (GL2JNILib.getLightPropertiesRed()*255.0f);
            green = (int) (GL2JNILib.getLightPropertiesGreen() * 255.0f);
            blue = (int) (int) (GL2JNILib.getLightPropertiesBlue() * 255.0f);
            seekBar_red_value.setProgress((int) (GL2JNILib.getLightPropertiesRed() * 255.0f));
            seekBar_green_value.setProgress((int) (GL2JNILib.getLightPropertiesGreen() * 255.0f));
            seekBar_blue_value.setProgress((int) (GL2JNILib.getLightPropertiesBlue() * 255.0f));
            seekBar_darkness_value.setMax(100);


            if(Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_TYPE.NODE_LIGHT.getValue()){
                darkness = (int) (GL2JNILib.getShadowDensity()*100);
                seekBar_darkness_value.setProgress(darkness);
                shadow_text.setText("SHADOW DARKNESS");
            }
            else{
                darkness = (int) (((GL2JNILib.getShadowDensity()/300.0)-0.001)*100);
                seekBar_darkness_value.setProgress(darkness);
                shadow_text.setText("DISTANCE");
            }

            scale_dialog.show();
            lightValues(c_text, red, green, blue);
            seekBar_red_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    red = progress;
                    lightValues(c_text, red, green, blue);
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    lightValuesEnded(c_text, red, green, blue,darkness);
                }
            });

            seekBar_green_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    green = progress;
                    lightValues(c_text, red,green,blue);
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    lightValuesEnded(c_text, red, green,blue,darkness);
                }
            });

            seekBar_blue_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    blue = progress;
                    lightValues(c_text, red,green,blue);
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    lightValuesEnded(c_text, red, green,blue,darkness);
                }
            });

            seekBar_darkness_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    darkness = progress;
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    lightValuesEnded(c_text, red, green,blue,darkness);
                }
            });
        }

        public void lightValues(TextView c_text, int R, int G, int B){
            c_text.setTextColor(Color.argb(255, R, G, B));
        }

    public void lightValuesEnded(TextView c_text, int R, int G, int B, int darkness){
        GL2JNILib.changeLightProperty(R / 255.0f, G / 255.0f, B / 255.0f, darkness/100.0f);
    }
}


