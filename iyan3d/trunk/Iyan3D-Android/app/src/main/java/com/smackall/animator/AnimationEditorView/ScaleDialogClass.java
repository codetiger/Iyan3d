package com.smackall.animator.AnimationEditorView;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.TextUtils;
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
import com.smackall.animator.opengl.GL2JNILibAutoRig;

import smackall.animator.R;


/**
 * Created by Sabish.M on 7/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ScaleDialogClass {

    String whichView = "";
    float x_scale_value , y_scale_value, z_scale_value;
    boolean mirror_lock_scale_image = false;
    boolean mirror_lock_scale = false;
    int animation_editor_scale_max = 20;
    SeekBar seekBar_x_value;
    SeekBar seekBar_y_value;
    SeekBar seekBar_z_value;
    ImageView mirror_lock;

    public void scaleDialog(Context context, int y, String view,float x_value,float y_value,float z_value){
        whichView = view;
        this.x_scale_value = x_value;
        this.y_scale_value = y_value;
        this.z_scale_value = z_value;
        final Dialog scale_dialog = new Dialog(context);
        scale_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        scale_dialog.setContentView(R.layout.scale_popup_layout);
        scale_dialog.setCancelable(true);
        scale_dialog.setCanceledOnTouchOutside(true);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                scale_dialog.getWindow().setLayout((int)(Constant.width / 1.5),(int) (Constant.height / 1.5));
                break;
            case "large":
                scale_dialog.getWindow().setLayout(Constant.width / 2, Constant.height / 2);
                break;
            case "xlarge":
                scale_dialog.getWindow().setLayout((int)(Constant.width / 2.5),(int) (Constant.height / 2.5));
                break;
            case "undefined":
                scale_dialog.getWindow().setLayout(Constant.width / 3, Constant.height / 3);
                break;
        }

        Window window = scale_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.CENTER | Gravity.LEFT;
        wlp.dimAmount=0.0f;
        wlp.x = 0;
        wlp.y = y - (Constant.height/7);

        window.setAttributes(wlp);

        seekBar_x_value = (SeekBar) scale_dialog.findViewById(R.id.seekbar_x_value);
        seekBar_y_value = (SeekBar) scale_dialog.findViewById(R.id.seekbar_y_value);
        seekBar_z_value = (SeekBar) scale_dialog.findViewById(R.id.seekbar_z_value);

        mirror_lock = (ImageView) scale_dialog.findViewById(R.id.mirror_lock);
        LinearLayout mirror_lock_holder = (LinearLayout) scale_dialog.findViewById(R.id.mirror_lock_holder);

        final TextView x_value_text = (TextView ) scale_dialog.findViewById(R.id.x_value);
        final TextView y_value_text = (TextView ) scale_dialog.findViewById(R.id.y_value);
        final TextView z_value_text = (TextView ) scale_dialog.findViewById(R.id.z_value);
        x_value_text.setMaxLines(1);
        x_value_text.setEllipsize(TextUtils.TruncateAt.END);
        y_value_text.setMaxLines(1);
        y_value_text.setEllipsize(TextUtils.TruncateAt.END);
        z_value_text.setMaxLines(1);
        z_value_text.setEllipsize(TextUtils.TruncateAt.END);

        seekBar_x_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.styled_progress));
        seekBar_y_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.styled_progress));
        seekBar_z_value.setProgressDrawable(context.getResources().getDrawable(R.drawable.styled_progress));
        seekBar_x_value.setMax((int) (x_scale_value * 10) * 2);
        seekBar_y_value.setMax((int) (y_scale_value*10)*2);
        seekBar_z_value.setMax((int) (z_scale_value*10)*2);
        seekBar_x_value.setProgress((int) (x_scale_value * 10));
        seekBar_y_value.setProgress((int) (y_scale_value*10));
        seekBar_z_value.setProgress((int) (z_scale_value*10));

        x_value_text.setText("X : " + String.valueOf(x_scale_value));
        y_value_text.setText("Y : " + String.valueOf(y_scale_value));
        z_value_text.setText("Z : " + String.valueOf(z_scale_value));

        scale_dialog.show();
        switchMirror();

        scale_dialog.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                mirror_lock_scale_image = false;
                mirror_lock_scale = false;
                animation_editor_scale_max = 20;
            }
        });

        mirror_lock_holder.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switchMirror();
            }
        });

        seekBar_x_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int min = 2;
                if(min < progress) {
                    if (!mirror_lock_scale) {
                        x_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        x_scale_value = (float) progress / 10.0f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        x_scale_value = (float) progress / 10.0f;
                        y_value_text.setText("Y : " + String.valueOf((float) progress / 10.0f));
                        y_scale_value = (float) progress / 10.0f;
                        z_value_text.setText("Z : " + String.valueOf((float) progress / 10.0f));
                        z_scale_value = (float) progress / 10.0f;
                        seekBar_y_value.setProgress(progress);
                        seekBar_z_value.setProgress(progress);
                        scaleValues(false);
                    }
                }
                else {
                    if (!mirror_lock_scale) {
                        x_value_text.setText("X : 0.2" );
                        x_scale_value = 0.2f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : 0.2");
                        x_scale_value = 0.2f;
                        y_value_text.setText("Y : 0.2");
                        y_scale_value = 0.2f;
                        z_value_text.setText("Z : 0.2" );
                        z_scale_value = 0.2f;
                        seekBar_y_value.setProgress(2);
                        seekBar_z_value.setProgress(2);
                        scaleValues(false);
                    }
                }
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
                final int current_Max = Math.max(Math.max(seekBar_x_value.getProgress(), seekBar_y_value.getProgress()), seekBar_z_value.getProgress());
                if (current_Max > 2) {
                    animation_editor_scale_max = current_Max * 2;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                } else {
                    animation_editor_scale_max = 4;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                }
                scaleValues(true);
            }
        });

        seekBar_y_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int min = 2;
                if(min < progress) {
                    if (!mirror_lock_scale) {
                        y_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        y_scale_value = (float) progress / 10.0f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        x_scale_value = (float) progress / 10.0f;
                        y_value_text.setText("Y : " + String.valueOf((float) progress / 10.0f));
                        y_scale_value = (float) progress / 10.0f;
                        z_value_text.setText("Z : " + String.valueOf((float) progress / 10.0f));
                        z_scale_value = (float) progress / 10.0f;
                        seekBar_x_value.setProgress(progress);
                        seekBar_z_value.setProgress(progress);
                        scaleValues(false);
                    }
                }
                else {
                    if (!mirror_lock_scale) {
                        y_value_text.setText("X : 0.2" );
                        x_scale_value = 0.2f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : 0.2");
                        x_scale_value = 0.2f;
                        y_value_text.setText("Y : 0.2");
                        y_scale_value = 0.2f;
                        z_value_text.setText("Z : 0.2" );
                        z_scale_value = 0.2f;
                        seekBar_x_value.setProgress(2);
                        seekBar_z_value.setProgress(2);
                        scaleValues(false);
                    }
                }
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
                final int current_Max = Math.max(Math.max(seekBar_x_value.getProgress(), seekBar_y_value.getProgress()), seekBar_z_value.getProgress());
                if (current_Max > 2) {
                    animation_editor_scale_max = current_Max * 2;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                } else {
                    animation_editor_scale_max = 4;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                }
                scaleValues(true);
            }
        });

        seekBar_z_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int min = 2;
                if(min < progress) {
                    if (!mirror_lock_scale) {
                        z_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        z_scale_value = (float) progress / 10.0f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : " + String.valueOf((float) progress / 10.0f));
                        x_scale_value = (float) progress / 10.0f;
                        y_value_text.setText("Y : " + String.valueOf((float) progress / 10.0f));
                        y_scale_value = (float) progress / 10.0f;
                        z_value_text.setText("Z : " + String.valueOf((float) progress / 10.0f));
                        z_scale_value = (float) progress / 10.0f;
                        seekBar_y_value.setProgress(progress);
                        seekBar_x_value.setProgress(progress);
                        scaleValues(false);
                    }
                }
                else {
                    if (!mirror_lock_scale) {
                        z_value_text.setText("X : 0.2" );
                        x_scale_value = 0.2f;
                        scaleValues(false);
                    } else {
                        x_value_text.setText("X : 0.2");
                        x_scale_value = 0.2f;
                        y_value_text.setText("Y : 0.2");
                        y_scale_value = 0.2f;
                        z_value_text.setText("Z : 0.2" );
                        z_scale_value = 0.2f;
                        seekBar_y_value.setProgress(2);
                        seekBar_x_value.setProgress(2);
                        scaleValues(false);
                    }
                }
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            public void onStopTrackingTouch(SeekBar seekBar) {
                final int current_Max = Math.max(Math.max(seekBar_x_value.getProgress(), seekBar_y_value.getProgress()), seekBar_z_value.getProgress());
                if (current_Max > 2) {
                    animation_editor_scale_max = current_Max * 2;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                } else {
                    animation_editor_scale_max = 4;
                    seekBar_x_value.setMax(animation_editor_scale_max);
                    seekBar_y_value.setMax(animation_editor_scale_max);
                    seekBar_z_value.setMax(animation_editor_scale_max);
                }
                scaleValues(true);
            }
        });
    }

    public void switchMirror(){
        final int max = Math.max(Math.max(seekBar_x_value.getMax(), seekBar_y_value.getMax()), seekBar_z_value.getMax());
        final int current_Max = Math.max(Math.max(seekBar_x_value.getProgress(), seekBar_y_value.getProgress()), seekBar_z_value.getProgress());
        if (!mirror_lock_scale_image) {
            mirror_lock.setImageResource(R.drawable.switch_on);
            animation_editor_scale_max = max;
            seekBar_x_value.setMax(animation_editor_scale_max);
            seekBar_y_value.setMax(animation_editor_scale_max);
            seekBar_z_value.setMax(animation_editor_scale_max);
            seekBar_x_value.setProgress(current_Max);
            seekBar_y_value.setProgress(current_Max);
            seekBar_z_value.setProgress(current_Max);
            mirror_lock_scale_image = true;
            mirror_lock_scale = true;
        } else {
            mirror_lock.setImageResource(R.drawable.switch_off);
            mirror_lock_scale_image = false;
            mirror_lock_scale = false;
        }
    }

    public void scaleValues(boolean state){
        System.out.println("X Value : " + x_scale_value);
        System.out.println("Y Value : " + y_scale_value);
        System.out.println("Z Value : " + z_scale_value);

        if(whichView.equals("animationeditor"))
        GL2JNILib.scaleAction(x_scale_value,y_scale_value,z_scale_value,state);

        if(whichView.equals("autorig")){
            System.out.println("X Value : " + x_scale_value);
            System.out.println("Y Value : " + y_scale_value);
            System.out.println("Z Value : " + z_scale_value);
            GL2JNILibAutoRig.scaleJointAutorig(x_scale_value,y_scale_value,z_scale_value,state);
        }

    }
}
