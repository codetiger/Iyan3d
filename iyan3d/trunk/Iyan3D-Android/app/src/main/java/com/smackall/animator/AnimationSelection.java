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
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationSelection {

    private Context mContext;
    private DatabaseHelper db;
    AnimationSelectionAdapter animationSelectionAdapter;
    public Spinner category;

    private String[] animationCategory = {"Trending", "Featured", "Top Rated", "Recent", "My Animations"};
    private int[] animationType = {4, 6, 5, 8, 7};

    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    ViewGroup insertPoint;

    public AnimationSelection(Context context,DatabaseHelper db,AddToDownloadManager addToDownloadManager
            ,DownloadManager downloadManager){
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    public void showAnimationSelection()
    {
        ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.HIDE);
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
                category.setSelection(position);
                animationSelectionAdapter.animDBs.clear();
                animationSelectionAdapter.animDBs = db.getAllMyAnimation(animationType[position],1,"");
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
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
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
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                animationSelectionAdapter = null;
            }
        });
    }

    private void initAssetGrid(GridView gridView)
    {
        animationSelectionAdapter = new AnimationSelectionAdapter(mContext, db, gridView,this.addToDownloadManager,this.downloadManager);
        animationSelectionAdapter.animDBs = this.db.getAllMyAnimation(4,1,"");
        gridView.setAdapter(animationSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }
}
