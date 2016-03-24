package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.Spinner;

import com.smackall.animator.Adapters.AnimationSelectionAdapter;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationSelection {

    private Context mContext;
    private DatabaseHelper db;
    private SharedPreferenceManager sp;
    AnimationSelectionAdapter animationSelectionAdapter;
    public Spinner category;

    private String[] animationCategory = {"Trending", "Featured", "Top Rated", "Recent", "My Animations"};
    public int[] animationType = {4, 6, 5, 8, 7};

    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    ViewGroup insertPoint;
    private int selectedNodeId = -1;
    private int selectedNodeType = -1;
    private int currentFrame, totalFrame;

    String filePath;

    public AnimationSelection(Context context,DatabaseHelper db,AddToDownloadManager addToDownloadManager
            ,DownloadManager downloadManager,SharedPreferenceManager sp){
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
        this.sp = sp;
    }

    public void showAnimationSelection()
    {
        selectedNodeId = GL2JNILib.getSelectedNodeId();
        selectedNodeType = (selectedNodeId != -1 ) ? GL2JNILib.getNodeType(selectedNodeId) : -1;
        if((selectedNodeType != Constants.NODE_RIG && selectedNodeType != Constants.NODE_TEXT_SKIN)) {
            UIHelper.informDialog(mContext, "Please select a text or character to apply the animation.");
            return;
        }
        ((EditorView)((Activity)mContext)).renderManager.unselectObject(selectedNodeId);
        currentFrame = GL2JNILib.currentFrame();
        totalFrame = GL2JNILib.totalFrame();

                ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView)((Activity)mContext)).sharedPreferenceManager.getInt(mContext,"toolbarPosition") == 1 ) ?
                (ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity)mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup)insertPoint.getParent()).getChildCount(); i++){
            if(((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup)insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();

        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = vi.inflate(R.layout.animation_view,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        GridView gridView = (GridView)v.findViewById(R.id.animation_grid);
        initAssetGrid(gridView);
        Button cancel = (Button)v.findViewById(R.id.cancel_animationView);
        category = (Spinner)v.findViewById(R.id.animation_categary);
        ArrayAdapter<String> adapter_state = new ArrayAdapter<String>(mContext,R.layout.spinner_cell, animationCategory);
        adapter_state.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        category.setAdapter(adapter_state);
        category.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
                category.setSelection(position);
                animationSelectionAdapter.animDBs.clear();
                animationSelectionAdapter.animDBs = ((animationType[position]) == 7) ? db.getAllMyAnimation((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1) : db.getAllMyAnimation(animationType[position], (GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1, "");
                animationSelectionAdapter.notifyDataSetChanged();
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                if(GL2JNILib.isPlaying())
                    GL2JNILib.setIsPlaying(false);
                GL2JNILib.setCurrentFrame(currentFrame);
                GL2JNILib.setTotalFrame(totalFrame);
                ((EditorView) ((Activity) mContext)).renderManager.removeTempNode();
                ((EditorView)((Activity)mContext)).renderManager.hideNode(selectedNodeId, false);
                ((EditorView)((Activity)mContext)).frameAdapter.notifyDataSetChanged();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
                mContext = null;
                db = null;
                animationSelectionAdapter = null;
            }
        });
        v.findViewById(R.id.add_animation_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                if(GL2JNILib.isPlaying())
                    GL2JNILib.setIsPlaying(false);
                GL2JNILib.setCurrentFrame(currentFrame);
                ((EditorView) ((Activity) mContext)).renderManager.removeTempNode();
                ((EditorView)((Activity)mContext)).renderManager.hideNode(selectedNodeId, false);
                GL2JNILib.applyAnimation(selectedNodeId, selectedNodeId, filePath);
                ((EditorView)((Activity)mContext)).frameAdapter.notifyDataSetChanged();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
                mContext = null;
                db = null;
                animationSelectionAdapter = null;
            }
        });
    }

    private void initAssetGrid(GridView gridView)
    {
        animationSelectionAdapter = new AnimationSelectionAdapter(mContext, db, gridView,this.addToDownloadManager,this.downloadManager,this.sp);
        animationSelectionAdapter.animDBs = this.db.getAllMyAnimation(4,(GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1,"");
        gridView.setAdapter(animationSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
        animationSelectionAdapter.downloadThumbnail();
    }

    public void createDuplicateForAnimation()
    {
        int selectedAssetId;
        int selectedNodeType;
        selectedAssetId = GL2JNILib.getAssetIdWithNodeId(selectedNodeId);
        selectedNodeType = GL2JNILib.getNodeType(selectedNodeId);
        if((selectedNodeType == Constants.NODE_RIG || selectedNodeType ==  Constants.NODE_SGM || selectedNodeType ==  Constants.NODE_OBJ) && selectedAssetId != -1)
        {
            final AssetsDB assetsDB;
            if(selectedAssetId > 20000 && selectedAssetId <=30000 || selectedAssetId > 40000 && selectedAssetId <=50000)
                assetsDB = ((EditorView)(Activity)mContext).db.getMyModelWithAssetId(selectedAssetId).get(0);
            else
                assetsDB = ((EditorView)(Activity)mContext).db.getModelWithAssetId(selectedAssetId).get(0);
            assetsDB.setTexture(GL2JNILib.getTexture(selectedNodeId));
            assetsDB.setIsTempNode(true);
            ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(),assetsDB.getX(),assetsDB.getY(),assetsDB.getZ() ,Constants.IMPORT_ASSET_ACTION);
                    ((EditorView)((Activity)mContext)).showOrHideLoading(Constants.HIDE);
                    doCopyingWork();
                }
            });
        }
        else if((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedAssetId != -1){
            String name = GL2JNILib.getNodeName(selectedNodeId);
            String fontName = GL2JNILib.optionalFilePath();
            float colorx = GL2JNILib.getVertexColorX();
            float colory = GL2JNILib.getVertexColorY();
            float colorz = GL2JNILib.getVertexColorZ();
            float bevalValue = GL2JNILib.nodeSpecificFloat();
            int fontSize = GL2JNILib.getFontSize();
        }
    }

    private void doCopyingWork()
    {
        GL2JNILib.copyKeysOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1);
        GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1);
        GL2JNILib.hideNode(GL2JNILib.getNodeCount() - 1, false);
        GL2JNILib.hideNode(selectedNodeId, true);
        GL2JNILib.unselectObject(selectedNodeId);
        GL2JNILib.setCurrentFrame(currentFrame);
        GL2JNILib.setTotalFrame(totalFrame);
        GL2JNILib.switchFrame();
        GL2JNILib.applyAnimation(selectedNodeId, GL2JNILib.getNodeCount() - 1, filePath);
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((EditorView) ((Activity) mContext)).play.play(true);
            }
        });
    }

    public void applyAnimation(int position){
        int animId = animationSelectionAdapter.animDBs.get(position).getAnimAssetId();
        String ext = (animationSelectionAdapter.animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
        filePath = PathManager.LocalAnimationFolder+"/"+animId+ext;
        if((animationSelectionAdapter.animDBs.get(position).getBonecount() != GL2JNILib.getBoneCount(selectedNodeId) && GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG)) {
            UIHelper.informDialog(mContext, "Bone count cannot match.");
            return;
        }
        createDuplicateForAnimation();
    }
}
