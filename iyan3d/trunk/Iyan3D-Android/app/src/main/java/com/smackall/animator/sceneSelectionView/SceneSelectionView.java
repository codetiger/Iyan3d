package com.smackall.animator.sceneSelectionView;

/**
 * Created by Sabish on 30/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.SearchView;
import android.widget.TextView;
import android.widget.Toast;

import com.smackall.animator.AnimationEditorView.AnimationEditor;
import com.smackall.animator.ImportAndRig.ImportObjAndTexture;
import com.smackall.animator.InAppPurchase.PremiumUpgrade;
import com.smackall.animator.common.AssetsManager;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.Services;
import com.smackall.animator.sceneSelectionView.DB.SceneDB;
import com.smackall.animator.sceneSelectionView.DB.SceneSelectionDatabaseHandler;
import com.smackall.animator.sceneSelectionView.newCoverFlow.SceneSelectionCoverFlow;
import com.smackall.animator.sceneSelectionView.newCoverFlow.ViewGroupExampleAdapter;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import smackall.animator.R;


public class SceneSelectionView extends Activity {

    public static Context context;

    TextView scene_selection_iyan3d_text, scene_selection_help_text, scene_selection_scenes_text;
    TextView scene_selection_new_text, scene_selection_clone_text, scene_selection_delete_text, scene_selection_feedback_text, scene_selection_rateme_text;
    TextView scene_selection_import_and_rig_text, scene_selection_trails_left_text;

    SearchView searchView;

    public LinearLayout new_btn, clone_btn, delete_btn, feedback_btn, rateme_btn, import_btn, help_btn_in_scene_selectionView;

    List<SceneDB> scenes = null;

    private static final String TABLE_SCENES = "Scenes";
    private static final String KEY_ID = "id";
    private static final String KEY_SCENE_NAME = "sceneName";
    private static final String KEY_IMAGE = "image";
    private static final String KEY_TIME = "time";

    AssetsManager assetsManager;
    SceneSelectionViewDialogs sceneSelectionViewDialogs;
    SceneSelectionDatabaseHandler db = new SceneSelectionDatabaseHandler(this);


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scene_selection_view);
        final Display d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        Constant.width = d.getWidth();
        Constant.height = d.getHeight();
        Constant.currentItemID = 0;
        context = this;
        Constant.sceneSelectionView = this;

        Constant.FullScreencall(SceneSelectionView.this,context);
        new_btn = (LinearLayout) findViewById(R.id.new_btn);
        clone_btn = (LinearLayout) findViewById(R.id.clone_btn);
        delete_btn = (LinearLayout) findViewById(R.id.delete_btn);
        feedback_btn = (LinearLayout) findViewById(R.id.feedback_btn);
        rateme_btn = (LinearLayout) findViewById(R.id.rateme_btn);
        import_btn = (LinearLayout) findViewById(R.id.import_btn);

        help_btn_in_scene_selectionView = (LinearLayout) findViewById(R.id.help_btn_in_scene_selectionView);
        scene_selection_iyan3d_text = (TextView) findViewById(R.id.scene_selection_iyan3d_text);
        scene_selection_help_text = (TextView) findViewById(R.id.scene_selection_help_text);
        scene_selection_scenes_text = (TextView) findViewById(R.id.scene_selection_scenes_text);
        scene_selection_new_text = (TextView) findViewById(R.id.scene_selection_new_text);
        scene_selection_clone_text = (TextView) findViewById(R.id.scene_selection_clone_text);
        scene_selection_delete_text = (TextView) findViewById(R.id.scene_selection_delete_text);
        scene_selection_feedback_text = (TextView) findViewById(R.id.scene_selection_feedback_text);
        scene_selection_import_and_rig_text = (TextView) findViewById(R.id.scene_selection_import_and_rig_text);
        scene_selection_rateme_text = (TextView) findViewById(R.id.scene_selection_rateme_text);

        searchView = (SearchView) findViewById(R.id.searchView);

        assetsManager = new AssetsManager();
        sceneSelectionViewDialogs = new SceneSelectionViewDialogs();

      //  scene_selection_trails_left_text.setText("Trial Left "+ Integer.toString(4-KeyMaker.getRigCount(context)));

        try{
            File dir = new File(Constant.sceneDatabaseDirectory);
            Constant.mkDir(dir);

            File sourceDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+getPackageName()+"/scenes/");
            if(!sourceDir.exists()) {
                sourceDir.mkdirs();

                assetsManager.copyFile(this, "sample.png", Constant.md5("Stay Hungry Stay Foolish"), ".png", null);
                assetsManager.copyFile(this, "sample.sgb",Constant.md5("Stay Hungry Stay Foolish"),".sgb",Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/projects");
            }
           SQLiteDatabase  sceneDatabase;
            sceneDatabase = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

            String CREATE_CONTACTS_TABLE = "CREATE TABLE " + TABLE_SCENES + "("
                    + KEY_ID + " INTEGER PRIMARY KEY," + KEY_SCENE_NAME + " TEXT,"
                    + KEY_IMAGE + " TEXT," + KEY_TIME + " TEXT"+ ")";
            sceneDatabase.execSQL(CREATE_CONTACTS_TABLE);
        }
        catch (SQLiteException e) {
        }

        if(db.getScensCount() == 0) {
            String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
            db.addScene(new SceneDB("Stay Hungry Stay Foolish",Constant.md5("Stay Hungry Stay Foolish"), date));
        }

        scenes = db.getAllScenes("", "");

        ViewGroupExampleAdapter.sceneImages = new String[scenes.size()];
        ViewGroupExampleAdapter.sceneNames = new String[scenes.size()];
        ViewGroupExampleAdapter.sceneCreatedTime = new String[scenes.size()];

        for(int i = 0; i < db.getScensCount(); i++){
            ViewGroupExampleAdapter.sceneNames[i] = scenes.get(i).getName();
            ViewGroupExampleAdapter.sceneImages[i] = scenes.get(i).getImage();
            ViewGroupExampleAdapter.sceneCreatedTime[i] = scenes.get(i).getTime();
        }

        SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) findViewById(R.id.sceneSelectionCoverFlow);
        fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());

        fancyCoverFlow.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (Constant.currentItemID == position) {
                    System.out.println("Scene Name : " + ViewGroupExampleAdapter.sceneNames[position]);
                    Intent intent = new Intent();
                    intent.setClass(SceneSelectionView.this, AnimationEditor.class);
                    intent.putExtra("projectFileName", Constant.md5(ViewGroupExampleAdapter.sceneNames[position]));
                    intent.putExtra("projectName", ViewGroupExampleAdapter.sceneNames[position]);
                    startActivity(intent);
                    SceneSelectionView.this.finish();
                }
            }
        });


        new_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sceneSelectionViewDialogs.newSceneDialog(context, SceneSelectionView.this, db);
                System.out.println("You touched in New Button");
            }
        });

        clone_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sceneSelectionViewDialogs.cloneSceneDialog(context, SceneSelectionView.this, db);
                System.out.println("You touched in Clone Button");
            }
        });

        delete_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sceneSelectionViewDialogs.deleteSceneDialog(context, SceneSelectionView.this, db);
                System.out.println("You touched in Delete Button");
            }
        });

        feedback_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent email = new Intent(Intent.ACTION_SEND);
                email.putExtra(android.content.Intent.EXTRA_EMAIL,new String[] { "iyan3d@smackall.com" });
                //email.putExtra(Intent.EXTRA_EMAIL, "iyan3d@smackall.com");
                email.putExtra(Intent.EXTRA_SUBJECT, "Feedback on Iyan3D app ( Device Model " + Build.MANUFACTURER.toUpperCase() + " " + Build.MODEL + " , Android Version " + Build.VERSION.RELEASE + " )");
                email.setType("message/rfc822");

                try {
                    context.startActivity(email);
                }
                catch (ActivityNotFoundException e){
                    Constant.informDialog(context, "Email Account not Configured.");
                }
                System.out.println("You touched in Feedback Button");
            }
        });

        rateme_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Uri uri = Uri.parse("market://details?id=" + getPackageName());
                Intent myAppLinkToMarket = new Intent(Intent.ACTION_VIEW, uri);
                try {
                    startActivity(myAppLinkToMarket);
                } catch (ActivityNotFoundException e) {
                    Toast.makeText(getApplicationContext(), "Googel PlayStore Not found!!!",
                            Toast.LENGTH_LONG).show();
                }
                System.out.println("You touched in Rate Me Button");
            }
        });

        import_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Constant.isPremium(context)){
                    Intent intent = new Intent();
                    intent.setClass(SceneSelectionView.this, ImportObjAndTexture.class);
                    startActivity(intent);
                    SceneSelectionView.this.finish();
                }else{
                    Intent in= new Intent(context,PremiumUpgrade.class);
                    in.putExtra("actiontype","importobj");
                    context.startActivity(in);
                }
//                if(!Constant.isPremium ) {
//                        if(Constant.isTempPremium){
//                            Intent intent = new Intent();
//                            intent.setClass(SceneSelectionView.this, ImportObjAndTexture.class);
//                            startActivity(intent);
//                            Constant.isTempPremium=false;
//                            SceneSelectionView.this.finish();
//                        }
//                        else{
//                            Intent intent = new Intent(SceneSelectionView.this, PremiumUpgrade.class);
//                            startActivity(intent);
//                        }
//                }
//                else {
//                    Intent intent = new Intent();
//                    intent.setClass(SceneSelectionView.this, ImportObjAndTexture.class);
//                    startActivity(intent);
//                    SceneSelectionView.this.finish();
//                }
            }
        });

        help_btn_in_scene_selectionView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent();
                i.setClass(SceneSelectionView.this, SceneSelectionHelpView.class);
                startActivity(i);
            }
        });

        searchView.setOnQueryTextFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {

            }
        });

        searchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {
                return false;
            }

            @Override
            public boolean onQueryTextChange(String newText) {

                scenes = db.getAllScenes(newText, "");

                if (scenes.isEmpty()) {
                    ViewGroupExampleAdapter.sceneImages = null;
                    ViewGroupExampleAdapter.sceneNames = null;
                    ViewGroupExampleAdapter.sceneCreatedTime = null;
                    SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) findViewById(R.id.sceneSelectionCoverFlow);
                    fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());
                }

                if (!scenes.isEmpty()) {
                    ViewGroupExampleAdapter.sceneImages = new String[scenes.size()];
                    ViewGroupExampleAdapter.sceneNames = new String[scenes.size()];
                    ViewGroupExampleAdapter.sceneCreatedTime = new String[scenes.size()];

                    for (int i = 0; i < scenes.size(); i++) {
                        ViewGroupExampleAdapter.sceneNames[i] = scenes.get(i).getName();
                        ViewGroupExampleAdapter.sceneImages[i] = scenes.get(i).getImage();
                        ViewGroupExampleAdapter.sceneCreatedTime[i] = scenes.get(i).getTime();
                    }

                    SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) findViewById(R.id.sceneSelectionCoverFlow);
                    fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());
                }
                return false;
            }
        });

//        Intent i = new Intent();
//        i.setClass(SceneSelectionView.this, SceneSelectionHelpView.class);
//        startActivity(i);
    }

    public void onBackPressed(){
        moveTaskToBack(true);
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
}







