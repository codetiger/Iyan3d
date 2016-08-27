package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.support.v4.view.GestureDetectorCompat;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;

import com.smackall.animator.EditorView;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.Calendar;

/**
 * Created by Sabish.M on 14/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TouchControl implements View.OnTouchListener, GestureDetector.OnGestureListener {

    long startClickTime, clickDuration;
    boolean swiping = false;
    int panBegan = 0;
    int moveBegan = 0;
    VelocityTracker velocityTracker;
    int fingerCount = 0;
    private Context mContext;
    private GestureDetectorCompat mDetector;

    public TouchControl(Context mContext) {
        this.mContext = mContext;
        velocityTracker = VelocityTracker.obtain();
        mDetector = new GestureDetectorCompat(mContext, this);
        initView();
    }

    private void initView() {
        ((Activity) mContext).findViewById(R.id.glView).setOnTouchListener(this);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (!((EditorView) mContext).isDisplayPrepared)
            return true;
        this.mDetector.onTouchEvent(event);
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                fingerCount = event.getPointerCount();
                startClickTime = Calendar.getInstance().getTimeInMillis();
                ((EditorView) mContext).renderManager.touchBegan(event);
                return true;
            case MotionEvent.ACTION_UP:
                clickDuration = Calendar.getInstance().getTimeInMillis() - startClickTime;
                if (fingerCount == 1 && clickDuration < 200) {
                    ((EditorView) mContext).renderManager.checktapposition(event);
                }
                panBegan = 0;
                moveBegan = 0;
                swiping = false;
                ((EditorView) mContext).renderManager.tapEnd(event);
                return true;
            case MotionEvent.ACTION_POINTER_DOWN:
                fingerCount = event.getPointerCount();
                return true;
            case MotionEvent.ACTION_MOVE: {
                fingerCount = event.getPointerCount();
                if (fingerCount == 1) {
                    velocityTracker.addMovement(event);
                    velocityTracker.computeCurrentVelocity(1000);
                    if (moveBegan == 0) {
                        ((EditorView) mContext).renderManager.checkControlSelection(event);
                        moveBegan++;
                    }
                    ((EditorView) mContext).renderManager.tapMove(event);
                    ((EditorView) mContext).renderManager.swipe(velocityTracker);
                    return true;
                } else if (fingerCount == 2) {
                    if (panBegan == 0) {
                        ((EditorView) mContext).renderManager.panBegin(event);
                        panBegan++;
                    } else
                        ((EditorView) mContext).renderManager.panProgress(event);
                    return true;
                }
                break;
            }
        }
        return true;
    }

    @Override
    public boolean onDown(MotionEvent e) {
        return false;
    }

    @Override
    public void onShowPress(MotionEvent e) {
    }

    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        if (GL2JNILib.isPlaying()) GL2JNILib.setIsPlaying(false, null);
        return false;
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        return false;
    }

    @Override
    public void onLongPress(MotionEvent e) {
        if (GL2JNILib.isPlaying()) GL2JNILib.setIsPlaying(false, null);
        if (GL2JNILib.selectedNodeIdsSize() > 0)
            ((EditorView) mContext).showCloneOption(null, e);
        else
            ((EditorView) mContext).popUpManager.initPopUpManager(GL2JNILib.getSelectedNodeId(), null, e);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        return false;
    }
}
