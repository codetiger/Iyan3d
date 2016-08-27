package com.smackall.animator.Helper.CustomViews;

import android.content.Context;
import android.support.v7.widget.AppCompatRadioButton;

/**
 * Created by Sabish.M on 24/8/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CustomRadioButton extends AppCompatRadioButton {

    int position;
    public CustomRadioButton(Context context) {
        super(context);

    }

    public void setPosition(int position){
        this.position = position;
    }

    public int getPosition()
    {
        return position;
    }

}
