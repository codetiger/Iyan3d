package com.smackall.animator.AnimationEditorView;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.text.TextUtils;
import android.view.Display;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.HorizontalScrollView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.smackall.animator.common.Constant;
import com.smackall.animator.common.Services;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNIView;
import com.smackall.animator.sceneSelectionView.SceneSelectionHelpView;

import java.io.File;
import java.util.Calendar;
import java.util.Timer;
import java.util.TimerTask;

import smackall.animator.R;


public class AnimationEditor extends Activity implements View.OnClickListener {

    VelocityTracker velocityTracker;
    FrameLayout glView;
    public GridView gridView;
    public int width, height;
    public GLSurfaceView glSurfaceView;
    public int selectedFrame=0;
    int fingerCount;
    int panBegan = 0;
    int moveBegan=0;
    public int animationFrameCount=24;
    boolean swiping = false;
    private final int MAX_CLICK_DURATION = 200;
    private long startClickTime;
    String projectName;
    public String projectFileName;
    GL2JNIView gl2JNIView;

    public LinearLayout animation_editor_home_btn, animation_editor_save_btn, animation_editor_help_btn, animation_editor_framesMode_btn, animation_editor_firstFrame_btn,
            animation_editor_frameLast_btn, animation_editor_addFrame_btn, animation_editor_play_btn, animation_editor_fullscreen_btn, animation_editor_views_btn,
            animation_editor_move_btn,animation_editor_rotate_btn,animation_editor_scale_btn,animation_editor_select_btn, animation_editor_option_btn,
            animation_editor_delete_btn, animation_editor_undo_btn, animation_editor_menu_btn, animation_editor_redo_btn,
            gridView_holder_for_animationEditor, timeline_layout, temp_fullscreen_holder, time_layout,mirror_btn;

    TextView animation_editor_home_text, animation_editor_save_text, animation_editor_help_text, animation_editor_framesMode_text,
            animation_editor_addFrame_text, animation_editor_play_text, animation_editor_move_text,animation_editor_rotate_text,animation_editor_scale_text,animation_editor_select_text, animation_editor_option_text,
            animation_editor_delete_text, animation_editor_undo_text, animation_editor_menu_text, animation_editor_redo_text, animation_editor_project_text, tips_text;

    ImageView play_or_pause_img,mirror_btn_img,frame_mode_img;

    public ProgressBar save_progress_top, save_progress_center;
    public HorizontalScrollView horizontalScrollView;

    boolean isFullscreenEnabled = true;

    boolean isPlay = true;

    boolean frameDuration = false;
    public int frameCount = 24;
    public String frameType = "frames";
    Timer play_animation_timer, play_frame_timer;


    public int previousSelectedGridItem = 0;
    public int selectedGridItem = 0;

    public Context context;
    public Activity activity;
    AnimationEditorMenuDialog animationEditorMenuDialog = new AnimationEditorMenuDialog();
    ScaleDialogClass scaleDialogClass = new ScaleDialogClass();
    ObjectProperties objectProperties = new ObjectProperties();
    SelectItems selectItems = new SelectItems();
    CameraSettings cameraSettings = new CameraSettings();
    LightPropertiesDialog lightPropertiesDialog = new LightPropertiesDialog();

    public int selectedNodeType = 3;
    public int selectedNodeId = -1;

    float scrollViewSize = 0.0f;
    int previousLength = 0;
    float previousFloatLength = 0.0f;
    //public  int currentGrid = 0;
    //public int previousGrid = 0;
    //public ArrayList<String> importedItems; //Select List

    public boolean renderer = true;
    public int jointSize = 0;

    public enum NODE_TYPE
    {
        NODE_CAMERA(0),
        NODE_LIGHT(1),
        NODE_SGM(2),
        NODE_TEXT(4),
        NODE_IMAGE(5),
        NODE_OBJ(6),
        NODE_RIG(3),
        NODE_UNDEFINED(-1),
        NODE_BITS(4),
        NODE_ADDITIONAL_LIGHT(7);

        private int value;
        NODE_TYPE(int i) {
            value = i;
        }
        int getValue(){
            return value;
        }
    }

