package com.smackall.animator;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.view.WindowManager;
import android.widget.ProgressBar;

import com.crashlytics.android.Crashlytics;
import com.crashlytics.android.ndk.CrashlyticsNdk;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.FullScreen;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;

import java.util.Timer;
import java.util.TimerTask;

import io.fabric.sdk.android.Fabric;

public class LoadingActivity extends AppCompatActivity {


    DownloadHelper assetJsonDownload;
    DownloadHelper animationJsonDownload;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Fabric.with(this, new Crashlytics(), new CrashlyticsNdk());
        if ((getIntent().getFlags() & Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT) != 0) {
            finish();
            return;
        }
        setContentView(R.layout.activity_loading);
        loadProgressBar();
        init();
    }


    private void init(){
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        FullScreen.HideStatusBar(this);

        DisplayMetrics displaymetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
        Constants.width= displaymetrics.widthPixels;
        Constants.height = displaymetrics.heightPixels;

        DatabaseHelper db = new DatabaseHelper();
        SharedPreferenceManager sp = new SharedPreferenceManager();
        assetJsonDownload = new DownloadHelper();
        animationJsonDownload = new DownloadHelper();

        PathManager.initPaths(this);
        db.createDataBase();
        assetJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/assetsDetailv5.json", db, this, sp, Constants.ASSET_JSON);
        animationJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/animationDetail.json", db, this, sp, Constants.ANIMATION_JSON);
    }

    private void loadProgressBar()
    {
        final ProgressBar loadingBar = (ProgressBar)findViewById(R.id.loadingBar);

      final Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                          loadingBar.setProgress(loadingBar.getProgress()+1);
                        if(loadingBar.getProgress() == 100) {
                            timer.cancel();
                            Intent i = new Intent(LoadingActivity.this,SceneSelection.class);
                            i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                            i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                            i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
                            i.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
                            startActivity(i);
                            LoadingActivity.this.dealloc();
                        }

                    }
                });
            }
        }, 1000, 100);
    }

    private void dealloc()
    {
        //assetJsonDownload = null;
        //animationJsonDownload = null;
        LoadingActivity.this.finish();
    }
}
