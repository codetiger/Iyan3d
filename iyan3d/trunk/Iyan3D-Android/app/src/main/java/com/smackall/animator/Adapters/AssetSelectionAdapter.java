package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.support.v4.content.ContextCompat;
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
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

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
    public int jsonDownloadId = -1;
    public HashMap downloadingAssets = new HashMap();
    public HashMap queueList = new HashMap();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    public int selectedAsset = -1;
    int previousPosition = -1;

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
        final View grid;
        if(convertView==null){
            LayoutInflater inflater=((Activity)mContext).getLayoutInflater();
            grid=inflater.inflate(R.layout.asset_cell, parent, false);
        }else{
            grid = (View)convertView;
        }

        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight()/4                                                                                                                                                                                                         ;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight()/5;
                break;
        }
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
            if(queueList != null && queueList.containsKey(position)){
                queueList.remove(position);
            }
        } else {
            if(queueList != null && !queueList.containsKey(position)){
                String thumpnailName = assetsDBs.get(position).getAssetsId() + ".png";
                String url = GL2JNILib.MeshThumbnail() + thumpnailName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAssets.put(addToDownloadManager.downloadAdd(mContext, url, thumpnailName, desPath,
                        DownloadRequest.Priority.LOW, downloadManager, AssetSelectionAdapter.this, null, null,null), assetsDBs.get(position).getAssetsId());
                queueList.put(position,"1");
            }
            ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
            ((ImageView) grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);
        }

        grid.setBackgroundResource(0);
        grid.setBackgroundColor(ContextCompat.getColor(mContext,R.color.cellBg));
        if(previousPosition != -1 && position == previousPosition) {
            grid.setBackgroundResource(R.drawable.cell_highlight);
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                previousPosition = position;
                notifyDataSetChanged();
                selectedAsset = position;
                importAsset();
            }
        });
        return grid;
    }

    private void importAsset(){
        if(assetsDBs == null || assetsDBs.size() <= selectedAsset) return;

        final AssetsDB assetsDB = assetsDBs.get(selectedAsset);
        String ext = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String mesh = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + ext;
        String texture = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + "-cm.png";

        String particleExt = ".json";
        String particleJson = assetsDB.getAssetsId()+particleExt;

        if(!FileHelper.checkValidFilePath(mesh) || !FileHelper.checkValidFilePath(texture)){
            download(assetsDB);
            if((assetsDB.getAssetsId() >= 50000 && assetsDB.getAssetsId() < 60000) && !FileHelper.checkValidFilePath(PathManager.LocalMeshFolder+"/"+particleJson)){
                String desPath = PathManager.LocalMeshFolder+"/";
                String url = GL2JNILib.Particle()+particleJson;
                addToDownloadManager.lastErrorShowingTime = 0;
                downloadManager.cancel(jsonDownloadId);
                jsonDownloadId = addToDownloadManager.downloadAdd(mContext, url, particleJson, desPath,DownloadRequest.Priority.HIGH, downloadManager, AssetSelectionAdapter.this, null,null,particleJson);
                return;
            }
        }
        else {
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
            assetsDB.setIsTempNode(true);
            assetsDB.setTexture(assetsDB.getAssetsId() + "-cm");
            ((EditorView)(Activity)mContext).renderManager.importAssets(assetsDB,false);
        }
    }

    private void download(AssetsDB assetsDB)
    {
        if(mContext == null || downloadManager == null || assetsDB == null || addToDownloadManager == null) return;
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        String meshExt = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String textureExt = ".png";
        downloadManager.cancel(meshDownloadId);
        downloadManager.cancel(textureDownloadId);
        String mesh = GL2JNILib.Mesh()+assetsDB.getAssetsId()+meshExt;
        String texture = GL2JNILib.Texture()+assetsDB.getAssetsId()+textureExt;
        String fileName = Integer.toString(assetsDB.getAssetsId()) + meshExt;
        String textureFileName = Integer.toString(assetsDB.getAssetsId()) + "-cm"+textureExt;
        String desPath = PathManager.LocalCacheFolder+"/";
        meshDownloadId = addToDownloadManager.downloadAdd(mContext,mesh,fileName,desPath, DownloadRequest.Priority.HIGH,downloadManager,this,null,null,null);
        addToDownloadManager.lastErrorShowingTime = 0;
        textureDownloadId = addToDownloadManager.downloadAdd(mContext,texture,textureFileName,desPath, DownloadRequest.Priority.HIGH,downloadManager,this,null,null,null);
    }

    public void downloadThumbnail() {
        if(downloadingAssets != null)
            downloadingAssets.clear();
        else
            downloadingAssets = new HashMap();
        if(queueList != null)
            queueList.clear();
        else
            queueList = new HashMap();
    }
}