    public enum NODE_RIG_OR_TEXT
    {
        NODE_RIG(3),
        NODE_TEXT(4);
        private int value;
        NODE_RIG_OR_TEXT(int i) {
            value = i;
        }
        public int getValue(){
            return value;
        }
    }

    public enum ENUM_FOR_BUTTON_HANDLER
    {
        DEFAULT_BUTTON(9);
        private int value;
        ENUM_FOR_BUTTON_HANDLER(int i) {
            value = i;
        }
        public int getValue(){
            return value;
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animation_editor);
        Display d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        width = d.getWidth();
        height = d.getHeight();
        context = this;
        activity = AnimationEditor.this;
        Constant.animationEditor = this;
        GL2JNIView.callBackSurfaceRendered(false);
        Constant.FullScreencall(AnimationEditor.this,context);
        renderer = true;

        Bundle extraDetail = getIntent().getExtras();
        projectFileName = extraDetail.getString("projectFileName");
        projectName = extraDetail.getString("projectName");
        System.out.println("File Name and Project Name : " + projectFileName + " " + projectName);

        animation_editor_home_btn = (LinearLayout) findViewById(R.id.animation_editor_home_btn);
        animation_editor_save_btn = (LinearLayout) findViewById(R.id.animation_editor_save_btn);
        animation_editor_help_btn = (LinearLayout) findViewById(R.id.animation_editor_help_btn);
        animation_editor_framesMode_btn = (LinearLayout) findViewById(R.id.animation_editor_mode_btn);
        animation_editor_firstFrame_btn = (LinearLayout) findViewById(R.id.animation_editor_firstFrame_btn);
        animation_editor_frameLast_btn = (LinearLayout) findViewById(R.id.animation_editor_lastFrame_btn);
        animation_editor_addFrame_btn = (LinearLayout) findViewById(R.id.animation_editor_addFrame_btn);
        animation_editor_fullscreen_btn = (LinearLayout) findViewById(R.id.animation_editor_fullScreen_btn);
        animation_editor_views_btn = (LinearLayout) findViewById(R.id.animation_editor_views_btn);
        animation_editor_move_btn = (LinearLayout) findViewById(R.id.animation_editor_move_btn);
        animation_editor_rotate_btn = (LinearLayout) findViewById(R.id.animation_editor_rotate_btn);
        animation_editor_play_btn = (LinearLayout) findViewById(R.id.animation_editor_play_btn);
        animation_editor_scale_btn = (LinearLayout) findViewById(R.id.animation_editor_scale_btn);
        animation_editor_select_btn = (LinearLayout) findViewById(R.id.animation_editor_select_btn);
        animation_editor_option_btn = (LinearLayout) findViewById(R.id.animation_editor_option_btn);
        animation_editor_delete_btn = (LinearLayout) findViewById(R.id.animation_editor_delete_btn);
        mirror_btn = (LinearLayout) findViewById(R.id.mirror_btn);
        time_layout = (LinearLayout) findViewById(R.id.timeLayout);
        time_layout.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);
        System.out.println("Time Layout Weight : " + time_layout.getMeasuredHeight());
//        animation_editor_delete_btn.setVisibility(View.VISIBLE); //TEST
        animation_editor_undo_btn = (LinearLayout) findViewById(R.id.animation_editor_undo_btn);
        animation_editor_undo_btn.setVisibility(View.INVISIBLE);
        animation_editor_menu_btn = (LinearLayout) findViewById(R.id.animation_editor_menu_btn);
        animation_editor_redo_btn = (LinearLayout) findViewById(R.id.animation_editor_redo_btn);
        animation_editor_redo_btn.setVisibility(View.INVISIBLE);
        gridView_holder_for_animationEditor = (LinearLayout) findViewById(R.id.gridView_holder_for_animationEditor);
        timeline_layout = (LinearLayout) findViewById(R.id.timeline_layout);
        temp_fullscreen_holder = (LinearLayout) findViewById(R.id.temp_fullscreen_holder);
        temp_fullscreen_holder.setVisibility(View.GONE);

        play_or_pause_img = (ImageView) findViewById(R.id.play_or_pause_img);
        frame_mode_img = (ImageView) findViewById(R.id.frame_mode_img);

        gridView = (GridView) findViewById(R.id.gridViewForFrames);

        glView = (FrameLayout) findViewById(R.id.gl_sur_view);

