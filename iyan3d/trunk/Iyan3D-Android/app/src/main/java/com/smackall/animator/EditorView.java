package com.smackall.animator;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
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
import android.widget.AbsListView;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListPopupWindow;

import com.smackall.animator.Adapters.FrameAdapter;
import com.smackall.animator.Adapters.ObjectListAdapter;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.Helper.SharedPreferenceManager;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;

public class EditorView extends AppCompatActivity implements View.OnClickListener{

    DatabaseHelper db = new DatabaseHelper();
    InfoPopUp infoPopUp = new InfoPopUp();
    private AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    private DownloadManager downloadManager = new DownloadManagerClass();
    public SharedPreferenceManager sharedPreferenceManager = new SharedPreferenceManager();

    DownloadHelper downloadHelper;
    AssetSelection assetSelection;
    TextSelection textSelection;
    AnimationSelection animationSelection;
    ImageSelection imageSelection;
    OBJSelection objSelection;
    FrameAdapter frameAdapter;
    Settings settings;
    ObjectListAdapter objectListAdapter;
    LightProps lightProps;
    CameraProps cameraProps;
    MeshProps meshProps;
    ColorPicker colorPicker;
    Scale scale;
    EnvelopScale envelopScale;
    Play play;
    Login login;
    Export export;
    CloudRenderingProgress cloudRenderingProgress;

