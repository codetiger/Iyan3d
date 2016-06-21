package com.smackall.animator;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.transition.Scene;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;

import com.crashlytics.android.Crashlytics;
import com.crashlytics.android.answers.Answers;
import com.crashlytics.android.answers.CustomEvent;
import com.crashlytics.android.ndk.CrashlyticsNdk;
import com.google.android.gms.analytics.Tracker;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.Events;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.FullScreen;
import com.smackall.animator.Helper.KeyMaker;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.PermissionManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.twitter.sdk.android.Twitter;
import com.twitter.sdk.android.core.TwitterAuthConfig;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

import io.fabric.sdk.android.Fabric;

public class LoadingActivity extends AppCompatActivity {

    DownloadHelper assetJsonDownload;
    DownloadHelper animationJsonDownload;
    int process = 0;
    public ProgressBar loadingBar;
    boolean activityStarted = false;
    int startTime = 0;
    Intent i;
    private Tracker mTracker;
    // Note: Your consumer key and secret should be obfuscated in your source code before shipping.
    private static final String TWITTER_KEY = "FVYtYJI6e4lZMHoZvYCt2ejao";
    private static final String TWITTER_SECRET = "eiFIXzb9zjoaH0lrDZ2Jrh2ezvbmuFv6rvPJdIXLYxgkaZ7YKC";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if ((getIntent().getFlags() & Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT) != 0) {
            Intent intent;
            if(Constants.currentActivity != -1) {
                int currentActivity = Constants.currentActivity;
                Class cl = (currentActivity == 0) ? SceneSelection.class : EditorView.class;
                intent = new Intent(Intent.ACTION_MAIN);
                intent.setClass(this, cl);
                intent.addCategory(Intent.ACTION_MAIN);
                intent.addCategory(Intent.CATEGORY_LAUNCHER);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP |
                        Intent.FLAG_ACTIVITY_SINGLE_TOP);
                if(getIntent().getExtras() != null)
                    intent.putExtras(getIntent().getExtras());
            }
            else{
                intent = new Intent(this, SceneSelection.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                if(getIntent().getExtras() != null)
                    intent.putExtras(getIntent().getExtras());
            }

            PendingIntent pi = PendingIntent.getActivity(this, 0, intent,
                    PendingIntent.FLAG_UPDATE_CURRENT);
            try {
                pi.send();
            } catch (PendingIntent.CanceledException e) {
                e.printStackTrace();
            }
            finish();
            return;
        }
        if(!isTaskRoot()){
            Intent intent = getIntent();
            String action = intent.getAction();
            if(action.compareTo(Intent.ACTION_VIEW) == 0) {
                String scheme = intent.getScheme();
                ContentResolver resolver = getContentResolver();
                if (scheme.compareTo(ContentResolver.SCHEME_FILE) == 0) {
                    Uri uri = intent.getData();
                    String name = uri.getPath();
                    if(name != null && FileHelper.checkValidFilePath(name)) {
                        intent.putExtra("hasExtraForOpenWith", true);
                        intent.putExtra("i3dPath", name);
                        i.putExtra("fromLoading",false);
                    }
                }
                PendingIntent pi = PendingIntent.getActivity(this, 0, intent,
                        PendingIntent.FLAG_UPDATE_CURRENT);
                try {
                    pi.send();
                } catch (PendingIntent.CanceledException e) {
                    e.printStackTrace();
                }
            }
            finish();
            return;
        }

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        FullScreen.HideStatusBar(this);
        setContentView(R.layout.activity_loading);
        TwitterAuthConfig authConfig = new TwitterAuthConfig(TWITTER_KEY, TWITTER_SECRET);
        Fabric.with(this,new Crashlytics(), new CrashlyticsNdk(), new Twitter(authConfig));
        HitScreens.LoadingView(LoadingActivity.this);
        Constants.deviceUniqueId = Settings.Secure.getString(getContentResolver(),
                Settings.Secure.ANDROID_ID);
        loadingBar = (ProgressBar)findViewById(R.id.loadingBar);
        float space = Constants.getFreeSpace();
        if(space < 50.0){
            UIHelper.informDialog(LoadingActivity.this,"Phone storage is low. Minimum 50MB of space is required.",true);
        }
        i = new Intent(LoadingActivity.this, SceneSelection.class);
        Intent intent = getIntent();
        String action = intent.getAction();
        if(action != null && action.compareTo(Intent.ACTION_VIEW) == 0) {
            String scheme = intent.getScheme();
            if (scheme.compareTo(ContentResolver.SCHEME_FILE) == 0) {
                Uri uri = intent.getData();
                String name = uri.getPath();
                if(name != null && FileHelper.checkValidFilePath(name)) {
                    i.putExtra("hasExtraForOpenWith", true);
                    i.putExtra("i3dPath", name);
                    i.putExtra("fromLoading",false);
                }
            }
        }

