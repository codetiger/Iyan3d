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
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.DownloadHelper;

import java.util.concurrent.TimeUnit;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AssetSelection implements AdapterView.OnItemSelectedListener {

    public AssetSelectionAdapter assetSelectionAdapter;
    public Spinner category;
    private Context mContext;
    private DatabaseHelper db;
    private String[] modelCategory;
    private int[] modelType = {0, 1, 2, 3, 5, 6, -1};

    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int PARTICLE = 13;
    private ViewGroup insertPoint;
    private View view;

    public AssetSelection(Context context, DatabaseHelper db, AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager) {
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    public void showAssetSelection(int viewType) {
        HitScreens.AssetsSelectionView(mContext);
        addToDownloadManager.lastErrorShowingTime = 0;

        modelCategory = new String[]{mContext.getString(R.string.allmodels), mContext.getString(R.string.characters), mContext.getString(R.string.backgrounds),
                mContext.getString(R.string.accessories), mContext.getString(R.string.minecraft),
                "FNAF", mContext.getString(R.string.my_library)};

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
        view = vi.inflate(R.layout.asset_view, insertPoint, false);
        insertPoint.addView(view, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        GridView gridView = (GridView) view.findViewById(R.id.asset_grid);
        initAssetGrid(gridView, viewType);
        Button cancel = (Button) view.findViewById(R.id.cancel_assetView);

        category = (Spinner) view.findViewById(R.id.model_categary);
        category.setVisibility((viewType == Constants.PARTICLE_VIEW) ? View.INVISIBLE : View.VISIBLE);
        ArrayAdapter<String> adapter_state = new ArrayAdapter<String>(mContext, R.layout.spinner_cell, modelCategory);
        adapter_state.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        category.setAdapter(adapter_state);
        category.setOnItemSelectedListener(this);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                downloadManager.cancelAll();
                ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
                insertPoint.removeAllViews();
                ((EditorView) mContext).renderManager.removeTempNode();
                mContext = null;
                db = null;
                assetSelectionAdapter = null;
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
            }
        });
        view.findViewById(R.id.add_asset_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (assetSelectionAdapter.selectedAsset == -1) {
                    return;
                }
                HitScreens.EditorView(mContext);
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                ((EditorView) mContext).showOrHideToolbarView(Constants.SHOW);
                AssetsDB assetsDBs = assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset);
                assetsDBs.setIsTempNode(false);
                assetsDBs.setTexture(assetsDBs.getAssetsId() + "-cm");
                ((EditorView) mContext).renderManager.importAssets(assetsDBs);
                ((EditorView) mContext).descriptionManager.helpForFirstTimeUser(mContext);
                ((EditorView) mContext).helpDialogs.showPop(mContext);
                mContext = null;
                db = null;
                assetSelectionAdapter = null;
                Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
            }
        });
    }

    public void onSpinnerItemSelected(int position) {
        category.setSelection(position);
        assetSelectionAdapter.assetsDBs.clear();
        assetSelectionAdapter.selectedAsset = -1;
        ((EditorView) mContext).renderManager.removeTempNode();
        view.findViewById(R.id.loadingJson).setVisibility(View.INVISIBLE);
        if ((db.getAllModelDetail((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[position]) == null ||
                db.getAllModelDetail((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[position]).size() == 0) && modelType[position] != -1) {
            view.findViewById(R.id.loadingJson).setVisibility(View.VISIBLE);
            DownloadHelper assetJsonDownload = new DownloadHelper();
            assetJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/assetsDetailv5.json", db, mContext, ((EditorView) mContext).sharedPreferenceManager, Constants.ASSET_JSON);
        } else if (mContext != null && mContext != null && ((EditorView) mContext).sharedPreferenceManager != null) {
            long lastTimeInHour = TimeUnit.MILLISECONDS.toHours(((EditorView) mContext).sharedPreferenceManager.getLong(mContext, "lastAssetJsonUpdatedTime"));
            long currentTimeInHour = TimeUnit.MILLISECONDS.toHours(System.currentTimeMillis());
            int difference = (int) ((Math.abs(Math.max(lastTimeInHour, currentTimeInHour) - Math.min(lastTimeInHour, currentTimeInHour))));
            if (difference >= 5) {
                ((EditorView) mContext).sharedPreferenceManager.setData(mContext, "lastAssetJsonUpdatedTime", System.currentTimeMillis());
                ((EditorView) mContext).assetsAniamtionRegularUpdate.jsonParse("https://iyan3dapp.com/appapi/json/assetsDetailv5.json", Constants.ASSET_JSON);
            }
        }
        assetSelectionAdapter.assetsDBs = db.getAllModelDetail((Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[position]);
        if (modelType[position] == 0 && (Constants.VIEW_TYPE != Constants.PARTICLE_VIEW)) {
            for (int i = 0; i < db.getMYModelAssetCount(); i++) {
                try {
                    assetSelectionAdapter.assetsDBs.add(db.getAllMyModelDetail().get(i));
                } catch (IndexOutOfBoundsException e) {
                    e.printStackTrace();
                }
            }
        }

        if ((Constants.VIEW_TYPE != Constants.PARTICLE_VIEW) && modelType[position] == -1) {
            if (assetSelectionAdapter.assetsDBs != null && assetSelectionAdapter.assetsDBs.size() > 0)
                assetSelectionAdapter.assetsDBs.clear();
            assetSelectionAdapter.assetsDBs = db.getAllMyModelDetail();
        }
        downloadManager.cancelAll();
        assetSelectionAdapter.downloadThumbnail();
        assetSelectionAdapter.notifyDataSetChanged();
    }

    private void initAssetGrid(GridView gridView, int viewType) {
        assetSelectionAdapter = new AssetSelectionAdapter(mContext, db, gridView, this.addToDownloadManager, this.downloadManager);
        assetSelectionAdapter.downloadThumbnail();
        if (db.getAllModelDetail((viewType == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[0]) == null ||
                db.getAllModelDetail((viewType == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[0]).size() == 0) {
            DownloadHelper assetJsonDownload = new DownloadHelper();
            assetJsonDownload.jsonParse("https://iyan3dapp.com/appapi/json/assetsDetailv5.json", db, mContext, ((EditorView) mContext).sharedPreferenceManager, Constants.ASSET_JSON);
        }
        assetSelectionAdapter.assetsDBs = db.getAllModelDetail((viewType == Constants.PARTICLE_VIEW) ? PARTICLE : modelType[0]);

        for (int i = 0; ((Constants.VIEW_TYPE != Constants.PARTICLE_VIEW) && i < db.getMYModelAssetCount()); i++) {
            try {
                assetSelectionAdapter.assetsDBs.add(db.getAllMyModelDetail().get(i));
            } catch (IndexOutOfBoundsException e) {
                e.printStackTrace();
            }
        }
        gridView.setAdapter(assetSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
        assetSelectionAdapter.notifyDataSetChanged();
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        onSpinnerItemSelected(position);
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }
}
