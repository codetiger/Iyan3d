package com.smackall.animator;

import android.app.Activity;
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
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class MeshProps  implements View.OnClickListener , SeekBar.OnSeekBarChangeListener{

    private Context mContext;
    private EditorView editorView;
    private Dialog dialog;

    public MeshProps(Context mContext)
    {
        this.mContext = mContext;
        editorView = ((EditorView)(Activity)mContext);
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
        setDefaultValues(mesh_prop);
        this.dialog = mesh_prop;
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
        boolean mirror = (GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_RIG && GL2JNILib.jointSize(GL2JNILib.getSelectedNodeId()) == Constants.HUMAN_JOINTS_SIZE);
        ((Switch)mesh_prop.findViewById(R.id.mirror)).setEnabled(mirror);
    }

    private void setDefaultValues(Dialog mesh_prop)
    {
        ((SeekBar)mesh_prop.findViewById(R.id.glassy_bar)).setProgress((int) (GL2JNILib.reflectionValue() * 10));
        ((SeekBar)mesh_prop.findViewById(R.id.reflection_bar)).setProgress((int) (GL2JNILib.refractionValue() * 10));
        ((Switch)mesh_prop.findViewById(R.id.light)).setChecked(GL2JNILib.isLightning());
        ((Switch) mesh_prop.findViewById(R.id.visiblity)).setChecked(GL2JNILib.isVisible());
        if(((Switch)mesh_prop.findViewById(R.id.mirror)).isEnabled())
            ((Switch)mesh_prop.findViewById(R.id.mirror)).setChecked(GL2JNILib.mirrorState());
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.delete_btn:
                this.dialog.dismiss();
                editorView.delete.showDelete();
                dealloc();
                break;
            case R.id.clone_btn:
                ((EditorView)((Activity)mContext)).renderManager.createDuplicate();
                dealloc();
                break;
            case R.id.skin_btn:
                ((EditorView)((Activity)mContext)).textureSelection.showChangeTexture();
                dealloc();
                break;
            case R.id.light:
                update(true);
                break;
            case R.id.visiblity:
                update(true);
                break;
            case R.id.mirror:
                GL2JNILib.switchMirror();
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
       update(false);
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
        update(true);
    }

    private void dealloc()
    {
        this.dialog.dismiss();
        this.dialog = null;
    }

    private void update(boolean storeAction)
    {
        float refraction = ((SeekBar)dialog.findViewById(R.id.glassy_bar)).getProgress()/100.0f;
        float reflection = ((SeekBar)dialog.findViewById(R.id.reflection_bar)).getProgress()/100.0f;
        boolean light = ((Switch)dialog.findViewById(R.id.light)).isChecked();
        boolean visible = ((Switch)dialog.findViewById(R.id.visiblity)).isChecked();
        GL2JNILib.changeMeshProperty(refraction,reflection,light,visible,storeAction);
    }
}
