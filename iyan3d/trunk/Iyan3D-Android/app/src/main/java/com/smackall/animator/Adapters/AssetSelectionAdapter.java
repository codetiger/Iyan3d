package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadRequest;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.R;

import java.util.HashMap;
import java.util.List;


/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AssetSelectionAdapter extends BaseAdapter {

    private Context mContext;
    private DatabaseHelper db;
    public GridView gridView;
    public List<AssetsDB> assetsDBs;
    public int meshDownloadId = -1;
    public int textureDownloadId = -1;
    public HashMap downloadingAssets = new HashMap();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int downloadLimit = 0;

    public AssetSelectionAdapter(Context c,DatabaseHelper db,GridView gridView,AddToDownloadManager addToDownloadManager
    , DownloadManager downloadManager) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.assetsDBs = this.db.getAllModelDetail(0);
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    @Override
    public int getCount() {
        return (assetsDBs != null && assetsDBs.size() > 0) ? assetsDBs.size() : 0;
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }



    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        View grid;
        if(convertView==null){
            LayoutInflater inflater=((Activity)mContext).getLayoutInflater();
            grid=inflater.inflate(R.layout.asset_cell, parent, false);

        }else{
            grid = (View)convertView;
        }
        grid.getLayoutParams().height = this.gridView.getHeight()/5;
        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
        ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);

        if(position >= assetsDBs.size())
            return grid;

        ((TextView) grid.findViewById(R.id.assetLable)).setText(assetsDBs.get(position).getAssetName());
        String fileName = assetsDBs.get(position).getAssetsId()+".png";
        if(FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder+"/"+fileName)){
            ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + fileName));
        } else {
            if(position == downloadLimit) {
                ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
                ((ImageView) grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);
                String url = Constants.urlMeshThumbnail + fileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAssets.put(addToDownloadManager.downloadAdd(mContext, url, fileName, desPath,
                        DownloadRequest.Priority.HIGH, downloadManager, AssetSelectionAdapter.this, null,null), assetsDBs.get(position).getAssetsId());
                downloadLimit++;
            }
        }
        return grid;
    }
}

