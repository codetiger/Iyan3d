package com.smackall.animator.Helper;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;

import com.smackall.animator.R;

/**
 * Created by Sabish.M on 8/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class PermissionManager {

    Snackbar snackbar;

    public boolean checkPermission(int type,Context mContext)
    {
        switch (type){
            case Constants.INTERNET:
                if(ContextCompat.checkSelfPermission((Activity)mContext,
                    Manifest.permission.INTERNET) != PackageManager.PERMISSION_GRANTED)
                    return false;
                break;
            case Constants.STORAGE:
                if(ContextCompat.checkSelfPermission((Activity)mContext,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED || ContextCompat.checkSelfPermission((Activity)mContext,
                        Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED)
                    return false;
                break;
            case Constants.GET_ACCOUNTS:
                if(ContextCompat.checkSelfPermission((Activity)mContext,
                        Manifest.permission.GET_ACCOUNTS) != PackageManager.PERMISSION_GRANTED)
                    return false;
                break;
            case Constants.WAKELOCK:
                if(ContextCompat.checkSelfPermission((Activity)mContext,
                        Manifest.permission.WAKE_LOCK) != PackageManager.PERMISSION_GRANTED)
                    return false;
                break;
        }
        return true;
    }

    public void requestReadAndWritePermission(final Context mContext,CoordinatorLayout coordinatorLayout)
    {
        if(mContext == null || coordinatorLayout == null) return;
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return;

        coordinatorLayout = ((CoordinatorLayout) ((Activity)mContext).findViewById(R.id.coordinatorLayout));

        String[] PERMISSIONS_STORAGE = new String[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            PERMISSIONS_STORAGE = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE};
        }
        if (ActivityCompat.shouldShowRequestPermissionRationale((Activity) mContext,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                || ActivityCompat.shouldShowRequestPermissionRationale((Activity) mContext,
                Manifest.permission.READ_EXTERNAL_STORAGE)) {
            final String[] finalPERMISSIONS_STORAGE = PERMISSIONS_STORAGE;
            snackbar = Snackbar
                    .make(coordinatorLayout, "Iyan 3D stores your scenes files and exports video to your SDCard, and needs your permission to write files to your disk.", Snackbar.LENGTH_INDEFINITE).setAction("Allow", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            ActivityCompat
                                    .requestPermissions((Activity)mContext, finalPERMISSIONS_STORAGE,
                                            Constants.REQUEST_STORAGE);
                            snackbar.dismiss();
                        }
                    });
            snackbar.show();
        } else {
            ActivityCompat.requestPermissions((Activity)mContext, PERMISSIONS_STORAGE, Constants.REQUEST_STORAGE);
        }
    }

    public void requestInternetPermission(final Context mContext, CoordinatorLayout coordinatorLayout)
    {
        if(mContext == null || coordinatorLayout == null) return;
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return;

        String[] PERMISSIONS_INTERNET = new String[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            PERMISSIONS_INTERNET = new String[]{Manifest.permission.INTERNET};
        }
        if (ActivityCompat.shouldShowRequestPermissionRationale((Activity) mContext,
                Manifest.permission.INTERNET)) {
            final String[] finalPERMISSIONS_INTERNET = PERMISSIONS_INTERNET;
            snackbar = Snackbar
                    .make(coordinatorLayout, "Iyan 3D downloads resources from the server, and needs your permission to connect to the Internet.", Snackbar.LENGTH_INDEFINITE).setAction("Allow", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
            ActivityCompat
                    .requestPermissions((Activity)mContext, finalPERMISSIONS_INTERNET,
                            Constants.REQUEST_INTERNET);
                            snackbar.dismiss();
                        }
                    });
            snackbar.show();
        } else {
            ActivityCompat.requestPermissions((Activity)mContext, PERMISSIONS_INTERNET, Constants.REQUEST_INTERNET);
        }
    }

    public void requestGetAccounts(final Context mContext,CoordinatorLayout coordinatorLayout)
    {
        if(mContext == null || coordinatorLayout == null) return;
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return;

        String[] PERMISSIONS_GET_ACCOUNTS = new String[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            PERMISSIONS_GET_ACCOUNTS = new String[]{Manifest.permission.GET_ACCOUNTS};
        }
        if (ActivityCompat.shouldShowRequestPermissionRationale((Activity) mContext,
                Manifest.permission.GET_ACCOUNTS)) {
            final String[] finalPERMISSIONS_GET_ACCOUNTS = PERMISSIONS_GET_ACCOUNTS;
            snackbar = Snackbar
                    .make(coordinatorLayout, "To login to your Iyan 3D account, the app requires your permission to access your account information.", Snackbar.LENGTH_INDEFINITE).setAction("Allow", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
            ActivityCompat
                    .requestPermissions((Activity)mContext, finalPERMISSIONS_GET_ACCOUNTS,
                            Constants.REQUEST_GET_ACCOUNTS);
                            snackbar.dismiss();
                        }
                    });
            snackbar.show();
        } else {
            ActivityCompat.requestPermissions((Activity)mContext, PERMISSIONS_GET_ACCOUNTS, Constants.REQUEST_GET_ACCOUNTS);
        }
    }

    public void requestWakeLock(final Context mContext, CoordinatorLayout coordinatorLayout)
    {
        if(mContext == null || coordinatorLayout == null) return;
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return;

        String[] PERMISSIONS_WAKELOCK = new String[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            PERMISSIONS_WAKELOCK = new String[]{Manifest.permission.WAKE_LOCK};
        }
        if (ActivityCompat.shouldShowRequestPermissionRationale((Activity) mContext,
                Manifest.permission.WAKE_LOCK)) {
            final String[] finalPERMISSIONS_WAKELOCK = PERMISSIONS_WAKELOCK;
            snackbar = Snackbar
                    .make(coordinatorLayout, "Iyan 3D rendering process requires the screen to be active and needs your permission.", Snackbar.LENGTH_INDEFINITE).setAction("Allow", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
            ActivityCompat
                    .requestPermissions((Activity)mContext, finalPERMISSIONS_WAKELOCK,
                            Constants.REQUEST_WAKELOCK);
                            snackbar.dismiss();
                        }
                    });
            snackbar.show();
        } else {
            ActivityCompat.requestPermissions((Activity)mContext, PERMISSIONS_WAKELOCK, Constants.REQUEST_WAKELOCK);
        }
    }
}
