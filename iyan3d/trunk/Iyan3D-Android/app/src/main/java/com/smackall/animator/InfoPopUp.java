package com.smackall.animator;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.support.v7.widget.PopupMenu;
import android.view.MenuItem;
import android.view.View;

import com.smackall.animator.UserOnBoarding.UserOnBoarding;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class InfoPopUp {

    private static final int ID_TUTORIAL = 0;
    private static final int ID_SETTINGS = 1;
    private static final int ID_RATE = 2;
    private static final int ID_SHARE = 3;
    private static final int ID_CONTACT = 4;
    private Context mContext;

    public void infoPopUpMenu(Context context, View v) {
        this.mContext = context;
        PopupMenu popup = new PopupMenu(context, v);
        popup.getMenuInflater().inflate(R.menu.info_props_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case ID_TUTORIAL:
                        openTutorial(mContext);
                        break;
                    case ID_SETTINGS:
                        openSettings(mContext);
                        break;
                    case ID_RATE:
                        rateThisApp(mContext);
                        break;
                    case ID_SHARE:
                        openFollowDialog(mContext);
                        break;
                    case ID_CONTACT:
                        openMail(mContext);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    private void openTutorial(Context context) {
        String className = context.getClass().getSimpleName();
        Intent i = new Intent((className.toLowerCase().equals("sceneselection") ? ((SceneSelection) (context)) : ((EditorView) (context))), UserOnBoarding.class);
        context.startActivity(i);
    }

    private void rateThisApp(Context context) {
        String url = "market://details?id=" + context.getPackageName();
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse(url));
        try {
            context.startActivity(i);
        } catch (ActivityNotFoundException ignored) {

        }
    }

    private void openMail(Context context) {
        Intent email = new Intent(Intent.ACTION_SEND);
        email.setType("message/rfc822");
        email.putExtra(Intent.EXTRA_EMAIL, new String[]{"iyan3d@smackall.com"});
        PackageInfo pInfo = null;
        String version = "";
        try {
            pInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
            version = pInfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            version = "";
        }
        email.putExtra(Intent.EXTRA_SUBJECT, "Feedback on Iyan3D " + version + " app ( Device Model " + Build.MANUFACTURER.toUpperCase() + " " + Build.MODEL + " , Android Version " + Build.VERSION.RELEASE + " )");
        try {
            context.startActivity(email);
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
        }
    }

    private void openSettings(Context context) {
        try {
            String className = context.getClass().getSimpleName();
            if (className.toLowerCase().equals("sceneselection"))
                ((SceneSelection) context).settings.showSettings();
            else
                ((EditorView) context).settings.showSettings();
        } catch (ClassCastException ignored) {
        }
    }

    private void openFollowDialog(Context context) {
        try {
            String className = context.getClass().getSimpleName();
            if (className.toLowerCase().equals("sceneselection"))
                ((SceneSelection) context).followApp.showFollowOption();
            else
                ((EditorView) context).followApp.showFollowOption();
        } catch (ClassCastException ignored) {
        }
    }
}
