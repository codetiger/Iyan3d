package com.smackall.animator.Helper.CustomViews;

import android.content.Context;
import android.support.v7.widget.AppCompatSeekBar;
import android.util.AttributeSet;
import android.widget.SeekBar;

import com.smackall.animator.Helper.Listeners.CustomSeekBarListener;

/**
 * Created by Sabish.M on 30/8/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CustomSeekBar extends AppCompatSeekBar implements AppCompatSeekBar.OnSeekBarChangeListener {

    public boolean sliderMoving = false;
    public boolean isDynamic = true;
    float min = 0.0f;
    float max = 1.0f;
    CustomSeekBarListener listener;
    float offset;

    public CustomSeekBar(Context context) {
        super(context);
    }

    public CustomSeekBar(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CustomSeekBar(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public void setCustomProgress(float value, boolean fromUser) {
        setProgress((int) (value * 100.0f));
        this.onProgressChanged(this, getProgress(), fromUser);
    }

    public void setOnSeekBarChangeListener(CustomSeekBarListener l) {
        this.listener = l;
        setOnSeekBarChangeListener(this);
    }

    public float getOffset() {
        return offset;
    }

    public void setOffset(float offset) {
        this.offset = offset;
    }

    public float getMin() {
        return this.min;
    }

    public void setMin(float value) {
        this.min = value;
        this.onProgressChanged(this, getProgress(), true);
    }

    public float getCustomProgress() {
        return (getProgress() / 100.0f) + min;
    }

    public float getCustomMax() {
        return this.max;
    }

    public void setCustomMax(float value) {
        this.max = value;
        this.setMax((int) (value * 100.0f));
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        float customProgress = (progress / 100.0f) + min;
        if (isDynamic && customProgress <= 0.0f) {
            setCustomProgress(0.1f, true);
            return;
        }
        if (fromUser)
            this.listener.onProgressChanged((CustomSeekBar) seekBar, customProgress, true);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        this.listener.onStartTrackingTouch((CustomSeekBar) seekBar);
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        this.listener.onStopTrackingTouch((CustomSeekBar) seekBar);
    }
}
