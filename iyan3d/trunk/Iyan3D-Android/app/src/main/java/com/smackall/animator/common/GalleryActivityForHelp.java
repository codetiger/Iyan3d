package com.smackall.animator.common;

/**
 * Created by Sabish on 31/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.Gallery;

public class GalleryActivityForHelp extends Gallery {
    private boolean stuck = false;
    public GalleryActivityForHelp(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
    public GalleryActivityForHelp(Context context, AttributeSet attrs) {


        super(context, attrs);
    }
    public GalleryActivityForHelp(Context context) {


        super(context);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return stuck || super.onTouchEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                return stuck || super.onKeyDown(keyCode, event);
        }
        return super.onKeyDown(keyCode, event);
    }
    public void setScrollingEnabled(boolean enabled) {
        stuck = !enabled;
    }
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY){
        return super.onFling(e1, e2, 0, velocityY);
    }
}