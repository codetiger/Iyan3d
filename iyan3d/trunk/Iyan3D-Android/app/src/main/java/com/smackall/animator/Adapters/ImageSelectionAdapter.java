package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImageSelectionAdapter extends BaseAdapter {

    private GridView gridView;
    private int previousPosition = -1;
    private Context mContext;
    private File[] images = null;

    public ImageSelectionAdapter(Context c, GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        getFileList();
        return (images == null) ? 0 : images.length;
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
        if (convertView == null) {
            LayoutInflater inflater = ((Activity) mContext).getLayoutInflater();
            grid = inflater.inflate(R.layout.asset_cell, parent, false);

        } else {
            grid = convertView;
        }
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight() / 4;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight() / 5;
                break;
        }
        grid.findViewById(R.id.progress_bar).setVisibility(View.INVISIBLE);
        grid.findViewById(R.id.thumbnail).setVisibility(View.VISIBLE);
        grid.findViewById(R.id.assetLable).setVisibility(View.VISIBLE);
        ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/" + FileHelper.getFileNameFromPath(images[position].toString())));
        ((TextView) grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(images[position]));

        grid.setBackgroundResource(0);
        grid.setBackgroundColor(ContextCompat.getColor(mContext, R.color.cellBg));
        if (previousPosition != -1 && position == previousPosition) {
            grid.setBackgroundResource(R.drawable.cell_highlight);
        }
        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                previousPosition = position;
                notifyDataSetChanged();
                importImage(images[position]);
            }
        });
        if (position == images.length - 1)
            ((EditorView) mContext).showOrHideLoading(Constants.HIDE);
        return grid;
    }

    private void getFileList() {
        final File f = new File(PathManager.LocalImportedImageFolder + "/");
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.toLowerCase().endsWith("png");
            }
        };
        images = f.listFiles(filenameFilter);
    }

    private void importImage(File path) {
        if (((EditorView) mContext).imageSelection == null) return;
        ((EditorView) mContext).showOrHideLoading(Constants.SHOW);
        Bitmap bmp = BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder + "/original" + FileHelper.getFileNameFromPath(path.toString()));
        if (bmp == null) return;
        ((EditorView) mContext).imageSelection.imageDB.setTempNode(true);
        ((EditorView) mContext).imageSelection.imageDB.setAssetAddType(0);
        ((EditorView) mContext).imageSelection.imageDB.setHeight(bmp.getHeight());
        ((EditorView) mContext).imageSelection.imageDB.setWidth(bmp.getWidth());
        ((EditorView) mContext).imageSelection.imageDB.setName(FileHelper.getFileWithoutExt(path));
        ((EditorView) mContext).imageSelection.imageDB.setNodeType(Constants.NODE_IMAGE);
        ((EditorView) mContext).imageSelection.importImage();
    }
}

