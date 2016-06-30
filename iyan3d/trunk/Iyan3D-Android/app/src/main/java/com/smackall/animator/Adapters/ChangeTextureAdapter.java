package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.PopupMenu;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Locale;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ChangeTextureAdapter extends BaseAdapter {

    private final static int ID_DELETE = 0;

    private Context mContext;
    private GridView gridView;
    public File[] files;
    int previousPosition = -1;
    public ChangeTextureAdapter(Context c,GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        this.files = getFileList();
        return ((this.files != null && this.files.length > 0) ? files.length : 0) +  1;
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
        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
        ((ImageView)grid.findViewById(R.id.thumbnail)).setVisibility(View.VISIBLE);

            if(position == 0){
                ((ImageView) grid.findViewById(R.id.thumbnail)).setBackgroundColor(Color.argb(255,(int)((EditorView)((Activity)mContext)).textureSelection.assetsDB.getX()*255
                ,(int)((EditorView)((Activity)mContext)).textureSelection.assetsDB.getY()*255,(int)((EditorView)((Activity)mContext)).textureSelection.assetsDB.getZ()*255));
                ((TextView)grid.findViewById(R.id.assetLable)).setText(String.format(Locale.getDefault(),"%s","Pick Color"));
                grid.findViewById(R.id.props_btn).setVisibility(View.INVISIBLE);
            }
            else if(files[position-1].exists()) {
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder+"/"+FileHelper.getFileNameFromPath(files[position-1].toString())));
                ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position-1].toString())));
                grid.findViewById(R.id.props_btn).setVisibility(View.VISIBLE);
                grid.findViewById(R.id.props_btn).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        showTexturePropsMenu(v, position);
                    }
                });
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
                if(position == 0)
                    ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(gridView.getChildAt(0),null,Constants.CHANGE_TEXTURE_MODE);
                else {
                    ((EditorView) ((Activity) mContext)).textureSelection.assetsDB.setTexture(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position - 1].toString())));
                    ((EditorView) ((Activity) mContext)).textureSelection.assetsDB.setIsTempNode(true);
                    ((EditorView) ((Activity) mContext)).textureSelection.changeTexture();
                }
            }
        });

        return grid;
    }

    private File[] getFileList()
    {
        final String userFolder = PathManager.LocalImportedImageFolder+"/";
        final File f = new File(userFolder);
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override                                                                                                                               
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith("png"))
                    return true;
                else
                    return false;
            }
        };
        File files[] = f.listFiles(filenameFilter);
        return files;
    }

    private void showTexturePropsMenu(View view, final int position) {

        final PopupMenu popup = new PopupMenu(mContext, view);
        popup.getMenuInflater().inflate(R.menu.my_model_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case ID_DELETE:
                        FileHelper.deleteFilesAndFolder(files[position - 1]);
                        if (previousPosition == position) {
                            ((EditorView) mContext).renderManager.removeTempTexture(((EditorView) mContext).textureSelection.selectedNodeid);
                            ((EditorView) mContext).textureSelection.assetsDB.setTexture("");
                        }
                        notifyDataSetChanged();
                        break;
                }
                return true;
            }
        });
        popup.show();
    }
}