        mirror_btn_img = (ImageView) findViewById(R.id.mirror_btn_img);


        horizontalScrollView = (HorizontalScrollView) findViewById(R.id.horizontalScrollView);

        animation_editor_home_text  = (TextView)  findViewById(R.id.animation_editor_home_text);
        animation_editor_save_text  = (TextView)  findViewById(R.id.animation_editor_save_text);
        animation_editor_help_text  = (TextView)  findViewById(R.id.animation_editor_help_text);
        animation_editor_framesMode_text  = (TextView)  findViewById(R.id.animation_editor_frames_text);
        animation_editor_addFrame_text  = (TextView)  findViewById(R.id.animation_editor_add_text);
        animation_editor_move_text  = (TextView)  findViewById(R.id.animation_editor_move_text);
        animation_editor_rotate_text  = (TextView)  findViewById(R.id.animation_editor_rotate_text);
        animation_editor_play_text  = (TextView)  findViewById(R.id.animation_editor_play_text);
        animation_editor_scale_text  = (TextView)  findViewById(R.id.animation_editor_scale_text);
        animation_editor_select_text  = (TextView)  findViewById(R.id.animation_editor_select_text);
        animation_editor_option_text  = (TextView)  findViewById(R.id.animation_editor_option_text);
        animation_editor_delete_text  = (TextView)  findViewById(R.id.animation_editor_delete_text);
        animation_editor_undo_text  = (TextView)  findViewById(R.id.animation_editor_undo_text);
        animation_editor_menu_text  = (TextView)  findViewById(R.id.animation_editor_menu_text);
        animation_editor_redo_text  = (TextView)  findViewById(R.id.animation_editor_redo_text);
        animation_editor_project_text = (TextView)  findViewById(R.id.animation_editor_project_text);
        tips_text = (TextView)  findViewById(R.id.tips_text);
        tips_text.setVisibility(View.INVISIBLE);

        save_progress_top = (ProgressBar) findViewById(R.id.save_progress_top);
        save_progress_center = (ProgressBar) findViewById(R.id.save_progress_center);

        //importedItems = new ArrayList<>();//Select List

        animation_editor_project_text.setMaxLines(1);
        animation_editor_project_text.setEllipsize(TextUtils.TruncateAt.END);
        animation_editor_project_text.setText(projectName.toUpperCase());

        animation_editor_home_btn.setOnClickListener(this);
        animation_editor_save_btn.setOnClickListener(this);
        animation_editor_help_btn.setOnClickListener(this);
        animation_editor_framesMode_btn.setOnClickListener(this);
        animation_editor_firstFrame_btn.setOnClickListener(this);
        animation_editor_frameLast_btn.setOnClickListener(this);
        animation_editor_addFrame_btn.setOnClickListener(this);
        animation_editor_views_btn.setOnClickListener(this);
        animation_editor_move_btn.setOnClickListener(this);
        animation_editor_rotate_btn.setOnClickListener(this);
        animation_editor_play_btn.setOnClickListener(this);
        animation_editor_scale_btn.setOnClickListener(this);
        animation_editor_select_btn.setOnClickListener(this);
        animation_editor_option_btn.setOnClickListener(this);
        animation_editor_delete_btn.setOnClickListener(this);
        animation_editor_undo_btn.setOnClickListener(this);
        animation_editor_redo_btn.setOnClickListener(this);

        mirror_btn.setVisibility(View.VISIBLE);

