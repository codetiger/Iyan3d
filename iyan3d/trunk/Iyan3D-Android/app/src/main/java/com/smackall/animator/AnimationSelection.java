package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.Spinner;

import com.smackall.animator.Adapters.AnimationSelectionAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.TextDB;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.concurrent.TimeUnit;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationSelection {

    public AnimationSelectionAdapter animationSelectionAdapter;
    public Spinner category;
    public int[] animationType = {4, 6, 5, 8, 7};
    public int selectedNodeId = -1;
    public String filePath = "";
    public boolean processCompleted = true;
    private Context mContext;
    private DatabaseHelper db;
    private SharedPreferenceManager sp;
    private String[] animationCategory;
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private ViewGroup insertPoint;
    private int currentFrame, totalFrame;
    private View view;

    public AnimationSelection(Context context, DatabaseHelper db, AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager, SharedPreferenceManager sp) {
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
        this.sp = sp;
        animationCategory = new String[]{mContext.getString(R.string.trending), mContext.getString(R.string.featured), mContext.getString(R.string.top_rated),
                mContext.getString(R.string.recent), mContext.getString(R.string.my_animations)};
    }

    public void showAnimationSelection() {
        HitScreens.AnimationSelectionView(mContext);
        processCompleted = true;
        addToDownloadManager.lastErrorShowingTime = 0;
        selectedNodeId = GL2JNILib.getSelectedNodeId();
        int selectedNodeType = (selectedNodeId != -1) ? GL2JNILib.getNodeType(selectedNodeId) : -1;
        if ((selectedNodeType != Constants.NODE_RIG && selectedNodeType != Constants.NODE_TEXT_SKIN)) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.select_text_or_character_msg));
            return;
        }
        ((EditorView) mContext).renderManager.unselectObject(selectedNodeId);
        currentFrame = GL2JNILib.currentFrame();
        totalFrame = GL2JNILib.totalFrame();

        ((EditorView) mContext).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView) mContext).sharedPreferenceManager.getInt(mContext, "toolbarPosition") == 1) ?
                (ViewGroup) ((Activity) mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity) mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup) insertPoint.getParent()).getChildCount(); i++) {
            if (((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();

        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        view = vi.inflate(R.layout.animation_view, insertPoint, false);
        insertPoint.addView(view, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        GridView gridView = (GridView) view.findViewById(R.id.animation_grid);
        initAssetGrid(gridView);
        Button cancel = (Button) view.findViewById(R.id.cancel_animationView);
        category = (Spinner) view.findViewById(R.id.animation_categary);
        ArrayAdapter<String> adapter_state = new ArrayAdapter<String>(mContext, R.layout.spinner_cell, animationCategory);
        adapter_state.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        category.setAdapter(adapter_state);
        category.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                onSpinnerItemSelected(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                applyAnimation(true);
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
            }
        });
        view.findViewById(R.id.add_animation_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (filePath.length() <= 0 || !processCompleted) return;
                HitScreens.EditorView(mContext);
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                applyAnimation(false);
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
            }
        });
    }

    public void onSpinnerItemSelected(int position) {
        ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
        category.setSelection(position);
        animationSelectionAdapter.animDBs.clear();
        animationSelectionAdapter.selectedId = -1;
        filePath = "";
        view.findViewById(R.id.loadingJson).setVisibility(View.INVISIBLE);
        if (((((animationType[position]) == 7) ? db.getAllMyAnimation((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1) : db.getAllMyAnimation(animationType[position], (GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1, "")) == null ||
                (((animationType[position]) == 7) ? db.getAllMyAnimation((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1) : db.getAllMyAnimation(animationType[position], (GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1, "")).size() == 0) && animationType[position] != 7) {
            view.findViewById(R.id.loadingJson).setVisibility(View.VISIBLE);
            DownloadHelper animationJsonDownload = new DownloadHelper();
            animationJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/animationDetail.json", db, mContext, sp, Constants.ANIMATION_JSON);
        } else if (mContext != null && mContext != null && ((EditorView) mContext).sharedPreferenceManager != null) {
            long lastTimeInHour = TimeUnit.MILLISECONDS.toHours(((EditorView) mContext).sharedPreferenceManager.getLong(mContext, "lastAnimationJsonUpdatedTime"));
            long currentTimeInHour = TimeUnit.MILLISECONDS.toHours(System.currentTimeMillis());
            int difference = (int) ((Math.abs(Math.max(lastTimeInHour, currentTimeInHour) - Math.min(lastTimeInHour, currentTimeInHour))));
            if (difference >= 5) {
                ((EditorView) mContext).sharedPreferenceManager.setData(mContext, "lastAnimationJsonUpdatedTime", System.currentTimeMillis());
                ((EditorView) mContext).assetsAniamtionRegularUpdate.jsonParse("https://iyan3dapp.com/appapi/json/animationDetail.json", Constants.ANIMATION_JSON);
            }
        }
        animationSelectionAdapter.animDBs = ((animationType[position]) == 7) ? db.getAllMyAnimation((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1) : db.getAllMyAnimation(animationType[position], (GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1, "");
        downloadManager.cancelAll();
        animationSelectionAdapter.downloadThumbnail();
        animationSelectionAdapter.notifyDataSetChanged();
    }

    private void applyAnimation(final boolean isCancel) {
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (GL2JNILib.isPlaying())
                    GL2JNILib.setIsPlaying(false, null);
                GL2JNILib.setCurrentFrame(currentFrame, ((EditorView) mContext).nativeCallBacks);
                if (isCancel)
                    GL2JNILib.setTotalFrame(totalFrame);
                GL2JNILib.removeTempNode();
                GL2JNILib.hideNode(selectedNodeId, false);
                if (!isCancel)
                    GL2JNILib.applyAnimation(selectedNodeId, selectedNodeId, filePath);
                ((Activity) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            if (((EditorView) mContext).frameAdapter != null) {

                                ((EditorView) mContext).frameAdapter.notifyDataSetChanged();
                            }
                            ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                            ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
                            mContext = null;
                            db = null;
                            animationSelectionAdapter = null;
                        } catch (IndexOutOfBoundsException e) {
                            e.printStackTrace();
                        }
                    }
                });
            }
        });
    }

    private void initAssetGrid(GridView gridView) {
        animationSelectionAdapter = new AnimationSelectionAdapter(mContext, db, gridView, this.addToDownloadManager, this.downloadManager, this.sp);
        animationSelectionAdapter.downloadThumbnail();
        animationSelectionAdapter.animDBs = this.db.getAllMyAnimation(4, (GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1, "");
        gridView.setAdapter(animationSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    private void createDuplicateForAnimation() {
        int selectedAssetId;
        int selectedNodeType;
        selectedAssetId = GL2JNILib.getAssetIdWithNodeId(selectedNodeId);
        selectedNodeType = GL2JNILib.getNodeType(selectedNodeId);
        if ((selectedNodeType == Constants.NODE_RIG || selectedNodeType == Constants.NODE_SGM || selectedNodeType == Constants.NODE_OBJ) && selectedAssetId != -1) {
            final AssetsDB assetsDB;
            try {
                assetsDB = selectedAssetId > 20000 && selectedAssetId <= 30000 || selectedAssetId > 40000 && selectedAssetId <= 50000 ? ((EditorView) mContext).db.getMyModelWithAssetId(selectedAssetId).get(0) : ((EditorView) mContext).db.getModelWithAssetId(selectedAssetId).get(0);
            } catch (IndexOutOfBoundsException e) {
                return;
            }
            assetsDB.setTexture(GL2JNILib.getTexture(selectedNodeId));
            assetsDB.setIsTempNode(true);
            ((EditorView) mContext).glView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GL2JNILib.importAsset(assetsDB.getType(), assetsDB.getAssetsId(), assetsDB.getAssetName(), assetsDB.getTexture(), 0, 0, assetsDB.getIsTempNode(), assetsDB.getX(), assetsDB.getY(), assetsDB.getZ(), Constants.IMPORT_ASSET_ACTION);
                    //((EditorView)((Activity)mContext)).showOrHideLoading(Constants.SHOW);
                    doCopyingWork();
                }
            });
        } else if ((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedAssetId != -1) {
            final TextDB textDB = new TextDB();
            textDB.setAssetName(GL2JNILib.getNodeName(selectedNodeId));
            textDB.setFilePath(GL2JNILib.optionalFilePathWithId(selectedNodeId));
            textDB.setRed(GL2JNILib.getVertexColorXWithId(selectedNodeId));
            textDB.setGreen(GL2JNILib.getVertexColorYWithId(selectedNodeId));
            textDB.setBlue(GL2JNILib.getVertexColorZWithId(selectedNodeId));
            textDB.setBevalValue((int) GL2JNILib.nodeSpecificFloatWithId(selectedNodeId));
            textDB.setFontSize(GL2JNILib.getFontSizeWithId(selectedNodeId));
            textDB.setTextureName(GL2JNILib.getTexture(selectedNodeId));
            textDB.setTypeOfNode((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_TEXT) ? Constants.ASSET_TEXT : Constants.ASSET_TEXT_RIG);
            textDB.setTempNode(true);
            ((EditorView) mContext).glView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GL2JNILib.loadText(textDB.getRed(), textDB.getGreen(), textDB.getBlue(), textDB.getTypeOfNode(), textDB.getTextureName(), textDB.getAssetName(), textDB.getFontSize(),
                            textDB.getBevalValue(), textDB.getAssetAddType(), textDB.getFilePath(), textDB.getTempNode());
                    ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
                    doCopyingWork();
                }
            });
        }
    }

    private void doCopyingWork() {
        GL2JNILib.copyKeysOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1);
        GL2JNILib.copyPropsOfNode(selectedNodeId, GL2JNILib.getNodeCount() - 1, true);
        GL2JNILib.setTransparency(1.0f, GL2JNILib.getNodeCount() - 1);
        //GL2JNILib.hideNode(GL2JNILib.getNodeCount() - 1, false);
        GL2JNILib.hideNode(selectedNodeId, true);
        GL2JNILib.unselectObject(selectedNodeId);
        GL2JNILib.setCurrentFrame(currentFrame, ((EditorView) mContext).nativeCallBacks);
        GL2JNILib.setTotalFrame(totalFrame);
        GL2JNILib.switchFrame();
        GL2JNILib.applyAnimation(selectedNodeId, GL2JNILib.getNodeCount() - 1, filePath);
        ((Activity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((EditorView) mContext).play.updatePhysics(true);
            }
        });
    }

    public void applyAnimation(int position) {
        int animId = animationSelectionAdapter.animDBs.get(position).getAnimAssetId();
        String ext = (animationSelectionAdapter.animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
        filePath = PathManager.LocalAnimationFolder + "/" + animId + ext;
        if ((animationSelectionAdapter.animDBs.get(position).getBoneCount() != GL2JNILib.getBoneCount(selectedNodeId) && GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG)) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.bone_count_not_match_msg));
            filePath = "";
            processCompleted = true;
            return;
        }
        createDuplicateForAnimation();
    }
}
