package com.smackall.animator.AnimationAssetsView;

/**
 * Created by Sabish on 25/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.SearchView;
import android.widget.TextView;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.plus.Plus;
import com.google.android.gms.plus.model.people.Person;
import com.smackall.animator.AnimationEditorView.AnimationEditor;
import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.Services;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNILibAssetView;
import com.smackall.animator.opengl.GL2JNIViewAssetView;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.HttpMultipartMode;
import org.apache.http.entity.mime.MultipartEntityBuilder;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import smackall.animator.R;


public class AnimationSelectionView extends Activity implements
        GoogleApiClient.ConnectionCallbacks, GoogleApiClient.OnConnectionFailedListener {

    int width, height;

    TextView anim_selection_help_text, anim_selection_trending_text, anim_selection_featured_text, anim_selection_top_rated_text, anim_selection_my_anim_text;


    public static TextView anim_selection_like_count_text, anim_selection_anim_date, anim_selection_anim_name_text,anim_selection_created_by_text,
            anim_selection_downloads_count;
    public static TextView anim_selection_anim_comp_text;

    public static Button anim_selection_add_to_scene, anim_publish_btn;

    public ImageView userImg;

    ImageView anim_selection_i_btn;
    VelocityTracker velocityTracker1;
    LinearLayout anim_selection_trending_holder, anim_selection_featured_holder, anim_selection_top_rated_holder, anim_selection_my_anim_holder;
    FrameLayout glViewAnimationView;
    GLSurfaceView glSurfaceViewAnimationSelection;
    public static ProgressBar anim_selection_progressBar;

    Button anim_selection_cancel_btn;

    public static GridView gridView;

    public static List<AnimDB> animDBs;
    public static String[] animNameList = {};
    public static String[] animImages = {};
    public static Integer[] animAssetsId ={};
    public static int selectedAssetId;

    String selectedAssetName = "";
    public  static int selectedAssetType;
    boolean trending_tab = true;
    boolean featured_tab = true;
    boolean top_rated_tab = true;
    boolean my_anim_tab = true;
    public static boolean isDownloadCanceled = false;
    int fingerCount,panBegan;
    public static int selectedAnimId = 0;
    AnimationDatabaseHandler animationDatabaseHandler;
    CustomAdapterForAnimationSelection customAdapterForAnimationSelection;

    public ImageView google_sign_in,like_btn;

    Context context;
    Activity activity;
    String userId;


    private static final int RC_SIGN_IN = 0;
    private static final String TAG = "GoogleSignIn";
    private static final int PROFILE_PIC_SIZE = 400;
    private GoogleApiClient mGoogleApiClient;
    private boolean mIntentInProgress;
    private boolean mSignInClicked;
    private ConnectionResult mConnectionResult;
    boolean isSignIn = false;
    String personName = "";
    String personPhotoUrl = "";
    String personGooglePlusProfile = "";
    String email = "";
    String id = "";
    String publishfilePathLocation;
    String publishimgPathLocation;
    String publishassetName;
    String publishkeyword;
    String publishusername;
    String publishtype;
    String publishuniqueId;
    String publishemail;
    String publishasset_id;
    String publishbonecount;
    File publishAnimationFile,publishImageFile;
    boolean disableEvents;

    AnimationJsonParsing animationJsonParsing = new AnimationJsonParsing();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animation_selection_view);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Display d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        width = d.getWidth();
        height = d.getHeight();
        disableEvents = false;
        context = this;
        GL2JNIViewAssetView.callBackSufaceCreated(false);
        selectedAnimId = 0;
        if(!Constant.isOnline(context))
            Constant.informDialog(this, "Failed connecting to the server! Please try again later.");

        animationDatabaseHandler = new AnimationDatabaseHandler(context);
        animationDatabaseHandler.createMyAnimationTable(Constant.animDatabase);
        Constant.FullScreencall(AnimationSelectionView.this,context);
        activity = AnimationSelectionView.this;
        File cacheDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+getPackageName()+"/.cache/");
        Constant.deleteDir(cacheDir);
        Constant.downloadViewType = "animation";
        selectedAssetId = this.getIntent().getIntExtra("assetId", 0);
        selectedAssetType=this.getIntent().getIntExtra("assetType", 0);
        List<AssetsDB> assetsDBs =  Constant .getAssetsDetail("assetsId", Integer.toString(selectedAssetId), "assets", "Assets");
        if(assetsDBs.size() != 0)
            selectedAssetName=assetsDBs.get(0).getAssetName();
        customAdapterForAnimationSelection = new CustomAdapterForAnimationSelection(context, animNameList, animImages, activity, AnimationSelectionView.this);

        anim_selection_help_text = (TextView) findViewById(R.id.anim_selection_help_text);
        anim_selection_trending_text = (TextView) findViewById(R.id.anim_selection_trending_text);
        anim_selection_featured_text = (TextView) findViewById(R.id.anim_selection_featured_text);
        anim_selection_top_rated_text = (TextView) findViewById(R.id.anim_selection_top_rated_text);
        anim_selection_anim_comp_text = (TextView) findViewById(R.id.anim_selection_anim_comp_text);
        anim_selection_anim_comp_text.setVisibility(View.INVISIBLE);
        anim_selection_my_anim_text = (TextView) findViewById(R.id.anim_selection_my_anim_text);
        anim_selection_like_count_text = (TextView) findViewById(R.id.anim_selection_like_count_text);
        anim_selection_anim_date = (TextView) findViewById(R.id.anim_selection_anim_date);
        anim_selection_anim_name_text = (TextView) findViewById(R.id.anim_selection_anim_name_text);
        anim_selection_created_by_text = (TextView) findViewById(R.id.anim_selection_created_by_text);
        anim_selection_downloads_count = (TextView) findViewById(R.id.anim_selection_downloads_count);
        glViewAnimationView=(FrameLayout) findViewById(R.id.glview_for_animation_selection);
        google_sign_in=(ImageView) findViewById(R.id.anim_selection_google_signin);
        anim_selection_cancel_btn = (Button) findViewById(R.id.anim_selection_cancel_btn);
        anim_selection_add_to_scene = (Button) findViewById(R.id.anim_selection_add_to_scene);
        like_btn=(ImageView) findViewById(R.id.anim_selection_like_btn);
        anim_publish_btn = (Button) findViewById(R.id.animation_publish_btn);
        anim_publish_btn.setVisibility(View.INVISIBLE);

        userImg = (ImageView) findViewById(R.id.userImg);

        anim_selection_i_btn = (ImageView) findViewById(R.id.anim_selection_i_btn);

        final SearchView searchView = (SearchView) findViewById(R.id.animationSearchView);
        searchView.setPadding(1,1,1,1);


        anim_selection_trending_holder = (LinearLayout) findViewById(R.id.anim_selection_trending_holder);
        anim_selection_featured_holder = (LinearLayout) findViewById(R.id.anim_selection_featured_holder);
        anim_selection_top_rated_holder = (LinearLayout) findViewById(R.id.anim_selection_top_rated_holder);
        anim_selection_my_anim_holder = (LinearLayout) findViewById(R.id.anim_selection_my_anim_holder);

        anim_selection_progressBar = (ProgressBar) findViewById(R.id.anim_selection_progressBar);

        gridView = (GridView) this.findViewById(R.id.gridView_animationSelection);

        velocityTracker1=  VelocityTracker.obtain();
        GL2JNILibAssetView.setViewType(5);
        glSurfaceViewAnimationSelection=new GL2JNIViewAssetView(this);
        System.out.println("Preview Scene is " + GL2JNILibAssetView.isPreviewScene());
        if(!GL2JNILibAssetView.isPreviewScene())
            GL2JNILibAssetView.initAssetView(width, height);
        System.out.println("Preview Scene is " + GL2JNILibAssetView.isPreviewScene());
        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        glViewAnimationView.addView(glSurfaceViewAnimationSelection, glParams);
        GL2JNILibAssetView.loadNodeAnimationSelection(selectedAssetId, selectedAssetType, selectedAssetName);
        glSurfaceViewAnimationSelection.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (disableEvents)
                    return false;
                if (!GL2JNIViewAssetView.isSurfaceCreated())
                    return false;
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    fingerCount = event.getPointerCount();
                    velocityTracker1.addMovement(event);
                    velocityTracker1.computeCurrentVelocity(1000);
                    float xVelocity = velocityTracker1.getXVelocity();
                    float yVelocity = velocityTracker1.getYVelocity();
                    GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                    return true;
                } else if (event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN) {
                    fingerCount = event.getPointerCount();
                    Log.d("TOUCH LISTENER", "POINTER DOWN ACTION & FINGER COUNT: " + fingerCount);
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    GL2JNILibAssetView.assetSwipetoEnd();
                    panBegan = 0;
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    fingerCount = event.getPointerCount();
                    if (fingerCount == 1) {
                        velocityTracker1.addMovement(event);
                        velocityTracker1.computeCurrentVelocity(1000);
                        float xVelocity = velocityTracker1.getXVelocity();
                        float yVelocity = velocityTracker1.getYVelocity();
                        GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                        return true;
                    } else if (fingerCount >= 2) {
                        float x1 = event.getX(0);
                        float y1 = event.getY(0);
                        float x2 = event.getX(1);
                        float y2 = event.getX(1);
                        if (panBegan == 0) {
                            GL2JNILibAssetView.paBeginanim(x1, y1, x2, y2);
                            panBegan++;
                        } else {
                            GL2JNILibAssetView.panProgressanim(x1, y1, x2, y2);
                        }

                    }

                }
                return false;
            }
        });

        anim_selection_trending_holder.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (disableEvents)
                    return false;
                if (trending_tab) {
                    anim_selection_trending_holder.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    anim_selection_featured_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_top_rated_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_my_anim_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    trending_tab = false;
                    featured_tab = true;
                    top_rated_tab = true;
                    my_anim_tab = true;
                    anim_publish_btn.setVisibility(View.INVISIBLE);
                    customAdapterForAnimationSelection.downloadManager.cancelAll();
                    loadAssetsFromDatabase("downloads", "AnimAssets");
                    gridViewForAnimationSelection();
                }
                return false;
            }
        });


        anim_selection_featured_holder.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(disableEvents)
                    return false;
                if (featured_tab) {
                    anim_selection_trending_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_featured_holder.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    anim_selection_top_rated_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_my_anim_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    trending_tab = true;
                    featured_tab = false;
                    top_rated_tab = true;
                    my_anim_tab = true;
                    anim_publish_btn.setVisibility(View.INVISIBLE);
                    customAdapterForAnimationSelection.downloadManager.cancelAll();
                    loadAssetsFromDatabase("featuredindex", "AnimAssets");
                    gridViewForAnimationSelection();
                }
                return false;
            }
        });


        anim_selection_top_rated_holder.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (disableEvents)
                    return false;
                if (top_rated_tab) {
                    anim_selection_trending_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_featured_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_top_rated_holder.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    anim_selection_my_anim_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    trending_tab = true;
                    featured_tab = true;
                    top_rated_tab = false;
                    my_anim_tab = true;
                    anim_publish_btn.setVisibility(View.INVISIBLE);
                    customAdapterForAnimationSelection.downloadManager.cancelAll();
                    loadAssetsFromDatabase("rating", "AnimAssets");
                    gridViewForAnimationSelection();
                }
                return false;
            }
        });


        anim_selection_my_anim_holder.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (disableEvents)
                    return false;
                if (my_anim_tab) {
                    anim_selection_trending_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_featured_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_top_rated_holder.setBackgroundResource(R.drawable.shape_for_tabs);
                    anim_selection_my_anim_holder.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    trending_tab = true;
                    featured_tab = true;
                    top_rated_tab = true;
                    my_anim_tab = false;
                    customAdapterForAnimationSelection.downloadManager.cancelAll();
                    loadAssetsFromDatabase("downloads", "MyAnimation");
                    gridViewForAnimationSelection();
                    if (animDBs.size() >0)
                        anim_publish_btn.setVisibility(View.VISIBLE);
                }
                return false;
            }
        });
        like_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (disableEvents)
                    return;
                if(!isSignIn){
                    Constant.informDialog(context, "Signin with google to publish the animation.");
                    return;
                }
                new likeOperation().execute();

            }
        });

        anim_publish_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (disableEvents)
                    return;
                if(!isSignIn){
                    Constant.informDialog(context, "Signin with google to publish the animation.");
                    return;
                }

                anim_publish_btn.setVisibility(View.INVISIBLE);
                UserNameDialog.userNameDialog(context, width, height, AnimationSelectionView.this);
            }
        });


        anim_selection_cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                customAdapterForAnimationSelection.downloadManager.cancelAll();
                if(disableEvents)
                    return;
                disableEvents = true;
                if(CustomAdapterForAnimationSelection.timer!=null) {
                    CustomAdapterForAnimationSelection.timer.cancel();
                    CustomAdapterForAnimationSelection.timer = null;
                }
                GL2JNILibAssetView.stopAllAnimations();
                GL2JNILibAssetView.assetViewFinish();
                GL2JNILib.resetContext();
                AnimationSelectionView.this.finish();
            }
        });

        anim_selection_add_to_scene.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                customAdapterForAnimationSelection.downloadManager.cancelAll();
                if (disableEvents)
                    return;
                disableEvents = true;
                if (CustomAdapterForAnimationSelection.timer != null) {
                    CustomAdapterForAnimationSelection.timer.cancel();
                    CustomAdapterForAnimationSelection.timer = null;
                }
                GL2JNILibAssetView.stopAllAnimations();

                if (selectedAssetType == 4) {
                    GL2JNILibAssetView.applyAnimationToNodeAnimationEditor(Constant.animationsDir + "/" + selectedAnimId + ".sgta");
                }
                if (selectedAssetType == 3) {
                    GL2JNILibAssetView.applyAnimationToNodeAnimationEditor(Constant.animationsDir + "/" + selectedAnimId + ".sgra");
                }

                Constant.animationEditor.animationFrameCount = GL2JNILib.getAnimationFrameCount();
                System.out.println("TotalFrames : " + GL2JNILib.getAnimationFrameCount());
                GL2JNILibAssetView.assetViewFinish();
                GL2JNILib.resetContext();
                AnimationSelectionView.this.finish();
            }
        });

        google_sign_in.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (disableEvents)
                    return;
                if (!isSignIn) {
                    Constant.showProgressDialog(activity, context, "Please Wait! SignIn Progress...", true);
                    signInWithGplus();
                }
                if (isSignIn) {
                    Constant.showProgressDialog(activity, context, "Please Wait! SignOut Progress...", true);
                    signOutFromGplus();
                }
            }
        });

        searchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {
                customAdapterForAnimationSelection.downloadManager.cancelAll();
                int gridViewcounts;
                String type = "";
                if (Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_RIG.getValue())
                    type = Constant.TYPE.NODE_RIG.getValue();
               else if (Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_TEXT.getValue())
                    type = Constant.TYPE.NODE_TEXT.getValue();
                else
                    return false;

                animDBs = animationDatabaseHandler.getAllAssets(query, "", "AnimAssets", type);
                if (animDBs.size() > 50)
                    gridViewcounts = 5;   //TEST
                else
                    gridViewcounts = animDBs.size();

                System.out.println("Assets Size : " + animDBs.size());
                animImages = new String[gridViewcounts];
                animNameList = new String[gridViewcounts];
                animAssetsId = new Integer[gridViewcounts];
                for (int n = 0; n < gridViewcounts; n++) {
                    animImages[n] = animDBs.get(n).getAnimAssetId();
                    animNameList[n] = animDBs.get(n).getAnimName();
                    animAssetsId[n] = Integer.parseInt(animDBs.get(n).getAnimAssetId());
                    System.out.println("Animation id : " + animDBs.get(n).getAnimAssetId());
                    System.out.println("Animation Names : " + animDBs.get(n).getAnimName());
                }

                customAdapterForAnimationSelection.firstAnimDownload = 0;
                CustomAdapterForAnimationSelection.currentAnimDownload = Constant.unDefined;
                CustomAdapterForAnimationSelection.clickedPosition = 0;
                CustomAdapterForAnimationSelection.animExtension = "";
                gridViewForAnimationSelection();
                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(searchView.getWindowToken(), 0);

                return false;
            }

            @Override
            public boolean onQueryTextChange(String newText) {

                return false;
            }
        });
        // Constant.showProgressDialog(activity, context, "Please Wait! SignIn Progress.", true);
        mGoogleApiClient = new GoogleApiClient.Builder(this)
                .addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this).addApi(Plus.API, Plus.PlusOptions.builder().build())
                .addScope(Plus.SCOPE_PLUS_LOGIN).build();


        loadAssetsFromDatabase("downloads","AnimAssets");
    }

    public void gridViewForAnimationSelection() {
        gridView.getLayoutParams().width = (width / 6) * animImages.length;
        gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                System.out.println("You clicked : " + position);
            }
        });

        customAdapterForAnimationSelection.result = animNameList;
        customAdapterForAnimationSelection.imageId = animImages;
        customAdapterForAnimationSelection.firstAnimDownload = 0;

        gridView.setAdapter(customAdapterForAnimationSelection);
        int cols = (int) Math.ceil((double) animImages.length / 1);
        gridView.setNumColumns(cols);
        System.out.println("No of Column : " + cols);
    }
    public void loadAssetsFromDatabase(String type, String table){

        if(animationDatabaseHandler != null && animationDatabaseHandler.getAssetsCount(table) != 0 ) {
            anim_selection_progressBar.setVisibility(View.GONE);
            int gridViewcounts;
            String animationType = "";
            if(Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_RIG.getValue())
                animationType = Constant.TYPE.NODE_RIG.getValue();
            if(Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_TEXT.getValue())
                animationType = Constant.TYPE.NODE_TEXT.getValue();
            else
                return;
            animDBs = animationDatabaseHandler.getAllAssets("", type, table, animationType);
            if (animDBs.size() > 50)
                gridViewcounts = 50;   //TEST
            else
                gridViewcounts = animDBs.size();

            System.out.println("Assets Size : " + animDBs.size());
            animImages = new String[gridViewcounts];
            animNameList = new String[gridViewcounts];
            animAssetsId = new Integer[gridViewcounts];
            for (int n = 0; n < gridViewcounts; n++) {
                animImages[n] = animDBs.get(n).getAnimAssetId();
                animNameList[n] = animDBs.get(n).getAnimName();
                animAssetsId[n] = Integer.parseInt(animDBs.get(n).getAnimAssetId());
                System.out.println("Animation id : " + animDBs.get(n).getAnimAssetId());
                System.out.println("Animation Names : " + animDBs.get(n).getAnimName());
            }

            gridViewForAnimationSelection();
        }
        else{
            try {
                anim_selection_progressBar.setVisibility(View.VISIBLE);
                Constant.animPrefs = PreferenceManager.getDefaultSharedPreferences(context);
                Constant.animPrefs.edit().putBoolean("animAdded", true).apply();
                animationJsonParsing.loadDatabaseFromJson(context, AnimationSelectionView.this, "AssetsView", AnimationSelectionView.this, animationDatabaseHandler);
                System.out.println("Redownload Animation Json");
            }
            catch (Exception e){
                System.out.println("Redownload Failed ");
                e.printStackTrace();
            }
            finally {

            }
        }
    }

    public void publishAnimation(String userName){
        List<AnimDB> animationDB =  Constant .getAssetsDetail(Constant.ANIMATION_DB.KEY_ASSETSID.getValue(),
                Integer.toString(selectedAnimId),
                Constant.ANIMATION_DB.DATABASE_NAME.getValue(),
                Constant.ANIMATION_DB.TABLE_MYANIMASSETS.getValue());

        String extention = "";
        if (Integer.parseInt(animationDB.get(0).getAnimType()) == 0)
            extention = ".sgra";
        if (Integer.parseInt(animationDB.get(0).getAnimType()) == 1)
            extention = ".sgta";

        publishfilePathLocation = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/animations/"+selectedAnimId+extention;
        publishimgPathLocation = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/animations/"+selectedAnimId+".png";
        publishassetName = animationDB.get(0).getAnimName();
        publishkeyword = animationDB.get(0).getAnimName();
        publishusername = userName;
        publishtype = animationDB.get(0).getAnimType();
        publishuniqueId =  id;
        publishemail = email;
        publishasset_id = Integer.toString(selectedAnimId);
        publishbonecount = animationDB.get(0).getBonecount();

        publishAnimationFile = new File(publishfilePathLocation);
        publishImageFile = new File(publishimgPathLocation);
        new PublishAnimation().execute();
    }

    protected void onStart() {
        super.onStart();
        Log.d("AnimationSEclectionview","Connecting");
        mGoogleApiClient.connect();
    }

    protected void onStop() {
        super.onStop();
        if (mGoogleApiClient.isConnected()) {
            mGoogleApiClient.disconnect();
        }
    }

    /**
     * Method to resolve any signin errors
     * */
    private void resolveSignInError() {
        Log.d("AnimationSEclectionview","Resolve Signin error: "+mConnectionResult.getErrorMessage());

        if (mConnectionResult.hasResolution()) {
            try {
                mIntentInProgress = true;
                mConnectionResult.startResolutionForResult(this, RC_SIGN_IN);
            } catch (IntentSender.SendIntentException e) {
                mIntentInProgress = false;
                mGoogleApiClient.connect();
            }
        }
    }

    private void updateUI(boolean isSignedIn) {
        this.isSignIn = isSignedIn;
        if (isSignedIn) {
            google_sign_in.setImageDrawable(getResources().getDrawable(R.drawable.google_signout));
            userImg.setVisibility(View.VISIBLE);
            if(Constant.progressAlertDialog != null && Constant.progressAlertDialog.isShowing())
                Constant.progressAlertDialog.dismiss();
        }
        else {
            google_sign_in.setImageDrawable(getResources().getDrawable(R.drawable.google_signin));
            userImg.setVisibility(View.GONE);
            if(Constant.progressAlertDialog != null && Constant.progressAlertDialog.isShowing())
                Constant.progressAlertDialog.dismiss();
        }
    }

    @Override
    public void onConnectionFailed(ConnectionResult result) {
        if (!result.hasResolution()) {
            GooglePlayServicesUtil.getErrorDialog(result.getErrorCode(), this,
                    0).show();
            Log.d("AnimationSEclectionview", "Connection failed: "+ result.getErrorMessage());
            return;
        }

        if (!mIntentInProgress) {
            mConnectionResult = result;
            if (mSignInClicked) {
                resolveSignInError();
            }
        }

    }

    @Override
    protected void onActivityResult(int requestCode, int responseCode,
                                    Intent intent) {
        if (requestCode == RC_SIGN_IN) {
            if (responseCode != RESULT_OK) {
                mSignInClicked = false;
            }

            mIntentInProgress = false;

            if (!mGoogleApiClient.isConnecting()) {
                mGoogleApiClient.connect();
            }
        }
    }

    @Override
    public void onConnected(Bundle arg0) {
        mSignInClicked = false;
        getProfileInformation();
        updateUI(true);
    }

    private void getProfileInformation() {
        try {
            if (Plus.PeopleApi.getCurrentPerson(mGoogleApiClient) != null) {
                Person currentPerson = Plus.PeopleApi
                        .getCurrentPerson(mGoogleApiClient);
                personName = currentPerson.getDisplayName();
                personPhotoUrl = currentPerson.getImage().getUrl();
                personGooglePlusProfile = currentPerson.getUrl();
                email = Plus.AccountApi.getAccountName(mGoogleApiClient);
                id = currentPerson.getId();
                personPhotoUrl = personPhotoUrl.substring(0,
                        personPhotoUrl.length() - 2)
                        + PROFILE_PIC_SIZE;
                new LoadProfileImage(userImg).execute(personPhotoUrl);
            } else {

            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onConnectionSuspended(int arg0) {
        mGoogleApiClient.connect();
        updateUI(false);
    }

    private void signInWithGplus() {
        if (!mGoogleApiClient.isConnecting()) {
            mSignInClicked = true;
            resolveSignInError();
        }

    }

    private void signOutFromGplus() {
        if (mGoogleApiClient.isConnected()) {
            Plus.AccountApi.clearDefaultAccount(mGoogleApiClient);
            Plus.AccountApi.revokeAccessAndDisconnect(mGoogleApiClient);
            mGoogleApiClient.disconnect();
            mGoogleApiClient.connect();
            updateUI(false);

        }
    }

    private class LoadProfileImage extends AsyncTask<String, Void, Bitmap> {
        ImageView bmImage;

        public LoadProfileImage(ImageView bmImage) {
            this.bmImage = bmImage;
        }

        protected Bitmap doInBackground(String... urls) {
            String urldisplay = urls[0];
            Bitmap mIcon11 = null;
            try {
                InputStream in = new java.net.URL(urldisplay).openStream();
                mIcon11 = BitmapFactory.decodeStream(in);
            } catch (Exception e) {
                Log.e("Error", e.getMessage());
                e.printStackTrace();
            }
            return mIcon11;
        }

        protected void onPostExecute(Bitmap result) {
            bmImage.setImageBitmap(Constant.getRoundedCornerBitmap(result));
        }
    }

    private class PublishAnimation extends AsyncTask<String,String,String> {
        HttpClient httpclient;
        HttpPost httppost;
        ResponseHandler<String> responseHandler = new BasicResponseHandler();
        String response;
        @Override
        protected String doInBackground(String... params) {
            try {
                httpclient = new DefaultHttpClient();
                httppost = new HttpPost("http://www.iyan3dapp.com/appapi/publish.php");
                MultipartEntityBuilder builder = MultipartEntityBuilder.create();
                builder.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);
                if(publishAnimationFile.exists()){
                    File animationFile= new File(publishfilePathLocation);
                    FileBody fb = new FileBody(animationFile);
                    builder.addPart("animationFile", fb);
                } else {
                    Log.e("Error", "File Not Found");
                }
                if(publishImageFile.exists()){
                    File animationimgFile= new File(publishimgPathLocation);
                    FileBody fb = new FileBody(animationimgFile);
                    builder.addPart("animationImgFile", fb);
                }
                else {
                    Log.e("Error", "File Not Found");
                }

                builder.addTextBody("userid", publishuniqueId);
                builder.addTextBody("uniqueId", publishuniqueId);
                builder.addTextBody("email", publishemail);
                builder.addTextBody("username", publishusername);
                builder.addTextBody("asset_name",publishassetName);
                builder.addTextBody("keyword",publishkeyword);
                builder.addTextBody("bonecount",publishbonecount);
                builder.addTextBody("asset_id", publishasset_id);
                builder.addTextBody("type", publishtype);

                final HttpEntity yourEntity = builder.build();
                httppost.setEntity(yourEntity);

                response = httpclient.execute(httppost,responseHandler);

            } catch (IOException e) {
                AnimationSelectionView.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Constant.progressAlertDialog.dismiss();
                        Constant.informDialog(context, "Something Wrong! Please Try again!");
                    }
                });
                e.printStackTrace();
            }

            AnimationSelectionView.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    anim_publish_btn.setVisibility(View.VISIBLE);
                }
            });
            return response;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(String s) {
            if (s != null) {
                String responseStr = null;
                responseStr = s;
                Log.v(TAG, "Response Publish id : " + responseStr);
                String time  = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
                List<AnimDB> animationDB =  Constant .getAssetsDetail(Constant.ANIMATION_DB.KEY_ASSETSID.getValue(),
                        Integer.toString(selectedAnimId),
                        Constant.ANIMATION_DB.DATABASE_NAME.getValue(),
                        Constant.ANIMATION_DB.TABLE_MYANIMASSETS.getValue());

                ContentValues values = new ContentValues();

                values.put(Constant.ANIMATION_DB.KEY_ASSETSID.getValue(), animationDB.get(0).getAnimAssetId());
                values.put(Constant.ANIMATION_DB.KEY_ANIM_NAME.getValue(), animationDB.get(0).getAnimName());
                values.put(Constant.ANIMATION_DB.KEY_KEYWORD.getValue(), animationDB.get(0).getKeyword());
                values.put(Constant.ANIMATION_DB.KEY_USERID.getValue(), userId);
                values.put(Constant.ANIMATION_DB.KEY_USERNAME.getValue(), publishusername);
                values.put(Constant.ANIMATION_DB.KEY_TYPE.getValue(), animationDB.get(0).getAnimType());
                values.put(Constant.ANIMATION_DB.KEY_BONECOUNT.getValue(), animationDB.get(0).getBonecount());
                values.put(Constant.ANIMATION_DB.KEY_FEATUREDINDEX.getValue(), animationDB.get(0).getFeaturedindex());
                values.put(Constant.ANIMATION_DB.KEY_UPLOADEDTIME.getValue(), time);
                values.put(Constant.ANIMATION_DB.KEY_DOWNLOADS.getValue(), animationDB.get(0).getDownloads());
                values.put(Constant.ANIMATION_DB.KEY_RATING.getValue(), animationDB.get(0).getRating());
                values.put(Constant.ANIMATION_DB.PUBLISH_ID.getValue(), -1);

                SQLiteDatabase animDb = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

                animDb.update(Constant.ANIMATION_DB.TABLE_MYANIMASSETS.getValue(),values,Constant.ANIMATION_DB.KEY_ASSETSID.getValue()+ " = "+animationDB.get(0).getAnimAssetId(),null);

                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Constant.progressAlertDialog.dismiss();
                        Constant.informDialog(context, "Your Aniamtion Successfully Published!");
                    }
                });
            }
            super.onPostExecute(s);
        }
    }

    private class likeOperation extends AsyncTask<String,String,String>{
        HttpResponse  response1;
        @Override
        protected String doInBackground(String... params) {
            String userId= id;
            String assetId= Integer.toString(selectedAnimId);
            String rating= "1";
            String download="0";
            ResponseHandler<String> responseHandler = new BasicResponseHandler();
            String response = null;
            HttpClient httpclient = new DefaultHttpClient();
            HttpPost httppost = new HttpPost("http://www.iyan3dapp.com/appapi/createUserInfo.php");
            MultipartEntityBuilder builder = MultipartEntityBuilder.create();
            builder.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);
            builder.addTextBody("userid", userId);
            builder.addTextBody("asset_id", assetId);
            builder.addTextBody("rating", rating);
            builder.addTextBody("download", download);
            try {
                final HttpEntity yourEntity = builder.build();
                httppost.setEntity(yourEntity);
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                response = httpclient.execute(httppost,responseHandler);
            } catch (IOException e) {
                e.printStackTrace();
            }

            return response;
        }

        @Override
        protected void onPostExecute(String s) {

            if (s != null) {
               System.out.println("Like Button response : " + s);
            }
            super.onPostExecute(s);
        }
    }

    private class DownloadOperation extends AsyncTask<String,String,String>{
        HttpResponse  response1;
        @Override
        protected String doInBackground(String... params) {
            String userId= id;
            String assetId= Integer.toString(selectedAnimId);
            String rating= "0";
            String download="1";
            HttpClient httpclient = new DefaultHttpClient();
            HttpPost httppost = new HttpPost("http://www.iyan3dapp.com/appapi/createUserInfo.php");
            MultipartEntityBuilder builder = MultipartEntityBuilder.create();
            builder.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);
            builder.addTextBody("userid", userId);
            builder.addTextBody("asset_id", assetId);
            builder.addTextBody("rating", rating);
            builder.addTextBody("download", download);
            try {
                final HttpEntity yourEntity = builder.build();
                httppost.setEntity(yourEntity);
                // execute HTTP post request

            } catch (Exception e) {
                e.printStackTrace();
            }
            // execute HTTP post request
            try {
                response1 = httpclient.execute(httppost);

            } catch (IOException e) {
                e.printStackTrace();
            }

            return null;
        }

        @Override
        protected void onPostExecute(String s) {
            HttpEntity resEntity = response1.getEntity();
            if (resEntity != null) {

                String responseStr = null;
                try {
                    responseStr = EntityUtils.toString(resEntity).trim();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Log.v(TAG, "Response Download action : " +  responseStr);

                // you can add an if statement here and do other actions based on the response
            }
            super.onPostExecute(s);
        }
    }

    @Override
    public void onBackPressed(){
        customAdapterForAnimationSelection.downloadManager.cancelAll();
        if(disableEvents)
            return;
        disableEvents = true;
        if(CustomAdapterForAnimationSelection.timer!=null) {
            CustomAdapterForAnimationSelection.timer.cancel();
            CustomAdapterForAnimationSelection.timer = null;
        }
        GL2JNILibAssetView.stopAllAnimations();
        GL2JNILibAssetView.assetViewFinish();
        AnimationSelectionView.this.finish();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(!GL2JNILibAssetView.isPreviewScene())
            GL2JNILibAssetView.initAssetView(width, height);
        stopService(new Intent(this, Services.class));
    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }

    public void publishBtnHandler(int position){
        if(!my_anim_tab) {
            final List<AnimDB> animationDB = Constant.getAssetsDetail(Constant.ANIMATION_DB.KEY_ASSETSID.getValue(),
                    AnimationSelectionView.animDBs.get(position).getAnimAssetId(),
                    Constant.ANIMATION_DB.DATABASE_NAME.getValue(),
                    Constant.ANIMATION_DB.TABLE_MYANIMASSETS.getValue());

            System.out.println("Publish Id : " + animationDB.get(0).publishedId);

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (animationDB.get(0).getpublishedId().toString().equals("0")) {
                        AnimationSelectionView.anim_publish_btn.setVisibility(View.VISIBLE);
                    } else {
                        AnimationSelectionView.anim_publish_btn.setVisibility(View.INVISIBLE);
                    }
                }
            });

        }

    }
}
