package com.smackall.animator.ImportAndRig;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.AnimationEditorView.AnimationEditorMenuDialog;
import com.smackall.animator.AnimationEditorView.ScaleDialogClass;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNILibAutoRig;
import com.smackall.animator.opengl.GL2JNIViewAutoRig;
import com.smackall.animator.sceneSelectionView.SceneSelectionView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;

import smackall.animator.R;


/**
 * Created by Sabish.M on 15/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class RigModel extends Activity {
    VelocityTracker velocityTracker;
    int fingerCount;
    int panBegan = 0;
    int moveBegan=0;
    boolean swiping = false;
    private static final int MAX_CLICK_DURATION = 200;
    private long startClickTime;
    boolean isFullscreenEnabled = false;
    Context context;
    AnimationEditorMenuDialog animationEditorMenuDialog;
    ScaleDialogClass scaleDialogClass = new ScaleDialogClass();
    public static String objPath, objFileName, texturePath, textureFileName;
    Bundle extraDetail;
    public GLSurfaceView glSurfaceViewAutoRigView;
    public int backOrnext = 0;

    static Button add_obj_to_my_library_btn, import_obj_btn;
    static LinearLayout rig_move_btn, rig_rotate_btn , rig_scale_btn,addjoint_btn, mirror_autorig_btn;
    ImageView mirrot_autorig_img;

    static TextView current_progress_view_text;
    public ProgressBar save_progress_center_rig;
    public boolean renderer = true;

    enum AUTORIG_VIEW
    {
        IMPORT_OBJ(0),
        ATTACH_BONE(1),
        ADJUST_ENVELOP(2),
        PREVIEW(3);
        private int value;
        AUTORIG_VIEW(int i) {
            value = i;
        }
        int getValue(){
            return value;
        }
    }
    TextView rig_project_text;
    LinearLayout rig_undo_btn;
    LinearLayout rig_redo_btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rig_model);
        extraDetail = getIntent().getExtras();
        context = this;
        Constant.FullScreencall(RigModel.this,context);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        renderer = true;
        Constant.rigModel = RigModel.this;
        animationEditorMenuDialog = new AnimationEditorMenuDialog();
        GL2JNILib.setAssetspath(Constant.defaultAssetsDir, Constant.defaultLocalAssetsDir,Constant.importedImagesdir);
        current_progress_view_text = (TextView) findViewById(R.id.current_progress_view_text);

        LinearLayout rig_home_btn = (LinearLayout) findViewById(R.id.rig_home_btn);
        LinearLayout rig_help_btn = (LinearLayout) findViewById(R.id.rig_help_btn);
        LinearLayout rig_fullscreen_btn = (LinearLayout) findViewById(R.id.rig_fullscreen_btn);
        final LinearLayout rig_view_btn = (LinearLayout) findViewById(R.id.rig_view_btn);
        rig_move_btn = (LinearLayout) findViewById(R.id.rig_move_btn);
        rig_rotate_btn = (LinearLayout) findViewById(R.id.rig_rotate_btn);
        rig_scale_btn = (LinearLayout) findViewById(R.id.rig_scale_btn);
        rig_undo_btn = (LinearLayout) findViewById(R.id.rig_undo_btn);
        rig_redo_btn = (LinearLayout) findViewById(R.id.rig_redo_btn);
        LinearLayout rig_back_btn = (LinearLayout) findViewById(R.id.rig_back_btn);
        LinearLayout rig_next_btn = (LinearLayout) findViewById(R.id.rig_next_btn);
        final LinearLayout fullview_rig = (LinearLayout) findViewById(R.id.fulView_rig);
        FrameLayout gl_layout_auto_rig=(FrameLayout) findViewById(R.id.gl_sur_view_rig);
        final LinearLayout temp_fullscreen_holder = (LinearLayout) findViewById(R.id.temp_fullscreen_holder);
        mirror_autorig_btn = (LinearLayout) findViewById(R.id.mirror_autorig_btn);
        mirrot_autorig_img = (ImageView) findViewById(R.id.mirror_autorig_img);

        rig_project_text = (TextView) findViewById(R.id.rig_project_text);

        import_obj_btn = (Button) findViewById(R.id.import_obj_btn);
        add_obj_to_my_library_btn = (Button) findViewById(R.id.add_obj_to_my_library_btn);
        addjoint_btn = (LinearLayout) findViewById(R.id.addjoint_btn);
        save_progress_center_rig = (ProgressBar)findViewById(R.id.save_progress_center_rig);

        glSurfaceViewAutoRigView=new GL2JNIViewAutoRig(this);
        FrameLayout.LayoutParams glParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        gl_layout_auto_rig.addView(glSurfaceViewAutoRigView, glParams);
        Constant.mkDir(Constant.defaultCacheDir);
        velocityTracker=  VelocityTracker.obtain();

        glSurfaceViewAutoRigView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(!GL2JNIViewAutoRig.surfaceCreated())
                    return false;
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    Constant.FullScreencall(RigModel.this,context);
                    startClickTime = Calendar.getInstance().getTimeInMillis();
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    panBegan = 0;
                    moveBegan = 0;
                    swiping = false;
                    long clickDuration = Calendar.getInstance().getTimeInMillis() - startClickTime;
                    if (clickDuration < MAX_CLICK_DURATION) {
                        float x = event.getX();
                        float y = event.getY();
                        GL2JNILibAutoRig.tapAutorig(x, y);
                    }
                    if (fingerCount == 1) {
                        GL2JNILibAutoRig.tapEndAutorig(event.getX(), event.getY());
                    }
                    return true;
                } else if (event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN) {
                    fingerCount = event.getPointerCount();
                    return true;
                } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    fingerCount = event.getPointerCount();
                    if (fingerCount == 1) {
                        velocityTracker.addMovement(event);
                        velocityTracker.computeCurrentVelocity(1000);
                        float x_velocity = velocityTracker.getXVelocity();
                        float y_velocity = velocityTracker.getYVelocity();
                        if (moveBegan == 0) {
                            GL2JNILibAutoRig.controlSelectionAutoRig(event.getX(), event.getY());
                            moveBegan++;
                        }
                        GL2JNILibAutoRig.tapMoveAutorig(event.getX(), event.getY());
                        GL2JNILibAutoRig.swipeAutorig(x_velocity, y_velocity);
                        return true;
                    }
                    else if (fingerCount == 2) {
                        float x1 = event.getX(0);
                        float y1 = event.getY(0);
                        float x2 = event.getX(1);
                        float y2 = event.getY(1);
                        if (panBegan == 0) {
                            GL2JNILibAutoRig.panBeginAutorig(x1, y1, x2, y2);
                            panBegan++;
                        } else {
                            GL2JNILibAutoRig.panProgress(x1, y1, x2, y2);
                        }
                        return true;

                    }
                    return true;

                }
                return true;
            }
        });

        rig_help_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent();
                i.setClass(RigModel.this, AutoRigHelp.class);
                startActivity(i);
            }
        });

        rig_home_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AutoRigCloseConfirmDialog.exitConfirmDialog(context, RigModel.this);
            }
        });

        addjoint_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILibAutoRig.addNewJoint();
            }
        });

        rig_move_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                moveButton();
            }
        });

        rig_rotate_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rotateButton();
            }
        });

        rig_undo_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                undoButton();
            }
        });

        rig_redo_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                redoButton();
            }
        });

        add_obj_to_my_library_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String temp = "";
                String extension ="";
                File fromMesh = null;
                File toMesh = null;
                String fileName = Integer.toString(addObjToLibrary());
                if(backOrnext == 0){
                    temp = objFileName;
                    System.out.println("Obj File Name : " + temp);
                    extension = ".obj";
                    fromMesh = new File(objPath);
                    toMesh = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/mesh/"+fileName+extension);
                }
                else {
                   temp = "autorig-20000";
                   extension = ".sgr";
                   fromMesh = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/.cache/"+temp+extension);
                   toMesh = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/mesh/"+fileName+extension);
                }
                saveThumpnailImage(texturePath, fileName);
                File fromTexture = new File(texturePath);
                File toTexture = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/mesh/"+fileName+"-cm.png");
                try {
                    Constant.copy(fromMesh,toMesh);
                    Constant.copy(fromTexture,toTexture);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                //fromMesh.delete();
               //fromTexture.delete();
                if(backOrnext == 0)
                    showImportSuccessFullyAlert(context, "OBJ Mesh Added to the 'My Library");
                else
                    showImportSuccessFullyAlert(context, "Rigged Mesh Added to 'My Library");
            }
        });

        rig_fullscreen_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isFullscreenEnabled) {

                    fullview_rig.animate()
                            .translationYBy(1).translationY(0).setDuration(500)
                            .setListener(new AnimatorListenerAdapter() {
                                @Override
                                public void onAnimationEnd(Animator animation) {
                                    super.onAnimationEnd(animation);
                                    fullview_rig.setVisibility(View.GONE);
                                    temp_fullscreen_holder.setVisibility(View.VISIBLE);
                                    isFullscreenEnabled = false;
                                }
                            });
                }
                else{
                    fullview_rig.animate()
                            .translationYBy(0).translationY(1)
                            .setListener(new AnimatorListenerAdapter() {
                                @Override
                                public void onAnimationEnd(Animator animation) {
                                    super.onAnimationEnd(animation);
                                    temp_fullscreen_holder.setVisibility(View.GONE);
                                    fullview_rig.setVisibility(View.VISIBLE);
                                    isFullscreenEnabled = true;
                                }
                            });
                }
            }
        });

        rig_view_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int[] loc = new int[2];
                rig_view_btn.getLocationOnScreen(loc);
                animationEditorMenuDialog.viewPicker(context, loc[0], loc[1],RigModel.this);
            }
        });

        rig_scale_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(backOrnext == AUTORIG_VIEW.ADJUST_ENVELOP.getValue()) {
                    float envelopeScale = GL2JNILibAutoRig.getSelectedNodeOrJointScaleAutoRig();
                    System.out.println("Envelop Value : " + envelopeScale);
                    EnvelopScale.scaleDialog(context, RigModel.this, 10, rig_scale_btn.getTop() , envelopeScale);
                }
                else {
                    float x = GL2JNILibAutoRig.getAutoRigScalevalueX();
                    float y = GL2JNILibAutoRig.getAutoRigScalevalueY();
                    float z = GL2JNILibAutoRig.getAutoRigScalevalueZ();
                    scaleDialogClass.scaleDialog(context, rig_scale_btn.getTop(), "autorig", x, y, z);
                }
                System.out.println("Scale Btn Clicked");
            }
        });


        rig_back_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(backOrnext > 0) {
                    if (backOrnext == AUTORIG_VIEW.PREVIEW.getValue()) {
                        adjustEnvelop();
                        backOrnext--;
                        buttonHandler("scalebone");
                        GL2JNILibAutoRig.prevBtnAction();
                    } else if (backOrnext == AUTORIG_VIEW.ADJUST_ENVELOP.getValue()) {
                        backOrnext--;
                        buttonHandler("attachbone");
                        GL2JNILibAutoRig.prevBtnAction();
                    } else if (backOrnext == AUTORIG_VIEW.ATTACH_BONE.getValue()) {
                        System.out.println("Import Obj Enabled");
                        WarningDialog.warningConfirmDialog(context, RigModel.this);
                    }
                }
            }
        });


        rig_next_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(backOrnext < 4) {
                    GL2JNILibAutoRig.nextBtnAurorig(Constant.defaultCacheDir + "autorig-20000.sgr");
                    if (backOrnext == AUTORIG_VIEW.IMPORT_OBJ.getValue()) {
                        SelectBoneStructure.selectboneStructureDialog(context, RigModel.this);
                    } else if (backOrnext == AUTORIG_VIEW.ATTACH_BONE.getValue()) {
                        adjustEnvelop();
                        backOrnext++;
                        buttonHandler("scalebone");
                    } else if (backOrnext == AUTORIG_VIEW.ADJUST_ENVELOP.getValue()) {
                        backOrnext++;
                        buttonHandler("preview");
                        save_progress_center_rig.setVisibility(View.VISIBLE);
                    }
                }
            }
        });

        import_obj_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent();
                i.setClass(RigModel.this, ImportObjAndTexture.class);
                startActivity(i);
            }
        });

        mirror_autorig_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(GL2JNILibAutoRig.updateMirror())
                    mirrot_autorig_img.setImageResource(R.drawable.switch_on);
                else
                    mirrot_autorig_img.setImageResource(R.drawable.switch_off);
            }
        });

        buttonHandler("preview");
        addJointBtnHandler(0);
    }

    @Override
    protected void onResume() {
        super.onResume();
        objPath = extraDetail.getString("objPath");
        objFileName = extraDetail.getString("objFileName");
        texturePath = extraDetail.getString("texturePath");
        textureFileName = extraDetail.getString("textureFileName");
        meshLoader();
    }

    public void buttonHandler(String type){

        if(type.equals("preview")){
            add_obj_to_my_library_btn.setVisibility(View.VISIBLE);
            import_obj_btn.setVisibility(View.VISIBLE);
            if(backOrnext ==0) {
                current_progress_view_text.setText("IMPORT OBJ");
                rig_project_text.setText("PREVIEW THE IMPORTED OBJ MODEL OR SAVE TO 'MY LIBRARY'");
                add_obj_to_my_library_btn.setText("ADD OBJ TO MY LIBRARY");
            }
            else {
                current_progress_view_text.setText("PREVIEW");
                rig_project_text.setText("MOVE AND ROTATE JOINTS TO VERIFY RIGGING ACCURACY");
                add_obj_to_my_library_btn.setText("ADD TO MY LIBRARY");
            }
        }
        else if(type.equals("attachbone")){
            add_obj_to_my_library_btn.setVisibility(View.INVISIBLE);
            import_obj_btn.setVisibility(View.INVISIBLE);
            current_progress_view_text.setText("ATTACH SKELETON");
            rig_project_text.setText("PLACE THE JOINTS ON THE APPROPRIATE LOCATION OF OBJ MODEL");

        }
        else if(type.equals("addjoint")){
            import_obj_btn.setVisibility(View.INVISIBLE);
            add_obj_to_my_library_btn.setVisibility(View.INVISIBLE);
        }
        else if(type.equals("scalebone")){
            current_progress_view_text.setText("ADJUST ENVELOP");
            rig_project_text.setText("SELECT JOINT AND SCALE TO COVER VERTICES");
        }
    }

    private int addObjToLibrary(){
        int assetId = 0;
        AddToMyLibraryDB addToMyLibraryDB = new AddToMyLibraryDB();
        int dotObj = objFileName.lastIndexOf(".");
        String objFileNameForDB;
        objFileNameForDB = objFileName.substring(0,dotObj);
        int dotTexture = textureFileName.lastIndexOf(".");
        String txtFileNameForDB;
        txtFileNameForDB = textureFileName.substring(0,dotTexture);

        if(backOrnext == AUTORIG_VIEW.IMPORT_OBJ.getValue()){
          assetId =  addToMyLibraryDB.addToMyLibraryDatabase(context, "obj", objFileNameForDB, txtFileNameForDB);
        }
        else if(backOrnext == AUTORIG_VIEW.PREVIEW.getValue()){
          assetId=   addToMyLibraryDB.addToMyLibraryDatabase(context, "autorig", objFileNameForDB, txtFileNameForDB );
        }
        return assetId;
    }

    private void meshLoader(){
        System.out.println("Obj Name : " + objFileName);
        System.out.println("Obj Path : " + objPath);
        System.out.println("Texture Name : " + textureFileName);
        System.out.println("Texture path : " + texturePath);
        GL2JNILibAutoRig.addObjToScene(objPath, texturePath);
    }

    public void addSkeleton(){
        GL2JNILibAutoRig.humanRigging();

    }

    public void addSingleBone(){
        GL2JNILibAutoRig.singleBone();

    }

    private void adjustEnvelop(){

    }

    private void moveButton(){
        GL2JNILibAutoRig.moveAction();
    }

    private void rotateButton(){
        GL2JNILibAutoRig.rotateAction();
    }

    private void undoButton(){
        GL2JNILibAutoRig.unodBtnAction();

    }

    private void redoButton(){
        GL2JNILibAutoRig.redoBtnAction();
    }

    public void progressUpdater(){
        save_progress_center_rig.setVisibility(View.GONE);
    }

    private void saveThumpnailImage(String userTexture,String fileName){
        Bitmap bmp;
        bmp = BitmapFactory.decodeFile(userTexture);
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
        String userImageThump = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/images/";
        Constant.mkDir(userImageThump);
        savePng(scaledBitmap, userImageThump, fileName);
    }

    public void savePng( Bitmap img,String filePath, String fileName) {

        try {
            File dumpFile = new File(filePath, fileName+".png");
            FileOutputStream os = new FileOutputStream(dumpFile);
            img.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void showImportSuccessFullyAlert(Context context, String msg){
        final AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder
                .setMessage(msg)
                .setCancelable(false)
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
//                        Constant.rigModel.renderer = false;
//                        Constant.rigModel.glSurfaceViewAutoRigView.onPause();
//                        final Timer timer = new Timer();
//                        timer.schedule(new TimerTask() {
//                            @Override
//                            public void run() {
//                                GL2JNILibAutoRig.deleteAutoRigScene();
//                                Intent i = new Intent();
//                                Constant.rigModel.glSurfaceViewAutoRigView.setWillNotDraw(true);
//                                i.setClass(RigModel.this, SceneSelectionView.class);
//                                RigModel.this.startActivity(i);
//                                RigModel.this.finish();
//                                timer.cancel();
//                            }
//                        }, 500);
                    dialog.dismiss();
                    }
                });
        builder.create().show();
    }

    @Override
    public void onBackPressed(){
        Intent i = new Intent();
        i.setClass(RigModel.this, SceneSelectionView.class);
        startActivity(i);
        RigModel.this.finish();
    }

    public void addJointBtnHandler(int state){
        System.out.println("Add Joint Button State : " + state);
        if(Constant.AUTORIG_SCENE_MODE.RIG_MODE_OBJVIEW.getValue() == state){
            addjoint_btn.setVisibility(View.INVISIBLE);
            rig_move_btn.setVisibility(View.INVISIBLE);
            rig_rotate_btn.setVisibility(View.INVISIBLE);
            rig_scale_btn.setVisibility(View.INVISIBLE);
            mirror_autorig_btn.setVisibility(View.INVISIBLE);
        }
        else if(Constant.AUTORIG_SCENE_MODE.RIG_MODE_MOVE_JOINTS.getValue() == state){
            rig_move_btn.setVisibility(View.VISIBLE);
            rig_rotate_btn.setVisibility(View.VISIBLE);
            rig_scale_btn.setVisibility(View.VISIBLE);
            if(GL2JNILibAutoRig.selectedNodeId() > 0) {
                mirror_autorig_btn.setVisibility((GL2JNILibAutoRig.skeletonType() != 1) ? View.VISIBLE : View.INVISIBLE);
                addjoint_btn.setVisibility(View.VISIBLE);
                rig_scale_btn.setVisibility(View.INVISIBLE);
            }
        }
        else if(Constant.AUTORIG_SCENE_MODE.RIG_MODE_EDIT_ENVELOPES.getValue() == state){
            addjoint_btn.setVisibility(View.INVISIBLE);
            rig_move_btn.setVisibility(View.INVISIBLE);
            rig_rotate_btn.setVisibility(View.INVISIBLE);
            rig_scale_btn.setVisibility(View.INVISIBLE);
            mirror_autorig_btn.setVisibility(View.INVISIBLE);
            if(GL2JNILibAutoRig.selectedNodeId() > 0) {
                mirror_autorig_btn.setVisibility((GL2JNILibAutoRig.skeletonType() != 1) ? View.VISIBLE : View.INVISIBLE);
                rig_scale_btn.setVisibility(View.VISIBLE);
            }
        }
        else if(Constant.AUTORIG_SCENE_MODE.RIG_MODE_PREVIEW.getValue() == state){
            addjoint_btn.setVisibility(View.INVISIBLE);
            rig_move_btn.setVisibility(View.INVISIBLE);
            rig_rotate_btn.setVisibility(View.INVISIBLE);
            rig_scale_btn.setVisibility(View.INVISIBLE);
            mirror_autorig_btn.setVisibility(View.INVISIBLE);
            import_obj_btn.setVisibility(View.INVISIBLE);
            if(GL2JNILibAutoRig.selectedNodeId() != Constant.unDefined) {
                rig_move_btn.setVisibility(View.VISIBLE);
                rig_rotate_btn.setVisibility(View.VISIBLE);
                rig_scale_btn.setVisibility(View.VISIBLE);
                mirror_autorig_btn.setVisibility(View.INVISIBLE);

                if (GL2JNILibAutoRig.isAutoRigJointSelected()) {
                    rig_scale_btn.setVisibility(View.INVISIBLE);
                    if (GL2JNILibAutoRig.skeletonType() != 1) {
                        mirror_autorig_btn.setVisibility(View.VISIBLE);
                    } else
                        mirror_autorig_btn.setVisibility(View.INVISIBLE);
                }
            }


        }
    }

    public void undoOrRedoBtnUpdater(String state){

        switch (state){
            case "DEACTIVATE_UNDO":
                rig_undo_btn.setVisibility(View.INVISIBLE);
                rig_redo_btn.setVisibility(View.VISIBLE);
                break;
            case "DEACTIVATE_BOTH":
                rig_undo_btn.setVisibility(View.INVISIBLE);
                rig_redo_btn.setVisibility(View.INVISIBLE);
                break;
            case "DEACTIVATE_REDO":
                rig_redo_btn.setVisibility(View.INVISIBLE);
                rig_undo_btn.setVisibility(View.VISIBLE);
                break;
            default:
                rig_undo_btn.setVisibility(View.VISIBLE);
                rig_redo_btn.setVisibility(View.VISIBLE);
        }

    }
}

