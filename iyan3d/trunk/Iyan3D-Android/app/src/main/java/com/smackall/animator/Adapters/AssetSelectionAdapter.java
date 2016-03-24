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
import com.smackall.animator.EditorView;
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
    public int selectedAsset = -1;

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
        if(assetsDBs.get(position).getAssetsId() >= 50000 && assetsDBs.get(position).getAssetsId() < 60000){
            String ext = ".json";
            String particleName = assetsDBs.get(position).getAssetsId()+ext;
            String url = Constants.urlForParticle+particleName;
            String desPath = PathManager.LocalMeshFolder+"/";
            if(!FileHelper.checkValidFilePath(PathManager.LocalMeshFolder+"/"+particleName))
                addToDownloadManager.downloadAdd(mContext, url, particleName, desPath,DownloadRequest.Priority.HIGH, downloadManager, AssetSelectionAdapter.this, null,null);
        }
        if(FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder+"/"+fileName)){
            ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + fileName));
        } else {
            ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
            ((ImageView) grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                selectedAsset = position;
                importAsset();
            }
        });
        return grid;
    }

    private void importAsset(){
        final AssetsDB assetsDB = assetsDBs.get(selectedAsset);
        String ext = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String mesh = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + ext;
        String texture = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + "-cm.png";
        if(!FileHelper.checkValidFilePath(mesh) || !FileHelper.checkValidFilePath(texture)){
            download(assetsDB);
        }
        else {
            assetsDB.setIsTempNode(true);
            assetsDB.setTexture(assetsDB.getAssetsId() + "-cm");
            ((EditorView)(Activity)mContext).renderManager.importAssets(assetsDB,false);
        }
    }

    private void download(AssetsDB assetsDB)
    {
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        String meshExt = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String textureExt = ".png";
        downloadManager.cancel(meshDownloadId);
        downloadManager.cancel(textureDownloadId);
        String mesh = Constants.urlForMesh+assetsDB.getAssetsId()+meshExt;
        String texture = Constants.urlForTexture+assetsDB.getAssetsId()+textureExt;
        String fileName = Integer.toString(assetsDB.getAssetsId()) + meshExt;
        String texturefileName = Integer.toString(assetsDB.getAssetsId()) + "-cm"+textureExt;
        String desPath = PathManager.LocalCacheFolder+"/";
        meshDownloadId = addToDownloadManager.downloadAdd(mContext,mesh,fileName,desPath, DownloadRequest.Priority.HIGH,downloadManager,this,null,null);
        textureDownloadId = addToDownloadManager.downloadAdd(mContext,texture,texturefileName,desPath, DownloadRequest.Priority.HIGH,downloadManager,this,null,null);
    }

    public void downloadThumbnail() {
        for (int i = 0; i < assetsDBs.size(); i++) {
            if (i >= assetsDBs.size()) return;
            String fileName = assetsDBs.get(i).getAssetsId() + ".png";
            if (!FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder + "/" + fileName)) {
                String url = Constants.urlMeshThumbnail + fileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAssets.put(addToDownloadManager.downloadAdd(mContext, url, fileName, desPath,
                        DownloadRequest.Priority.LOW, downloadManager, AssetSelectionAdapter.this, null, null), assetsDBs.get(i).getAssetsId());
            }
        }
    }
}

