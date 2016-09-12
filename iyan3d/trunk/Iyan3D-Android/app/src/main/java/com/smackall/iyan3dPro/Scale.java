package com.smackall.iyan3dPro;

import android.app.Dialog;
import android.content.Context;
import android.support.v4.content.ContextCompat;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Scale implements SeekBar.OnSeekBarChangeListener, View.OnClickListener {
    private Context mContext;
    private Dialog scale;
    private boolean defaultValueInitialized = false;

    public Scale(Context context) {
        this.mContext = context;
    }

    public void showScale(View v, MotionEvent event, float x, float y, float z) {
        Dialog scale = new Dialog(mContext);
        scale.requestWindowFeature(Window.FEATURE_NO_TITLE);
        scale.setContentView(R.layout.scale);
        scale.setCancelable(false);
        scale.setCanceledOnTouchOutside(true);
        scale.getWindow().setLayout(Constants.width / 2, Constants.height / 2);
        Window window = scale.getWindow();
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

        this.scale = scale;
        defaultValueInitialized = false;
        setStartingValue(x, y, z);
        initViews(scale);
        updateText(false);
        defaultValueInitialized = true;
        scale.show();
    }

    private void setStartingValue(float x, float y, float z) {
        ((SeekBar) scale.findViewById(R.id.x)).setMax((((int) (x * 100.0f)) > 100) ? ((int) (x * 100.0f)) * 2 : 100);
        ((SeekBar) scale.findViewById(R.id.y)).setMax((((int) (y * 100.0f)) > 100) ? ((int) (y * 100.0f)) * 2 : 100);
        ((SeekBar) scale.findViewById(R.id.z)).setMax((((int) (z * 100.0f)) > 100) ? ((int) (z * 100.0f)) * 2 : 100);

        ((SeekBar) scale.findViewById(R.id.x)).setProgress((int) (x * 100.0f));
        ((SeekBar) scale.findViewById(R.id.y)).setProgress((int) (y * 100.0f));
        ((SeekBar) scale.findViewById(R.id.z)).setProgress((int) (z * 100.0f));
        setMax();
    }

    private void initViews(Dialog scale) {

        ((SeekBar) scale.findViewById(R.id.x)).setOnSeekBarChangeListener(this);
        ((SeekBar) scale.findViewById(R.id.y)).setOnSeekBarChangeListener(this);
        ((SeekBar) scale.findViewById(R.id.z)).setOnSeekBarChangeListener(this);
        scale.findViewById(R.id.lock_switch).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.lock_switch) {
            scale.findViewById(R.id.brace_img).setVisibility((((Switch) v).isChecked()) ? View.VISIBLE : View.INVISIBLE);
            ((ImageView) scale.findViewById(R.id.lock_img)).setImageDrawable(ContextCompat.getDrawable(mContext, (((Switch) v).isChecked()) ? R.drawable.lock : R.drawable.unlock));
            if (((Switch) v).isChecked())
                setMax();
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (((Switch) scale.findViewById(R.id.lock_switch)).isChecked() && progress < 1) {
            ((SeekBar) scale.findViewById(R.id.x)).setMax(100);
            ((SeekBar) scale.findViewById(R.id.y)).setMax(100);
            ((SeekBar) scale.findViewById(R.id.z)).setMax(100);
        }
        if (progress < 1) {
            seekBar.setProgress(1);
            return;
        }
        if (((Switch) scale.findViewById(R.id.lock_switch)).isChecked()) {
            ((SeekBar) scale.findViewById(R.id.x)).setProgress(progress);
            ((SeekBar) scale.findViewById(R.id.y)).setProgress(progress);
            ((SeekBar) scale.findViewById(R.id.z)).setProgress(progress);
        }
        updateText(false);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        updateText(false);
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (seekBar.getProgress() > 5)
            setMax();
        updateText(true);
    }

    private void updateText(boolean store) {
        ((TextView) scale.findViewById(R.id.x_value)).setText(String.valueOf((float) ((SeekBar) scale.findViewById(R.id.x)).getProgress() / 100.0f));
        ((TextView) scale.findViewById(R.id.y_value)).setText(String.valueOf((float) ((SeekBar) scale.findViewById(R.id.y)).getProgress() / 100.0f));
        ((TextView) scale.findViewById(R.id.z_value)).setText(String.valueOf((float) ((SeekBar) scale.findViewById(R.id.z)).getProgress() / 100.0f));
        if (!defaultValueInitialized) return;
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        if (!GL2JNILib.isRigMode())
            ((EditorView) mContext).renderManager.setScale(
                    (float) ((SeekBar) scale.findViewById(R.id.x)).getProgress() / 100.0f,
                    (float) ((SeekBar) scale.findViewById(R.id.y)).getProgress() / 100.0f,
                    (float) ((SeekBar) scale.findViewById(R.id.z)).getProgress() / 100.0f, store
            );
        else
            ((EditorView) mContext).renderManager.setRigScale(
                    (float) ((SeekBar) scale.findViewById(R.id.x)).getProgress() / 100.0f,
                    (float) ((SeekBar) scale.findViewById(R.id.y)).getProgress() / 100.0f,
                    (float) ((SeekBar) scale.findViewById(R.id.z)).getProgress() / 100.0f
            );
    }

    private void setMax() {
        int x = ((SeekBar) scale.findViewById(R.id.x)).getProgress();
        int y = ((SeekBar) scale.findViewById(R.id.y)).getProgress();
        int z = ((SeekBar) scale.findViewById(R.id.z)).getProgress();
        int max = Math.max(Math.max(x, y), z);
        if (((Switch) scale.findViewById(R.id.lock_switch)).isChecked()) {
            ((SeekBar) scale.findViewById(R.id.x)).setProgress(max);
            ((SeekBar) scale.findViewById(R.id.y)).setProgress(max);
            ((SeekBar) scale.findViewById(R.id.z)).setProgress(max);
        }
        if (max < 6) return;
        ((SeekBar) scale.findViewById(R.id.x)).setMax(max * 2);
        ((SeekBar) scale.findViewById(R.id.y)).setMax(max * 2);
        ((SeekBar) scale.findViewById(R.id.z)).setMax(max * 2);
        updateText(false);
    }
}
