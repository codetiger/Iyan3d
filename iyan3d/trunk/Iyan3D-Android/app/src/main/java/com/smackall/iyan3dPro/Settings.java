package com.smackall.iyan3dPro;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Build;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.Switch;

import com.android.vending.billing.IInAppBillingService;
import com.smackall.iyan3dPro.Analytics.HitScreens;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.DescriptionManager;
import com.smackall.iyan3dPro.Helper.SharedPreferenceManager;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.OverlayDialogs.HelpDialogs;

import java.util.HashMap;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Settings implements CompoundButton.OnCheckedChangeListener, View.OnClickListener {

    public IInAppBillingService mService;
    Context mContext;
    SharedPreferenceManager sp;
    RadioButton toolbar_left, toolbar_right, preview_small, preview_large, frame_count, frame_duration, preview_left_bottom, preview_left_top, preview_right_bottom, preview_right_top;
    Switch multi_select;
    Button done_btn;
    ImageView img_toolbar_left, img_toolbar_right, img_preview_small, img_preview_large, preview_left_bottom_btn, preview_left_top_btn, preview_right_bottom_btn, preview_right_top_btn;
    LinearLayout frame_count_btn, frame_duration_btn;
    boolean Ui;
    HashMap<String, String> map;

    float divideValue = 0.0f;

    public Settings(Context context, SharedPreferenceManager sp, IInAppBillingService service) {
        this.mContext = context;
        this.sp = sp;
        this.mService = service;

    }

    public static void informDialog(final Context context, final String msg) {
        ((Activity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                if (informDialog != null)
                                    informDialog.dismiss();
                                try {
                                    String className = context.getClass().getSimpleName();
                                    if (className.toLowerCase().equals("sceneselection")) {
                                        ((SceneSelection) context).settings.done_btn.performClick();
                                    } else {
                                        ((EditorView) context).settings.done_btn.performClick();
                                    }
                                } catch (ClassCastException ignored) {
                                    return;
                                }
                            }
                        });
                informDialog.create();
                try {
                    informDialog.show();
                } catch (WindowManager.BadTokenException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void showSettings() {
        Constants.VIEW_TYPE = Constants.SETTINGS_VIEW;
        HitScreens.SettingsView(mContext);
        if (map != null) map.clear();
        final Dialog dialog = new Dialog(mContext);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.setContentView(R.layout.settings_view);

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                dialog.getWindow().setLayout(Constants.width, Constants.height);
                break;
            default:
                divideValue = 1.5f;
                dialog.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height);
                break;
        }

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
        multi_select.setOnCheckedChangeListener(this);

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


        final String className = mContext.getClass().getSimpleName();
        done_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                String className = mContext.getClass().getSimpleName();
                if (!className.toLowerCase().equals("sceneselection")) {
                    ((EditorView) mContext).swapViews();
                    ((EditorView) mContext).renderManager.cameraPosition(Constants.height - ((Activity) mContext).findViewById(R.id.glView).getHeight());
                    if (Constants.VIEW_TYPE != Constants.AUTO_RIG_VIEW)
                        Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                }
                if (dialog != null && dialog.isShowing())
                    dialog.dismiss();
            }
        });

        settingsBtnHandler();
        dialog.show();
        final DescriptionManager descriptionManager = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection) (mContext)).descriptionManager : ((EditorView) (mContext)).descriptionManager);
        final HelpDialogs helpDialogs = ((className.toLowerCase().equals("sceneselection")) ? ((SceneSelection) (mContext)).helpDialogs : ((EditorView) (mContext)).helpDialogs);
        dialog.findViewById(R.id.help).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.findViewById(R.id.viewGroup).bringToFront();
                descriptionManager.addSettingsViewDescriptions(mContext, dialog);
                helpDialogs.showPop(mContext, ((ViewGroup) dialog.findViewById(R.id.viewGroup)), (divideValue > 0) ? dialog.findViewById(R.id.viewGroup).getWidth() : 0);
            }
        });

        dialog.findViewById(R.id.viewGroup).bringToFront();
        dialog.findViewById(R.id.viewGroup).getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            public void onGlobalLayout() {
                if (Build.VERSION.SDK_INT < 16) {
                    dialog.findViewById(R.id.viewGroup).getViewTreeObserver().removeGlobalOnLayoutListener(this);
                } else {
                    dialog.findViewById(R.id.viewGroup).getViewTreeObserver().removeOnGlobalLayoutListener(this);
                }
                if (sp.getInt(mContext, "firstTimeUserForSetting") == 0) {
                    sp.setData(mContext, "firstTimeUserForSetting", 1);
                    descriptionManager.addSettingsViewDescriptions(mContext, dialog);
                    helpDialogs.showPop(mContext, (ViewGroup) dialog.getWindow().getDecorView(), (divideValue > 0) ? dialog.findViewById(R.id.viewGroup).getWidth() : 0);
                }
            }
        });
    }

    private void initButtons(Dialog dialog) {
        toolbar_left = (RadioButton) dialog.findViewById(R.id.toolBar_left_radio);
        toolbar_right = (RadioButton) dialog.findViewById(R.id.toolBar_Right_radio);
        preview_small = (RadioButton) dialog.findViewById(R.id.preview_small_radio);
        preview_large = (RadioButton) dialog.findViewById(R.id.preview_Large_radio);
        frame_count = (RadioButton) dialog.findViewById(R.id.frame_count_radio);
        frame_duration = (RadioButton) dialog.findViewById(R.id.frame_duration_radio);
        preview_left_bottom = (RadioButton) dialog.findViewById(R.id.preview_left_bottom_radio);
        preview_left_top = (RadioButton) dialog.findViewById(R.id.preview_left_top_radio);
        preview_right_bottom = (RadioButton) dialog.findViewById(R.id.preview_right_bottom_radio);
        preview_right_top = (RadioButton) dialog.findViewById(R.id.preview_right_top_radio);
        multi_select = (Switch) dialog.findViewById(R.id.mutiSelect_switch);

        img_toolbar_left = (ImageView) dialog.findViewById(R.id.img_toolbar_left);
        img_toolbar_right = (ImageView) dialog.findViewById(R.id.img_toobar_right);
        img_preview_small = (ImageView) dialog.findViewById(R.id.img_preview_small);
        img_preview_large = (ImageView) dialog.findViewById(R.id.img_preview_large);
        preview_left_bottom_btn = (ImageView) dialog.findViewById(R.id.img_left_bottom_btn);
        preview_left_top_btn = (ImageView) dialog.findViewById(R.id.img_left_top_tn);
        preview_right_bottom_btn = (ImageView) dialog.findViewById(R.id.img_right_bottom_btn);
        preview_right_top_btn = (ImageView) dialog.findViewById(R.id.img_right_top_btn);
        frame_count_btn = (LinearLayout) dialog.findViewById(R.id.frame_count_btn);
        frame_duration_btn = (LinearLayout) dialog.findViewById(R.id.frame_duration_btn);
        done_btn = (Button) dialog.findViewById(R.id.settings_done_btn);
    }

    private void settingsBtnHandler() {
        Ui = true;
        int toolBarPosition = this.sp.getInt(mContext, "toolbarPosition");
        int frameType = this.sp.getInt(mContext, "frameType");
        int previewPosition = this.sp.getInt(mContext, "previewPosition");
        int multiSelect = this.sp.getInt(mContext, "multiSelect");
        int previewSize = this.sp.getInt(mContext, "previewSize");


        toolbar_left.setChecked((toolBarPosition == Constants.TOOLBAR_LEFT));
        toolbar_right.setChecked((toolBarPosition == Constants.TOOLBAR_RIGHT));

        preview_small.setChecked((previewSize == Constants.PREVIEW_SMALL));
        preview_large.setChecked((previewSize == Constants.PREVIEW_LARGE));

        frame_count.setChecked((frameType == Constants.FRAME_COUNT));
        frame_duration.setChecked((frameType == Constants.FRAME_DURATION));

        preview_left_bottom.setChecked((previewPosition == Constants.PREVIEW_LEFT_BOTTOM));
        preview_left_top.setChecked((previewPosition == Constants.PREVIEW_LEFT_TOP));
        preview_right_bottom.setChecked((previewPosition == Constants.PREVIEW_RIGHT_BOTTOM));
        preview_right_top.setChecked((previewPosition == Constants.PREVIEW_RIGHT_TOP));

        multi_select.setChecked((multiSelect == 1));
        Ui = false;
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (Ui) return;
        int toolBarPreviewPosition = this.sp.getInt(mContext, "previewPosition");
        switch (buttonView.getId()) {
            case R.id.toolBar_left_radio:
                sp.setData(mContext, "toolbarPosition", (isChecked) ? 0 : 1);
                break;
            case R.id.toolBar_Right_radio:
                sp.setData(mContext, "toolbarPosition", (isChecked) ? 1 : 0);
                break;
            case R.id.preview_small_radio:
                sp.setData(mContext, "previewSize", (isChecked) ? 0 : 1);
                break;
            case R.id.preview_Large_radio:
                sp.setData(mContext, "previewSize", (isChecked) ? 1 : 0);
                break;
            case R.id.frame_count_radio:
                this.sp.setData(mContext, "frameType", (isChecked) ? 0 : 1);
                break;
            case R.id.frame_duration_radio:
                this.sp.setData(mContext, "frameType", (isChecked) ? 1 : 0);
                break;
            case R.id.preview_left_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 0 : toolBarPreviewPosition);
                break;
            case R.id.preview_left_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 1 : toolBarPreviewPosition);
                break;
            case R.id.preview_right_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 2 : toolBarPreviewPosition);
                break;
            case R.id.preview_right_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 3 : toolBarPreviewPosition);
                break;
            case R.id.mutiSelect_switch:
                this.sp.setData(mContext, "multiSelect", multi_select.isChecked() ? 1 : 0);
                break;
        }
        settingsBtnHandler();
    }

    @Override
    public void onClick(View v) {
        int toolBarPreviewPosition = this.sp.getInt(mContext, "previewPosition");
        switch (v.getId()) {
            case R.id.img_toolbar_left:
                sp.setData(mContext, "toolbarPosition", 0);
                break;
            case R.id.img_toobar_right:
                sp.setData(mContext, "toolbarPosition", 1);
                break;
            case R.id.img_preview_small:
                sp.setData(mContext, "previewSize", 0);
                break;
            case R.id.img_preview_large:
                sp.setData(mContext, "previewSize", 1);
                break;
            case R.id.frame_count_btn:
                this.sp.setData(mContext, "frameType", 0);
                break;
            case R.id.frame_duration_btn:
                this.sp.setData(mContext, "frameType", 1);
                break;
            case R.id.img_left_bottom_btn:
                this.sp.setData(mContext, "previewPosition", 0);
                break;
            case R.id.img_left_top_tn:
                this.sp.setData(mContext, "previewPosition", 1);
                break;
            case R.id.img_right_bottom_btn:
                this.sp.setData(mContext, "previewPosition", 2);
                break;
            case R.id.img_right_top_btn:
                this.sp.setData(mContext, "previewPosition", 3);
                break;
        }
        settingsBtnHandler();
    }
}

