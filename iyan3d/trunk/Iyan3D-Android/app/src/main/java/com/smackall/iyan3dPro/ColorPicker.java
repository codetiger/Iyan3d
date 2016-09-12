package com.smackall.iyan3dPro;

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

import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.UIHelper;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ColorPicker implements View.OnTouchListener {

    Context mContext;
    LinearLayout colorPreview;
    Bitmap bitmap;
    private int ViewType;

    public ColorPicker(Context mContext) {
        this.mContext = mContext;
    }

    public void showColorPicker(View v, MotionEvent event, int ViewType) {
        this.ViewType = ViewType;
        Dialog color_prop = new Dialog(mContext);
        color_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        color_prop.setContentView(R.layout.color_picker);
        color_prop.setCancelable(false);
        color_prop.setCanceledOnTouchOutside(true);

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                color_prop.getWindow().setLayout(Constants.width / 2, Constants.height);
                break;
            default:
                color_prop.getWindow().setLayout(Constants.width / 3, Constants.height / 2);
                break;
        }

        Window window = color_prop.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.START;
        wlp.dimAmount = 0.0f;
        if (event != null) {
            wlp.x = (int) event.getX();
            wlp.y = (int) event.getY();
        } else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        ImageView color_picker = (ImageView) color_prop.findViewById(R.id.color_picker);
        bitmap = ((BitmapDrawable) color_picker.getDrawable()).getBitmap();
        initViews(color_prop);
        color_prop.show();
    }

    private void initViews(Dialog color_prop) {
        color_prop.findViewById(R.id.color_picker).setOnTouchListener(this);
        colorPreview = (LinearLayout) color_prop.findViewById(R.id.color_preview);
    }


    @Override
    public boolean onTouch(View v, MotionEvent event) {

        changeColor(v, event, bitmap, colorPreview);

        return true;
    }

    public void changeColor(View v, MotionEvent event, Bitmap bitmap, LinearLayout colorPreview) {
        int touchedRGB = getColorFromPixel(v, event, bitmap);
        colorPreview.setBackgroundColor(touchedRGB);
        int red = Color.red(touchedRGB);
        int green = Color.green(touchedRGB);
        int blue = Color.blue(touchedRGB);
        colorPreview.setBackgroundColor(touchedRGB);
        switch (ViewType) {
            case Constants.TEXT_VIEW:
                ((EditorView) mContext).textSelection.textDB.setRed(red / 255.0f);
                ((EditorView) mContext).textSelection.textDB.setGreen(green / 255.0f);
                ((EditorView) mContext).textSelection.textDB.setBlue(blue / 255.0f);
                ((EditorView) mContext).textSelection.textDB.setTempNode(true);
                if (event.getAction() == MotionEvent.ACTION_UP)
                    ((EditorView) mContext).textSelection.importText();
                break;
            case Constants.TEXTURE_MODE:
                break;
            case Constants.CHANGE_TEXTURE_MODE:
                ((EditorView) mContext).textureSelection.assetsDB.setX(red / 255.0f);
                ((EditorView) mContext).textureSelection.assetsDB.setY(green / 255.0f);
                ((EditorView) mContext).textureSelection.assetsDB.setZ(blue / 255.0f);
                ((EditorView) mContext).textureSelection.assetsDB.setTexture("-1");
                ((EditorView) mContext).textureSelection.changeTextureAdapter.notifyDataSetChanged();
                ((EditorView) mContext).textureSelection.assetsDB.setIsTempNode(true);
                if (event.getAction() == MotionEvent.ACTION_UP)
                    ((EditorView) mContext).textureSelection.changeTexture(false);
                break;
            case Constants.RENDERING_VIEW:
                if (event.getAction() == MotionEvent.ACTION_UP)
                    ((EditorView) mContext).export.updateColor(red / 255.0f, green / 255.0f, blue / 255.0f);
                break;
        }
    }

    public int getColorFromPixel(View v, MotionEvent event, Bitmap bitmap) {
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
        return bitmap.getPixel(x, y);
    }
}
