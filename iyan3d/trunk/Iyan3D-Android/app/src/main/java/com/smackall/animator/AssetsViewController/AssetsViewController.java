package com.smackall.animator.AssetsViewController;

/**
 * Created by Sabish on 29/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
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

import com.smackall.animator.common.Constant;
import com.smackall.animator.common.Services;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNILibAssetView;
import com.smackall.animator.opengl.GL2JNIViewAssetView;

import java.io.File;
import java.util.List;

import smackall.animator.R;


public class AssetsViewController extends Activity {

    public static int width, height, selectedItemId , nodeType;

    Context context;

    public static GridView gridView;
    public static TextView assets_view_selection_name;
    GLSurfaceView glSurfaceViewAssetview;
    FrameLayout glViewAssetView;
    Button assets_view_cancel_btn, assets_view_restore_btn;
    public static Button assets_view_add_to_scene_btn;
    VelocityTracker velocityTracker;
    TextView assets_view_character_text, assets_view_backgrounds_text, assets_view_accessories_text, assets_view_mylibrary_text;

    TextView top_bar_help_text;
    ImageView top_bar_info_img;

   public static ProgressBar assetsViewCenterProgressbar;

    LinearLayout assets_view_character_tab_layout, assets_view_background_tab_layout, assets_view_accessories_tab_layout, assets_view_mylibrary_tab_layout;

    boolean character_tab = false;
    boolean backgrounds_tab = true;
    boolean accessories_tab = true;
    boolean mylibrary_tab = true;
    public static boolean downloadIsCanceled = false;


    AssetsViewDatabaseHandler assetsViewDatabaseHandler;
    public static List<AssetsDB> assets;
    public static String[] prgmNameList = {};
    public static String[] prgmImages = {};
    public static Integer[] assetsId ={};

    CustomAdapterForAssetsSelection customAdaptorForAssetsSelection;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_assets_view_controller);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Display d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        width = d.getWidth();
        height = d.getHeight();
        context =  this;
        GL2JNIViewAssetView.callBackSufaceCreated(false);
        if(!Constant.isOnline(context))
            Constant.informDialog(this, "Failed connecting to the server! Please try again later.");
        Constant.FullScreencall(AssetsViewController.this,context);
        customAdaptorForAssetsSelection =  new CustomAdapterForAssetsSelection(this, prgmNameList, prgmImages, AssetsViewController.this);

        File cacheDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+getPackageName()+"/.cache/");
        Constant.deleteDir(cacheDir);

        Constant.downloadViewType = "assets";

        assets_view_character_tab_layout = (LinearLayout) findViewById(R.id.assets_view_character_tab_layout);
        assets_view_background_tab_layout = (LinearLayout) findViewById(R.id.assets_view_background_tab_layout);
        assets_view_accessories_tab_layout = (LinearLayout) findViewById(R.id.assets_view_accessories_tab_layout);
        assets_view_mylibrary_tab_layout = (LinearLayout) findViewById(R.id.assets_view_mylibrary_tab_layout);

        gridView = (GridView) this.findViewById(R.id.gridview2);

        glViewAssetView=(FrameLayout)findViewById(R.id.glview_assetsView);
        assets_view_selection_name = (TextView) findViewById(R.id.assets_view_selection_name);
        assets_view_character_text = (TextView) findViewById(R.id.assets_view_character_text);
        assets_view_backgrounds_text = (TextView) findViewById(R.id.assets_view_backgrounds_text);
        assets_view_accessories_text = (TextView) findViewById(R.id.assets_view_accessories_text);
        assets_view_mylibrary_text = (TextView) findViewById(R.id.assets_view_mylibrary_text);
        top_bar_help_text = (TextView) findViewById(R.id.top_bar_help_text);

        assets_view_cancel_btn = (Button) findViewById(R.id.assets_view_cancel_btn);
        assets_view_restore_btn = (Button) findViewById(R.id.assets_view_restore_purchase);
        assets_view_add_to_scene_btn = (Button) findViewById(R.id.assets_view_add_to_scene);

        final SearchView searchView = (SearchView) findViewById(R.id.assetsSearch);
        searchView.setPadding(1,1,1,1);

        top_bar_info_img = (ImageView) findViewById(R.id.top_bar_info_img);

        assetsViewCenterProgressbar = (ProgressBar) findViewById(R.id.assetsViewCenterProgressbar);

        assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(this);
        assetsViewDatabaseHandler.createMyLibraryTable(Constant.assetsDatabase);
        GL2JNILibAssetView.setViewType(3);
        glSurfaceViewAssetview=new GL2JNIViewAssetView(this);

        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        glViewAssetView.addView(glSurfaceViewAssetview, glParams);

        assets_view_character_tab_layout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (character_tab) {
                    assets_view_character_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    assets_view_background_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_accessories_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_mylibrary_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    character_tab = false;
                    backgrounds_tab = true;
                    accessories_tab = true;
                    mylibrary_tab = true;
                    nodeType = 1;
                    customAdaptorForAssetsSelection.firstAssetDownload = 0;
                    loadAssetsFromDatabase("1");
                    customAdaptorForAssetsSelection.downloadManager.cancelAll();
                    gridViewHorizontally();
                }
                return false;
            }
        });


        assets_view_background_tab_layout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (backgrounds_tab) {
                    assets_view_character_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_background_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    assets_view_accessories_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_mylibrary_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    character_tab = true;
                    backgrounds_tab = false;
                    accessories_tab = true;
                    mylibrary_tab = true;
                    nodeType = 2;
                    customAdaptorForAssetsSelection.firstAssetDownload = 0;
                    loadAssetsFromDatabase("2");
                    customAdaptorForAssetsSelection.downloadManager.cancelAll();
                    gridViewHorizontally();
                }
                return false;
            }
        });


        assets_view_accessories_tab_layout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (accessories_tab) {
                    assets_view_character_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_background_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_accessories_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    assets_view_mylibrary_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    character_tab = true;
                    backgrounds_tab = true;
                    accessories_tab = false;
                    mylibrary_tab = true;
                    nodeType = 3;
                    customAdaptorForAssetsSelection.firstAssetDownload = 0;
                    loadAssetsFromDatabase("3");
                    customAdaptorForAssetsSelection.downloadManager.cancelAll();
                    gridViewHorizontally();
                }
                return false;
            }
        });


        assets_view_mylibrary_tab_layout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (mylibrary_tab) {
                    assets_view_character_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_background_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_accessories_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs);
                    assets_view_mylibrary_tab_layout.setBackgroundResource(R.drawable.shape_for_tabs_pressed);
                    character_tab = true;
                    backgrounds_tab = true;
                    accessories_tab = true;
                    mylibrary_tab = false;
                    nodeType = -1; //TEST
                    loadAssetsFromDatabase("-1");
                    gridViewHorizontally();
                }
                return false;
            }
        });

        assets_view_cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                customAdaptorForAssetsSelection.downloadManager.cancelAll();
                GL2JNILib.resetClickedAssetId();
                GL2JNILibAssetView.assetViewFinish();
//                GL2JNILibAssetView.deletePreviewScene();

                GL2JNILib.resetContext();
                AssetsViewController.this.finish();

            }
        });
        assets_view_add_to_scene_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AssetsViewController.downloadIsCanceled = false;

                List<AssetsDB> assetsDBs;
                String selectedAssetName;
                if(mylibrary_tab) {
                    assetsDBs = Constant.getAssetsDetail("assetsId", Integer.toString(selectedItemId), "assets", "Assets"); // Get Selected Assets Details using AssetID
                    selectedAssetName = assetsDBs.get(0).getAssetName(); //Get Asset Name from Assets Detail
                    GL2JNILibAssetView.addAssetToScene(selectedItemId,nodeType,selectedAssetName);
                    addToMyLibrary();
                }
                else {
                    assetsDBs = Constant.getAssetsDetail("assetsId", Integer.toString(selectedItemId), "assets", Constant.ASSETS_DB.MY_LIBRARY_TABLE.getValue()); // Get Selected Assets Details using AssetID
                    selectedAssetName = assetsDBs.get(0).getAssetName(); //Get Asset Name from Assets Detail
                    GL2JNILibAssetView.addAssetToScene(selectedItemId, Integer.parseInt(assetsDBs.get(0).getType()),selectedAssetName);
                }
                  //Add selected assets to My Library

               // Constant.animationEditor.importedItems.add(selectedAssetName);//Select List

                GL2JNILibAssetView.assetViewFinish();
                GL2JNILib.resetContext();
                AssetsViewController.this.finish();
                //GL2JNILibAssetView.deletePreviewScene();

            }
        });
        nodeType = 1;
        loadAssetsFromDatabase("1");

        searchView.setOnCloseListener(new SearchView.OnCloseListener() {
            @Override
            public boolean onClose() {
                loadAssetsFromDatabase(String.valueOf(nodeType));
                InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(searchView.getWindowToken(), 0);
                return false;
            }
        });


        searchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {

                assets = assetsViewDatabaseHandler.getAllAssets(query, "", "Assets");
                System.out.println("Assets Size : " + assets.size());
                prgmImages = new String[assets.size()];
                prgmNameList = new String[assets.size()];
                assetsId = new Integer[assets.size()];
                for (int n = 0; n < assets.size(); n++) {
                    prgmImages[n] = Integer.toString(assets.get(n).getAssetsId());
                    prgmNameList[n] = assets.get(n).getAssetName();
                    assetsId[n] = assets.get(n).getAssetsId();
                    System.out.println("Assets Names : " + assets.get(n).getAssetName());
                }
                gridViewHorizontally();
                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(searchView.getWindowToken(), 0);
                CustomAdapterForAssetsSelection.clickedPosition = 0;
                customAdaptorForAssetsSelection.firstAssetDownload = 0;
                return false;
            }

            @Override
            public boolean onQueryTextChange(String newText) {

                return false;
            }
        });



        velocityTracker=  VelocityTracker.obtain();
        glSurfaceViewAssetview.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(!GL2JNIViewAssetView.isSurfaceCreated())
                    return false;
                System.out.println("Touch Working.");
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    velocityTracker.addMovement(event);
                    velocityTracker.computeCurrentVelocity(1000);
                    float xVelocity = velocityTracker.getXVelocity();
                    float yVelocity = velocityTracker.getYVelocity();
                    GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    GL2JNILibAssetView.assetSwipetoEnd();
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    velocityTracker.addMovement(event);
                    velocityTracker.computeCurrentVelocity(1000);
                    float xVelocity = velocityTracker.getXVelocity();
                    float yVelocity = velocityTracker.getYVelocity();
                    GL2JNILibAssetView.assetSwipetoRotate(xVelocity, yVelocity);
                    return true;
                }

                return false;
            }
        });
    }

    public void gridViewHorizontally() {
        gridView.getLayoutParams().width = (width / 6) * prgmImages.length;
        gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                System.out.println("You clicked : " + position);
            }
        });
        customAdaptorForAssetsSelection.result = prgmNameList;
        customAdaptorForAssetsSelection.imageId = prgmImages;
        customAdaptorForAssetsSelection.firstAssetDownload = 0;
        //CustomAdapterForAssetsSelection.clickedPosition = 0;
        gridView.setAdapter(customAdaptorForAssetsSelection);

        int cols = (int) Math.ceil((double) prgmImages.length / 1);
        System.out.println("No of Column : " + cols);
        gridView.setNumColumns(cols);
    }

    public void loadAssetsFromDatabase(String type){


            if (assetsViewDatabaseHandler != null && assetsViewDatabaseHandler.getAssetsCount("Assets") != 0) {
                assetsViewCenterProgressbar.setVisibility(View.GONE);
                if(!type.equals("-1"))
                assets = assetsViewDatabaseHandler.getAllAssets("", type, "Assets");
                else
                    assets = assetsViewDatabaseHandler.getAllAssets("", "", "MyLibrary");
                System.out.println("Assets Size : " + assets.size());
                prgmImages = new String[assets.size()];
                prgmNameList = new String[assets.size()];
                assetsId = new Integer[assets.size()];
                for (int n = 0; n < assets.size(); n++) {
                    prgmImages[n] = Integer.toString(assets.get(n).getAssetsId());
                    prgmNameList[n] = assets.get(n).getAssetName();
                    assetsId[n] = assets.get(n).getAssetsId();
                    System.out.println("Assets Names : " + assets.get(n).getAssetName());
                }
                if(assets.size() == 0)
                    assets_view_add_to_scene_btn.setVisibility(View.INVISIBLE);
                else
                    assets_view_add_to_scene_btn.setVisibility(View.VISIBLE);
                gridViewHorizontally();
            } else {
                try {
                    assetsViewCenterProgressbar.setVisibility(View.VISIBLE);
                    Constant.assetsPrefs.edit().putBoolean("added", true).apply();
                    AssetsJsonParsing assetsJsonParsing = new AssetsJsonParsing();
                    assetsJsonParsing.loadDatabaseFromJson(context, AssetsViewController.this, "AssetsView",assetsViewDatabaseHandler,this,null);
                } catch (Exception e) {

                } finally {

                }

            }

    }

    private void addToMyLibrary(){

        System.out.println("My Library asset Added Function Called");
        assetsViewDatabaseHandler.createMyLibraryTable(Constant.assetsDatabase);

        List<AssetsDB> assetsDBs =  Constant.getAssetsDetail("assetsId", Integer.toString(selectedItemId), "assets", "MyLibrary");

        if(assetsDBs.size() == 0){
            assetsDBs = Constant.getAssetsDetail("assetsId", Integer.toString(selectedItemId), "assets", "Assets");
            assetsViewDatabaseHandler.addAssets(new AssetsDB(assetsDBs.get(0).getAssetName(),
                    assetsDBs.get(0).getIap(), assetsDBs.get(0).getAssetsId()
                    , assetsDBs.get(0).getType(), assetsDBs.get(0).getNbones(), assetsDBs.get(0).getKeywords()
                    , assetsDBs.get(0).getHash(), assetsDBs.get(0).getDateTime()
            ), "MyLibrary");
        }
        else if(assetsDBs.get(0).getAssetsId() != selectedItemId) {
            assetsDBs = Constant.getAssetsDetail("assetsId", Integer.toString(selectedItemId), "assets", "Assets");
            assetsViewDatabaseHandler.addAssets(new AssetsDB(assetsDBs.get(0).getAssetName(),
                    assetsDBs.get(0).getIap(), assetsDBs.get(0).getAssetsId()
                    , assetsDBs.get(0).getType(), assetsDBs.get(0).getNbones(), assetsDBs.get(0).getKeywords()
                    , assetsDBs.get(0).getHash(), assetsDBs.get(0).getDateTime()
            ), "MyLibrary");
        }
    }

    public static void hideImportBtn(boolean hideBtn){
       if(hideBtn)
           assets_view_add_to_scene_btn.setVisibility(View.INVISIBLE);
        else
           assets_view_add_to_scene_btn.setVisibility(View.VISIBLE);
    }

    @Override
    public void onBackPressed(){
        customAdaptorForAssetsSelection.downloadManager.cancelAll();
        GL2JNILib.resetClickedAssetId();
        GL2JNILibAssetView.assetViewFinish();
//                GL2JNILibAssetView.deletePreviewScene();
        AssetsViewController.this.finish();
    }

    @Override
    protected void onResume() {
        super.onResume();
        stopService(new Intent(this, Services.class));
    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        System.out.println("Activity Destroy");
    }
}





