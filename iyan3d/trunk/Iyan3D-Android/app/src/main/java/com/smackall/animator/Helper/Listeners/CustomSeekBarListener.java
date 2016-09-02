package com.smackall.animator.Helper.Listeners;

import com.smackall.animator.Helper.CustomViews.CustomSeekBar;

/**
 * Created by Sabish.M on 30/8/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public interface CustomSeekBarListener {

    public void onProgressChanged(CustomSeekBar seekBar, float progress, boolean fromUser);

    public void onStartTrackingTouch(CustomSeekBar seekBar);

    public void onStopTrackingTouch(CustomSeekBar seekBar);

}
