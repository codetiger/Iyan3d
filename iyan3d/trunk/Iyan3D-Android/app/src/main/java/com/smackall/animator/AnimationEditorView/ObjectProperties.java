package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
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
public class ObjectProperties {

    ImageView light_switch_img;
    ImageView visible_switch_img;
    float brightness;
    float specular;
    boolean lighting_switch_on,visible_switch_on;

    public void objectPropertiesDialog(Context context, Activity activity, final int x , int y,float brightness_passed,float specular_passed,boolean lighting_switch_on_passed,boolean visible_switch_on_passed) {
        final Dialog object_properties_dialog = new Dialog(context);
        object_properties_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        object_properties_dialog.setContentView(R.layout.animation_editor_object_properties);
        object_properties_dialog.setCancelable(true);
        object_properties_dialog.setCanceledOnTouchOutside(true);
        this.brightness = brightness_passed;
        this.specular = specular_passed;
        this.lighting_switch_on = lighting_switch_on_passed;
        this.visible_switch_on = visible_switch_on_passed;

        switch (Constant.getScreenCategory(context)){
            case "normal":
                object_properties_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 1.5),(int) (Constant.animationEditor.height / 1.5));
                break;
            case "large":
                object_properties_dialog.getWindow().setLayout(Constant.animationEditor.width / 2, Constant.animationEditor.height / 2);
                break;
            case "xlarge":
                object_properties_dialog.getWindow().setLayout((int)(Constant.animationEditor.width / 2.5),(int) (Constant.animationEditor.height / 2.5));
                break;
            case "undefined":
                object_properties_dialog.getWindow().setLayout(Constant.animationEditor.width / 3, Constant.animationEditor.height / 3);
                break;
        }

        Window window = object_properties_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.CENTER | Gravity.RIGHT;
        wlp.dimAmount = 0.0f;
        wlp.x = 0;
        wlp.y = y - (Constant.animationEditor.height / 7);
        System.out.println("X Y Coordinate : " + x + " " + y);
        window.setAttributes(wlp);

        TextView brightness_text = (TextView) object_properties_dialog.findViewById(R.id.brightness_text);
        TextView specular_text = (TextView) object_properties_dialog.findViewById(R.id.specular_text);
        TextView lighting_text = (TextView) object_properties_dialog.findViewById(R.id.lighting_text);
        TextView visible_text = (TextView) object_properties_dialog.findViewById(R.id.visible_text);

        SeekBar brightness_seekbar = (SeekBar) object_properties_dialog.findViewById(R.id.birght_seekbar);
        SeekBar specular_seekbar = (SeekBar) object_properties_dialog.findViewById(R.id.specular_seekbar);

       light_switch_img =(ImageView) object_properties_dialog.findViewById(R.id.light_switch_img);
       visible_switch_img =(ImageView) object_properties_dialog.findViewById(R.id.visible_switch_img);

        LinearLayout light_switch_btn = (LinearLayout) object_properties_dialog.findViewById(R.id.switch_btn_lighting);
        LinearLayout visible_switch_btn = (LinearLayout) object_properties_dialog.findViewById(R.id.switch_btn_visible);

        brightness_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.animationEditor.width/60);
        specular_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.animationEditor.width / 60);
        lighting_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.animationEditor.width / 60);
        visible_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.animationEditor.width / 60);

        light_switch_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (lighting_switch_on) {
                    lighting_switch_on = false;
                    lightButtonHandler();
                } else {
                    lighting_switch_on = true;
                    lightButtonHandler();
                }
                objectPropertiesValues();
            }
        });

        visible_switch_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (visible_switch_on) {
                    visible_switch_on = false;
                    visibleButtonHandler();
                } else {
                    visible_switch_on = true;
                    visibleButtonHandler();
                }
                objectPropertiesValues();
            }
        });

        brightness_seekbar.setMax(10);
        specular_seekbar.setMax(10);



        brightness_seekbar.setProgress((int) (this.brightness*10));
        specular_seekbar.setProgress((int) (this.specular*10));

        //System.out.println("Brightness Value : " + brightness_seekbar.getProgress() + " " + Constant.lighting_switch_on + " " + brightness);
        //System.out.println("Specular Value : " + specular_seekbar.getProgress() + " " + Constant.visible_switch_on+ " " + specular );

        brightness_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = (float) progress / 10.0f;
                System.out.println("Brightness Value : " + String.valueOf(value));
                brightness = value;
                objectPropertiesValues();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        specular_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = (float) progress / 10.0f;
                System.out.println("Specular Value : " + String.valueOf(value));
                specular = value;
                objectPropertiesValues();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        lightButtonHandler();
        visibleButtonHandler();

        object_properties_dialog.show();

    }

    private void lightButtonHandler(){
        if (lighting_switch_on) {
            light_switch_img.setImageResource(R.drawable.switch_on);
        } else {
            light_switch_img.setImageResource(R.drawable.switch_off);
        }
    }

    private void visibleButtonHandler(){
        if(visible_switch_on){
            visible_switch_img.setImageResource(R.drawable.switch_on);
        }
        else {
            visible_switch_img.setImageResource(R.drawable.switch_off);

        }
    }

    private void objectPropertiesValues(){
        System.out.println("The Object Properties Changed");
        GL2JNILib.meshPropertyChanged(this.brightness, this.specular, lighting_switch_on, visible_switch_on);
    }
}
