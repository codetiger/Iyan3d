package com.smackall.animator.Helper.Listeners;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.support.v7.widget.SwitchCompat;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.Animation;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;

import com.smackall.animator.ColorPicker;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CustomViews.CustomRadioButton;
import com.smackall.animator.Helper.Property;
import com.smackall.animator.Props;
import com.smackall.animator.TextureSelection;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by Sabish.M on 27/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CustomListeners implements SeekBar.OnSeekBarChangeListener, View.OnClickListener,SwitchCompat.OnCheckedChangeListener, Animation.AnimationListener, View.OnTouchListener
,RadioGroup.OnCheckedChangeListener{

    boolean forAnimation = false;
    private Property property;
    private ArrayList<Property> propertyMap;
    private int position;
    private Context mContext;
    private Props props;
    //Start Animation
    private Animation animation;
    private View view;
    //For Color Picker
    private Bitmap bitmap;
    private LinearLayout preview;

    public CustomListeners(Property property, int position, Context mContext, Props props) {
        this.property = property;
        this.position = position;
        this.mContext = mContext;
        this.props = props;
    }
    public CustomListeners(ArrayList<Property> propertyMap,Context mContext, Props props) {
        this.propertyMap = propertyMap;
        this.mContext = mContext;
        this.props = props;
    }

    public CustomListeners(Animation animation, View view, boolean forAnimation, Props props) {
        this.animation = animation;
        this.view = view;
        this.forAnimation = forAnimation;
        this.props = props;
    }

    public CustomListeners(Property property, int position, Context mContext, Props props, Bitmap bitmap, LinearLayout preview) {
        this.props = props;
        this.bitmap = bitmap;
        this.preview = preview;
        this.property = property;
        this.position = position;
        this.mContext = mContext;
    }

    private void dismissDialog() {
        if (props == null || props.dialog == null) return;

        if (props.dialog.isShowing())
            props.dialog.dismiss();
    }

    @Override
    public void onClick(View v) {
        if (forAnimation) {
            animation.setAnimationListener(this);
            view.clearAnimation();
            view.setAnimation(animation);
            view.animate().start();
        } else {
            switch (property.index) {
                case Constants.VERTEX_COLOR: {
                    HashMap<Integer, Property> temp = new HashMap<>();
                    temp.put(property.index, property);
                    props.getPropsView(temp, true, true);
                    return;
                }
            }
            GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,property.index,property.valueX,property.valueY,property.valueZ,property.valueW,false);
            dismissDialog();
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        float value = (isChecked) ? 1.0f : 0.0f;
        GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,property.index,value,property.valueY,property.valueZ,property.valueW,false);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,property.index,progress,property.valueY,property.valueZ,property.valueW,false);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (seekBar.getProgress() > 5)
            seekBar.setMax((property.valueW == 1 && seekBar.getProgress() > seekBar.getMax() / 2) ? seekBar.getProgress() * 2 : seekBar.getMax());
    }

    @Override
    public void onAnimationStart(Animation animation) {

    }

    @Override
    public void onAnimationEnd(Animation animation) {
        props.props_frame.removeView(view);
    }

    @Override
    public void onAnimationRepeat(Animation animation) {

    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (property.type == Constants.COLOR_TYPE) {
            switch (property.index) {
                case Constants.VERTEX_COLOR:
                    int color = props.editorView.colorPicker.getColorFromPixel(v, event, bitmap);
                    preview.setBackgroundColor(color);
                    GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,
                    property.index, Color.red(color)/255.0f,Color.green(color)/255.0f,Color.blue(color)/255.0f,1.0f,(event.getAction() == MotionEvent.ACTION_UP));
                    break;
            }
        }
        return true;
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
        int position = ((CustomRadioButton)group.findViewById(checkedId)).getPosition();
        Property property = propertyMap.get(position);
        GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,property.index,1.0f,property.valueY,property.valueZ,property.valueW,false);
        GL2JNILib.changedPropertyAtIndex(props.editorView.nativeCallBacks,property.index,1.0f,property.valueY,property.valueZ,property.valueW,true);
    }
}
