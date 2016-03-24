package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;

import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ColorPicker implements View.OnTouchListener {

    Context mContext;
    LinearLayout colorPreview;
    Bitmap bitmap;
    private int ViewType;

    public ColorPicker(Context mContext)
    {
        this.mContext = mContext;
    }

    public void showColorPicker(View v,MotionEvent event,int ViewType)
    {
        this.ViewType = ViewType;
        Dialog color_prop = new Dialog(mContext);
        color_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        color_prop.setContentView(R.layout.color_picker);
        color_prop.setCancelable(false);
        color_prop.setCanceledOnTouchOutside(true);
        color_prop.getWindow().setLayout(Constants.width / 3, Constants.height/2);
        Window window = color_prop.getWindow();
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
        ImageView color_picker = (ImageView)color_prop.findViewById(R.id.color_picker);
        bitmap = ((BitmapDrawable)color_picker.getDrawable()).getBitmap();
        initViews(color_prop);
        color_prop.show();
    }

    private void initViews(Dialog color_prop)
    {
        ((ImageView)color_prop.findViewById(R.id.color_picker)).setOnTouchListener(this);
        colorPreview = (LinearLayout)color_prop.findViewById(R.id.color_preview);
    }


    @Override
    public boolean onTouch(View v, MotionEvent event) {
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
        int touchedRGB = bitmap.getPixel(x, y);
        int red = Color.red(touchedRGB);
        int green = Color.green(touchedRGB);
        int blue = Color.blue(touchedRGB);
        colorPreview.setBackgroundColor(touchedRGB);
        if(ViewType == Constants.TEXT_VIEW){
            ((EditorView)((Activity)mContext)).textSelection.textDB.setRed(red/255.0f);
            ((EditorView)((Activity)mContext)).textSelection.textDB.setGreen(green / 255.0f);
            ((EditorView)((Activity)mContext)).textSelection.textDB.setBlue(blue / 255.0f);
            ((EditorView)((Activity)mContext)).textSelection.textDB.setTempNode(true);
            if(event.getAction() == MotionEvent.ACTION_UP)
                ((EditorView)((Activity)mContext)).textSelection.importText();
        }
        else if(ViewType == Constants.TEXTURE_MODE){
            ((EditorView)((Activity)mContext)).objSelection.objSelectionAdapter.assetsDB.setx(red / 255.0f);
            ((EditorView)((Activity)mContext)).objSelection.objSelectionAdapter.assetsDB.setY(green / 255.0f);
            ((EditorView)((Activity)mContext)).objSelection.objSelectionAdapter.assetsDB.setZ(blue / 255.0f);
            ((EditorView)((Activity)mContext)).objSelection.objSelectionAdapter.assetsDB.setTexture("-1");
            if(event.getAction() == MotionEvent.ACTION_UP)
                ((EditorView)((Activity)mContext)).objSelection.objSelectionAdapter.importOBJ();
        }
        else if(ViewType == Constants.CHANGE_TEXTURE_MODE){
            ((EditorView)((Activity)mContext)).textureSelection.assetsDB.setx(red/255.0f);
            ((EditorView)((Activity)mContext)).textureSelection.assetsDB.setY(green / 255.0f);
            ((EditorView)((Activity)mContext)).textureSelection.assetsDB.setZ(blue / 255.0f);
            ((EditorView)((Activity)mContext)).textureSelection.assetsDB.setTexture("-1");
            ((EditorView)((Activity)mContext)).textureSelection.changeTextureAdapter.notifyDataSetChanged();
            ((EditorView) ((Activity) mContext)).textureSelection.assetsDB.setIsTempNode(true);
            if(event.getAction() == MotionEvent.ACTION_UP)
                ((EditorView)((Activity)mContext)).textureSelection.changeTexture();
        }

        return true;
    }
}
