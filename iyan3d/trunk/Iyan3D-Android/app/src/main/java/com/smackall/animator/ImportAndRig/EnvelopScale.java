package com.smackall.animator.ImportAndRig;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILibAutoRig;

import java.text.DecimalFormat;

import smackall.animator.R;

/**
 * Created by Sabish.M on 17/11/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class EnvelopScale {
   static DecimalFormat form;
        public static void scaleDialog(Context context, Activity activity, final int max , int y, final float currentScale){
            final Dialog scale_dialog = new Dialog(context);
            scale_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
            scale_dialog.setContentView(R.layout.envelop_scale);
            scale_dialog.setCancelable(true);
            scale_dialog.setCanceledOnTouchOutside(true);

            switch (Constant.getScreenCategory(context)){
                case "normal":
                    scale_dialog.getWindow().setLayout((int)(Constant.width / 1.5),(int) (Constant.height / 3));
                    break;
                case "large":
                    scale_dialog.getWindow().setLayout(Constant.width / 2, Constant.height / 4);
                    break;
                case "xlarge":
                    scale_dialog.getWindow().setLayout((int)(Constant.width / 2.5),(int) (Constant.height / 5));
                    break;
                case "undefined":
                    scale_dialog.getWindow().setLayout(Constant.width / 3, Constant.height / 6);
                    break;
            }

            Window window = scale_dialog.getWindow();
            WindowManager.LayoutParams wlp = window.getAttributes();
            wlp.gravity= Gravity.CENTER | Gravity.LEFT;
            wlp.dimAmount=0.0f;
            wlp.x = 0;
            wlp.y = y - (Constant.height/7);

            window.setAttributes(wlp);

            final SeekBar seekBar_x_value = (SeekBar) scale_dialog.findViewById(R.id.envelop_seekbar);
            final TextView x_value_text = (TextView ) scale_dialog.findViewById(R.id.envelop_value_text);
            int seekBarMax = (int)(currentScale*10 * 2);
            int seekBarVal = (int)(currentScale*10);
            form = new DecimalFormat("0.00");
            x_value_text.setText(form.format(currentScale));
            seekBar_x_value.setMax(seekBarMax);
            seekBar_x_value.setProgress(seekBarVal);
            scale_dialog.show();

            seekBar_x_value.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                    if(progress < 0.2*10)
                        scaleValues((float) (0.2/10.0f),false);
                    else {
                        scaleValues(progress / 10.0f,false);
                        x_value_text.setText(form.format(progress / 10.0));
                    }
                }

                public void onStartTrackingTouch(SeekBar seekBar) {
                    // TODO Auto-generated method stub
                }

                public void onStopTrackingTouch(SeekBar seekBar) {

                    if(seekBar_x_value.getProgress() < 0.2*10) {
                        seekBar.setProgress((int) (0.2*10));
                        seekBar.setMax((int) (0.2 * 10 * 2));
                        scaleValues(0.10f,true);
                    }
                    else {
                        seekBar_x_value.setMax(seekBar_x_value.getProgress() * 2);
                        scaleValues(seekBar_x_value.getProgress() / 10.0f, true);
                    }
                }
            });
        }

        public static void scaleValues(float value,boolean state){
            System.out.println("Envelop Scale Value : " + value);
            GL2JNILibAutoRig.scaleAutoRig(value,state);
        }
    }