package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.Locale;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CameraProps implements View.OnClickListener,SeekBar.OnSeekBarChangeListener{

    private Context mContext;
    private SharedPreferenceManager sp;
    private TextView fov_lable;
    private Dialog dialog;

    public CameraProps(Context mContext,SharedPreferenceManager sp)
    {
        this.mContext = mContext;
        this.sp = sp;
    }

    public void showCameraProps(View v,MotionEvent event)
    {
        HitScreens.CameraPropsView(mContext);
        Dialog camera_prop = new Dialog(mContext);
        camera_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        camera_prop.setContentView(R.layout.camera_props);
        camera_prop.setCancelable(false);
        camera_prop.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                camera_prop.getWindow().setLayout((int) (Constants.width/1.3), (int) (Constants.height/1.3));
                break;
            default:
                camera_prop.getWindow().setLayout(Constants.width / 2, (int) (Constants.height/2));
                break;
        }

        Window window = camera_prop.getWindow();
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
        dialog = camera_prop;
        initViews(camera_prop);
        camera_prop.show();
        dialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                HitScreens.EditorView(mContext);
            }
        });
    }

    private void initViews(Dialog camera_prop)
    {
        ((RadioButton)camera_prop.findViewById(R.id.THOUSAND_EIGHTY)).setOnClickListener(this);
        ((RadioButton)camera_prop.findViewById(R.id.SEVEN_TWENTY)).setOnClickListener(this);
        ((RadioButton)camera_prop.findViewById(R.id.FOUR_EIGHTY)).setOnClickListener(this);
        ((RadioButton)camera_prop.findViewById(R.id.THREE_SIXTY)).setOnClickListener(this);
        ((RadioButton)camera_prop.findViewById(R.id.TWO_FOURTY)).setOnClickListener(this);
        ((Button)camera_prop.findViewById(R.id.delete)).setOnClickListener(this);
        ((SeekBar)camera_prop.findViewById(R.id.field_of_view)).setOnSeekBarChangeListener(this);
        fov_lable = ((TextView)camera_prop.findViewById(R.id.fov_text));
        setDefaultValues(camera_prop);
    }

    private void setDefaultValues(Dialog camera_prop)
    {
        int cameraResolution = GL2JNILib.resolutionType();
        ((SeekBar)camera_prop.findViewById(R.id.field_of_view)).setProgress((int) (GL2JNILib.cameraFov()));
        ((RadioButton)camera_prop.findViewById(R.id.THOUSAND_EIGHTY)).setChecked((cameraResolution == Constants.THOUSAND_EIGHTY));
        ((RadioButton)camera_prop.findViewById(R.id.SEVEN_TWENTY)).setChecked((cameraResolution == Constants.SEVEN_TWENTY));
        ((RadioButton)camera_prop.findViewById(R.id.FOUR_EIGHTY)).setChecked((cameraResolution == Constants.FOUR_EIGHTY));
        ((RadioButton)camera_prop.findViewById(R.id.THREE_SIXTY)).setChecked((cameraResolution == Constants.THREE_SIXTY));
        ((RadioButton)camera_prop.findViewById(R.id.TWO_FOURTY)).setChecked((cameraResolution == Constants.TWO_FOURTY));
    }

    @Override
    public void onClick(View v) {
        if(v.getId() == R.id.delete){
            ((EditorView)((Activity)mContext)).delete.showDelete();
            this.dialog.dismiss();
        }
        else
        switch (Integer.parseInt(v.getTag().toString())){
            case Constants.THOUSAND_EIGHTY:
               sp.setData(mContext, "cameraResolution", Constants.THOUSAND_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.THOUSAND_EIGHTY)
                update(true);
                break;
            case Constants.SEVEN_TWENTY:
                sp.setData(mContext, "cameraResolution", Constants.SEVEN_TWENTY);
                if(GL2JNILib.resolutionType() != Constants.SEVEN_TWENTY)
                update(true);
                break;
            case Constants.FOUR_EIGHTY:
                sp.setData(mContext, "cameraResolution", Constants.FOUR_EIGHTY);
                if(GL2JNILib.resolutionType() != Constants.FOUR_EIGHTY)
                update(true);
                break;
            case Constants.THREE_SIXTY:
                sp.setData(mContext, "cameraResolution", Constants.THREE_SIXTY);
                if(GL2JNILib.resolutionType() != Constants.THREE_SIXTY)
                update(true);
                break;
            case Constants.TWO_FOURTY:
                sp.setData(mContext, "cameraResolution", Constants.TWO_FOURTY);
                if(GL2JNILib.resolutionType() != Constants.TWO_FOURTY)
                update(true);
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if(progress < 1) {seekBar.setProgress(1); return;}
        fov_lable.setText(String.format(Locale.getDefault(),"%d",progress));
        update(false);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        update(true);
    }

    private void update(boolean storeAction){
        ((EditorView)((Activity)mContext)).renderManager.cameraProperty(((SeekBar)dialog.findViewById(R.id.field_of_view)).getProgress(),sp.getInt(mContext, "cameraResolution"),storeAction);
    }
}
