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
public class Login implements View.OnClickListener{

    Context mContext;
    private Dialog login;
    public Login(Context context){
        this.mContext = context;
    }

    public void showLogin(View v,MotionEvent event)
    {
        Dialog login = new Dialog(mContext);
        login.requestWindowFeature(Window.FEATURE_NO_TITLE);
        login.setContentView(R.layout.signin);
        login.setCancelable(false);
        login.setCanceledOnTouchOutside(true);
        login.getWindow().setLayout(Constants.width / 3, (int) (Constants.height / 2));
        Window window = login.getWindow();
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
        this.login = login;
        initViews(login);
        login.show();
    }

    private void initViews(Dialog login)
    {
        ((Button)login.findViewById(R.id.facebook)).setOnClickListener(this);
        ((Button)login.findViewById(R.id.twitter)).setOnClickListener(this);
        ((Button)login.findViewById(R.id.google)).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.facebook:
                login.dismiss();
                break;
            case R.id.twitter:
                login.dismiss();
                break;
            case R.id.google:
                login.dismiss();
                break;
        }
    }
}
