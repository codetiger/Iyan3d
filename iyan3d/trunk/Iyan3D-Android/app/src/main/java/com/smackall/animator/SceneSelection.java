package com.smackall.animator;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.transition.Scene;
import android.view.View;
import android.view.WindowManager;
import android.webkit.MimeTypeMap;
import android.widget.EditText;
import android.widget.LinearLayout;

import com.android.vending.billing.IInAppBillingService;
import com.facebook.FacebookSdk;
import com.google.android.gms.analytics.Tracker;
import com.smackall.animator.Adapters.NoSpacingItemDecoreation;
import com.smackall.animator.Adapters.SceneSelectionAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.GCM.QuickstartPreferences;
import com.smackall.animator.GCM.RegistrationIntentService;
import com.smackall.animator.Helper.CheckLicense;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CreditsManager;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.FollowApp;
import com.smackall.animator.Helper.FullScreen;
import com.smackall.animator.Helper.OpenWithManager;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.PermissionManager;
import com.smackall.animator.Helper.RestoreBackUp;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.twitter.sdk.android.core.TwitterAuthConfig;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

public class SceneSelection extends AppCompatActivity implements ServiceConnection {

    public DatabaseHelper db = new DatabaseHelper();
    InfoPopUp infoPopUp = new InfoPopUp();
    public SceneSelectionAdapter sceneAdapter;

    public FollowApp followApp;
    public Login login;
    public CloudRenderingProgress cloudRenderingProgress;
    public UserDetails userDetails;
    public SharedPreferenceManager sharedPreferenceManager = new SharedPreferenceManager();
    public CreditsManager creditsManager;
    public Settings settings;
    public RestoreBackUp restoreBackUp;
    IInAppBillingService mService;
    private BroadcastReceiver mRegistrationBroadcastReceiver;
    private boolean isReceiverRegistered;
    private Tracker mTracker;

