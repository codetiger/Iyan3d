package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.RecyclerView;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListPopupWindow;
import android.widget.ProgressBar;

import com.android.vending.billing.IInAppBillingService;
import com.facebook.FacebookSdk;
import com.smackall.animator.Adapters.FrameAdapter;
import com.smackall.animator.Adapters.ObjectListAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CreditsManager;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.Delete;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.FollowApp;
import com.smackall.animator.Helper.FullScreen;
import com.smackall.animator.Helper.ImageManager;
import com.smackall.animator.Helper.OpenWithManager;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.PopUpManager;
import com.smackall.animator.Helper.RenderManager;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.TouchControl;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.Helper.UpdateXYZValues;
import com.smackall.animator.Helper.WrapContentLinearLayoutManager;
import com.smackall.animator.Helper.ZipManager;
import com.smackall.animator.NativeCallBackClasses.NativeCallBacks;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNIView;
import com.twitter.sdk.android.core.TwitterAuthConfig;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;


/**
 * Created by Sabish.M on 08/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class EditorView extends AppCompatActivity implements View.OnClickListener , ServiceConnection{

    public  DatabaseHelper db = new DatabaseHelper();
    InfoPopUp infoPopUp = new InfoPopUp();
    public AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    private DownloadManager downloadManager = new DownloadManagerClass();
    public SharedPreferenceManager sharedPreferenceManager = new SharedPreferenceManager();
    ListPopupWindow listView;

    public GLSurfaceView glView;

    public FollowApp followApp;
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
    public OtherProps otherProps;
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
//  public VideoManager videoManager; // Video Restricted
//  public VideoSelection videoSelection;
    public Rig rig;
    public Delete delete;
    public Save save;
    public UpdateXYZValues updateXYZValues;
    public Publish publish;
    public MissingAssetHandler missingAssetHandler;
    public UndoRedo undoRedo;
    public UserDetails userDetails;
    public CreditsManager creditsManager;
    public ZipManager zipManager;
    public NativeCallBacks nativeCallBacks;

    ImageView referenceImg;

    boolean isActivityStartFirstTime = true;

    public String projectNameHash;
    public String projectName;

    public boolean renderingPaused = false;
    public boolean isDisplayPrepared = false;

    IInAppBillingService mService;

    public Context mContext;
    public ImageView demo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_editor_view);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        HitScreens.EditorView(EditorView.this);
        FullScreen.HideStatusBar(this);
        Constants.currentActivity =1;
        referenceImg = (ImageView)findViewById(R.id.last_frame_img);

        Intent serviceIntent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
        serviceIntent.setPackage("com.android.vending");
        EditorView.this.bindService(serviceIntent, EditorView.this, Context.BIND_AUTO_CREATE);

        Bundle bundle = getIntent().getExtras();
        int position = bundle.getInt("scenePosition");
        try {
            List<SceneDB> sceneDBs = db.getAllScenes();
            projectNameHash = sceneDBs.get(position).getImage();
            projectName = sceneDBs.get(position).getName();
        }
        catch (IndexOutOfBoundsException e){
            UIHelper.informDialog(EditorView.this,"Something wrong please restart Iyan3D App",false);
        }
        showOrHideLoading(Constants.SHOW);
        initViews();
        swapViews();
        mContext = EditorView.this;
        demo = (ImageView) findViewById(R.id.demo);
        Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Bundle a = intent.getExtras();
        if(a != null && a.getBoolean("hasExtraForOpenWith")) {
            Uri uri = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
            String path = OpenWithManager.handleOpenWithFile(uri,EditorView.this);
            if(path != null && !path.toLowerCase().equals("null")){
                String ext = FileHelper.getFileExt(path);
                switch (ext){
                    case "png":
                        if(imageManager != null)
                            imageManager.manageImageFile(path);
                        UIHelper.informDialog(EditorView.this,"Image Imported Successfully.");
                        if(imageSelection != null)
                            imageSelection.notifyDataChanged();
                        if(textureSelection != null)
                            textureSelection.notifyDataChanged();
                        if(objSelection != null)
                            objSelection.notifyDataChanged();
                        break;
                    case "obj":
                        UIHelper.informDialog(EditorView.this,"Model Imported Successfully.");
                        if(objSelection!=null)
                            objSelection.notifyDataChanged();
                        break;
                    case "ttf":
                    case "otf":
                        UIHelper.informDialog(EditorView.this,"Font Imported Successfully.");
                        if(textSelection != null)
                            textSelection.notifyDataSetChanged(true);
                        break;
                }
            }
        }
        else{
            findViewById(R.id.login).performClick();
        }
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        // TODO Auto-generated method stub
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus)
            FullScreen.HideStatusBar(EditorView.this);
        if(isActivityStartFirstTime) {
            isActivityStartFirstTime = false;
            referenceImg = (ImageView) findViewById(R.id.last_frame_img);
            initFrameGrid();
            renderManager.cameraPosition(Constants.height - ((FrameLayout) findViewById(R.id.glView)).getHeight());
        }
    }

    @Override
    protected void onPause() {
        if(export != null && export.isRendering)
            export.destroyRendering();
        overridePendingTransition(0,0);
        super.onPause();
    }
    @Override
    protected void onResume() {
        super.onResume();
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
                frames.setLayoutManager(new WrapContentLinearLayoutManager(EditorView.this, LinearLayoutManager.HORIZONTAL, false));
                frames.setAdapter(frameAdapter);
            }
        });
    }

    private void initViews()
    {
        followApp = new FollowApp(EditorView.this);
        downloadHelper = new DownloadHelper();
        assetSelection = new AssetSelection(this,this.db,addToDownloadManager,downloadManager);
        textSelection = new TextSelection(this,this.db,addToDownloadManager,downloadManager);
        imageSelection = new ImageSelection(this);
        animationSelection = new AnimationSelection(this,this.db,addToDownloadManager,downloadManager,sharedPreferenceManager);
        objSelection = new OBJSelection(EditorView.this,db);
        settings = new Settings(EditorView.this,sharedPreferenceManager,mService);
        play = new Play(EditorView.this,(RecyclerView) findViewById(R.id.frames));
        lightProps = new LightProps(EditorView.this);
        cameraProps = new CameraProps(EditorView.this,sharedPreferenceManager);
        meshProps = new MeshProps(EditorView.this);
        otherProps = new OtherProps(EditorView.this);
        colorPicker = new ColorPicker(EditorView.this);
        scale = new Scale(EditorView.this);
        envelopScale = new EnvelopScale(EditorView.this);
        login = new Login(EditorView.this);
        login.initAllSignSdk();
        cloudRenderingProgress = new CloudRenderingProgress(EditorView.this,db,mService);
        export = new Export(EditorView.this,sharedPreferenceManager);
        new TouchControl(EditorView.this);
        popUpManager = new PopUpManager(EditorView.this);
        renderManager = new RenderManager(EditorView.this,sharedPreferenceManager);
        additionalLight = new AdditionalLight(EditorView.this);
        listView = new ListPopupWindow(EditorView.this);
        objectListAdapter = new ObjectListAdapter(EditorView.this,listView.getListView(),0,this.sharedPreferenceManager.getInt(EditorView.this,"multiSelect"));
        imageManager = new ImageManager(EditorView.this);
        textureSelection = new TextureSelection(EditorView.this);
//      videoManager = new VideoManager(EditorView.this);
//      videoSelection = new VideoSelection(EditorView.this);// Video Restricted
        delete = new Delete(EditorView.this);
        save = new Save(EditorView.this,db);
        updateXYZValues = new UpdateXYZValues(EditorView.this);
        updateXYZValues.updateXyzValue(true,0.0f,0.0f,0.0f);
        rig = new Rig(EditorView.this,sharedPreferenceManager,db);
        publish = new Publish(EditorView.this);
        missingAssetHandler = new MissingAssetHandler(EditorView.this);
        undoRedo = new UndoRedo(EditorView.this,db);
        userDetails = new UserDetails(EditorView.this,sharedPreferenceManager);
        userDetails.updateUserDetails();
        creditsManager = new CreditsManager(EditorView.this);
        zipManager = new ZipManager(EditorView.this,db);
        nativeCallBacks = new NativeCallBacks(EditorView.this);
//      GL2JNILib.initVideoManagerClass(videoManager);// Video Restricted
        new GL2JNILib().setGl2JNIView(EditorView.this);
        GL2JNILib.setAssetspath(PathManager.DefaultAssetsDir, PathManager.LocalDataFolder, PathManager.LocalImportedImageFolder);
        glView = new GL2JNIView(EditorView.this,sharedPreferenceManager);
        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        ((FrameLayout)findViewById(R.id.glView)).addView(glView, glParams);
        renderManager.glView = glView;
    }

    public void goToFirstOrLastFrame(View view)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
        showOrHideLoading(Constants.SHOW);
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.frames);
        if (recyclerView != null) {
            recyclerView.scrollToPosition((view.getTag().equals("firstFrame")) ? 0 : GL2JNILib.totalFrame() - 1);
        }
        final int frame = (view.getTag().equals("firstFrame")) ? 0 : GL2JNILib.totalFrame() - 1;
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.setCurrentFrame(frame,nativeCallBacks);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        frameAdapter.notifyDataSetChanged();
                    }
                });
            }
        });
    }

    public void openInfoPopUp(View v) {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
        infoPopUp.infoPopUpMenu(EditorView.this, v);
    }

    public void showLogin(View v)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
        userDetails.updateUserDetails();
        if(userDetails.signInType > 0)
            cloudRenderingProgress.showCloudRenderingProgress(v, null);
        else
            login.showLogin(v, null);
    }

    public void addFramePopUpMenu(View v)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
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

                if(!isDisplayPrepared) return true;
                if(GL2JNILib.isPlaying()) return true;
                RecyclerView recyclerView = (RecyclerView)findViewById(R.id.frames);
                if (recyclerView != null) {
                    recyclerView.scrollToPosition(GL2JNILib.totalFrame() - 1);
                }
                return true;
            }
        });
        popup.show();
    }

    public void viewsPopUpMenu(View v)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
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

    public void addRightView(boolean isRigMode)
    {
        if(isRigMode){
            rig.addRigToolBar();
            return;
        }

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
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
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
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
        switch (v.getId()){
            case R.id.play_btn:
                if(GL2JNILib.currentFrame() >= GL2JNILib.totalFrame()-1) return;
                if(play == null)
                    play = new Play(EditorView.this,(RecyclerView)findViewById(R.id.frames));
                play.updatePhysics(false);
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
                final View view = v;
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if(GL2JNILib.scale(nativeCallBacks))
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    scale.showScale(view,null,GL2JNILib.scaleValueX(),GL2JNILib.scaleValueY(),GL2JNILib.scaleValueZ());
                                }
                            });
                    }
                });
                break;
            case R.id.undo:
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.undo(((EditorView)mContext).nativeCallBacks);
                    }
                });
                break;
            case R.id.redo:
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.redo(((EditorView)mContext).nativeCallBacks);
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

    public void deleteNodeAtPosition(int position,boolean isUndoRedo) {
        renderManager.removeObject(position, isUndoRedo);
    }

    public void showImportPopUpMenu(View v)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
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
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
//                            videoSelection.showVideoSelection();// Video Restricted
                        }
                        else
                            UIHelper.informDialog(EditorView.this,"Your device doesn't support video. Need minimum OS Version 4.2");
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
                        showParticlesView();
                }
                return true;
            }
        });
        popup.show();
    }

    public void showParticlesView()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                    if (assetSelection == null)
                        assetSelection = new AssetSelection(EditorView.this, db, addToDownloadManager, downloadManager);
                    Constants.VIEW_TYPE = Constants.PARTICLE_VIEW;
                    assetSelection.showAssetSelection(Constants.PARTICLE_VIEW);
            }
        });
    }

    public void showAnimationPopUp(View v)
    {
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
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
                        if(userDetails.signInType <= 0){
                            UIHelper.showSignInPanelWithMessage(EditorView.this,"Please sign in to save your animation.");
                        }
                        else if (GL2JNILib.getSelectedNodeId() <= 1 ||
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
        if(!isDisplayPrepared) return;
        if(GL2JNILib.isPlaying()) return;
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
                Constants.VIEW_TYPE = Constants.RENDERING_VIEW;
                return true;
            }
        });
        popup.show();
    }

    public void showOrHideToolbarView(final int hideOrShow)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                int rightViewStatus = View.VISIBLE;
                int leftviewStatus = View.INVISIBLE;
                if(hideOrShow == Constants.SHOW) {
                    if ((sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 0)) {
                        rightViewStatus = View.INVISIBLE; leftviewStatus = View.VISIBLE;
                    }
                    else
                    {
                        rightViewStatus = View.VISIBLE; leftviewStatus = View.INVISIBLE;
                    }
                }
                else if(hideOrShow == Constants.HIDE){
                    if ((sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 0)) {
                        rightViewStatus = View.VISIBLE; leftviewStatus = View.INVISIBLE;
                    }
                    else{
                        rightViewStatus = View.INVISIBLE; leftviewStatus = View.VISIBLE;
                    }
                }
                ((ViewGroup) findViewById(R.id.rightView).getParent()).setVisibility(rightViewStatus);
                ((ViewGroup) findViewById(R.id.leftView).getParent()).setVisibility(leftviewStatus);
                ((ViewGroup) findViewById(R.id.leftView).getParent()).requestLayout();
                if (hideOrShow == Constants.SHOW)
                    dellocAllSubViews();
            }
        });
    }

    public void showOrHideLoading(final int hideOrShow) {
        EditorView.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((ProgressBar) findViewById(R.id.loading_progress)).setVisibility((hideOrShow == Constants.SHOW) ? View.VISIBLE : View.INVISIBLE);
            }
        });
    }

    public void swapViews() {

        View leftView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? (ViewGroup) findViewById(R.id.leftView).getParent()
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
        addRightView((!isActivityStartFirstTime && GL2JNILib.isRigMode()));
    }

    public void showCloneOption(View v, MotionEvent event){
        final Dialog clone_btn = new Dialog(mContext);
        clone_btn.requestWindowFeature(Window.FEATURE_NO_TITLE);
        clone_btn.setContentView(R.layout.clone);
        clone_btn.setCanceledOnTouchOutside(true);
        Window window = clone_btn.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.TOP | Gravity.START;
        wlp.dimAmount=0.0f;
        if(event != null) {
            wlp.x = (int)event.getX();
            wlp.y = (int)event.getY();
        }
        else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        ((Button)clone_btn.findViewById(R.id.multi_clone)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showOrHideLoading(Constants.SHOW);
                renderManager.createDuplicate();
                clone_btn.dismiss();
            }
        });
        ((Button)clone_btn.findViewById(R.id.multi_delete)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.removeSelectedObjects();
                    }
                });
                clone_btn.dismiss();
            }
        });
//        ((Button)clone_btn.findViewById(R.id.multi_select_all)).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                glView.queueEvent(new Runnable() {
//                    @Override
//                    public void run() {
//                        for (int i = 2; i < GL2JNILib.getNodeCount(); i++){
//                            boolean state = true;
//                            for (int j = 0; j < GL2JNILib.selectedNodeIdsSize(); j++){
//                                if(GL2JNILib.getSelectedNodeIdAtIndex(j) == i)
//                                    state = false;
//                            }
//                            if(state)
//                                GL2JNILib.selectNode(i,true);
//                        }
//                    }
//                });
//            }
//        });
        clone_btn.show();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(data == null) return;
        if(requestCode == Constants.IMAGE_IMPORT_RESPONSE)
            imageManager.startActivityForResult(data,requestCode,resultCode);
        else if(requestCode == Constants.OBJ_IMPORT_RESPONSE){
            objSelection.startActivityForResult(data,requestCode,resultCode);
        }
//        else if(requestCode == Constants.VIDEO_IMPORT_RESPONSE){
//            videoManager.startActivityForResult(data,requestCode,resultCode); // Video Restricted
//        }
        else if (requestCode == 1001){
            settings.result(requestCode,resultCode,data);
        }
        else if (requestCode == 1002){
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
        if (!isDisplayPrepared || GL2JNILib.isPlaying() || Constants.VIEW_TYPE == Constants.ANIMATION_VIEW || Constants.VIEW_TYPE == Constants.ASSET_VIEW || Constants.VIEW_TYPE == Constants.PARTICLE_VIEW
                 || Constants.VIEW_TYPE == Constants.TEXT_VIEW || Constants.VIEW_TYPE == Constants.RENDERING_VIEW) return;

        isDisplayPrepared = false;
        showOrHideLoading(Constants.SHOW);
        glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.syncPhysicsWithWorld(0,GL2JNILib.totalFrame()-1,true);
                GL2JNILib.save(nativeCallBacks,false,projectNameHash);

            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void onDestroy() {
        unbindService(EditorView.this);
        Intent i = new Intent(EditorView.this, SceneSelection.class);
        i.setFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        startActivity(i);
        overridePendingTransition(0,0);
        super.onDestroy();
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

    public void saveCompletedCallBack()
    {
        renderingPaused = true;
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                save.saveI3DFile(projectNameHash);
                glView.setPreserveEGLContextOnPause(true);
                glView.onPause();
                dellocAllSubViews();
                dealloc();
                final Timer timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        timer.cancel();
                        GL2JNILib.dealloc();
                        glView.setWillNotDraw(true);
                        EditorView.this.finish();
                    }},500);
            }
        });
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

    public static void updateDemo(final byte[] image){
//        ((Activity)mContext).runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//                //demo.setImageBitmap(image);
//                demo.setImageBitmap(BitmapFactory.decodeByteArray(image , 0, image.length));
//            }
//        });
    }
}
