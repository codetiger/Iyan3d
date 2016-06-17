package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;

import com.google.android.gms.analytics.Tracker;
import com.smackall.animator.Adapters.ChangeTextureAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.opengl.GL2JNILib;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TextureSelection {

    private Context mContext;
    public ChangeTextureAdapter changeTextureAdapter;
    public AssetsDB assetsDB = new AssetsDB();
    int selectedNodeid;
    private Tracker mTracker;

    public TextureSelection(Context context){
        this.mContext = context;
    }

    public void showChangeTexture()
    {
        HitScreens.TextureSelectionView(mContext);
        selectedNodeid = GL2JNILib.getSelectedNodeId();
                ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.HIDE);
        final ViewGroup insertPoint = (((EditorView) ((Activity) mContext)).sharedPreferenceManager.getInt(mContext, "toolbarPosition") == 1) ?
                (ViewGroup) ((Activity) mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity) mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup) insertPoint.getParent()).getChildCount(); i++){
            if(((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();

        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = vi.inflate(R.layout.image_selection, insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        GridView gridView = (GridView)v.findViewById(R.id.image_grid);
        initImagesGrid(gridView);
        Button cancel = (Button)v.findViewById(R.id.cancel_image);
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                ((EditorView)((Activity)mContext)).renderManager.removeTempTexture(selectedNodeid);
                insertPoint.removeAllViews();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                changeTextureAdapter = null;
            }
        });
        v.findViewById(R.id.add_image).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(assetsDB == null || assetsDB.getTexture().equals("") || (assetsDB.getX() == -1.0f && assetsDB.getY() == -1.0f && assetsDB.getZ() == -11.0f))
                    return;
                HitScreens.EditorView(mContext);
                assetsDB.setIsTempNode(false);
                changeTexture();
                insertPoint.removeAllViews();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                changeTextureAdapter = null;
            }
        });
        v.findViewById(R.id.import_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((EditorView)((Activity)mContext)).imageManager.getImageFromStorage(Constants.CHANGE_TEXTURE_MODE);
            }
        });
    }

    private void initImagesGrid(GridView gridView)
    {
        changeTextureAdapter = new ChangeTextureAdapter(mContext,gridView);
        gridView.setAdapter(changeTextureAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    public void notifyDataChanged(){
        if(changeTextureAdapter !=null)
            changeTextureAdapter.notifyDataSetChanged();
    }

    public void changeTexture()
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        ((EditorView)((Activity)mContext)).renderManager.changeTexture(selectedNodeid,assetsDB.getTexture(),assetsDB.getX(),assetsDB.getY(),assetsDB.getZ(),assetsDB.getIsTempNode());
    }
}
