package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.RecyclerView;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * Created by Sabish.M on 19/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class Rig implements View.OnClickListener, CompoundButton.OnCheckedChangeListener{
    public Context mContext;
    private SharedPreferenceManager sharedPreferenceManager;
    private DatabaseHelper db;
    private int selectedNodeId = -1;
    private int selectedNodeType = -1;

    public int currentSceneMode = 0;
    private boolean rigCompleted = false;

    public Rig(Context mContext,SharedPreferenceManager sp,DatabaseHelper db){
        this.mContext = mContext;
        this.sharedPreferenceManager = sp;
        this.db = db;
    }

    public void rig()
    {
        selectedNodeId = GL2JNILib.getSelectedNodeId();
        selectedNodeType = (selectedNodeId != -1) ? GL2JNILib.getNodeType(selectedNodeId) : -1;
        int selectedNodeType = (GL2JNILib.getSelectedNodeId() != -1) ? ((GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()))) : -1;
        if(selectedNodeType != -1 && (selectedNodeType == Constants.NODE_SGM || selectedNodeType == Constants.NODE_RIG)){
            ((RecyclerView) ((Activity)mContext).findViewById(R.id.frames)).setVisibility(View.GONE);
            ((LinearLayout) ((Activity)mContext).findViewById(R.id.frameCtr)).setVisibility(View.GONE);
            ((TextView) ((Activity)mContext).findViewById(R.id.rigHeader)).setVisibility(View.VISIBLE);
            ((TextView) ((Activity)mContext).findViewById(R.id.rigHeader)).setText(String.format(Locale.getDefault(),"%s","RIGGING MODE"));
            if(selectedNodeType == Constants.NODE_RIG) rigRiggedMesh();
            else rigMesh();
        }
        else if(selectedNodeType != -1) UIHelper.informDialog(mContext,"Bones cannot be added to this model.");
        else UIHelper.informDialog(mContext,"Please Select any Node to add Bone.");
    }

    private void rigMesh(){
        initViews();
    }

    public void rigRiggedMesh()
    {
        if(GL2JNILib.canEditRigBones()) {
            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(
                    mContext);
            alertDialogBuilder.setTitle("Alert");
            alertDialogBuilder
                    .setMessage("This model was rigged using the previous version. Some of the envelope information might be missing. Do you want to continue?")
                    .setCancelable(false)
                    .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            initViews();
                        }
                    })
                    .setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                        }
                    });

            AlertDialog alertDialog = alertDialogBuilder.create();
            alertDialog.setCanceledOnTouchOutside(false);
            alertDialog.show();
        }
        else
            initViews();
    }

    private void initViews()
    {
        HitScreens.AutoRigView(mContext);
        currentSceneMode =0;
        rigCompleted = false;
        addRigToolBar();
        ((EditorView)((Activity)mContext)).renderManager.beginRigging();
    }

    public void addRigToolBar()
    {
        ((FrameLayout)((Activity)mContext).findViewById(R.id.autorig_bottom_tool)).setVisibility(View.VISIBLE);
        if(sharedPreferenceManager.getInt(((Activity)mContext),"toolbarPosition") == 1 )
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.autorig_bottom_tool)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.START);
        else
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.autorig_bottom_tool)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.END);

        ViewGroup insertPointParent = (sharedPreferenceManager.getInt(((Activity)mContext),"toolbarPosition") == 1 ) ? (ViewGroup)((ViewGroup) ((Activity)mContext).findViewById(R.id.rightView)).getParent()
                : (ViewGroup)((ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)).getParent();
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
        LayoutInflater vi = (LayoutInflater) ((Activity)mContext).getApplicationContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = vi.inflate(R.layout.rig_toolbar,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        v.findViewById(R.id.add_joint).setOnClickListener(this);
        v.findViewById(R.id.rig_move).setOnClickListener(this);
        v.findViewById(R.id.rig_rotate).setOnClickListener(this);
        v.findViewById(R.id.rig_scale).setOnClickListener(this);
        ((Switch)v.findViewById(R.id.rig_mirror)).setOnCheckedChangeListener(this);
        ((Switch)v.findViewById(R.id.rig_mirror)).setChecked(GL2JNILib.getRigMirrorState());
        ((Activity)mContext).findViewById(R.id.rig_cancel).setOnClickListener(this);
        ((Activity)mContext).findViewById(R.id.rig_pre_scene).setOnClickListener(this);
        ((Activity)mContext).findViewById(R.id.rig_next_scene).setOnClickListener(this);
        ((Activity)mContext).findViewById(R.id.rig_add_to_scene).setOnClickListener(this);
        ((Activity)mContext).findViewById(R.id.rig_add_to_scene).setVisibility(View.INVISIBLE);
    }

    private void addRigToDB(String nodeName,String textureName,int boneCount)
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        AssetsDB assetsDB = new AssetsDB();
        assetsDB.setAssetName(nodeName);
        assetsDB.setAssetsId(40000 + ((db.getMYModelAssetCount() == 0) ? 1 : db.getAllMyModelDetail().get(db.getMYModelAssetCount() - 1).getID()));
        assetsDB.setTexture((textureName.equals("-1")) ? "white_texture" : textureName);
        assetsDB.setDateTime(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date()));
        assetsDB.setHash(FileHelper.md5(assetsDB.getAssetName()));
        assetsDB.setType(1);
        assetsDB.setNbones(boneCount);
        String meshFrom = PathManager.LocalMeshFolder+"/"+123456+".sgr";
        String textureFrom = (assetsDB.getTexture().equals("-1")) ? PathManager.DefaultAssetsDir+"/white_texture.png" : PathManager.LocalImportedImageFolder+"/"+assetsDB.getTexture()+".png";
        if(!FileHelper.checkValidFilePath(textureFrom)) {
            textureFrom = PathManager.LocalMeshFolder + "/" + assetsDB.getTexture() + ".png";
            if(!FileHelper.checkValidFilePath(textureFrom)){
                textureFrom = PathManager.DefaultAssetsDir+"/"+"white_texture.png";
            }
        }
        String meshTo = PathManager.LocalMeshFolder+"/"+ assetsDB.getAssetsId()+".sgr";
        String textureTo = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId()+"-cm.png";
        String thumbnailTo = PathManager.LocalThumbnailFolder+"/"+assetsDB.getAssetsId()+".png";
        FileHelper.move(meshFrom, meshTo);
        FileHelper.copy(textureFrom, textureTo);
        ((EditorView)(Activity)mContext).imageManager.makeThumbnail(textureTo, String.valueOf(assetsDB.getAssetsId()));
        db.addNewMyModelAssets(assetsDB);
        assetsDB.setIsTempNode(false);
        assetsDB.setTexture(assetsDB.getAssetsId()+"-cm");
        ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB, false);
    }

    public void switchSceneMode(int sceneMode)
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        ((Activity) mContext).findViewById(R.id.rig_add_to_scene).setVisibility(View.INVISIBLE);
        if(currentSceneMode  <= Constants.RIG_MODE_PREVIEW){
            if(currentSceneMode  == Constants.RIG_MODE_OBJVIEW){
                if(selectedNodeType == Constants.NODE_RIG) {
                    ((EditorView)((Activity)mContext)).renderManager.switchSceneMode(this,sceneMode);
                    currentSceneMode += sceneMode;
                    updateText();
                } else {
                    boneStructure();
                }
            }
            else
            {
               ((EditorView)((Activity)mContext)).renderManager.switchSceneMode(this,sceneMode);
                currentSceneMode += sceneMode;
                updateText();
            }
        }
    }

    private void boneStructure()
    {
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final AlertDialog builder;
                LayoutInflater inflater = LayoutInflater.from(mContext);
                final View textEntryView = inflater
                        .inflate(R.layout.bone_structure, null);
                builder = new AlertDialog.Builder(mContext).create();
                builder.setTitle("Select Bone Structure");
                builder.setMessage("You can either start with a complete Human Bone structure or with a single bone?");
                Button OKBut = (Button) textEntryView.findViewById(R.id.single);
                OKBut.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        ((EditorView)((Activity)mContext)).renderManager.setSkeletonType(Constants.OWN_RIGGING);
                        //currentSceneMode += 1;
                        updateText();
                        switchSceneMode(1);
                        builder.cancel();
                    }
                });

                Button BackBut = (Button) textEntryView.findViewById(R.id.human);
                BackBut.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        ((EditorView)((Activity)mContext)).renderManager.setSkeletonType(Constants.HUMAN_RIGGING);
                        //currentSceneMode += 1;
                        updateText();
                        switchSceneMode(1);
                        builder.cancel();
                    }
                });
                Button BackBut1 = (Button) textEntryView.findViewById(R.id.cancel);
                BackBut1.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        ((Activity)mContext).findViewById(R.id.rig_cancel).performClick();
                        builder.cancel();
                    }
                });

                builder.setView(textEntryView);
                builder.setCanceledOnTouchOutside(false);
                builder.show();
            }
        });
    }

    @Override
    public void onClick(final View v) {
        switch (v.getId()) {
            case R.id.add_joint:
                ((EditorView)((Activity)mContext)).renderManager.addJoint(this);
                break;
            case R.id.rig_move:
                ((EditorView)((Activity)mContext)).renderManager.setMove();
                break;
            case R.id.rig_rotate:
                ((EditorView)((Activity)mContext)).renderManager.setRotate();
                break;
            case R.id.rig_scale:
                ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if(GL2JNILib.changeEnvelopScale(GL2JNILib.envelopScale())) {
                            ((Activity) mContext).runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    ((EditorView) ((Activity) mContext)).envelopScale.showEnvelopScale(v, null);
                                }
                            });
                        }
                        else if(GL2JNILib.scale(((EditorView) (Activity) mContext).nativeCallBacks)) {
                            ((Activity)mContext).runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    ((EditorView) ((Activity) mContext)).scale.showScale(v, null, GL2JNILib.scaleValueX(), GL2JNILib.scaleValueY(), GL2JNILib.scaleValueZ());
                                }
                            });
                        }
                    }
                });
                break;
            case R.id.rig_cancel:
                HitScreens.EditorView(mContext);
                ((EditorView)(Activity)mContext).renderManager.cancelRig(false);
                ((EditorView)(Activity)mContext).addRightView(false);
                ((TextView) ((Activity)mContext).findViewById(R.id.rigHeader)).setVisibility(View.GONE);
                ((RecyclerView) ((Activity)mContext).findViewById(R.id.frames)).setVisibility(View.VISIBLE);
                ((LinearLayout) ((Activity)mContext).findViewById(R.id.frameCtr)).setVisibility(View.VISIBLE);
                break;
            case R.id.rig_pre_scene:
                rigCompleted = false;
                currentSceneMode--;
                ((EditorView)((Activity)mContext)).renderManager.switchSceneMode(this,-1);
                updateText();
                break;
            case R.id.rig_next_scene:
                if(!rigCompleted)
                    switchSceneMode(1);
                break;
            case R.id.rig_add_to_scene:
                HitScreens.EditorView(mContext);
                String nodeName = GL2JNILib.getNodeName(selectedNodeId);
                String textureName = GL2JNILib.getTexture(selectedNodeId);
                int boneCount = GL2JNILib.getBoneCount(GL2JNILib.getNodeCount()-1);
                ((EditorView)(Activity)mContext).renderManager.cancelRig(true);
                addRigToDB(nodeName,textureName,boneCount);
                ((EditorView)(Activity)mContext).addRightView(false);
                ((TextView) ((Activity)mContext).findViewById(R.id.rigHeader)).setVisibility(View.GONE);
                ((RecyclerView) ((Activity)mContext).findViewById(R.id.frames)).setVisibility(View.VISIBLE);
                ((LinearLayout) ((Activity)mContext).findViewById(R.id.frameCtr)).setVisibility(View.VISIBLE);
                break;
        }
    }

    public void boneLimitCallBack()
    {
        UIHelper.informDialog(mContext,"Maximum bone limit reached.");
    }

    public void updateText() {
        TextView textView = (TextView) ((Activity) mContext).findViewById(R.id.rig_scene_lable);
        switch (currentSceneMode-1) {
            case Constants.RIG_MODE_OBJVIEW:
                textView.setText(String.format(Locale.getDefault(),"%s","ATTACH SKELETON"));
                break;
            case Constants.RIG_MODE_MOVE_JOINTS:
                textView.setText(String.format(Locale.getDefault(),"%s","ATTACH SKELETON"));
                break;
            case Constants.RIG_MODE_EDIT_ENVELOPES:
                textView.setText(String.format(Locale.getDefault(),"%s","ADJUST ENVELOP"));
                break;
            case Constants.RIG_MODE_PREVIEW: {
                textView.setText(String.format(Locale.getDefault(),"%s","PREVIEW"));
                break;
            }
        }
    }

    public void rigCompletedCallBack(final boolean completed)
    {
         (((Activity) mContext)).runOnUiThread(new Runnable() {
             @Override
             public void run() {
                 ((EditorView)(Activity)mContext).showOrHideLoading(Constants.HIDE);
                 if(completed) {
                     rigCompleted = true;
                     (((Activity) mContext)).findViewById(R.id.rig_add_to_scene).setVisibility(View.VISIBLE);
                 }
             }
         });
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()){
            case R.id.rig_mirror:
                ((EditorView)mContext).glView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        GL2JNILib.switchMirror();
                    }
                });
                break;
        }
    }
}