    ImageView referenceImg;
    boolean isActivityStartFirstTime = true;
    public int totalFrames = 500;
    public int objectCount = 24;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_editor_view);
        referenceImg = (ImageView)findViewById(R.id.last_frame_img);
        Bundle bundle = getIntent().getExtras();
        int position = bundle.getInt("scenePosition");
        List<SceneDB> sceneDBs = db.getAllScenes();
        initViews();
        swapViews();


        findViewById(R.id.glView).setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {

                return false;
            }
        });
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        // TODO Auto-generated method stub
        super.onWindowFocusChanged(hasFocus);
        if(isActivityStartFirstTime) {
            isActivityStartFirstTime = false;
            referenceImg = (ImageView) findViewById(R.id.last_frame_img);
            initFrameGrid();
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
        animationSelection = new AnimationSelection(this,this.db,addToDownloadManager,downloadManager);
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
        export = new Export(EditorView.this);
    }

    public void goToFirstOrLastFrame(View view)
    {
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.frames);
        recyclerView.scrollToPosition((view.getTag().equals("firstFrame")) ? 0 : totalFrames - 1);
    }

    public void openInfoPopUp(View v)
    {
        infoPopUp.infoPopUpMenu(EditorView.this, v);
    }

    public void showLogin(View v)
    {
        cloudRenderingProgress.showCloudRenderingProgress(v,null);
    }
    public void addFramePopUpMenu(View v)
    {
        PopupMenu popup = new PopupMenu(this, v);
        popup.getMenuInflater().inflate(R.menu.add_frames_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case Constants.ONE_FRAME:
                        totalFrames += Constants.ONE_FRAME;
                        break;
                    case Constants.TWENTY_FOUR_FRAME:
                        totalFrames += Constants.TWENTY_FOUR_FRAME;
                        break;
                    case Constants.TWO_FORTY_FRAME:
                        totalFrames += Constants.TWO_FORTY_FRAME;
                        break;
                }
                frameAdapter.notifyDataSetChanged();
                goToFirstOrLastFrame(((LinearLayout)findViewById(R.id.lastFrameBtn)));
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
                switch (item.getOrder()) {
                    case Constants.FRONT_VIEW:
                        break;
                    case Constants.TOP_VIEW:
                        break;
                    case Constants.LEFT_VIEW:
                        break;
                    case Constants.BACK_VIEW:
                        break;
                    case Constants.RIGHT_VIEW:
                        break;
                    case Constants.BOTTOM_VIEW:
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    private void addRightView()
    {
        ((FrameLayout)findViewById(R.id.autorig_bottom_tool)).setVisibility(View.GONE);
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
    }

    private void addRigToolBar()
    {
        ((FrameLayout)findViewById(R.id.autorig_bottom_tool)).setVisibility(View.VISIBLE);
        if(sharedPreferenceManager.getInt(EditorView.this,"toolbarPosition") == 1 )
            ((LinearLayout)((ViewGroup)((FrameLayout)findViewById(R.id.autorig_bottom_tool)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.LEFT);
        else
            ((LinearLayout)((ViewGroup)((FrameLayout)findViewById(R.id.autorig_bottom_tool)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.RIGHT);

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
        View v = vi.inflate(R.layout.rig_toolbar,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        v.findViewById(R.id.add_joint).setOnClickListener(EditorView.this);
        v.findViewById(R.id.rig_move).setOnClickListener(EditorView.this);
        v.findViewById(R.id.rig_rotate).setOnClickListener(EditorView.this);
        v.findViewById(R.id.rig_scale).setOnClickListener(EditorView.this);
        v.findViewById(R.id.rig_mirror).setOnClickListener(EditorView.this);
        findViewById(R.id.rig_cancel).setOnClickListener(EditorView.this);
        findViewById(R.id.rig_pre_scene).setOnClickListener(EditorView.this);
        findViewById(R.id.rig_next_scene).setOnClickListener(EditorView.this);
        findViewById(R.id.rig_add_to_scene).setOnClickListener(EditorView.this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.play_btn:
                if(play == null)
                    play = new Play(EditorView.this,(RecyclerView)findViewById(R.id.frames));
                play.play(0,frameAdapter.getItemCount());
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
                lightProps.showLightProps(v,null);
                break;
            case R.id.export_btn:
                showExportPopUp(v);
                break;
            case R.id.move_btn:
                addRigToolBar();
                break;
            case R.id.rotate_btn:
                break;
            case R.id.scale_btn:
                break;
            case R.id.add_joint:
                break;
            case R.id.rig_move:
                break;
            case R.id.rig_rotate:
                break;
            case R.id.rig_scale:
                scale.showScale(v,null,1.0f,5.0f,100.0f);
                break;
            case R.id.rig_mirror:
                break;
            case R.id.rig_cancel:
                addRightView();
                break;
            case R.id.rig_pre_scene:
                break;
            case R.id.rig_next_scene:
                break;
            case R.id.rig_add_to_scene:
                break;
        }
    }

    private void initSceneObjectList(View v)
    {
        ListPopupWindow listView = new ListPopupWindow(EditorView.this);
        objectListAdapter = new ObjectListAdapter(EditorView.this,listView.getListView(),objectCount,this.sharedPreferenceManager.getInt(EditorView.this,"multiSelect"));
        listView.setAdapter(objectListAdapter);
        listView.setAnchorView(v);
        listView.show();
        listView.getListView().setChoiceMode(AbsListView.CHOICE_MODE_MULTIPLE);
        objectListAdapter.listView = listView.getListView();
        objectListAdapter.listView.setMultiChoiceModeListener(objectListAdapter);
        objectListAdapter.removeSelection();
    }

    public void deleteNodeAtPosition(int position){
        objectCount--;
        objectListAdapter.objectCount = objectCount;
        objectListAdapter.notifyDataSetChanged();
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
                        break;
                    case Constants.IMPORT_TEXT:
                        if (textSelection == null)
                            textSelection = new TextSelection(EditorView.this, db,addToDownloadManager,downloadManager);
                        Constants.VIEW_TYPE = Constants.TEXT_VIEW;
                        textSelection.showTextSelection();
                        break;
                    case Constants.IMPORT_LIGHT:
                        break;
                    case Constants.IMPORT_OBJ:
                        if (objSelection == null)
                            objSelection= new OBJSelection(EditorView.this, db);
                        objSelection.showObjSelection(Constants.OBJ_MODE);
                        break;
                    case Constants.IMPORT_ADD_BONE:
                        addRigToolBar();
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
                            animationSelection = new AnimationSelection(EditorView.this, db,addToDownloadManager,downloadManager);
                        Constants.VIEW_TYPE = Constants.ANIMATION_VIEW;
                        animationSelection.showAnimationSelection();
                        break;
                    case Constants.SAVE_ANIMATION:
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

    public void showOrHideToolbarView(int hideOrShow)
    {
        View rightView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ?  (ViewGroup)findViewById(R.id.rightView).getParent() : (ViewGroup)findViewById(R.id.leftView).getParent();
        View leftView = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ?  (ViewGroup)findViewById(R.id.leftView).getParent() : (ViewGroup)findViewById(R.id.rightView).getParent();

        float rightValue = 0.0f;
        float leftValue = 0.0f;

        if(hideOrShow == Constants.HIDE) {
            rightValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? rightView.getWidth() : rightView.getWidth()*-1;
            leftValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? 0 : 0;
        } else{
            rightValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? 0 : 0;
            leftValue = (sharedPreferenceManager.getInt(EditorView.this, "toolbarPosition") == 1) ? leftView.getWidth()*-1 : leftView.getWidth();
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1) {
            rightView.animate().translationX(rightValue);
            leftView.animate().translationX(leftValue);
        }
        if(hideOrShow == Constants.SHOW)
            dellocAllSubViews();
    }

    public void swapViews()
    {
        View leftView = (sharedPreferenceManager.getInt(EditorView.this,"toolbarPosition") == 1 ) ? (ViewGroup) findViewById(R.id.leftView).getParent()
                : (ViewGroup) findViewById(R.id.rightView).getParent();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1) {
            leftView.animate().translationX((sharedPreferenceManager.getInt(EditorView.this,"toolbarPosition") == 1 ) ? -leftView.getWidth() : leftView.getWidth());
        }
        ViewGroup insertPoint = null;
        for(int i = 0; i < ((ViewGroup)leftView).getChildCount(); i++){
            if(((ViewGroup)leftView).getChildAt(i).getTag() != null && ((ViewGroup)leftView).getChildAt(i).getTag().toString().equals("-1")) {
                insertPoint = (ViewGroup) ((ViewGroup)leftView).getChildAt(i);
                continue;
            }
            ((ViewGroup)leftView).getChildAt(i).setVisibility(View.GONE);
        }
        if(insertPoint != null)
            insertPoint.removeAllViews();
        showOrHideToolbarView(Constants.SHOW);
        addRightView();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        dellocAllSubViews();
        dealloc();
        Intent i = new Intent(EditorView.this,SceneSelection.class);
        i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        i.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        startActivity(i);
        finish();
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