    Snackbar snackbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FullScreen.HideStatusBar(this);
        setContentView(R.layout.activity_scene_selection);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
//            Window window = getWindow();
//            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
//            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
//            window.setStatusBarColor(ContextCompat.getColor(this, R.color.topbar));
//        }
        HitScreens.SceneSelectionView(SceneSelection.this);
        Constants.currentActivity =0;
        restoreBackUp = new RestoreBackUp(this);
        initGridView();
        initClass();
        if(!CheckLicense.checkLicense(SceneSelection.this)){
            android.app.AlertDialog.Builder informDialog = new android.app.AlertDialog.Builder(SceneSelection.this);
            informDialog
                    .setMessage("License Verification Failed!")
                    .setCancelable(false)
                    .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dealloc();
                        }
                    });
            informDialog.create();
            try {
                informDialog.show();
            } catch (WindowManager.BadTokenException e) {
                e.printStackTrace();
            }
        }

        mRegistrationBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                SharedPreferences sharedPreferences =
                        PreferenceManager.getDefaultSharedPreferences(context);
                boolean sentToken = sharedPreferences
                        .getBoolean(QuickstartPreferences.SENT_TOKEN_TO_SERVER, false);
            }
        };
        Bundle data = getIntent().getExtras();
        if (data != null && data.getBoolean("isNotification")) {
            findViewById(R.id.login_btn).performClick();
        }
        Bundle a =  getIntent().getExtras();
        if(a != null && a.getBoolean("hasExtraForOpenWith")) {
            handleOpenWith(getIntent());
        }
        else {
            try {
                Intent serviceIntent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
                serviceIntent.setPackage("com.android.vending");
                SceneSelection.this.bindService(serviceIntent, SceneSelection.this, Context.BIND_AUTO_CREATE);
            }
            catch (RuntimeException ignored){}
        }

        registerReceiver();

        if (Constants.checkPlayServices(SceneSelection.this)) {
            // Start IntentService to register this application with GCM.
            Intent intent = new Intent(this, RegistrationIntentService.class);
            startService(intent);
        }

        float space = Constants.getFreeSpace();
        if(space < 50.0){
            UIHelper.informDialog(SceneSelection.this,"Phone storage is low. Minimum 50MB of space is required.",true);
        }
    }

    @Override
    protected void onResume() {
        sceneAdapter.notifyDataSetChanged();
        super.onResume();
    }

    @Override
    protected void onPause() {
        overridePendingTransition(0,0);
        super.onPause();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Bundle a = intent.getExtras();
        if(a != null && a.getBoolean("hasExtraForOpenWith")) {
            handleOpenWith(intent);
        }
        else{
            findViewById(R.id.login_btn).performClick();
        }
    }

    public void handleOpenWith(Intent intent){

        String path = intent.getStringExtra("i3dPath");
        if(path != null && !path.toLowerCase().equals("null")){
            String ext = FileHelper.getFileExt(path);
            switch (ext){
                case "png":
                    FileHelper.manageImageFile(path,SceneSelection.this);
                    UIHelper.informDialog(SceneSelection.this,"Image Imported Successfully.",true);
                    break;
                case "obj":
                    UIHelper.informDialog(SceneSelection.this,"Model Imported Successfully.",true);
                    break;
                case "ttf":
                case "otf":
                    UIHelper.informDialog(SceneSelection.this,"Font Imported Successfully.",true);
                    break;
                case "i3d":
                    restoreBackUp.restoreI3D(path);
                    break;
            }
        }
    }

    private void registerReceiver(){
        if(!isReceiverRegistered) {
            LocalBroadcastManager.getInstance(this).registerReceiver(mRegistrationBroadcastReceiver,
                    new IntentFilter(QuickstartPreferences.REGISTRATION_COMPLETE));
            isReceiverRegistered = true;
        }
    }

    private void initClass()
    {
        followApp = new FollowApp(SceneSelection.this);
        login = new Login(SceneSelection.this);
        login.initAllSignSdk();
        cloudRenderingProgress = new CloudRenderingProgress(SceneSelection.this,db,mService);
        userDetails = new UserDetails(SceneSelection.this,sharedPreferenceManager);
        creditsManager = new CreditsManager(SceneSelection.this);
        settings = new Settings(SceneSelection.this,sharedPreferenceManager,mService);
    }

    private void initGridView(){
        RecyclerView gridview = (RecyclerView) findViewById(R.id.gridView);
        sceneAdapter = new SceneSelectionAdapter(this, db,gridview);

        if (gridview != null) {
            gridview.setItemAnimator(new DefaultItemAnimator());
            GridLayoutManager gridLayoutManager = new GridLayoutManager(SceneSelection.this,4);
            gridview.setLayoutManager(gridLayoutManager);
            gridview.addItemDecoration(new NoSpacingItemDecoreation());
            gridview.setAdapter(sceneAdapter);
        }
    }

    public void addNewScene(View view)
    {
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date());
        List<SceneDB> sceneDBs = db.getAllScenes();
        String sceneName = (sceneDBs == null || sceneDBs.size() <= 0) ? "My Scene " + 1 : "My Scene " + Integer.toString(sceneDBs.get(db.getSceneCount() - 1).getID() + 1);
        FileHelper.copySingleAssetFile(SceneSelection.this, "Stay Hungry Stay Foolish.png", FileHelper.md5(sceneName), ".png", null);
        db.addNewScene(new SceneDB(sceneName, FileHelper.md5(sceneName), date));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
        RecyclerView gridview = (RecyclerView) findViewById(R.id.gridView);
        if(gridview != null)
            gridview.scrollToPosition((sceneAdapter != null && sceneAdapter.sceneDBs != null) ? this.sceneAdapter.sceneDBs.size() : 0);
    }

    public void showRenameDialog(final int position){
        final AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setMessage("Enter Scene Name");
        final EditText input = new EditText(this);
        input.setHint("Scene Name");
        alert.setView(input);
        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (input.getText().toString().trim().length() != 0) {
                    if (FileHelper.isItHaveSpecialChar(input.getText().toString().trim())) {
                        UIHelper.informDialog(SceneSelection.this, "You cannot use Special Characters in scene name.",true);
                        return;
                    }
                    List<SceneDB> sceneDBs = db.getAllSceneDetailsWithSearch(input.getText().toString());
                    if (sceneDBs.size() != 0) {
                        UIHelper.informDialog(SceneSelection.this, "Scene Already Exists.",true);
                    } else {
                        renameScene(input.getText().toString().trim(), position);
                    }
                } else {
                    dialog.dismiss();
                    UIHelper.informDialog(SceneSelection.this, "Scene name Cannot be empty.",true);
                }
            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                FullScreen.HideStatusBar(SceneSelection.this);
                dialog.dismiss();
            }
        });
        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }

    private void renameScene(String name, int position) {
        List<SceneDB> sceneDBs = db.getAllScenes();
        db.updateSceneDetails(new SceneDB(sceneDBs.get(position).getID(), name, sceneDBs.get(position).getImage(), sceneDBs.get(position).getTime()));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void deleteScene(int position)
    {
        List<SceneDB> sceneDBs = db.getAllScenes();
        File image = new File(PathManager.LocalScenesFolder+"/" + sceneDBs.get(position).getImage() + ".png");
        File projectFile = new File(PathManager.LocalProjectFolder+"/" + sceneDBs.get(position).getImage() + ".sgb");
        File backUpFile = new File(PathManager.LocalProjectFolder+"/" + sceneDBs.get(position).getImage() + ".i3d");
        if(projectFile.exists())
            projectFile.delete();
        if(image.exists())
            image.delete();
        if(backUpFile.exists())
            backUpFile.delete();
        db.deleteScene(sceneDBs.get(position).getName());
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void cloneScene(int position)
    {
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss",Locale.getDefault()).format(new Date());
        List<SceneDB> sceneDBs = db.getAllScenes();
        String sceneName = (sceneDBs.size() <= 0) ? "My Scene " + 1 : "My Scene " + Integer.toString(sceneDBs.get(db.getSceneCount() - 1).getID() + 1);
        File thumpnailFrom = new File(PathManager.LocalScenesFolder+"/" + sceneDBs.get(position).getImage() + ".png");
        File thumpnailTo = new File(PathManager.LocalScenesFolder+"/" + FileHelper.md5(sceneName) + ".png");

        File projectFrom = new File(PathManager.LocalProjectFolder+"/" + sceneDBs.get(position).getImage() + ".sgb");
        File projectTo = new File(PathManager.LocalProjectFolder + "/" + FileHelper.md5(sceneName) + ".sgb");
        if(thumpnailFrom.exists())
            FileHelper.copy(thumpnailFrom, thumpnailTo);
        if (projectFrom.exists())
            FileHelper.copy(projectFrom, projectTo);
        db.addNewScene(new SceneDB(sceneName, FileHelper.md5(sceneName), date));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void backUp(String fileName){
        Intent shareIntent = new Intent();
        shareIntent.setAction(Intent.ACTION_SEND);
        shareIntent.putExtra(Intent.EXTRA_STREAM,Uri.fromFile(new File(PathManager.LocalProjectFolder+"/"+fileName)));
        shareIntent.setType("*/*");
        startActivity(Intent.createChooser(shareIntent,"Backup to"));
    }

    public void loadScene(int position) {
        Intent editorScene = new Intent(SceneSelection.this,EditorView.class);
        editorScene.putExtra("scenePosition", position);
        editorScene.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        editorScene.setFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        overridePendingTransition(0,0);
        startActivity(editorScene);
        dealloc();
    }

    public void openSceneProps(View v){
        if(infoPopUp != null)
            infoPopUp.infoPopUpMenu(SceneSelection.this, v);
    }

    public void showLogIn(View v)
    {
        userDetails.updateUserDetails();
        if(userDetails.signInType > 0)
            cloudRenderingProgress.showCloudRenderingProgress(v, null);
        else
            login.showLogin(v, null);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1002){
            cloudRenderingProgress.purchaseResult(requestCode,resultCode,data);
        }
        else if(requestCode == TwitterAuthConfig.DEFAULT_AUTH_REQUEST_CODE) {
            login.twitterButton.onActivityResult(requestCode, resultCode, data);
        }
        else if(FacebookSdk.isFacebookRequestCode(requestCode)) {
            login.callbackManager.onActivityResult(requestCode, resultCode, data);
        }
        else if(requestCode == Constants.GOOGLE_SIGNIN_REQUESTCODE){
            login.googleSignInActivityResult(requestCode,resultCode,data);
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        moveTaskToBack(true);
    }



    private void dealloc()
    {
        try {
            unbindService(SceneSelection.this);
        }
        catch (IllegalArgumentException e){
            e.printStackTrace();
        }

        db = null;
        sceneAdapter = null;
        infoPopUp = null;

        this.finish();
    }

    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        mService = IInAppBillingService.Stub.asInterface(service);
        settings.mService = mService;
        cloudRenderingProgress.mService = mService;
    }

    @Override
    public void onServiceDisconnected(ComponentName name) {
        mService = null;
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        FullScreen.HideStatusBar(SceneSelection.this);
        super.onWindowFocusChanged(hasFocus);
    }
}
