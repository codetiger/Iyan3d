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
import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.R;

import java.util.HashMap;
import java.util.List;


/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationSelectionAdapter extends BaseAdapter {
    private Context mContext;
    private DatabaseHelper db;
    public GridView gridView;
    public List<AnimDB> animDBs;
    public HashMap downloadingAnimation = new HashMap();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    private int downloadLimit = 0;
    public int animDownloadId = -1;

    public AnimationSelectionAdapter(Context c,DatabaseHelper db,GridView gridView,AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.animDBs = this.db.getAllMyAnimation(4,1,"");
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    @Override
    public int getCount() {
        return (animDBs == null || animDBs.size() == 0) ? 0 : animDBs.size();
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

        if(position >= animDBs.size())
            return grid;

        ((TextView) grid.findViewById(R.id.assetLable)).setText(animDBs.get(position).getAnimName());
        String fileName = animDBs.get(position).getAnimAssetId()+".png";

        if(FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder + "/" + fileName)){
            ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + fileName));
        } else {
            if(position == downloadLimit) {
                ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
                ((ImageView) grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);
                String url = Constants.urlAnimationThumbnail + fileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAnimation.put(addToDownloadManager.downloadAdd(mContext, url, fileName, desPath,
                        DownloadRequest.Priority.HIGH, downloadManager, null, null,AnimationSelectionAdapter.this), animDBs.get(position).getAnimAssetId());
                downloadLimit++;
            }
        }
        return grid;
    }
}

