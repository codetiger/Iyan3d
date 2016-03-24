package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.Typeface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
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
public class TextSelectionAdapter extends BaseAdapter {

    private Context mContext;
    private DatabaseHelper db;
    private GridView gridView;
    public List<AssetsDB> assetsDBs;
    public HashMap<Integer, String> downloadingFonts = new HashMap<Integer, String>();
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    public TextSelectionAdapter(Context c,DatabaseHelper db,GridView gridView,AddToDownloadManager addToDownloadManager,DownloadManager downloadManager) {
        mContext = c;
        this.db = db;
        this.gridView = gridView;
        this.assetsDBs = this.db.getAllModelDetail(50);
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    @Override
    public int getCount() {
        return (assetsDBs != null && assetsDBs.size() > 0 ) ? assetsDBs.size() : 0;
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
            grid = new View(mContext);
            LayoutInflater inflater=((Activity)mContext).getLayoutInflater();
            grid=inflater.inflate(R.layout.text_cell, parent, false);
        }else{
            grid = (View)convertView;
        }
        grid.getLayoutParams().height = this.gridView.getHeight()/4;
        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
        ((TextView)grid.findViewById(R.id.display_text)).setVisibility(View.INVISIBLE);

        String DisplayName = assetsDBs.get(position).getAssetName();
        ((TextView)grid.findViewById(R.id.lable_text)).setText(FileHelper.getFileWithoutExt(DisplayName));
        String downloadFileName = assetsDBs.get(position).getAssetsId()+"."+FileHelper.getFileExt(DisplayName);

        if(FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" +downloadFileName)){
            ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
            ((TextView)grid.findViewById(R.id.display_text)).setVisibility(View.VISIBLE);
            Typeface fontType = Typeface.createFromFile(PathManager.LocalFontsFolder+"/"+downloadFileName);
            ((TextView)grid.findViewById(R.id.display_text)).setText("TEXT");
            ((TextView)grid.findViewById(R.id.display_text)).setTypeface(fontType);
        } else {
                ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.VISIBLE);
                ((TextView)grid.findViewById(R.id.display_text)).setVisibility(View.INVISIBLE);
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                importText(assetsDBs.get(position).getAssetsId()+"."+FileHelper.getFileExt(assetsDBs.get(position).getAssetName()));
            }
        });
        return grid;
    }

    private void importText(String file){
        ((EditorView)((Activity)mContext)).textSelection.textDB.setFilePath(file);
        ((EditorView)((Activity)mContext)).textSelection.importText();
    }

    public void downloadFonts()
    {
        for (int i = 0; i < assetsDBs.size(); ++i){
            if(i >= assetsDBs.size()) continue;
            String DisplayName = assetsDBs.get(i).getAssetName();
            String downloadFileName = assetsDBs.get(i).getAssetsId()+"."+FileHelper.getFileExt(DisplayName);
            if(!FileHelper.checkValidFilePath(PathManager.LocalFontsFolder + "/" +downloadFileName)){
                String url = Constants.urlFont + downloadFileName;
                String desPath = PathManager.LocalCacheFolder + "/";
                downloadingFonts.put(addToDownloadManager.downloadAdd(mContext, url, downloadFileName, desPath,
                        DownloadRequest.Priority.HIGH, downloadManager, null, TextSelectionAdapter.this,null), downloadFileName);
            }
        }
    }
}

