package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.media.MediaMetadataRetriever;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;

/**
 * Created by Sabish.M on 19/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class VideoSelectionAdapter extends BaseAdapter{


    private Context mContext;
    public GridView gridView;
    private File[] videos = null;
    int previousPosition = -1;
    public VideoSelectionAdapter(Context c,GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        getFileList();
        return (videos == null) ? 0 : videos.length;
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
    public View getView(final int position, View convertView, final ViewGroup parent) {
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
        }        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
        ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);
        ((TextView)grid.findViewById(R.id.assetLable)).setVisibility(View.VISIBLE);
        ((ImageView)grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + FileHelper.getFileWithoutExt(videos[position].toString()) + ".png"));
        ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(videos[position]));

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
                importVideo(position                                                                                                                                                                                                                                            );
            }
        });
        return grid;
    }

    private void importVideo(int position)
    {
        String path = videos[position].getAbsolutePath();
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        retriever.setDataSource(path);
        int width = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH));
        int height = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT));
        retriever.release();
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setTempNode(true);// Video Restricted
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setAssetAddType(0);
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setWidth(width);
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setHeight(height);
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setName(FileHelper.getFileWithoutExt(path));
//        ((EditorView)((Activity)mContext)).videoSelection.videoDB.setNodeType(Constants.NODE_VIDEO);
//        ((EditorView)((Activity)mContext)).videoSelection.importVideo();
    }

    private void getFileList()
    {
        final File f = new File(PathManager.LocalUserVideoFolder+"/");
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith("mp4"))
                    return true;
                else
                    return false;
            }
        };
        videos = f.listFiles(filenameFilter);
    }
}
