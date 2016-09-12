package com.smackall.iyan3dPro;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;

import com.smackall.iyan3dPro.Adapters.AnimationSelectionAdapter;
import com.smackall.iyan3dPro.Analytics.HitScreens;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.DatabaseHelper;
import com.smackall.iyan3dPro.Helper.PathManager;
import com.smackall.iyan3dPro.Helper.SharedPreferenceManager;
import com.smackall.iyan3dPro.Helper.TextDB;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationSelection {

    public AnimationSelectionAdapter animationSelectionAdapter;
    public int selectedNodeId = -1;
    public String filePath = "";
    public boolean processCompleted = true;
    private Context mContext;
    private DatabaseHelper db;
    private SharedPreferenceManager sp;
    private ViewGroup insertPoint;
    private int currentFrame, totalFrame;
    private View view;

    public AnimationSelection(Context context, DatabaseHelper db,SharedPreferenceManager sp) {
        this.mContext = context;
        this.db = db;
        this.sp = sp;
    }

    public void showAnimationSelection() {
        HitScreens.AnimationSelectionView(mContext);
        processCompleted = true;
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

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
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
                insertPoint.removeAllViews();
                applyAnimation(false);
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
            }
        });
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
        animationSelectionAdapter = new AnimationSelectionAdapter(mContext, db, gridView);
        animationSelectionAdapter.animDBs = (db.getAllMyAnimation((GL2JNILib.getNodeType(selectedNodeId) == Constants.NODE_RIG) ? 0 : 1));
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

            ((EditorView) mContext).glView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    //((EditorView)((Activity)mContext)).showOrHideLoading(Constants.SHOW);
                    doCopyingWork();
                }
            });

        } else if ((selectedNodeType == Constants.NODE_TEXT_SKIN || selectedNodeType == Constants.NODE_TEXT) && selectedAssetId != -1) {
            final TextDB textDB = new TextDB();
            textDB.setText(GL2JNILib.getNodeName(selectedNodeId));
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
                    GL2JNILib.loadText(textDB.getTypeOfNode(), textDB.getText(), textDB.getFontSize(),
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
