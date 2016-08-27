package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Switch;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 27/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OtherProps implements View.OnClickListener {

    private Context mContext;
    private EditorView editorView;
    private Dialog dialog;
    private boolean defaultValueInitialized = false;

    public OtherProps(Context mContext) {
        this.mContext = mContext;
        editorView = ((EditorView) mContext);
    }

    public void showOtherProps(View v, MotionEvent event) {
        defaultValueInitialized = false;
        Dialog mesh_prop = new Dialog(mContext);
        mesh_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        mesh_prop.setContentView(R.layout.image_props);
        mesh_prop.setCancelable(false);
        mesh_prop.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                mesh_prop.getWindow().setLayout(Constants.width / 2, Constants.height / 2);
                break;
            default:
                mesh_prop.getWindow().setLayout(Constants.width / 3, Constants.height / 3);
                break;
        }
        Window window = mesh_prop.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.START;
        wlp.dimAmount = 0.0f;
        if (event != null) {
            wlp.x = (int) event.getX();
            wlp.y = (int) event.getY();
        } else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        this.dialog = mesh_prop;
        initViews(mesh_prop);
        setDefaultValues(mesh_prop);
        mesh_prop.show();
        defaultValueInitialized = true;
    }

    private void initViews(Dialog mesh_prop) {
        mesh_prop.findViewById(R.id.delete_btn).setOnClickListener(this);
        mesh_prop.findViewById(R.id.clone_btn).setOnClickListener(this);
        mesh_prop.findViewById(R.id.light).setOnClickListener(this);
        mesh_prop.findViewById(R.id.visiblity).setOnClickListener(this);
    }

    private void setDefaultValues(Dialog mesh_prop) {
        ((Switch) mesh_prop.findViewById(R.id.light)).setChecked(GL2JNILib.isLightning());
        ((Switch) mesh_prop.findViewById(R.id.visiblity)).setChecked(GL2JNILib.isVisible());
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.delete_btn:
                if (dialog != null && dialog.isShowing())
                    dialog.dismiss();
                editorView.delete.showDelete();
                dealloc();
                break;
            case R.id.clone_btn:
                ((EditorView) mContext).renderManager.createDuplicate();
                dealloc();
                break;
            case R.id.light:
                update(true);
                break;
            case R.id.visiblity:
                update(true);
                break;
        }
    }

    private void dealloc() {
        if (this.dialog != null && this.dialog.isShowing()) {
            this.dialog.dismiss();
            this.dialog = null;
        }
    }

    private void update(boolean storeAction) {
        if (!defaultValueInitialized) return;
        boolean light = ((Switch) dialog.findViewById(R.id.light)).isChecked();
        boolean visible = ((Switch) dialog.findViewById(R.id.visiblity)).isChecked();
//        GL2JNILib.setNodeLighting(GL2JNILib.getSelectedNodeId(),light);
//        GL2JNILib.setNodeVisiblity(GL2JNILib.getSelectedNodeId(),visible);
        float refraction = GL2JNILib.refractionValue();
        float reflection = GL2JNILib.reflectionValue();
        ((EditorView) mContext).renderManager.changeMeshProps(refraction, reflection, light, visible, storeAction);

    }
}
