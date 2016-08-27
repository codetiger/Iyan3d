package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.os.Build;

import com.smackall.animator.Helper.SharedPreferenceManager;

/**
 * Created by Sabish.M on 28/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UserDetails {

    public String userName = "0";
    public String uniqueId = "0";
    public String mail = "0";
    public String hwversion = Build.BRAND + " " + Build.MODEL;
    public String osversion = Build.VERSION.RELEASE;
    public int signInType = -1;
    private Context mContext;
    private SharedPreferenceManager sp;

    public UserDetails(Context context, SharedPreferenceManager sp) {
        this.mContext = context;
        this.sp = sp;
    }

    public void setUserDetails() {
        sp.setData(mContext, "username", userName);
        sp.setData(mContext, "uniqueid", uniqueId);
        sp.setData(mContext, "mail", mail);
        sp.setData(mContext, "signinType", signInType);
        if (uniqueId != null && !uniqueId.equals("") && uniqueId.length() > 5) {
            try {
                String className = mContext.getClass().getSimpleName();
                if (className.toLowerCase().equals("sceneselection"))
                    ((SceneSelection) mContext).showLogIn(((Activity) mContext).findViewById(R.id.login_btn));
                else
                    ((EditorView) mContext).showLogin(((Activity) mContext).findViewById(R.id.login));
            } catch (ClassCastException ignored) {

            }
        }
    }

    public void resetUserDetails() {
        sp.setData(mContext, "username", "");
        sp.setData(mContext, "uniqueid", "");
        sp.setData(mContext, "mail", "");
        sp.setData(mContext, "signinType", -1);
        updateUserDetails();
    }

    public void updateUserDetails() {
        userName = (sp.getString(mContext, "username") == null || sp.getString(mContext, "username").equals("")) ? "0" : sp.getString(mContext, "username");
        uniqueId = (sp.getString(mContext, "uniqueid") == null || sp.getString(mContext, "uniqueid").equals("")) ? "0" : sp.getString(mContext, "uniqueid");
        mail = (sp.getString(mContext, "mail") == null || sp.getString(mContext, "mail").equals("")) ? "0" : sp.getString(mContext, "mail");
        signInType = (sp.getInt(mContext, "signinType") == 0) ? -1 : sp.getInt(mContext, "signinType");
    }
}
