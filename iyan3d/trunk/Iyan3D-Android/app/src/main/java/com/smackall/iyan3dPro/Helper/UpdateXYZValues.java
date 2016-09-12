package com.smackall.iyan3dPro.Helper;

import android.app.Activity;
import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.widget.TextView;

import com.smackall.iyan3dPro.R;

import java.util.Locale;

/**
 * Created by Sabish.M on 27/5/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UpdateXYZValues {

    boolean status;
    float xValue, yValue, zValue;
    private Context mContext;

    public UpdateXYZValues(Context mContext) {
        this.mContext = mContext;
    }

    public void updateXyzValue(boolean hide, float x, float y, float z) {
        status = hide;
        xValue = x;
        yValue = y;
        zValue = z;
        if (x == -999.0 && y == -999.0 & z == -999.0)
            status = true;
        ((Activity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((Activity) mContext).findViewById(R.id.mesurmentLable).setVisibility((status) ? View.INVISIBLE : View.VISIBLE);
                ((TextView) ((Activity) mContext).findViewById(R.id.mesurmentLable)).setGravity(Gravity.CENTER);
                String value = "X : ";
                value += String.format(Locale.getDefault(), "%.1f", xValue);
                value += " Y : ";
                value += String.format(Locale.getDefault(), "%.1f", yValue);
                value += " Z : ";
                value += String.format(Locale.getDefault(), "%.1f", zValue);
                ((TextView) ((Activity) mContext).findViewById(R.id.mesurmentLable)).setText(value);
            }
        });
    }
}
