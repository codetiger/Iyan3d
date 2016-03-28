package com.smackall.animator;

import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.RecyclerView;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListPopupWindow;
import android.widget.ProgressBar;

import com.smackall.animator.Adapters.FrameAdapter;
import com.smackall.animator.Adapters.ObjectListAdapter;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.Delete;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.ImageManager;
import com.smackall.animator.Helper.MissingAssetHandler;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.PopUpManager;
import com.smackall.animator.Helper.RenderManager;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.TouchControl;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.Helper.VideoManager;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNIView;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Sabish.M on 08/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class EditorView extends AppCompatActivity implements View.OnClickListener{

    public  DatabaseHelper db = new DatabaseHelper();
    InfoPopUp infoPopUp = new InfoPopUp();
    private AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    private DownloadManager downloadManager = new DownloadManagerClass();
    public SharedPreferenceManager sharedPreferenceManager = new SharedPreferenceManager();
    ListPopupWindow listView;

    public GLSurfaceView glView;

    public DownloadHelper downloadHelper;
    public AssetSelection assetSelection;
    public TextSelection textSelection;
    public AnimationSelection animationSelection;
    public ImageSelection imageSelection;
    public OBJSelection objSelection;
    public FrameAdapter frameAdapter;
    public Settings settings;
    public ObjectListAdapter objectListAdapter;
    public LightProps lightProps;
    public CameraProps cameraProps;
    public MeshProps meshProps;
    public ColorPicker colorPicker;
    public Scale scale;
    public EnvelopScale envelopScale;
    public Play play;
    public Login login;
    public Export export;
    public CloudRenderingProgress cloudRenderingProgress;
    public RenderManager renderManager;
    public PopUpManager popUpManager;
    public ImageManager imageManager;
    public AdditionalLight additionalLight;
    public TextureSelection textureSelection;
    public VideoManager videoManager;
    public VideoSelection videoSelection;
    public Rig rig;
    public Delete delete;
    public Save save;
    public Publish publish;
    public MissingAssetHandler missingAssetHandler;
    public UndoRedo undoRedo;

    ImageView referenceImg;
    boolean isActivityStartFirstTime = true;
    public String projectName;
    public boolean renderingPaused = false;
    public boolean isDisplayPrepared = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_editor_view);
        referenceImg = (ImageView)findViewById(R.id.last_frame_img);
        Bundle bundle = getIntent().getExtras();
        int position = bundle.getInt("scenePosition");
        List<SceneDB> sceneDBs = db.getAllScenes();
        projectName = sceneDBs.get(position).getImage();
        showOrHideLoading(Constants.SHOW);
        initViews();
        swapViews();
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        // TODO Auto-generated method stub
        super.onWindowFocusChanged(hasFocus);
        if(isActivityStartFirstTime) {
            isActivityStartFirstTime = false;
            referenceImg = (ImageView) findViewById(R.id.last_frame_img);
            initFrameGrid();
            renderManager.cameraPosition();
        }
    }

    private void initFrameGrid()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                frameAdapter = new FrameAdapter(EditorView.this, referenceImg);
                ((LinearLayout) findViewById(R.id.recycleViewHolder)).getLayoutParams().height = ((LinearLayout) findViewById(R.id.frameHolder)).getHeight();
                ((LinearLayout) findViewById(R.id.frameHolder)).getLayoutParams().height = referenceImg.getHeight();
                RecyclerView frames = (RecyclerView) findViewById(R.id.frames);
                frames.getLayoutParams().width = ((LinearLayout) findViewById(R.id.frameHolder)).getWidth();
                frames.getLayoutParams().height = referenceImg.getHeight();
                int[] location = new int[2];
                ((LinearLayout) findViewById(R.id.frameHolder)).getLocationOnScreen(location);
                frames.setX(location[0]);
                frames.setItemAnimator(new DefaultItemAnimator());
                frames.setLayoutManager(new LinearLayoutManager(EditorView.this, LinearLayoutManager.HORIZONTAL, false));
                frames.setAdapter(frameAdapter);
            }
        });
    }

    private void initViews()
    {
        downloadHelper = new DownloadHelper();
        assetSelection = new AssetSelection(this,this.db,addToDownloadManager,downloadManager);
        textSelection = new TextSelection(this,this.db,addToDownloadManager,downloadManager);
        imageSelection = new ImageSelection(this);
        animationSelection = new AnimationSelection(this,this.db,addToDownloadManager,downloadManager,sharedPreferenceManager);
        objSelection = new OBJSelection(EditorView.this,db);
        settings = new Settings(EditorView.this,sharedPreferenceManager);
        play = new Play(EditorView.this,(RecyclerView) findViewById(R.id.frames));
        lightProps = new LightProps(EditorView.this);
        cameraProps = new CameraProps(EditorView.this,sharedPreferenceManager);
        meshProps = new MeshProps(EditorView.this);
        colorPicker = new ColorPicker(EditorView.this);
        scale = new Scale(EditorView.this);
        envelopScale = new EnvelopScale(EditorView.this);
        login = new Login(EditorView.this);
        cloudRenderingProgress = new CloudRenderingProgress(EditorView.this);
        export = new Export(EditorView.this,sharedPreferenceManager);
        new TouchControl(EditorView.this);
        popUpManager = new PopUpManager(EditorView.this);
        renderManager = new RenderManager(EditorView.this,sharedPreferenceManager);
        additionalLight = new AdditionalLight(EditorView.this);
        listView = new ListPopupWindow(EditorView.this);
        objectListAdapter = new ObjectListAdapter(EditorView.this,listView.getListView(),0,this.sharedPreferenceManager.getInt(EditorView.this,"multiSelect"));
        imageManager = new ImageManager(EditorView.this);
        textureSelection = new TextureSelection(EditorView.this);
        videoManager = new VideoManager(EditorView.this);
        videoSelection = new VideoSelection(EditorView.this);
        delete = new Delete(EditorView.this);
        save = new Save(EditorView.this,db);
        rig = new Rig(EditorView.this,sharedPreferenceManager,db);
        publish = new Publish(EditorView.this);
        missingAssetHandler = new MissingAssetHandler(EditorView.this);
        undoRedo = new UndoRedo(EditorView.this,db);
        new GL2JNILib().setGl2JNIView(EditorView.this);
        GL2JNILib.setAssetspath(PathManager.DefaultAssetsDir, PathManager.LocalDataFolder, PathManager.LocalImportedImageFolder);
        glView = new GL2JNIView(EditorView.this);
        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        ((FrameLayout)findViewById(R.id.glView)).addView(glView, glParams);
        renderManager.glView = glView;
    }



    public void goToFirstOrLastFrame(final View view)
    {
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.frames);
        recyclerView.scrollToPosition((view.getTag().equals("firstFrame")) ? 0 : GL2JNILib.totalFrame() - 1);
        final int frame = (view.getTag().equals("firstFrame")) ? 0 : GL2JNILib.totalFrame() - 1;
        renderManager.setCurrentFrame(frame);
        frameAdapter.notifyDataSetChanged();
    }

    public void openInfoPopUp(View v)
    {
        infoPopUp.infoPopUpMenu(EditorView.this, v);
    }

    public void showLogin(View v)
    {
        cloudRenderingProgress.showCloudRenderingProgress(v, null);
    }
    public void addFramePopUpMenu(View v)
    {
        PopupMenu popup = new PopupMenu(this, v);
        popup.getMenuInflater().inflate(R.menu.add_frames_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.ONE_FRAME:
                        GL2JNILib.addFrame(Constants.ONE_FRAME);
                        break;
                    case Constants.TWENTY_FOUR_FRAME:
                        GL2JNILib.addFrame(Constants.TWENTY_FOUR_FRAME);
                        break;
                    case Constants.TWO_FORTY_FRAME:
                        GL2JNILib.addFrame(Constants.TWO_FORTY_FRAME);
                        break;
                }
                frameAdapter.notifyDataSetChanged();
                goToFirstOrLastFrame(((LinearLayout) findViewById(R.id.lastFrameBtn)));
                return true;
            }
        });
        popup.show();
    }

    public void viewsPopUpMenu(View v)
    {
        PopupMenu popup = new PopupMenu(this, v);
        popup.getMenuInflater().inflate(R.menu.view_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                renderManager.cameraView(item.getOrder());
                return true;
            }
        });
        popup.show();
    }

    public void addRightView()
    {
        ((FrameLayout)findViewById(R.id.autorig_bottom_tool)).setVisibility(View.GONE);
        ((FrameLayout)findViewById(R.id.publishFrame)).setVisibility(View.GONE);
        ViewGroup insertPointParent = (sharedPreferenceManager.getInt(EditorView.this,"toolbarPosition") == 1 ) ? (ViewGroup)((ViewGroup) findViewById(R.id.rightView)).getParent()
                : (ViewGroup)((ViewGroup) findViewById(R.id.leftView)).getParent();
        ViewGroup insertPoint = null;
        for(int i = 0; i < insertPointParent.getChildCount(); i++){
            if(insertPointParent.getChildAt(i).getTag() != null && insertPointParent.getChildAt(i).getTag().toString().equals("-1")) {
                insertPoint = (ViewGroup) insertPointParent.getChildAt(i);
                continue;
            }
            insertPointParent.getChildAt(i).setVisibility(View.GONE);
        }
        if(insertPoint == null) return;
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) getApplicationContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = vi.inflate(R.layout.toolbar_view,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        v.findViewById(R.id.my_object_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.play_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.import_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.animation_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.option_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.export_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.move_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.rotate_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.scale_btn).setOnClickListener(EditorView.this);
        v.findViewById(R.id.undo).setOnClickListener(EditorView.this);
        v.findViewById(R.id.redo).setOnClickListener(EditorView.this);

    }


    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.play_btn:
                if(play == null)
                    play = new Play(EditorView.this,(RecyclerView)findViewById(R.id.frames));
                play.play(false);
                break;
            case R.id.my_object_btn:
                initSceneObjectList(v);
                break;
            case R.id.import_btn:
                showImportPopUpMenu(v);
                break;
            case R.id.animation_btn:
                showAnimationPopUp(v);
                break;
            case R.id.option_btn:
                popUpManager.initPopUpManager(GL2JNILib.getSelectedNodeId(),v,null);
                break;
            case R.id.export_btn:
                showExportPopUp(v);
                break;
            case R.id.move_btn:
                renderManager.setMove();
                break;
            case R.id.rotate_btn:
                renderManager.setRotate();
                break;
            case R.id.scale_btn:
                if(GL2JNILib.scale())
                    scale.showScale(v,null,GL2JNILib.scaleValueX(),GL2JNILib.scaleValueY(),GL2JNILib.scaleValueZ());
                break;
            case R.id.undo:
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.undo(undoRedo);
                    }
                });
                break;
            case R.id.redo:
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.redo(undoRedo);
                    }
                });
                break;
        }
    }

    private void initSceneObjectList(View v)
    {
        objectListAdapter.listView = listView.getListView();
        objectListAdapter.objectCount = GL2JNILib.getNodeCount();
        objectListAdapter.isMultiSelectEnable = (this.sharedPreferenceManager.getInt(EditorView.this,"multiSelect") == 1);
        listView.setAdapter(objectListAdapter);
        listView.setAnchorView(v);
        listView.show();
        listView.getListView().setChoiceMode(AbsListView.CHOICE_MODE_MULTIPLE);
        objectListAdapter.listView = listView.getListView();
        objectListAdapter.listView.setMultiChoiceModeListener(objectListAdapter);
        objectListAdapter.removeSelection();
    }

    public void reloadMyObjectList()
    {
        objectListAdapter.objectCount = GL2JNILib.getNodeCount();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                objectListAdapter.notifyDataSetChanged();
            }
        });
    }

    public void deleteNodeAtPosition(int position,boolean isUndoRedo){
        renderManager.removeObject(position, isUndoRedo);
    }

    public void showImportPopUpMenu(View v)
    {
        PopupMenu popup = new PopupMenu(this, v, Gravity.CENTER_HORIZONTAL);
        popup.getMenuInflater().inflate(R.menu.import_menu, popup.getMenu());

        try {
            Field[] fields = popup.getClass().getDeclaredFields();
            for (Field field : fields) {
                if ("mPopup".equals(field.getName())) {
                    field.setAccessible(true);
                    Object menuPopupHelper = field.get(popup);
                    Class<?> classPopupHelper = Class.forName(menuPopupHelper
                            .getClass().getName());
                    Method setForceIcons = classPopupHelper.getMethod(
                            "setForceShowIcon", boolean.class);
                    setForceIcons.invoke(menuPopupHelper, true);
                    break;
                }
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }

        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.IMPORT_MODELS:
                        if (assetSelection == null)
                            assetSelection = new AssetSelection(EditorView.this, db,addToDownloadManager,downloadManager);
                        Constants.VIEW_TYPE = Constants.ASSET_VIEW;
                        assetSelection.showAssetSelection(Constants.ASSET_VIEW);
                        break;
                    case Constants.IMPORT_IMAGES:
                        if(imageSelection == null)
                            imageSelection = new ImageSelection(EditorView.this);
                        imageSelection.showImageSelection();
                        break;
                    case Constants.IMPORT_VIDEOS:
                        videoSelection.showVideoSelection();
                        break;
                    case Constants.IMPORT_TEXT:
                        if (textSelection == null)
                            textSelection = new TextSelection(EditorView.this, db,addToDownloadManager,downloadManager);
                        Constants.VIEW_TYPE = Constants.TEXT_VIEW;
                        textSelection.showTextSelection();
                        break;
                    case Constants.IMPORT_LIGHT:
                        additionalLight.addLight();
                        break;
                    case Constants.IMPORT_OBJ:
                        if (objSelection == null)
                            objSelection= new OBJSelection(EditorView.this, db);
                        objSelection.showObjSelection(Constants.OBJ_MODE);
                        break;
                    case Constants.IMPORT_ADD_BONE:
                        rig.rig();
                        break;
                    case Constants.IMPORT_PARTICLE:
                        if (assetSelection == null)
                            assetSelection = new AssetSelection(EditorView.this, db,addToDownloadManager,downloadManager);
                        Constants.VIEW_TYPE = Constants.PARTICLE_VIEW;
                        assetSelection.showAssetSelection(Constants.PARTICLE_VIEW);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    public void showAnimationPopUp(View v)
    {
        PopupMenu popup = new PopupMenu(this, v, Gravity.CENTER_HORIZONTAL);
        popup.getMenuInflater().inflate(R.menu.animation_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.APPLY_ANIMATION:
                        if (animationSelection == null)
                            animationSelection = new AnimationSelection(EditorView.this, db,addToDownloadManager,downloadManager,sharedPreferenceManager);
                        Constants.VIEW_TYPE = Constants.ANIMATION_VIEW;
                        animationSelection.showAnimationSelection();
                        break;
                    case Constants.SAVE_ANIMATION:
                        if (GL2JNILib.getSelectedNodeId() <= 1 ||
                                (!(GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_RIG) &&
                                        !(GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_TEXT_SKIN)) ||
                                GL2JNILib.isJointSelected())
                            UIHelper.informDialog(EditorView.this,"Please select a text or character to save the animation as a template.");
                        else
                            save.enterNameForAnimation();
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    public void showExportPopUp(View v)
    {
        PopupMenu popup = new PopupMenu(this, v, Gravity.CENTER_HORIZONTAL);
        popup.getMenuInflater().inflate(R.menu.export_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.EXPORT_IMAGES:
                        export.showExport(Constants.EXPORT_IMAGES);
                        break;
                    case Constants.EXPORT_VIDEO:
                        export.showExport(Constants.EXPORT_VIDEO);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    public void showOrHideToolbarView(final int hideOrShow)
    {
//        View rightView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? (ViewGroup) findViewById(R.id.rightView).getParent() : (ViewGroup) findViewById(R.id.leftView).getParent();
//        View leftView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? (ViewGroup) findViewById(R.id.leftView).getParent() : (ViewGroup) findViewById(R.id.rightView).getParent();//

        System.out.println("Called " + hideOrShow);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(hideOrShow == Constants.SHOW) {
                    if ((sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 0)) {
                        ((ViewGroup) findViewById(R.id.leftView).getParent()).setVisibility(View.VISIBLE);
                        ((ViewGroup) findViewById(R.id.rightView).getParent()).setVisibility(View.INVISIBLE);
                    }
                    else
                    {
                        ((ViewGroup) findViewById(R.id.leftView).getParent()).setVisibility(View.INVISIBLE);
                        ((ViewGroup) findViewById(R.id.rightView).getParent()).setVisibility(View.VISIBLE);
                    }
                }
                else if(hideOrShow == Constants.HIDE){
                    if ((sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 0)) {
                        ((ViewGroup) findViewById(R.id.leftView).getParent()).setVisibility(View.INVISIBLE);
                        ((ViewGroup) findViewById(R.id.rightView).getParent()).setVisibility(View.VISIBLE);
                    }
                    else{
                        ((ViewGroup) findViewById(R.id.leftView).getParent()).setVisibility(View.VISIBLE);
                        ((ViewGroup) findViewById(R.id.rightView).getParent()).setVisibility(View.INVISIBLE);
                    }
                }

            }
        });

//        float rightValue = 0.0f;
//        float leftValue = 0.0f;
//
//        if (hideOrShow == Constants.HIDE) {
//            rightValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? rightView.getWidth() : rightView.getWidth() * -1;
//            leftValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? 0 : 0;
//        } else {
//            rightValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? 0 : 0;
//            leftValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? leftView.getWidth() * -1 : leftView.getWidth();
//        }
//
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1) {
//            rightView.animate().translationX(rightValue);
//            leftView.animate().translationX(leftValue);
//        }
        if (hideOrShow == Constants.SHOW)
            dellocAllSubViews();
    }

    public void showOrHideLoading(final int hideOrShow)
    {
        EditorView.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                System.out.println("Loading View Hidded  " + hideOrShow);
                ((ProgressBar) findViewById(R.id.loading_progress)).setVisibility((hideOrShow == Constants.SHOW) ? View.VISIBLE : View.INVISIBLE);
            }
        });
    }

    public void swapViews() {
        final View leftView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? (ViewGroup) findViewById(R.id.leftView).getParent()
                : (ViewGroup) findViewById(R.id.rightView).getParent();
        //leftView.animate().translationX((sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? -leftView.getWidth() : leftView.getWidth());
        ViewGroup insertPoint = null;
        for (int i = 0; i < ((ViewGroup) leftView).getChildCount(); i++) {
            if (((ViewGroup) leftView).getChildAt(i).getTag() != null && ((ViewGroup) leftView).getChildAt(i).getTag().toString().equals("-1")) {
                insertPoint = (ViewGroup) ((ViewGroup) leftView).getChildAt(i);
                continue;
            }
            ((ViewGroup) leftView).getChildAt(i).setVisibility(View.GONE);
        }
        if (insertPoint != null)
            insertPoint.removeAllViews();
        showOrHideToolbarView(Constants.SHOW);
        addRightView();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(requestCode == Constants.IMAGE_IMPORT_RESPONSE)
            imageManager.startActivityForResult(data,requestCode,resultCode);
        else if(requestCode == Constants.OBJ_IMPORT_RESPONSE){

        }
        else if(requestCode == Constants.VIDEO_IMPORT_RESPONSE){
            videoManager.startActivityForResult(data,requestCode,resultCode);
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        showOrHideLoading(Constants.SHOW);
        GL2JNILib.save(projectName);
        renderingPaused = true;
        glView.onPause();
        final Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                GL2JNILib.dealloc();
                glView.setWillNotDraw(true);
                Intent i = new Intent(EditorView.this, SceneSelection.class);
                startActivity(i);
                finish();
                timer.cancel();
            }
        }, 500);
        dellocAllSubViews();
        dealloc();
    }

    private void dellocAllSubViews()
    {
        assetSelection = null;
        textSelection = null;
        imageSelection = null;
        animationSelection = null;
        objSelection = null;
    }
    private void dealloc()
    {
        db = null;
        infoPopUp = null;
        frameAdapter = null;
        referenceImg = null;
    }
}
