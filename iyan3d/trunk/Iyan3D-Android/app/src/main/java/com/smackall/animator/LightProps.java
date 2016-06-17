package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.SeekBar;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 11/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class LightProps implements View.OnTouchListener,SeekBar.OnSeekBarChangeListener,View.OnClickListener,RadioButton.OnCheckedChangeListener{

    private Context mContext;
    LinearLayout colorPreview;
    Bitmap bitmap;
    int touchedRGB;
    boolean touchBegan = false;
    private Dialog dialog;
    int lightType;
    boolean defaultValueInitialized = false;
    public LightProps(Context mContext)
    {
        this.mContext = mContext;
    }

    public void showLightProps(View v,MotionEvent event)
    {
        HitScreens.LightPropsView(mContext);
        int nodeType = GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId());
        Dialog light_prop = new Dialog(mContext);
        light_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        light_prop.setContentView(R.layout.light_props);
        light_prop.setCancelable(false);
        light_prop.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                light_prop.getWindow().setLayout(Constants.width / 2, (int) (Constants.height));
                break;
            default:
                light_prop.getWindow().setLayout(Constants.width / 3, (int) (Constants.height / 1.5));
                break;
        }
        Window window = light_prop.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.TOP | Gravity.START;
        wlp.dimAmount=0.0f;
        if(event != null) {
            wlp.x = (int)event.getX();
            wlp.y = (int)event.getY();
        }
        else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        ImageView color_picker = (ImageView)light_prop.findViewById(R.id.color_picker);
        bitmap = ((BitmapDrawable)color_picker.getDrawable()).getBitmap();
        initViews(light_prop);
        dialog = light_prop;
        if(nodeType == Constants.NODE_LIGHT){
            ((LinearLayout)dialog.findViewById(R.id.distanceOptionHolder)).setVisibility(View.GONE);
        }
        light_prop.show();
        light_prop.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                HitScreens.EditorView(mContext);
            }
        });
        defaultValueInitialized = true;
        dialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                update(true);
            }
        });
    }

    private void initViews(Dialog light_prop)
    {
        ((ImageView)light_prop.findViewById(R.id.color_picker)).setOnTouchListener(this);
        colorPreview = (LinearLayout)light_prop.findViewById(R.id.color_preview);
        ((SeekBar)light_prop.findViewById(R.id.shadow_darkness)).setOnSeekBarChangeListener(this);
        ((SeekBar)light_prop.findViewById(R.id.distance)).setOnSeekBarChangeListener(this);
        ((Button)light_prop.findViewById(R.id.delete_animation)).setOnClickListener(this);

        float distance = (float) ((GL2JNILib.nodeSpecificFloat()/300.0)-0.001);
        colorPreview.setBackgroundColor(Color.argb(255,(int) (GL2JNILib.lightx() * 255), (int) (GL2JNILib.lighty() * 255), (int) (GL2JNILib.lightz() * 255)));

        touchedRGB = (int) (GL2JNILib.lightx() * 255);
        touchedRGB = (touchedRGB << 8) + (int) (GL2JNILib.lighty() * 255);
        touchedRGB = (touchedRGB << 8) + (int) (GL2JNILib.lightz() * 255);

        ((SeekBar)light_prop.findViewById(R.id.distance)).setProgress((int) (distance * 100));
        ((SeekBar)light_prop.findViewById(R.id.shadow_darkness)).setProgress((int) (GL2JNILib.shadowDensity()*100));
        ((RadioButton)light_prop.findViewById(R.id.point_light)).setOnCheckedChangeListener(this);
        ((RadioButton)light_prop.findViewById(R.id.directional_light)).setOnCheckedChangeListener(this);
        lightType = GL2JNILib.getLightType();
        ((RadioButton)light_prop.findViewById(R.id.directional_light)).setChecked(lightType == Constants.DIRECTIONAL);
        ((RadioButton)light_prop.findViewById(R.id.point_light)).setChecked(lightType == Constants.POINT);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {

        touchBegan = true;
        float eventX = event.getX();
        float eventY = event.getY();
        float[] eventXY = new float[]{eventX, eventY};
        Matrix invertMatrix = new Matrix();
        ((ImageView) v).getImageMatrix().invert(invertMatrix);
        invertMatrix.mapPoints(eventXY);
        int x = (int) eventXY[0];
        int y = (int) eventXY[1];
        //Limit x, y range within bitmap
        if (x < 0) {
            x = 0;
        } else if (x > (bitmap.getWidth() - 1)) {
            x = (bitmap.getWidth() - 1);
        }
        if (y < 0) {
            y = 0;
        } else if (y > (bitmap.getHeight() - 1)) {
            y = (bitmap.getHeight() - 1);
        }
        touchedRGB = bitmap.getPixel(x, y);
        colorPreview.setBackgroundColor(touchedRGB);
        update(false);
        if(event.getAction() == MotionEvent.ACTION_UP)
            update(false);
        return true;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if(!defaultValueInitialized) return;
        if(touchBegan)
            update(false);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        update(false);
    }

    @Override
    public void onClick(View v) {
        if(v.getId() == R.id.delete_animation) {
            ((EditorView) ((Activity) mContext)).delete.showDelete();
            this.dialog.dismiss();
        }
    }

    private void update(boolean status)
    {
        if(!defaultValueInitialized) return;
        final boolean storeAction = status;
        final int red = Color.red(touchedRGB);
        final int green = Color.green(touchedRGB);
        final int blue = Color.blue(touchedRGB);
        ((EditorView)mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.changeLightProperty(red/255.0f,green/255.0f,blue/255.0f,((SeekBar)dialog.findViewById(R.id.shadow_darkness)).getProgress()/100.0f,((SeekBar)dialog.findViewById(R.id.distance)).getProgress()/100.0f,lightType,storeAction);
            }
        });

    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(!isChecked) return;
        switch (buttonView.getId()){
            case R.id.point_light:
                lightType = 0;
                update(false);
                update(true);
                break;
            case R.id.directional_light:
                update(false);
                update(true);
                lightType = 1;
                break;
        }
    }
}
