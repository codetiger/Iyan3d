package com.smackall.iyan3dPro;

import android.app.PendingIntent;
import android.content.ContentResolver;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.design.widget.CoordinatorLayout;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.app.AppCompatDelegate;
import android.util.DisplayMetrics;
import android.view.WindowManager;
import android.widget.ProgressBar;

import com.crashlytics.android.Crashlytics;
import com.crashlytics.android.ndk.CrashlyticsNdk;
import com.smackall.iyan3dPro.Analytics.HitScreens;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.DatabaseHelper;
import com.smackall.iyan3dPro.Helper.DownloadHelper;
import com.smackall.iyan3dPro.Helper.Events;
import com.smackall.iyan3dPro.Helper.FileHelper;
import com.smackall.iyan3dPro.Helper.FullScreen;
import com.smackall.iyan3dPro.Helper.LicenceCheck.AESObfuscator;
import com.smackall.iyan3dPro.Helper.LicenceCheck.LicenseChecker;
import com.smackall.iyan3dPro.Helper.LicenceCheck.LicenseCheckerCallback;
import com.smackall.iyan3dPro.Helper.LicenceCheck.ResponseData;
import com.smackall.iyan3dPro.Helper.LicenceCheck.ServerManagedPolicy;
import com.smackall.iyan3dPro.Helper.PathManager;
import com.smackall.iyan3dPro.Helper.PermissionManager;
import com.smackall.iyan3dPro.Helper.SharedPreferenceManager;
import com.smackall.iyan3dPro.Helper.UIHelper;

import java.util.Timer;
import java.util.TimerTask;

import io.fabric.sdk.android.Fabric;

public class LoadingActivity extends AppCompatActivity implements LicenseCheckerCallback {

    private static final byte[] SALT = new byte[]{
            -46, 65, 30, -128, -103, -57, 74, -64, 51, 88, -95,
            -45, 77, -117, -36, -113, -11, 32, -64, 89
    };

    static {
        AppCompatDelegate.setCompatVectorFromResourcesEnabled(true);
    }

