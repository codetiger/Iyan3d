package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CloudRenderingProgress implements View.OnClickListener {

    private Context mContext;

    public CloudRenderingProgress(Context context){
        this.mContext = context;
    }

    public void showCloudRenderingProgress(View v, MotionEvent event)
    {
        Dialog cloud_rendering = new Dialog(mContext);
        cloud_rendering.requestWindowFeature(Window.FEATURE_NO_TITLE);
        cloud_rendering.setContentView(R.layout.cloud_rendering);
        cloud_rendering.setCancelable(false);
        cloud_rendering.setCanceledOnTouchOutside(true);
        cloud_rendering.getWindow().setLayout(Constants.width / 3, (int) (Constants.height / 1.25));
        Window window = cloud_rendering.getWindow();
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
        initViews(cloud_rendering);
        cloud_rendering.show();
    }

    private void initViews(Dialog cloudeRendering)
    {
        ((Button)cloudeRendering.findViewById(R.id.add5K)).setOnClickListener(this);
        ((Button)cloudeRendering.findViewById(R.id.add20K)).setOnClickListener(this);
        ((Button)cloudeRendering.findViewById(R.id.add50K)).setOnClickListener(this);
        ((Button)cloudeRendering.findViewById(R.id.sign_out)).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.add5K:
                break;
            case R.id.add20K:
                break;
            case R.id.add50K:
                break;
            case R.id.sign_out:
                break;
        }
    }
}
