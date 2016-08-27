package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.Switch;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class MeshProps implements View.OnClickListener, SeekBar.OnSeekBarChangeListener, RadioButton.OnCheckedChangeListener {

    int physicsType, velocity, tempPhysicsType;
    boolean canApplyPhysics;
    int selectedNodeId;
    private Context mContext;
    private EditorView editorView;
    private Dialog dialog;
    private boolean defaultValueInitialized = false;

    public MeshProps(Context mContext) {
        this.mContext = mContext;
        editorView = ((EditorView) mContext);
    }

    public void showMeshProps(View v, MotionEvent event) {
        HitScreens.MeshPropsView(mContext);
        defaultValueInitialized = false;
        selectedNodeId = -1;
        Dialog mesh_prop = new Dialog(mContext);
        mesh_prop.requestWindowFeature(Window.FEATURE_NO_TITLE);
        mesh_prop.setContentView(R.layout.mesh_props);
        mesh_prop.setCancelable(false);
        mesh_prop.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                mesh_prop.getWindow().setLayout(Constants.width, Constants.height);
                break;
            default:
                mesh_prop.getWindow().setLayout((int) (Constants.width / 1.3), (int) (Constants.height / 1.7));
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
        this.selectedNodeId = GL2JNILib.getSelectedNodeId();
        initViews(mesh_prop);
        setDefaultValues(mesh_prop);
        mesh_prop.show();
        defaultValueInitialized = true;
        mesh_prop.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                HitScreens.EditorView(mContext);
            }
        });
    }

    private void initViews(final Dialog mesh_prop) {
        mesh_prop.findViewById(R.id.delete_btn).setOnClickListener(this);
        mesh_prop.findViewById(R.id.clone_btn).setOnClickListener(this);
        mesh_prop.findViewById(R.id.skin_btn).setOnClickListener(this);
        ((SeekBar) mesh_prop.findViewById(R.id.glassy_bar)).setOnSeekBarChangeListener(this);
        ((SeekBar) mesh_prop.findViewById(R.id.reflection_bar)).setOnSeekBarChangeListener(this);
        mesh_prop.findViewById(R.id.hqInform1).setOnClickListener(this);
        mesh_prop.findViewById(R.id.hqInform2).setOnClickListener(this);

        ((Switch) mesh_prop.findViewById(R.id.light)).setOnCheckedChangeListener(this);
        ((Switch) mesh_prop.findViewById(R.id.visiblity)).setOnCheckedChangeListener(this);
        ((Switch) mesh_prop.findViewById(R.id.mirror)).setOnCheckedChangeListener(this);
        boolean mirror = (GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_RIG && GL2JNILib.jointSize(GL2JNILib.getSelectedNodeId()) == Constants.HUMAN_JOINTS_SIZE);
        mesh_prop.findViewById(R.id.mirror).setEnabled(mirror);
        ((Switch) mesh_prop.findViewById(R.id.texture_smooth)).setOnCheckedChangeListener(this);

        int smoothTexture = GL2JNILib.smoothTexState();
        mesh_prop.findViewById(R.id.texture_smooth).setEnabled(smoothTexture != -1);
        ((Switch) mesh_prop.findViewById(R.id.texture_smooth)).setChecked((smoothTexture > 0));

        ((RadioButton) mesh_prop.findViewById(R.id.physics_none)).setOnCheckedChangeListener(this);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_static)).setOnCheckedChangeListener(this);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_light)).setOnCheckedChangeListener(this);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_medium)).setOnCheckedChangeListener(this);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_heavy)).setOnCheckedChangeListener(this);
        ((SeekBar) mesh_prop.findViewById(R.id.velocity)).setOnSeekBarChangeListener(this);
        mesh_prop.findViewById(R.id.setDirection_btn).setOnClickListener(this);

        mesh_prop.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                applyPhysicsProps(mesh_prop);
            }
        });
    }

    private void setDefaultValues(Dialog mesh_prop) {
        ((SeekBar) mesh_prop.findViewById(R.id.glassy_bar)).setProgress((int) (GL2JNILib.refractionValue() * 100));
        ((SeekBar) mesh_prop.findViewById(R.id.reflection_bar)).setProgress((int) (GL2JNILib.reflectionValue() * 100));
        ((Switch) mesh_prop.findViewById(R.id.light)).setChecked(GL2JNILib.isLightning());
        ((Switch) mesh_prop.findViewById(R.id.visiblity)).setChecked(GL2JNILib.isVisible());
        if (mesh_prop.findViewById(R.id.mirror).isEnabled())
            ((Switch) mesh_prop.findViewById(R.id.mirror)).setChecked(GL2JNILib.mirrorState());

        tempPhysicsType = physicsType = (GL2JNILib.isPhysicsEnabled(selectedNodeId)) ? GL2JNILib.physicsType(selectedNodeId) : Constants.NONE;
        velocity = GL2JNILib.velocity(selectedNodeId);
        int type = GL2JNILib.getNodeType(selectedNodeId);
        canApplyPhysics = (type == Constants.NODE_SGM || type == Constants.NODE_OBJ || type == Constants.NODE_TEXT);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_none)).setChecked(Constants.NONE == physicsType);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_static)).setChecked(Constants.STATIC == physicsType);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_light)).setChecked(Constants.LIGHT == physicsType);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_medium)).setChecked(Constants.MEDIUM == physicsType);
        ((RadioButton) mesh_prop.findViewById(R.id.physics_heavy)).setChecked(Constants.HEAVY == physicsType);
        ((SeekBar) mesh_prop.findViewById(R.id.velocity)).setProgress(GL2JNILib.velocity(selectedNodeId));
        if (!canApplyPhysics) {
            mesh_prop.findViewById(R.id.physics_none).setEnabled(false);
            mesh_prop.findViewById(R.id.physics_static).setEnabled(false);
            mesh_prop.findViewById(R.id.physics_light).setEnabled(false);
            mesh_prop.findViewById(R.id.physics_medium).setEnabled(false);
            mesh_prop.findViewById(R.id.physics_heavy).setEnabled(false);
            mesh_prop.findViewById(R.id.velocity).setEnabled(false);
            mesh_prop.findViewById(R.id.physics_segment_group).setEnabled(false);
            mesh_prop.findViewById(R.id.velocity).setEnabled(false);
            mesh_prop.findViewById(R.id.setDirection_btn).setEnabled(false);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.delete_btn:
                if (this.dialog != null && this.dialog.isShowing())
                    this.dialog.dismiss();
                editorView.delete.showDelete();
                dealloc();
                break;
            case R.id.clone_btn:
                ((EditorView) mContext).renderManager.createDuplicate();
                dealloc();
                break;
            case R.id.skin_btn:
                Constants.VIEW_TYPE = Constants.CHANGE_TEXTURE;
                ((EditorView) mContext).textureSelection.showChangeTexture();
                dealloc();
                break;
            case R.id.hqInform1:
            case R.id.hqInform2:
                UIHelper.informDialog(mContext, "This Property is only for HighQuality Rendering.");
                break;
            case R.id.setDirection_btn:
                applyPhysicsProps(dialog);
                setDirection();
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (!defaultValueInitialized) return;
        if (seekBar.getId() != R.id.velocity)
            update(false);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        switch (seekBar.getId()) {
            case R.id.glassy_bar:
                break;
            case R.id.reflection_bar:
                break;
            case R.id.velocity:
                break;
        }
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (seekBar.getId() != R.id.velocity)
            update(true);
    }

    private void dealloc() {
        if (this.dialog != null && this.dialog.isShowing()) {
            this.dialog.dismiss();
            this.dialog = null;
        }
    }

    private void update(boolean storeAction) {
        if (!defaultValueInitialized) return;
        try {
            float refraction = ((SeekBar) dialog.findViewById(R.id.glassy_bar)).getProgress() / 100.0f;
            float reflection = ((SeekBar) dialog.findViewById(R.id.reflection_bar)).getProgress() / 100.0f;
            boolean light = ((Switch) dialog.findViewById(R.id.light)).isChecked();
            boolean visible = ((Switch) dialog.findViewById(R.id.visiblity)).isChecked();
            ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
            ((EditorView) mContext).renderManager.changeMeshProps(refraction, reflection, light, visible, storeAction);
        } catch (NullPointerException ignored) {
        }
    }

    private void applyPhysicsProps(Dialog mesh_prop) {
        if (!defaultValueInitialized) return;
        if (canApplyPhysics) {
            if (velocity != ((SeekBar) mesh_prop.findViewById(R.id.velocity)).getProgress()) {
                velocityChanged(((SeekBar) mesh_prop.findViewById(R.id.velocity)).getProgress());
                update(true);
            }
            if (physicsType != tempPhysicsType) {
                update(false);

                if (tempPhysicsType == Constants.NONE)
                    setPhysics(false);
                else {
                    setPhysics(true);
                    setPhysicsType(tempPhysicsType);
                }
                update(true);
            }
        }
    }

    private void setPhysics(final boolean state) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.enablePhysics(state);
            }
        });
    }

    private void setPhysicsType(final int physicsType) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setPhysicsType(physicsType);
            }
        });
    }

    private void velocityChanged(final int value) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.velocityChanged(value);
            }
        });
    }

    private void setDirection() {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setDirection();
                ((EditorView) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (dialog != null && dialog.isShowing())
                            dialog.dismiss();
                    }
                });
            }
        });
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (!defaultValueInitialized) return;

        switch (buttonView.getId()) {
            case R.id.light:
                update(true);
                break;
            case R.id.visiblity:
                update(true);
                break;
            case R.id.mirror:
                GL2JNILib.switchMirror();
                break;
            case R.id.texture_smooth:
                ((EditorView) mContext).renderManager.setTextureSmoothStatus(isChecked);
                break;
        }

        if (!isChecked) return;
        switch (buttonView.getId()) {
            case R.id.physics_none:
                tempPhysicsType = Constants.NONE;
                break;
            case R.id.physics_static:
                tempPhysicsType = Constants.STATIC;
                break;
            case R.id.physics_light:
                tempPhysicsType = Constants.LIGHT;
                break;
            case R.id.physics_medium:
                tempPhysicsType = Constants.MEDIUM;
                break;
            case R.id.physics_heavy:
                tempPhysicsType = Constants.HEAVY;
                break;
        }
    }
}
