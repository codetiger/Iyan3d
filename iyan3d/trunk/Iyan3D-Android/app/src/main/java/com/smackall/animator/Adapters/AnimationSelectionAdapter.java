package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadRequest;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
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
    private SharedPreferenceManager sp;
    public GridView gridView;
    public List<AnimDB> animDBs;
    public HashMap downloadingAnimation = new HashMap();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    public int animDownloadId = -1;
    public int  selectedId = -1;


    public AnimationSelectionAdapter(Context c,DatabaseHelper db,GridView gridView,AddToDownloadManager addToDownloadManager
            , DownloadManager downloadManager,SharedPreferenceManager sp) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.animDBs = this.db.getAllMyAnimation(4,0,"");
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
        this.sp = sp;
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
                ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
                ((ImageView) grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                importAnimation(position);
            }
        });
        return grid;
    }

    public void importAnimation(int position){
        int animId = animDBs.get(position).getAnimAssetId();
        String ext = (animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
        String url = Constants.urlForAnimation+animId+ext;
        String dest = PathManager.LocalCacheFolder+"/";
        if(!FileHelper.checkValidFilePath(PathManager.LocalAnimationFolder+"/"+animId+ext)) {
            downloadManager.cancel(animDownloadId);
            selectedId = position;
            animDownloadId = addToDownloadManager.downloadAdd(mContext, url, Integer.toString(animId) + ext, dest, DownloadRequest.Priority.HIGH, downloadManager, null, null, this);
        }
        else{
            ((EditorView)((Activity)mContext)).animationSelection.applyAnimation(position);
            if((((EditorView)((Activity)mContext)).animationSelection.animationType[((EditorView)((Activity)mContext)).animationSelection.category.getSelectedItemPosition()]) == 7)
                showOrHidePublish(position);
            else
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);

        }

    }

    public void downloadThumbnail()
    {
        for (int i = 0; i < animDBs.size(); ++i){
            if(i >= animDBs.size()) continue;
            String fileName = animDBs.get(i).getAnimAssetId()+".png";
            if(!FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder + "/" + fileName)) {
                String url = Constants.urlAnimationThumbnail + fileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingAnimation.put(addToDownloadManager.downloadAdd(mContext, url, fileName, desPath,
                        DownloadRequest.Priority.LOW, downloadManager, null, null, AnimationSelectionAdapter.this), animDBs.get(i).getAnimAssetId());
            }
        }
    }

    private void showOrHidePublish(final int position)
    {
        ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.VISIBLE);
        if(sp.getInt(((Activity)mContext),"toolbarPosition") == 1 )
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.LEFT);
        else
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.CENTER | Gravity.RIGHT);

        ((Button)((Activity)mContext).findViewById(R.id.publish)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((EditorView)((Activity)mContext)).publish.publishAnimation(position);
            }
        });

    }
}