        animation_editor_fullscreen_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isFullscreenEnabled) {
                    timeline_layout.animate()
                            .translationYBy(1).translationY(0).setDuration(500)
                            .setListener(new AnimatorListenerAdapter() {
                                @Override
                                public void onAnimationEnd(Animator animation) {
                                    super.onAnimationEnd(animation);
                                    timeline_layout.setVisibility(View.GONE);
                                    temp_fullscreen_holder.setVisibility(View.VISIBLE);
                                    isFullscreenEnabled = false;
                                }
                            });
                }
                else{
                    timeline_layout.animate()
                            .translationYBy(0).translationY(1)
                            .setListener(new AnimatorListenerAdapter() {
                                @Override
                                public void onAnimationEnd(Animator animation) {
                                    super.onAnimationEnd(animation);
                                    temp_fullscreen_holder.setVisibility(View.GONE);
                                    timeline_layout.setVisibility(View.VISIBLE);
                                    isFullscreenEnabled = true;

                                }
                            });
                }
            }
        });

        animation_editor_menu_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!isPlay)
                    return;
                if(AnimationEditorMenuDialog.popWindow == null || !AnimationEditorMenuDialog.popWindow.isShowing())
                AnimationEditorMenuDialog.menuDialog(context, AnimationEditor.this, v);
                Constant.FullScreencall(activity, context);
            }
        });

        mirror_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mirrorBtnUpdater(jointSize);
            }
        });

        gridView.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
        showOrHideSaveProgress("sceneLoading");
        GL2JNILib.setAssetspath(Constant.defaultAssetsDir, Constant.defaultLocalAssetsDir,Constant.importedImagesdir);
        gl2JNIView = new GL2JNIView(this);
        glSurfaceView = gl2JNIView;
        animationFrameCount = frameCount;

        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        glView.addView(glSurfaceView, glParams);

        velocityTracker=  VelocityTracker.obtain();

        glSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(!GL2JNIView.isSurfaceCreated())
                    return false;
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        GL2JNILib.setLightingOff();
                        fingerCount = event.getPointerCount();
                        startClickTime = Calendar.getInstance().getTimeInMillis();
                        Constant.FullScreencall(activity,context);
                        return true;
                    case MotionEvent.ACTION_UP:
                        if(isPlay) {
                            GL2JNILib.setLightingOn();
                            long clickDuration = Calendar.getInstance().getTimeInMillis() - startClickTime;
                            if (moveBegan < 2 && fingerCount == 1 && clickDuration < MAX_CLICK_DURATION) {
                                float x = event.getX();
                                float y = event.getY();
                                GL2JNILib.tap(x, y);
                            }
                        }

                        panBegan = 0;
                        moveBegan = 0;
                        swiping = false;

                        if (fingerCount == 1 && isPlay) {
                            GL2JNILib.tapEnd(event.getX(), event.getY());
                        }

