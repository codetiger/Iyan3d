package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.Switch;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.SharedPreferenceManager;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Settings implements CompoundButton.OnCheckedChangeListener , View.OnClickListener {

    Context mContext;
    SharedPreferenceManager sp;

    RadioButton  toolbar_left, toolbar_right,preview_small,preview_large,frame_count,frame_duration,preview_left_bottom,preview_left_top,preview_right_bottom,preview_right_top;
    Switch multi_select;
    Button restore_btn,dont_btn;
    ImageView img_toolbar_left,img_toolbar_right,img_preview_small,img_preview_large,preview_left_bottom_btn,preview_left_top_btn,preview_right_bottom_btn,preview_right_top_btn;
    LinearLayout frame_count_btn,frame_duration_btn;
    boolean Ui;

    public Settings(Context context,SharedPreferenceManager sp){
        this.mContext = context;
        this.sp = sp;
    }

    public void showSettings() {
        final Dialog dialog = new Dialog(mContext);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.setContentView(R.layout.settings_view);
        dialog.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height);
        dialog.setCanceledOnTouchOutside(false);
        initButtons(dialog);
        toolbar_left.setOnCheckedChangeListener(this);
        toolbar_right.setOnCheckedChangeListener(this);
        preview_small.setOnCheckedChangeListener(this);
        preview_large.setOnCheckedChangeListener(this);
        frame_count.setOnCheckedChangeListener(this);
        frame_duration.setOnCheckedChangeListener(this);
        preview_left_bottom.setOnCheckedChangeListener(this);
        preview_left_top.setOnCheckedChangeListener(this);
        preview_right_bottom.setOnCheckedChangeListener(this);
        preview_right_top.setOnCheckedChangeListener(this);

        img_toolbar_left.setOnClickListener(this);
        img_toolbar_right.setOnClickListener(this);
        img_preview_small.setOnClickListener(this);
        img_preview_large.setOnClickListener(this);
        preview_left_bottom_btn.setOnClickListener(this);
        preview_left_top_btn.setOnClickListener(this);
        preview_right_bottom_btn.setOnClickListener(this);
        preview_right_top_btn.setOnClickListener(this);
        frame_count_btn.setOnClickListener(this);
        frame_duration_btn.setOnClickListener(this);
        multi_select.setOnClickListener(this);

        dont_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((EditorView)((Activity)mContext)).swapViews();
                ((EditorView)((Activity)mContext)).renderManager.cameraPosition();
                dialog.dismiss();
            }
        });

        restore_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        settingsBtnHandler();
        dialog.show();
    }

    private void initButtons(Dialog dialog)
    {
        toolbar_left = (RadioButton)dialog.findViewById(R.id.toolBar_left_radio);
        toolbar_right = (RadioButton)dialog.findViewById(R.id.toolBar_Right_radio);
        preview_small = (RadioButton)dialog.findViewById(R.id.preview_small_radio);
        preview_large = (RadioButton)dialog.findViewById(R.id.preview_Large_radio);
        frame_count = (RadioButton)dialog.findViewById(R.id.frame_count_radio);
        frame_duration = (RadioButton)dialog.findViewById(R.id.frame_duration_radio);
        preview_left_bottom = (RadioButton)dialog.findViewById(R.id.preview_left_bottom_radio);
        preview_left_top = (RadioButton)dialog.findViewById(R.id.preview_left_top_radio);
        preview_right_bottom = (RadioButton)dialog.findViewById(R.id.preview_right_bottom_radio);
        preview_right_top = (RadioButton)dialog.findViewById(R.id.preview_right_top_radio);
        multi_select = (Switch)dialog.findViewById(R.id.mutiSelect_switch);

        img_toolbar_left = (ImageView)dialog.findViewById(R.id.img_toolbar_left);
        img_toolbar_right = (ImageView)dialog.findViewById(R.id.img_toobar_right);
        img_preview_small =(ImageView)dialog.findViewById(R.id.img_preview_small);
        img_preview_large =(ImageView)dialog.findViewById(R.id.img_preview_large);
        preview_left_bottom_btn=(ImageView)dialog.findViewById(R.id.img_left_bottom_btn);
        preview_left_top_btn=(ImageView)dialog.findViewById(R.id.img_left_top_tn);
        preview_right_bottom_btn=(ImageView)dialog.findViewById(R.id.img_right_bottom_btn);
        preview_right_top_btn=(ImageView)dialog.findViewById(R.id.img_right_top_btn);
        frame_count_btn = (LinearLayout)dialog.findViewById(R.id.frame_count_btn);
        frame_duration_btn = (LinearLayout)dialog.findViewById(R.id.frame_duration_btn);
        dont_btn = (Button)dialog.findViewById(R.id.settings_done_btn);
        restore_btn = (Button)dialog.findViewById(R.id.restore_btn);

    }

    private void settingsBtnHandler()
    {
        Ui = true;
        int toolBarPosition = this.sp.getInt(mContext,"toolbarPosition");
        int frameType = this.sp.getInt(mContext,"frameType");
        int previewPosition = this.sp.getInt(mContext,"previewPosition");
        int multiSelect = this.sp.getInt(mContext,"multiSelect");
        int previewSize = this.sp.getInt(mContext,"previewSize");


        toolbar_left.setChecked((toolBarPosition == Constants.TOOLBAR_LEFT));
        toolbar_right.setChecked((toolBarPosition == Constants.TOOLBAR_RIGHT));

        preview_small.setChecked((previewSize == Constants.PREVIEW_SMALL));
        preview_large.setChecked((previewSize == Constants.PREVIEW_LARGE));

        frame_count.setChecked((frameType == Constants.FRAME_COUNT) ? true : false);
        frame_duration.setChecked((frameType == Constants.FRAME_DURATION) ? true : false);

        preview_left_bottom.setChecked((previewPosition == Constants.PREVIEW_LEFT_BOTTOM));
        preview_left_top.setChecked((previewPosition == Constants.PREVIEW_LEFT_TOP));
        preview_right_bottom.setChecked((previewPosition == Constants.PREVIEW_RIGHT_BOTTOM));
        preview_right_top.setChecked((previewPosition == Constants.PREVIEW_RIGHT_TOP));

        multi_select.setChecked((multiSelect == 1));
        Ui = false;
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(Ui)return;
        int toolBarPreviewPosition = this.sp.getInt(mContext,"previewPosition");
        switch (buttonView.getId()){
            case R.id.toolBar_left_radio:
                sp.setData(mContext,"toolbarPosition",(isChecked) ? 0 : 1);
                break;
            case R.id.toolBar_Right_radio:
                sp.setData(mContext,"toolbarPosition",(isChecked) ? 1 : 0);
                break;
            case R.id.preview_small_radio:
                sp.setData(mContext,"previewSize",(isChecked) ? 0 : 1);
                break;
            case R.id.preview_Large_radio:
                sp.setData(mContext,"previewSize",(isChecked) ? 1 : 0);
                break;
            case R.id.frame_count_radio:
                this.sp.setData(mContext,"frameType",(isChecked) ? 0 : 1);
                break;
            case R.id.frame_duration_radio:
                this.sp.setData(mContext,"frameType",(isChecked) ? 1 : 0);
                break;
            case R.id.preview_left_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 0 : toolBarPreviewPosition);
                break;
            case R.id.preview_left_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 1 : toolBarPreviewPosition);
                break;
            case R.id.preview_right_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 2 :toolBarPreviewPosition);
                break;
            case R.id.preview_right_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 3 : toolBarPreviewPosition);
                break;
        }
        settingsBtnHandler();
    }

    @Override
    public void onClick(View v) {
        int toolBarPreviewPosition = this.sp.getInt(mContext,"previewPosition");
        switch (v.getId()){
            case R.id.img_toolbar_left:
                sp.setData(mContext,"toolbarPosition",0);
                break;
            case R.id.img_toobar_right:
                sp.setData(mContext,"toolbarPosition",1);
                break;
            case R.id.img_preview_small:
                sp.setData(mContext,"previewSize",0);
                break;
            case R.id.img_preview_large:
                sp.setData(mContext,"previewSize",1);
                break;
            case R.id.frame_count_btn:
                this.sp.setData(mContext,"frameType",0);
                break;
            case R.id.frame_duration_btn:
                this.sp.setData(mContext,"frameType",1);
                break;
            case R.id.img_left_bottom_btn:
                this.sp.setData(mContext,  "previewPosition",0);
                break;
            case R.id.img_left_top_tn:
                this.sp.setData(mContext,  "previewPosition", 1);
                break;
            case R.id.img_right_bottom_btn:
                this.sp.setData(mContext,  "previewPosition", 2);
                break;
            case R.id.img_right_top_btn:
                this.sp.setData(mContext,  "previewPosition", 3);
                break;
            case R.id.mutiSelect_switch:
                this.sp.setData(mContext, "multiSelect", multi_select.isChecked() ? 1 : 0);
                break;
        }
        settingsBtnHandler();
    }
}

