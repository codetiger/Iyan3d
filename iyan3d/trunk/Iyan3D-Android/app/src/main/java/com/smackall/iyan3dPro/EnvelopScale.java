package com.smackall.iyan3dPro;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

import java.util.Locale;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class EnvelopScale implements SeekBar.OnSeekBarChangeListener {

    boolean defaultValueInitialized = false;
    private Context mContext;
    private Dialog envelop_scale;

    public EnvelopScale(Context mContext) {
        this.mContext = mContext;
    }

    public void showEnvelopScale(View v, MotionEvent event) {
        Dialog envelop_scale = new Dialog(mContext);
        envelop_scale.requestWindowFeature(Window.FEATURE_NO_TITLE);
        envelop_scale.setContentView(R.layout.envelop_scale);
        envelop_scale.setCancelable(false);
        envelop_scale.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                envelop_scale.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height / 2);
                break;
            default:
                envelop_scale.getWindow().setLayout(Constants.width / 2, Constants.height / 4);
                break;
        }

        Window window = envelop_scale.getWindow();
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
        this.envelop_scale = envelop_scale;
        defaultValueInitialized = false;
        initViews(envelop_scale);
        setStartingValue(GL2JNILib.envelopScale());
        defaultValueInitialized = true;
        envelop_scale.show();
    }

    private void initViews(Dialog envelop_scale) {
        ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).setOnSeekBarChangeListener(this);
    }

    private void setStartingValue(float value) {
        int valueForAndroidSlider = (int) (value * 100.0f);
        ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).setMax(valueForAndroidSlider * 2);
        ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).setProgress(valueForAndroidSlider);
        updateText();
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (!defaultValueInitialized) return;
        if (progress < 1)
            seekBar.setProgress(1);
        updateText();
        updateScale();
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        updateText();
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        updateText();
        setMax(seekBar);
    }

    private void updateText() {
        String value = String.format(Locale.getDefault(), "%.2f", (float) ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).getProgress() / 100.0f);
        ((TextView) envelop_scale.findViewById(R.id.envelop_value)).setText(value);
    }

    private void setMax(SeekBar seekBar) {
        int value = seekBar.getProgress();
        if (value > 5)
            seekBar.setMax(value * 2);
    }

    private void updateScale() {
        if (!defaultValueInitialized) return;
        float x = ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).getProgress() / 100.0f;
        ((EditorView) mContext).renderManager.changeEnvelopScale(x);
    }
}