//                        if(selectedNodeId != -1)
//                            gridViewHorizontally(selectedFrame);
                        return true;

                    case MotionEvent.ACTION_POINTER_DOWN:
                        fingerCount = event.getPointerCount();
                        return true;

                    case MotionEvent.ACTION_MOVE:
                        fingerCount = event.getPointerCount();
                        if (fingerCount == 1) {
                            velocityTracker.addMovement(event);
                            velocityTracker.computeCurrentVelocity(1000);
                            float x_velocity = velocityTracker.getXVelocity();
                            float y_velocity = velocityTracker.getYVelocity();
                            if (moveBegan == 0) {
                                GL2JNILib.controlSelection(event.getX(), event.getY());
                                moveBegan++;
                            }
                            GL2JNILib.tapMove(event.getX(), event.getY());
                            GL2JNILib.swipe(x_velocity, y_velocity);

                            System.out.println("X Position : " + event.getX() +" "+"Y Position : " + event.getY());

                            return true;
                        } else if (fingerCount == 2) {
                            float x1 = event.getX(0);
                            float y1 = event.getY(0);
                            float x2 = event.getX(1);
                            float y2 = event.getY(1);
                            if (panBegan == 0) {
                                GL2JNILib.paBegin(x1, y1, x2, y2);
                                panBegan++;
                            } else {
                                GL2JNILib.panProgress(x1, y1, x2, y2);
                            }
                            return true;

                        }
                        break;
                }
                return true;
            }
        });
        ButtonHandler.buttonHanler();
        gridViewHorizontally(0);
    }


    @Override
    protected void onResume() {
        super.onResume();
        stopService(new Intent(this, Services.class));
        System.out.println("Animation Frame Count : " + animationFrameCount);
        if(animationFrameCount != 0) {
            frameCount = animationFrameCount;
            gridViewHorizontally(frameCount);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public void gridViewHorizontally(int selection) {
        System.out.println("Frame Count : " + frameCount);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                gridView.getLayoutParams().width = (int) (((width*0.55) / 6) * frameCount);
                break;
            case "large":
                gridView.getLayoutParams().width = (int) (((width*0.55) / 11) * frameCount);
                break;
            case "xlarge":
                gridView.getLayoutParams().width = (int) (((width*0.55) / 20) * frameCount);
                break;
            case "undefined":
                gridView.getLayoutParams().width = (int) (((width*0.55) / 20) * frameCount);
                break;
        }

        System.out.println("Screen Category : " + Constant.getScreenCategory(context));

        gridView.setHorizontalSpacing(1);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH) {
            gridView.canScrollVertically(0);
        }
        gridView.setAdapter(new AdaptorForAnimationFrames(context, frameCount, frameType));
        int cols = (int) Math.ceil((double) frameCount / 1);
        System.out.println("No of Column : " + cols);
        gridView.setNumColumns(cols);
        gridView.setSelection(selection);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.animation_editor_addFrame_btn:
                if(!isPlay)
                    break;
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                frameCount++;
                animationFrameCount++;
                GL2JNILib.addFramebtn();
                gridViewHorizontally(frameCount);
                horizontalScrollView.fullScroll(HorizontalScrollView.FOCUS_RIGHT);
                break;
            case R.id.animation_editor_home_btn:
                if(!isPlay)
                    break;
                SaveConfirmDialog.exitConfirmDialog(context, activity);
                break;
            case R.id.animation_editor_save_btn:
                if(!isPlay)
                    break;
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                showOrHideSaveProgress("saveStart");
                File projectDir = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/projects");
                Constant.mkDir(projectDir);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.saveAnimation(projectFileName);
                    }
                }).start();
                //showOrHideSaveProgress("saveFinish");
                break;
            case R.id.animation_editor_help_btn:
                if(!isPlay)
                    break;
                Intent in = new Intent();
                in.setClass(AnimationEditor.this, SceneSelectionHelpView.class);
                startActivity(in);
                break;
            case R.id.animation_editor_mode_btn:
                if(!isPlay)
                    break;
                if(frameDuration) {
                    frameType = "frames";
                    animation_editor_framesMode_text.setText("FRAMES");
                    frame_mode_img.setImageResource(R.drawable.frames);
                    gridViewHorizontally(selectedFrame);
                    frameDuration = false;
                }
                else {
                    frameType = "duration";
                    animation_editor_framesMode_text.setText("DURATION");
                    frame_mode_img.setImageResource(R.drawable.duration);
                    gridViewHorizontally(selectedFrame);
                    frameDuration = true;
                }
                break;
            case R.id.animation_editor_lastFrame_btn:
                if(!isPlay)
                    break;
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                horizontalScrollView.fullScroll(HorizontalScrollView.FOCUS_RIGHT);
                selectedGridItem = frameCount-1;
                UiUpdater.frameUpdater("last", context);
                GL2JNILib.frameClicked(frameCount-1);
                break;
            case R.id.animation_editor_firstFrame_btn:
                if(!isPlay)
                    break;
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                UiUpdater.frameUpdater("first", context);
                horizontalScrollView.fullScroll(HorizontalScrollView.FOCUS_LEFT);
                selectedGridItem = 0;
                previousSelectedGridItem =0;
                GL2JNILib.frameClicked(0);
                break;
            case R.id.animation_editor_move_btn:
                if(!isPlay)
                    break;
                GL2JNILib.moveBtn();
                break;
            case R.id.animation_editor_rotate_btn:
                if(!isPlay)
                    break;
                GL2JNILib.rotateBtn();
                break;
            case R.id.animation_editor_views_btn:
                int[] loc = new int[2];
                animation_editor_views_btn.getLocationOnScreen(loc);
                animationEditorMenuDialog.viewPicker(context, loc[0], loc[1],activity);
                break;
            case R.id.animation_editor_scale_btn:
                if(!isPlay)
                    break;
                float x_value = (GL2JNILib.getScaleXval());
                float y_value = (GL2JNILib.getScaleYval());
                float z_value = (GL2JNILib.getScaleZval());
                scaleDialogClass.scaleDialog(context, animation_editor_scale_btn.getTop(), "animationeditor",x_value,y_value,z_value );
                break;
            case R.id.animation_editor_option_btn:
                if(!isPlay)
                    break;
                if(selectedNodeId != -1 && selectedNodeType == NODE_TYPE.NODE_CAMERA.getValue()){
                    float field_of_view = GL2JNILib.getCameraFov();
                    int resolution=GL2JNILib.getCameraResolution();
                    cameraSettings.cameraPropertiesDialog(context, this, animation_editor_option_btn.getLeft(), animation_editor_option_btn.getTop(),resolution,field_of_view);
                }
                else if(selectedNodeId != -1 && selectedNodeType == NODE_TYPE.NODE_LIGHT.getValue()){
                    lightPropertiesDialog.lightPropertiesDialog(context, activity, animation_editor_views_btn.getLeft(), animation_editor_views_btn.getTop() + (animation_editor_views_btn.getHeight() * 3));
                }
                else if(selectedNodeId != -1 && selectedNodeType == NODE_TYPE.NODE_ADDITIONAL_LIGHT.getValue()){
                    lightPropertiesDialog.lightPropertiesDialog(context, activity, animation_editor_views_btn.getLeft(), animation_editor_views_btn.getTop() + (animation_editor_views_btn.getHeight() * 3));
                }
                else if(selectedNodeId != -1 && selectedNodeType != NODE_TYPE.NODE_UNDEFINED.getValue()){
                    float brightness=GL2JNILib.getMeshBrightness();
                    float specular=GL2JNILib.getMeshSpecular();
                    boolean lighting_switch_on=GL2JNILib.getLightingSwitch();
                    boolean visible_switch_on=GL2JNILib.getVisibilitySwitch();
                    objectProperties.objectPropertiesDialog(context, AnimationEditor.this, animation_editor_option_btn.getLeft(), animation_editor_option_btn.getTop(),brightness,specular,lighting_switch_on,visible_switch_on);
                }
                break;
            case  R.id.animation_editor_delete_btn:
                if(!isPlay)
                    break;
                DeleteObjectAndAnimationDialog.deleteObjectAndAnimationDialog(context, activity);
                break;
            case R.id.animation_editor_play_btn:
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                playOrPause();
                break;

            case R.id.animation_editor_select_btn:
                if(!isPlay)
                    break;
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                selectItems.selectItemInArray(context, animation_editor_select_btn.getLeft(), animation_editor_select_btn.getTop() + (animation_editor_select_btn.getHeight() * 3));
                break;
            case R.id.animation_editor_undo_btn:
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                if(!isPlay)
                    break;
                GL2JNILib.undoFunction();
                break;
            case R.id.animation_editor_redo_btn:
                if(!GL2JNIView.isSurfaceCreated())
                    break;
                if(!isPlay)
                    break;
                GL2JNILib.redoFunction();
                break;
        }

    }

    public void importImageFromLocal(String filePath, String fileName, int originalWidth, int originalHeight){
        System.out.println("File Path : " + filePath);
        System.out.println("File Name : " + fileName);
        System.out.println("Original Width : " + originalWidth);
        System.out.println("Original Height : " + originalHeight);

        GL2JNILib.importImageAction(fileName, originalWidth, originalHeight);
        //importedItems.add("IMAGE: "+fileName);//Select List
    }

    public void showOrHideSaveProgress(final String process){
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (process.equals("saveStart")) {
                    save_progress_top.setVisibility(View.VISIBLE);
                    save_progress_center.setVisibility(View.VISIBLE);
                } else if (process.equals("saveFinish")) {
                    save_progress_top.setVisibility(View.INVISIBLE);
                    save_progress_center.setVisibility(View.INVISIBLE);
                    Toast.makeText(context, "Save Completed", Toast.LENGTH_SHORT).show();
                } else if (process.equals("sceneLoading")) {
                    save_progress_top.setVisibility(View.INVISIBLE);
                    save_progress_center.setVisibility(View.VISIBLE);
                } else if (process.equals("sceneLoadingFinish")) {
                    save_progress_top.setVisibility(View.INVISIBLE);
                    save_progress_center.setVisibility(View.INVISIBLE);
                }
                //System.out.println("Show Or Hide Function Finished");
            }
        });
    }

    private void playOrPause(){
        Constant.animationEditor.selectedNodeType = 3;
        Constant.animationEditor.selectedNodeId = -1;
        ButtonHandler.buttonHanler();
        if(isPlay){
            isPlay = false;
            GL2JNILib.setLightingOff();
            animation_editor_play_text.setText("   STOP");
            play_or_pause_img.setImageResource(R.drawable.play);
            GL2JNILib.playAnimation();
            playAnimation();
            scrollViewSize = horizontalScrollView.getWidth();
            play_frame_timer = new Timer();
            play_frame_timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            View gridPreviousSelected = gridView.getChildAt(previousSelectedGridItem);
                            LinearLayout gridPreviousHolder = (LinearLayout) gridPreviousSelected.findViewById(R.id.animation_editor_frame_holder);
                            gridPreviousHolder.setBackgroundDrawable(getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
                            if(selectedGridItem >= frameCount-1){
                                GL2JNILib.setLightingOn();
                                animation_editor_play_text.setText("   PLAY");
                                play_or_pause_img.setImageResource(R.drawable.play);
                                isPlay = true;
                                play_frame_timer.cancel();
                                play_animation_timer.cancel();
                                GL2JNILib.stopAnimation();
                            }
                            if(previousSelectedGridItem < frameCount-2)
                                previousSelectedGridItem++;
                            if(selectedGridItem < frameCount-1)
                                selectedGridItem++;
                            View gridSelected = gridView.getChildAt(selectedGridItem);
                            LinearLayout gridHolder = (LinearLayout) gridSelected.findViewById(R.id.animation_editor_frame_holder);
                            gridHolder.setBackgroundDrawable(getResources().getDrawable(R.drawable.border_grid_frames_pressed));
                            previousLength = (int) (previousLength + (scrollViewSize/frameCount));
                            previousFloatLength = (previousFloatLength+ (scrollViewSize/frameCount));
                            horizontalScrollView.smoothScrollTo((int) gridSelected.getX(), 0);
                        }
                    });
                }
            }, 0, 1000/24);
        }
        else {
            GL2JNILib.setLightingOn();
            animation_editor_play_text.setText("   PLAY");
            play_or_pause_img.setImageResource(R.drawable.play);
            play_animation_timer.cancel();
            play_frame_timer.cancel();

            GL2JNILib.stopAnimation();
            isPlay = true;
        }
    }

    private void playAnimation(){
        play_animation_timer = new Timer();
        play_animation_timer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (selectedGridItem >= frameCount - 1) {
                    play_animation_timer.cancel();
                }
                GL2JNILib.playTimer();
            }
        }, 0, 1000 / 24);
    }

    public static void selectItem(int position, String value){
        GL2JNILib.selectObject(position);
    }

    public void changeUndoRedoButtonState(int state){
        System.out.println("Action Type : " + state);
        if(state == Constant.ACTION_TYPE.DEACTIVATE_UNDO.getValue()) {
            animation_editor_undo_btn.setVisibility(View.INVISIBLE);
            animation_editor_redo_btn.setVisibility(View.VISIBLE);
        }
        else if (state == Constant.ACTION_TYPE.DEACTIVATE_REDO.getValue()) {
            animation_editor_undo_btn.setVisibility(View.VISIBLE);
            animation_editor_redo_btn.setVisibility(View.INVISIBLE);
        }
        else if(state == Constant.ACTION_TYPE.DEACTIVATE_BOTH.getValue()) {
            animation_editor_undo_btn.setVisibility(View.INVISIBLE);
            animation_editor_redo_btn.setVisibility(View.INVISIBLE);
        }
        else if(state == Constant.ACTION_TYPE.ACTIVATE_BOTH.getValue()) {
            animation_editor_undo_btn.setVisibility(View.VISIBLE);
            animation_editor_redo_btn.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onBackPressed(){
        if(!isPlay)
            return;
        SaveConfirmDialog.exitConfirmDialog(context, activity);
    }

    public void mirrorBtnUpdater(int jointSize){
        if(jointSize == 54) {
            mirror_btn.setVisibility(View.VISIBLE);
            if (!GL2JNILib.getMirrorState()) {
                mirror_btn_img.setImageResource(R.drawable.switch_on);
                GL2JNILib.switchMirrorState();
            } else if (GL2JNILib.getMirrorState()) {
                mirror_btn_img.setImageResource(R.drawable.switch_off);
                GL2JNILib.switchMirrorState();
            }
        }else
            mirror_btn.setVisibility(View.GONE);
    }

}

