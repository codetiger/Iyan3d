package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class EnvelopScale implements SeekBar.OnSeekBarChangeListener {

    private Context mContext;
    private Dialog envelop_scale;
    public EnvelopScale(Context mContext){
        this.mContext = mContext;
    }

    public void showEnvelopScale(View v,MotionEvent event)
    {
        Dialog envelop_scale = new Dialog(mContext);
        envelop_scale.requestWindowFeature(Window.FEATURE_NO_TITLE);
        envelop_scale.setContentView(R.layout.envelop_scale);
        envelop_scale.setCancelable(false);
        envelop_scale.setCanceledOnTouchOutside(true);
        envelop_scale.getWindow().setLayout(Constants.width / 2, Constants.height/4);
        Window window = envelop_scale.getWindow();
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
        this.envelop_scale = envelop_scale;
        initViews(envelop_scale);
        setStartingValue(0.2f);
        envelop_scale.show();
    }

    private void initViews(Dialog envelop_scale){
        ((SeekBar)envelop_scale.findViewById(R.id.envelop_bar)).setOnSeekBarChangeListener(this);
    }

    private void setStartingValue(float value)
    {
        ((SeekBar)envelop_scale.findViewById(R.id.envelop_bar)).setMax(((int)value*10.0f) > 10 ? (int) (((int) value * 10.0f) * 2) : 10);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if(progress < 1)
            seekBar.setProgress(1);
        updateText();
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

    private void updateText()
    {
        ((TextView)envelop_scale.findViewById(R.id.envelop_value)).setText(String.valueOf((float) ((SeekBar) envelop_scale.findViewById(R.id.envelop_bar)).getProgress() / 10.0f));
    }

    private void setMax(SeekBar seekBar)
    {
        int value = seekBar.getProgress();
        if(value > 5)
            seekBar.setMax(value*2);
    }
}
