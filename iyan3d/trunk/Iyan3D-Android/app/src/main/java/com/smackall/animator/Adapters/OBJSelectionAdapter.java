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

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.R;

import java.io.File;


/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelectionAdapter extends BaseAdapter {
    private Context mContext;
    private GridView gridView;
    public File[] files;
    private String[] basicShapes = {"Cone","Cube","Cylinder","Plane","Sphere","Torus"};
    private int[] basicShapesId = {600001,600002,600003,600004,600005,600006};
    private int[] basicShapesDrawableId = {R.drawable.cone,R.drawable.cube,R.drawable.cylinder,R.drawable.plane,R.drawable.sphere,R.drawable.torus};


    public OBJSelectionAdapter(Context c,GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        return ((this.files != null && this.files.length > 0) ? files.length : 0) + ((gridView.getTag() == Constants.OBJ_MODE) ? 6 : 0);
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
        ((ProgressBar)grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);

        if(position <= 5 && this.gridView.getTag() == Constants.OBJ_MODE){
            ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(basicShapesDrawableId[position]);
            ((TextView)grid.findViewById(R.id.assetLable)).setText(basicShapes[position]);
        }
        else if(position > 5 && this.gridView.getTag() == Constants.OBJ_MODE ) {
            if(files[position-6].exists()) {
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(R.drawable.obj_file);
                ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position-6].toString())));
            }
        }
        else if(this.gridView.getTag() == Constants.TEXTURE_MODE){
            if(files[position].exists()) {
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(files[position].toString()));
                ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position].toString())));
            }
        }
        return grid;
    }
}
