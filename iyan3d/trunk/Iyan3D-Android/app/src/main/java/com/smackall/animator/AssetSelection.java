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

import com.smackall.animator.Adapters.AssetSelectionAdapter;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AssetSelection {

    private Context mContext;
    private DatabaseHelper db;
    AssetSelectionAdapter assetSelectionAdapter;
    public Spinner category;

    private String[] modelCategory = {"All Models", "Characters", "Backgrounds", "Accessories", "Minecraft",
            "FNAF", "My Library" };
    private int[] modelType = {0, 1, 2, 3, 5,6, -1 };

    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int PARTICLE = 13;
    ViewGroup insertPoint;

    public AssetSelection(Context context,DatabaseHelper db,AddToDownloadManager addToDownloadManager
    ,DownloadManager downloadManager){
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    public void showAssetSelection(int viewType)
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
        View v = vi.inflate(R.layout.asset_view,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        GridView gridView = (GridView)v.findViewById(R.id.asset_grid);
        initAssetGrid(gridView,viewType);
        Button cancel = (Button)v.findViewById(R.id.cancel_assetView);
        category = (Spinner)v.findViewById(R.id.model_categary);
        category.setVisibility((viewType == Constants.PARTICLE_VIEW) ? View.INVISIBLE :View.VISIBLE);
        ArrayAdapter<String> adapter_state = new ArrayAdapter<String>(mContext,R.layout.spinner_cell, modelCategory);
        adapter_state.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        category.setAdapter(adapter_state);
        category.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                category.setSelection(position);
                assetSelectionAdapter.assetsDBs.clear();
                assetSelectionAdapter.assetsDBs = db.getAllModelDetail((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[position]);
                assetSelectionAdapter.notifyDataSetChanged();
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
                assetSelectionAdapter = null;
            }
        });
        v.findViewById(R.id.add_asset_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                assetSelectionAdapter = null;
            }
        });
    }

    private void initAssetGrid(GridView gridView,int viewType)
    {
        assetSelectionAdapter = new AssetSelectionAdapter(mContext, db, gridView,this.addToDownloadManager,this.downloadManager);
        assetSelectionAdapter.assetsDBs = db.getAllModelDetail((viewType == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[0]);
        gridView.setAdapter(assetSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }
}