    public ProgressBar loadingBar;
    DownloadHelper assetJsonDownload;
    DownloadHelper animationJsonDownload;
    int process = 0;
    boolean activityStarted = false;
    int startTime = 0;
    Intent i;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if ((getIntent().getFlags() & Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT) != 0) {
            Intent intent;
            if (Constants.currentActivity != -1) {
                int currentActivity = Constants.currentActivity;
                Class cl = (currentActivity == 0) ? SceneSelection.class : EditorView.class;
                intent = new Intent(Intent.ACTION_MAIN);
                intent.setClass(this, cl);
                intent.addCategory(Intent.ACTION_MAIN);
                intent.addCategory(Intent.CATEGORY_LAUNCHER);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP |
                        Intent.FLAG_ACTIVITY_SINGLE_TOP);
                if (getIntent().getExtras() != null)
                    intent.putExtras(getIntent().getExtras());
            } else {
                intent = new Intent(this, SceneSelection.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                if (getIntent().getExtras() != null)
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
        if (!isTaskRoot()) {
            Intent intent = getIntent();
            String action = intent.getAction();
            if (action.compareTo(Intent.ACTION_VIEW) == 0) {
                String scheme = intent.getScheme();
                ContentResolver resolver = getContentResolver();
                if (scheme.compareTo(ContentResolver.SCHEME_FILE) == 0) {
                    Uri uri = intent.getData();
                    String name = uri.getPath();
                    if (name != null && FileHelper.checkValidFilePath(name)) {
                        intent.putExtra("hasExtraForOpenWith", true);
                        intent.putExtra("i3dPath", name);
                        i.putExtra("fromLoading", false);
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
        Fabric.with(this, new Crashlytics(), new CrashlyticsNdk());
        HitScreens.LoadingView(LoadingActivity.this);
        Constants.deviceUniqueId = Settings.Secure.getString(getContentResolver(),
                Settings.Secure.ANDROID_ID);
        loadingBar = (ProgressBar) findViewById(R.id.loadingBar);
        float space = Constants.getFreeSpace();
        if (space < 50.0) {
            UIHelper.informDialog(LoadingActivity.this, getString(R.string.phone_storage_low_need_50mb), true);
        }
        i = new Intent(LoadingActivity.this, SceneSelection.class);
        Intent intent = getIntent();
        String action = intent.getAction();
        if (action != null && action.compareTo(Intent.ACTION_VIEW) == 0) {
            String scheme = intent.getScheme();
            if (scheme.compareTo(ContentResolver.SCHEME_FILE) == 0) {
                Uri uri = intent.getData();
                String name = uri.getPath();
                if (name != null && FileHelper.checkValidFilePath(name)) {
                    i.putExtra("hasExtraForOpenWith", true);
                    i.putExtra("i3dPath", name);
                    i.putExtra("fromLoading", false);
                }
            }
        }

        LicenseChecker mChecker = new LicenseChecker(
                this, new ServerManagedPolicy(this,
                new AESObfuscator(SALT, getPackageName(), Constants.deviceUniqueId)),
                "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAhgEytquA6AZ5e4O4NqrwtaqgRUGOqX1aS6gsJsDQass+10U3u/LMTqP+UAdYYC1B57emgiZRHEVr8S4UQScmlh6ahMPgPl4UDavgf3M8nbHhMJbxNyPnlZib7fHkBeBBFmttJ1ZRXKBaN2gu6KLOAdJgyKV4bM+7jwn2DExFZ5zP8BeelGajavlHiP+zyRiM66t0nhFes0sWwoBsW2dscY3qkd+OYKmykFN/mxzfl+4Qyf93petdWnWvThl/mHM02CTwZidTAAfKLbLF3cL1S//o4jLYvqsYVybAxyZpDpvP0lzUXamQDnVQPldRJcU0nOjJwzznk0f4r00+nmB/9QIDAQAB"  // Your public licensing key.
        );

        mChecker.checkAccess(this);

        interpolateRot();
    }

    void interpolateRot()
    {
        int start = -90;//(int) Math.toDegrees(Math.atan2(200,100));
        int end = 0; //(int) Math.toDegrees(Math.atan2(0,100));
        int startFrame = 0;
        int endFrame = 24;
        int current = 5;

        System.out.println("Start End  : " +start + " " + end);

//
//        if(start < 0)
//            start += 360;
//        if(end < 0)
//            end += 360;


        for (int i = current; i < endFrame; i++){
            int angle = start + ((Math.abs(end-start)/(endFrame-startFrame)) * (i - startFrame));
            if(angle > 360)
                angle -= 360;
            else if(angle < 0)
                angle += 360;

            System.out.println("Angle : " +Math.toDegrees(Math.atan(angle)));

        }
    }

    private void init() {

        boolean needPermission = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M);
        CoordinatorLayout coordinatorLayout = ((CoordinatorLayout) findViewById(R.id.coordinatorLayout));
        PermissionManager permissionManager = new PermissionManager();
        if (needPermission && !permissionManager.checkPermission(Constants.INTERNET, this)) {
            permissionManager.requestInternetPermission(this, coordinatorLayout);
        } else if (needPermission && !permissionManager.checkPermission(Constants.STORAGE, this)) {
            permissionManager.requestReadAndWritePermission(this, coordinatorLayout);
        } else if (needPermission && !permissionManager.checkPermission(Constants.GET_ACCOUNTS, this)) {
            permissionManager.requestGetAccounts(this, coordinatorLayout);
        } else if (needPermission && !permissionManager.checkPermission(Constants.WAKELOCK, this)) {
            permissionManager.requestWakeLock(this, coordinatorLayout);
        } else {
            loadProgressBar();
            Events.archEvent();
            Events.appStart(LoadingActivity.this);
            startTime = (int) (System.currentTimeMillis() / 1000);
            DisplayMetrics displaymetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
            Constants.width = displaymetrics.widthPixels;
            Constants.height = displaymetrics.heightPixels;
            UIHelper.setScreenCategory(LoadingActivity.this);

            final SharedPreferenceManager sp = new SharedPreferenceManager();
            PathManager.initPaths(this);
            FileHelper.copyAssetsDirToLocal(this);
            FileHelper.copySingleAssetFile(LoadingActivity.this, "white_texture.png", PathManager.LocalTextureFolder,"white_texture.png");
            final DatabaseHelper db = new DatabaseHelper();
            db.createDataBase();
            loadingBar.setProgress(100);
        }
    }

    private void loadProgressBar() {
        final Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (loadingBar != null)
                            loadingBar.setProgress(loadingBar.getProgress() + 1);
                        process++;
                        if (process >= 50)
                            timer.cancel();
                        startSceneSelection();
                    }
                });
            }
        }, 1000, 100);
    }

    private void dealloc() {
        LoadingActivity.this.finish();
    }

    public void startSceneSelection() {
        if (loadingBar.getProgress() >= 100) {
            if (!activityStarted) {
                int finishTime = (int) Math.abs(((System.currentTimeMillis() / 1000)) - startTime);
                Events.loadingCompleted(LoadingActivity.this, finishTime);
                activityStarted = true;
                i.putExtra("isNotification", false);
                i.putExtra("fromLoading", true);
                startActivity(i);
                LoadingActivity.this.dealloc();
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == Constants.REQUEST_GET_ACCOUNTS || requestCode == Constants.REQUEST_INTERNET || requestCode == Constants.REQUEST_STORAGE || requestCode == Constants.REQUEST_WAKELOCK)
            init();
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    public void allow(int reason, ResponseData rawData) {
        init();
    }

    @Override
    public void dontAllow(int reason) {
        init();
       // showLicenceFailedDialog();
    }

    @Override
    public void applicationError(int errorCode) {
        init();
    }

    void showLicenceFailedDialog()
    {
        Events.licenseVerificationFailed(LoadingActivity.this);
        android.app.AlertDialog.Builder informDialog = new android.app.AlertDialog.Builder(LoadingActivity.this);
        informDialog
                .setMessage(R.string.licence_verification_failed)
                .setCancelable(false)
                .setPositiveButton(R.string.exit, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dealloc();
                    }
                });
        informDialog.create();
        try {
            informDialog.show();
        } catch (WindowManager.BadTokenException e) {
            dealloc();
            e.printStackTrace();
        }
    }
}
