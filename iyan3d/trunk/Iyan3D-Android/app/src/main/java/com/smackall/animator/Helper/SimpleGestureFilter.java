package com.smackall.animator.Helper;

import android.app.Activity;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.View;

/**
 * Created by Sabish.M on 10/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class SimpleGestureFilter extends SimpleOnGestureListener{

    public final static int SWIPE_UP    = 1;
    public final static int SWIPE_DOWN  = 2;
    public final static int SWIPE_LEFT  = 3;
    public final static int SWIPE_RIGHT = 4;

    public final static int MODE_TRANSPARENT = 0;
    public final static int MODE_SOLID       = 1;
    public final static int MODE_DYNAMIC     = 2;

    private final static int ACTION_FAKE = -13; //just an unlikely number
    private int swipe_Min_Distance = 100;
    private int swipe_Max_Distance = 300;
    private int swipe_Min_Velocity = 100;

    private int mode             = MODE_DYNAMIC;
    private boolean running      = true;
    private boolean tapIndicator = false;
    private int position;
    private View view;

    private Activity context;
    private GestureDetector detector;
    private SimpleGestureListener listener;

    public SimpleGestureFilter(Activity context,SimpleGestureListener sgl) {

        this.context = context;
        this.detector = new GestureDetector(context, this);
        this.listener = sgl;
    }

    public void onTouchEvent(MotionEvent event,int position,View view){

        if(!this.running)
            return;

        this.position = position;
        this.view = view;
        boolean result = this.detector.onTouchEvent(event);

        if(this.mode == MODE_SOLID)
            event.setAction(MotionEvent.ACTION_CANCEL);
        else if (this.mode == MODE_DYNAMIC) {

            if(event.getAction() == ACTION_FAKE)
                event.setAction(MotionEvent.ACTION_UP);
            else if (result)
                event.setAction(MotionEvent.ACTION_CANCEL);
            else if(this.tapIndicator){
                event.setAction(MotionEvent.ACTION_DOWN);
                this.tapIndicator = false;
            }

        }
        //else just do nothing, it's Transparent
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                           float velocityY) {

        final float xDistance = Math.abs(e1.getX() - e2.getX());
        final float yDistance = Math.abs(e1.getY() - e2.getY());

        if(xDistance > this.swipe_Max_Distance || yDistance > this.swipe_Max_Distance)
            return false;

        velocityX = Math.abs(velocityX);
        velocityY = Math.abs(velocityY);
        boolean result = false;

        if(velocityX > this.swipe_Min_Velocity && xDistance > this.swipe_Min_Distance){
            if(e1.getX() > e2.getX()) // right to left
                this.listener.onSwipe(SWIPE_LEFT,this.position,this.view);
            else
                this.listener.onSwipe(SWIPE_RIGHT,this.position,this.view);

            result = true;
        }
        else if(velocityY > this.swipe_Min_Velocity && yDistance > this.swipe_Min_Distance){
            if(e1.getY() > e2.getY()) // bottom to up
                this.listener.onSwipe(SWIPE_UP,this.position,this.view);
            else
                this.listener.onSwipe(SWIPE_DOWN,this.position,this.view);

            result = true;
        }

        return result;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        this.tapIndicator = true;
        this.listener.onSingleTap(this.position);
        return false;
    }

    @Override
    public boolean onDoubleTap(MotionEvent arg) {
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent arg) {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent arg) {

        if(this.mode == MODE_DYNAMIC){        // we owe an ACTION_UP, so we fake an
            arg.setAction(ACTION_FAKE);      //action which will be converted to an ACTION_UP later.
            this.context.dispatchTouchEvent(arg);
        }

        return false;
    }

    public static interface SimpleGestureListener{
        void onSwipe(int direction, int position, View view);
        void onSingleTap(int position);
    }
}