    init();
    }

    private void init(){
        boolean needPermission = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M);
        CoordinatorLayout coordinatorLayout = ((CoordinatorLayout)findViewById(R.id.coordinatorLayout));
        PermissionManager permissionManager = new PermissionManager();
        if(needPermission && !permissionManager.checkPermission(Constants.INTERNET,this)) {
            permissionManager.requestInternetPermission(this,coordinatorLayout);
        }
        else if(needPermission && !permissionManager.checkPermission(Constants.STORAGE,this)){
            permissionManager.requestReadAndWritePermission(this,coordinatorLayout);
        }
        else if(needPermission && !permissionManager.checkPermission(Constants.GET_ACCOUNTS,this)){
            permissionManager.requestGetAccounts(this,coordinatorLayout);
        }
        else if(needPermission && !permissionManager.checkPermission(Constants.WAKELOCK,this)){
            permissionManager.requestWakeLock(this,coordinatorLayout);
        }
        else {
            Events.appStart(LoadingActivity.this);
            startTime = (int) (System.currentTimeMillis()/1000);
            DisplayMetrics displaymetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
            Constants.width = displaymetrics.widthPixels;
            Constants.height = displaymetrics.heightPixels;
            UIHelper.getScreenCategory(LoadingActivity.this);

            final SharedPreferenceManager sp = new SharedPreferenceManager();
            PathManager.initPaths(this);
            FileHelper.copyAssetsDirToLocal(this);
            KeyMaker.makeKey(this);
            final DatabaseHelper db = new DatabaseHelper();
            db.createDataBase();

            assetJsonDownload = new DownloadHelper();
            animationJsonDownload = new DownloadHelper();

            loadProgressBar();
            (new Thread(new Runnable() {
                @Override
                public void run() {
                    assetJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/assetsDetailv5.json", db, LoadingActivity.this, sp, Constants.ASSET_JSON);
                }
            })).start();
            (new Thread(new Runnable() {
                @Override
                public void run() {
                    animationJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/animationDetail.json", db, LoadingActivity.this, sp, Constants.ANIMATION_JSON);
                }
            })).start();
        }
    }

    private void loadProgressBar()
    {
        final Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(loadingBar != null)
                            loadingBar.setProgress(loadingBar.getProgress()+1);
                        process++;
                        if(process == 50)
                            timer.cancel();
                            startSceneSelection();
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

    public void startSceneSelection()
    {
        if(loadingBar.getProgress() >= 100) {
            if(!activityStarted) {
                int finishTime = (int) Math.abs(((System.currentTimeMillis()/1000)) - startTime);
                Events.loadingCompleted(LoadingActivity.this,finishTime);
                activityStarted = true;
                i.putExtra("isNotification", false);
                i.putExtra("fromLoading",true);
                startActivity(i);
                LoadingActivity.this.dealloc();
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if(requestCode == Constants.REQUEST_GET_ACCOUNTS ||requestCode == Constants.REQUEST_INTERNET || requestCode == Constants.REQUEST_STORAGE || requestCode == Constants.REQUEST_WAKELOCK)
            init();
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
