package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.smackall.animator.InAppPurchase.PremiumUpgrade;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.RangeSeekBar;

import smackall.animator.R;


/**
 * Created by Sabish.M on 14/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class ExportDialog {

    static LinearLayout export_fullhd_btn;
    static LinearLayout export_hd_btn;
    static LinearLayout export_dvd_btn;
    static LinearLayout normal_shader_btn,toon_shader_btn;
    public static ImageView water_mark_switch;
    public int shaderType;


    public enum SHADER_TYPE{
        NORMAL_SHADER(0),
        TOON_SHADER(1);

        private int value;
        SHADER_TYPE(int i) {
            value = i;
        }

        int getValue(){
            return value;
        }
    }

    int minFrame = 0 , maxFrame = 0;

    boolean waterMark = false;
    int resolution = 0;

    public void exportDialog(final Context context, final Activity activity, final String type,int resolution_passed) {
        final Dialog export_dialog = new Dialog(context);
        export_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        export_dialog.setContentView(R.layout.export_dialog);
        export_dialog.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        export_dialog.setCancelable(false);
        export_dialog.setCanceledOnTouchOutside(false);
        shaderType=0;
        waterMark=false;
        resolution = resolution_passed;

        final TextView min = (TextView) export_dialog.findViewById(R.id.minValue);
        final TextView max = (TextView) export_dialog.findViewById(R.id.maxValue);

        min.setTextSize(Constant.width/60);
        max.setTextSize(Constant.width/60);

        min.setText("1");
        max.setText(String.valueOf(Constant.animationEditor.frameCount));
        minFrame = 1;
        maxFrame = Constant.animationEditor.frameCount;

        // create RangeSeekBar as Integer range between 20 and 75
        RangeSeekBar<Integer> seekBar = new RangeSeekBar<Integer>(minFrame, maxFrame, context);
        seekBar.setOnRangeSeekBarChangeListener(new RangeSeekBar.OnRangeSeekBarChangeListener<Integer>() {
            @Override
            public void onRangeSeekBarValuesChanged(RangeSeekBar<?> bar, Integer minValue, Integer maxValue) {
                min.setText(minValue.toString());
                max.setText(maxValue.toString());
                minFrame = minValue;
                maxFrame = maxValue;
            }
        });

        // add RangeSeekBar to pre-defined layout
        ViewGroup rangeBarHolder = (ViewGroup) export_dialog.findViewById(R.id.seekbar_placeholder);
        rangeBarHolder.addView(seekBar);
        if(type.equals("video")) {
            rangeBarHolder.setVisibility(View.VISIBLE);
        }
            else {
            rangeBarHolder.setVisibility(View.INVISIBLE);
            min.setVisibility(View.INVISIBLE);
            max.setVisibility(View.INVISIBLE);
            minFrame = Constant.animationEditor.selectedGridItem;
            maxFrame = Constant.animationEditor.selectedGridItem;
        }

        water_mark_switch = (ImageView) export_dialog.findViewById(R.id.water_mark_switch);

        export_fullhd_btn = (LinearLayout) export_dialog.findViewById(R.id.export_fullhd_btn);
        export_hd_btn = (LinearLayout) export_dialog.findViewById(R.id.export_hd_btn);
        export_dvd_btn = (LinearLayout) export_dialog.findViewById(R.id.export_dvd_btn);
        normal_shader_btn = (LinearLayout) export_dialog.findViewById(R.id.normal_shader_btn);
        toon_shader_btn = (LinearLayout) export_dialog.findViewById(R.id.toon_shader_btn);

        final Button export_cancel_btn = (Button) export_dialog.findViewById(R.id.export_cancel_btn);
        
        Button export_next_btn = (Button) export_dialog.findViewById(R.id.export_next_btn);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                export_dialog.getWindow().setLayout(Constant.width, Constant.height);
                break;
            case "large":
                export_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "xlarge":
                export_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "undefined":
                export_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
        }

        export_dialog.show();


        water_mark_switch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.out.println("Watermark: " + waterMark);
                if (Constant.isPremium(context)) {
                    if (!waterMark) {
                        water_mark_switch.setImageResource(R.drawable.switch_off);
                        waterMark = true;
                    } else {
                        water_mark_switch.setImageResource(R.drawable.switch_on);
                        waterMark = false;
                    }
                } else {
                    if (!waterMark) {
                        Intent in = new Intent(context, PremiumUpgrade.class);
                        in.putExtra("actiontype", "watermark");
                        context.startActivity(in);
                    } else {
                        water_mark_switch.setImageResource(R.drawable.switch_on);
                        waterMark = false;
                    }
                }

            }
        });

        export_fullhd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution = 0;
                buttonHandler(context);

            }
        });

        export_hd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution = 1;
                buttonHandler(context);
            }
        });

        export_dvd_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resolution = 2;
                buttonHandler(context);
            }
        });

        toon_shader_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Constant.isPremium(context)){
                    toon_shader_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_pressed));
                    normal_shader_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_non_pressed));
                    shaderType = SHADER_TYPE.TOON_SHADER.getValue();
                }
                else{
                    Intent in= new Intent(context,PremiumUpgrade.class);
                    in.putExtra("actiontype","toonshader");
                    context.startActivity(in);
                }
            }
        });

        normal_shader_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toon_shader_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_non_pressed));
                normal_shader_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_pressed));
                shaderType = SHADER_TYPE.NORMAL_SHADER.getValue();
            }
        });

        export_cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                export_dialog.dismiss();
                Constant.exportDialog = null;
            }
        });

        export_next_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.out.println("Shader Type : " + shaderType);
                Constant.renderingDialog = new RenderingDialog();
                Constant.renderingDialog.renderingDialogFunction(context, activity, waterMark, shaderType, minFrame, maxFrame, type,resolution);
                export_dialog.dismiss();
                Constant.exportDialog = null;
            }
        });
        shaderType = SHADER_TYPE.NORMAL_SHADER.getValue();
        buttonHandler(context);
    }

    public void buttonHandler(Context context){
        if(resolution == 0){
            export_fullhd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_pressed));
            export_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border));
            export_dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_radi));
        }
        if(resolution == 1){
            export_fullhd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_radi));
            export_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_pressed));
            export_dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_radi));
        }
        if(resolution == 2){
            export_fullhd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_left_radi));
            export_hd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border));
            export_dvd_btn.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.pink_border_right_pressed));
        }
    }

    public void disableWaterwark(){
        water_mark_switch.setImageResource(R.drawable.switch_off);
        waterMark = true;
    }

    public void enableToonShader(){
        toon_shader_btn.setBackgroundResource(R.drawable.grid_layout_pressed);
        normal_shader_btn.setBackgroundResource(R.drawable.grid_layout_non_pressed);
        shaderType = SHADER_TYPE.TOON_SHADER.getValue();
    }
}
