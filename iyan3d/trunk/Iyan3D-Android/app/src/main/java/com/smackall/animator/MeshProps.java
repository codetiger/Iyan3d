package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Switch;

import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class MeshProps  implements View.OnClickListener , SeekBar.OnSeekBarChangeListener{

    private Context mContext;

    public MeshProps(Context mContext)
    {
        this.mContext = mContext;
    }

    public void showMeshProps(View v,MotionEvent event)
    {
        Dialog mesh_prop = new Dialog(mContext);
        mesh_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        mesh_prop.setContentView(R.layout.mesh_props);
        mesh_prop.setCancelable(false);
        mesh_prop.setCanceledOnTouchOutside(true);
        mesh_prop.getWindow().setLayout(Constants.width / 2, (int) (Constants.height/2));
        Window window = mesh_prop.getWindow();
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

        initViews(mesh_prop);
        setDefaultValues(mesh_prop,50,10,true,false,false);
        mesh_prop.show();
    }

    private void initViews(Dialog mesh_prop)
    {
        ((Button)mesh_prop.findViewById(R.id.delete_btn)).setOnClickListener(this);
        ((Button)mesh_prop.findViewById(R.id.clone_btn)).setOnClickListener(this);
        ((Button)mesh_prop.findViewById(R.id.skin_btn)).setOnClickListener(this);
        ((SeekBar)mesh_prop.findViewById(R.id.glassy_bar)).setOnSeekBarChangeListener(this);
        ((SeekBar)mesh_prop.findViewById(R.id.reflection_bar)).setOnSeekBarChangeListener(this);
        ((Switch)mesh_prop.findViewById(R.id.light)).setOnClickListener(this);
        ((Switch)mesh_prop.findViewById(R.id.visiblity)).setOnClickListener(this);
        ((Switch)mesh_prop.findViewById(R.id.mirror)).setOnClickListener(this);
    }

    private void setDefaultValues(Dialog mesh_prop,int glassy,int reflection,boolean light, boolean visiblity, boolean mirror)
    {
        ((SeekBar)mesh_prop.findViewById(R.id.glassy_bar)).setProgress(glassy);
        ((SeekBar)mesh_prop.findViewById(R.id.reflection_bar)).setProgress(reflection);
        ((Switch)mesh_prop.findViewById(R.id.light)).setChecked(light);
        ((Switch) mesh_prop.findViewById(R.id.visiblity)).setChecked(visiblity);
        ((Switch)mesh_prop.findViewById(R.id.mirror)).setChecked(mirror);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.delete_btn:
                break;
            case R.id.clone_btn:
                break;
            case R.id.skin_btn:
                break;
            case R.id.light:
                break;
            case R.id.visiblity:
                break;
            case R.id.mirror:
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        switch (seekBar.getId()){
            case R.id.glassy_bar:
                break;
            case R.id.reflection_bar:
                break;
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        switch (seekBar.getId()){
            case R.id.glassy_bar:
                break;
            case R.id.reflection_bar:
                break;
        }
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        switch (seekBar.getId()){
            case R.id.glassy_bar:
                break;
            case R.id.reflection_bar:
                break;
        }
    }
}
