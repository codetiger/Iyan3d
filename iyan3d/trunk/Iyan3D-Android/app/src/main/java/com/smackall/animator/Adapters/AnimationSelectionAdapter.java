package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.support.v4.content.ContextCompat;
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
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.opengl.GL2JNILib;

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
    public HashMap queueList = new HashMap();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    public int animDownloadId = -1;
    public int  selectedId = -1;
    public int previousPosition = -1;

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
        }        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
        ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.INVISIBLE);

        if(position >= animDBs.size())
            return grid;

        ((TextView) grid.findViewById(R.id.assetLable)).setText(animDBs.get(position).getAnimName());
        String fileName = animDBs.get(position).getAnimAssetId()+".png";

        if(FileHelper.checkValidFilePath(PathManager.LocalThumbnailFolder + "/" + fileName)){
            ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);
            ((ImageView)grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + fileName));
            if(queueList!= null && queueList.containsKey(position)){queueList.remove(position);}
        } else {
            if(queueList != null && !queueList.containsKey(position)){
                String thumpnailName = animDBs.get(position).getAnimAssetId()+".png";
                    String url = GL2JNILib.AnimationThumbnail() + thumpnailName;
                    String desPath = PathManager.LocalCacheFolder + "/";
                    addToDownloadManager.lastErrorShowingTime = 0;
                    downloadingAnimation.put(addToDownloadManager.downloadAdd(mContext, url, thumpnailName, desPath,
                            DownloadRequest.Priority.LOW, downloadManager, null, null, AnimationSelectionAdapter.this,null), animDBs.get(position).getAnimAssetId());
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
                if(!((EditorView)((Activity)mContext)).animationSelection.processCompleted) return;
                ((EditorView)mContext).showOrHideLoading(Constants.SHOW);
                previousPosition = position;
                notifyDataSetChanged();
                GL2JNILib.setIsPlaying(false,null);
                importAnimation(position);
            }
        });
        return grid;
    }

    public void importAnimation(int position){
        int animId = animDBs.get(position).getAnimAssetId();
        String ext = (animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
        String url = GL2JNILib.Animation()+animId+ext;
        String dest = PathManager.LocalCacheFolder+"/";
        if(!FileHelper.checkValidFilePath(PathManager.LocalAnimationFolder+"/"+animId+ext)) {
            downloadManager.cancel(animDownloadId);
            selectedId = position;
            animDownloadId = addToDownloadManager.downloadAdd(mContext, url, Integer.toString(animId) + ext, dest, DownloadRequest.Priority.HIGH, downloadManager, null, null, this,null);
        }
        else{
            ((EditorView)((Activity)mContext)).animationSelection.processCompleted = false;
            ((EditorView)((Activity)mContext)).animationSelection.applyAnimation(position);
            if((((EditorView)((Activity)mContext)).animationSelection.animationType[((EditorView)((Activity)mContext)).animationSelection.category.getSelectedItemPosition()]) == 7
                    && animDBs.get(position).getpublishedId() <= 0)
                showOrHidePublish(position);
            else
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);

        }

    }

    public void downloadThumbnail()
    {
        if(downloadingAnimation != null)
            downloadingAnimation.clear();
        else
            downloadingAnimation = new HashMap();
        if(queueList != null)
            queueList.clear();
        else
            queueList = new HashMap();
    }

    private void showOrHidePublish(final int position)
    {
        ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.VISIBLE);
        if(sp.getInt(((Activity)mContext),"toolbarPosition") == 1 )
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.END);
        else
            ((LinearLayout)((ViewGroup)((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).getChildAt(0)).getChildAt(0)).setGravity(Gravity.START);

        ((Button)((Activity)mContext).findViewById(R.id.publish)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(((EditorView)mContext).userDetails.signInType <= 0){
                    UIHelper.showSignInPanelWithMessage(mContext,"Please sign in to save your animation.");
                    return;
                }
                ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
                ((EditorView)((Activity)mContext)).publish.publishAnimation(position);
            }
        });
    }
}

