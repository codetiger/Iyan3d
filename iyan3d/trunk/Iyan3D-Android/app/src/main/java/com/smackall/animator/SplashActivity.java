package com.smackall.animator;

/**
 * Created by Sabish on 31/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 * */

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.WindowManager;

import com.crashlytics.android.Crashlytics;
import com.crashlytics.android.ndk.CrashlyticsNdk;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.common.AssetsManager;
import com.smackall.animator.common.CheckNotification;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.KeyMaker;
import com.smackall.animator.common.NumberProgressBar;
import com.smackall.animator.common.OnProgressBarListener;
import com.smackall.animator.common.Services;
import com.smackall.animator.common.WholeDatabaseCreator;
import com.smackall.animator.sceneSelectionView.SceneSelectionView;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import io.fabric.sdk.android.Fabric;
import smackall.animator.R;

public class SplashActivity extends Activity implements OnProgressBarListener {

    Timer timer;
    Context mContext;

    AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    DownloadManagerClass downloadManager = new DownloadManagerClass(3);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if ((getIntent().getFlags() & Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT) != 0) {
            finish();
            return;
        }

        Fabric.with(this, new Crashlytics(), new CrashlyticsNdk());
        setContentView(R.layout.activity_splash);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Constant.FullScreencall(SplashActivity.this, this);
        Constant.bnp = (NumberProgressBar)findViewById(R.id.numberbar1);
        Constant.bnp.setOnProgressBarListener(this);
        mContext = this;
        Constant.splashActivity = this;
        Constant.downloadViewType = "json";

        Constant.deviceUniqueId = Settings.Secure.getString(getContentResolver(),
                Settings.Secure.ANDROID_ID);

        //TEST
//         Constant.assetsDatabaseDirectory = Environment.getExternalStorageDirectory()+"/Android/";
//         Constant.assetsDatabaseLocation = Environment.getExternalStorageDirectory()+"/Android/iyan3dAssetsDB";
           //Constant.animDatabaseLocation = Environment.getExternalStorageDirectory()+"/Android/iyan3dAnimDB";


        /** Update all Local Project location and create required folder in Internal or SD-Card Storage **/

        Constant.defaultLocalAssetsDir = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName();
        Constant.mkDir(Constant.defaultLocalAssetsDir);
        Constant.localProjectDir = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/projects";
        Constant.animationsDir = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/animations";
        Constant.defaultCacheDir = Environment.getExternalStorageDirectory() + "/Android/data/"+getPackageName()+"/.cache/";
        Constant.importedImagesdir = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/importedImages/";
        Constant.mkDir(Constant.animationsDir);
        Constant.mkDir(Environment.getExternalStorageDirectory() + "/Android/data/" + getPackageName() + "/projects/");
        File cacheDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+getPackageName()+"/.cache/");
        Constant.deleteDir(cacheDir);
        Constant.mkDir(cacheDir);
        Constant.screenInch(this);
        File projectDir = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/projects");
        Constant.mkDir(projectDir);


        /** Copy project assets to root Folder **/

        AssetsManager.copyAssetsDirToLocal(this);

        /** Do loading background process like Json Download Database Updation **/

        String assetJsonUrl = "http://iyan3dapp.com/appapi/json/assetsDetail.json";
        String animationJsonUrl = "http://iyan3dapp.com/appapi/json/animationDetail.json";
        if(Constant.isOnline(this)) {
            Constant.assetJsonId = addToDownloadManager.downloadAdd(this, assetJsonUrl, "assets", Environment.getExternalStorageDirectory() + "/Android/data/" + getPackageName() + "/.cache/", "high", downloadManager);
            Constant.animationJsonId = addToDownloadManager.downloadAdd(this, animationJsonUrl, "animation", Environment.getExternalStorageDirectory() + "/Android/data/" + getPackageName() + "/.cache/", "high", downloadManager);
        }
        else
            Constant.informDialog(this, "Failed connecting to the server! Please try again later.");
        KeyMaker.makeKey(mContext);
        Constant.isPremium(mContext);

        CheckNotification.loadNotificationUpdate(this, SplashActivity.this);
        WholeDatabaseCreator.backGroundTaskLoader(SplashActivity.this, SplashActivity.this);

        /** Update the Loading progress bar with Timer **/

        new Thread(new Runnable() {
            @Override
            public void run() {
                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (Constant.bnp.getProgress() < Constant.bnp.getMax()) {
                                    Constant.bnp.incrementProgressBy(1);
                                } else {
                                    if (timer != null) {
                                        timer.cancel();
                                        timer.purge();
                                    }
                                }
                            }
                        });
                    }
                }, 5000, 100);
            }
        }).start();
    }

    @Override
    public void onProgressChange(int current, int max) {
      if(current == max) {
           timer.cancel();
           timer = null;
            downloadManager.cancelAll();
            Constant.informDialog = null;
            Intent i = new Intent();
            i.setClass(SplashActivity.this, SceneSelectionView.class);
            startActivity(i);
            SplashActivity.this.finish();
        }
    }

    @Override
    public void onResume(){
        super.onResume();
        stopService(new Intent(this, Services.class));
    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }


}

