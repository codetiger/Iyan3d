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

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;


/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelectionAdapter extends BaseAdapter {
    private Context mContext;
    private GridView gridView;
    public File[] files;
    private String[] basicShapes = {"Cone","Cube","Cylinder","Plane","Sphere","Torus"};
    private int[] basicShapesId = {60001,60002,60003,60004,60005,60006};
    private int[] basicShapesDrawableId = {R.drawable.cone,R.drawable.cube,R.drawable.cylinder,R.drawable.plane,R.drawable.sphere,R.drawable.torus};
    private int objSelectedPosition = 0;
    public AssetsDB assetsDB = new AssetsDB();

    public OBJSelectionAdapter(Context c,GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        this.files = getFileList(this.gridView);
        return ((this.files != null && this.files.length > 0) ? files.length : 0) + ((gridView.getTag() == Constants.OBJ_MODE) ? 6 : 1);
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
            if(position == 0){
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.DefaultAssetsDir+"/white_texture.png"));
                ((TextView)grid.findViewById(R.id.assetLable)).setText("Pick Color");
            }
            else if(files[position-1].exists()) {
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.LocalThumbnailFolder+"/"+FileHelper.getFileNameFromPath(files[position-1].toString())));
                ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position-1].toString())));
            }
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                manageSelection(position);
            }
        });

        return grid;
    }

    private File[] getFileList(final GridView gridView)
    {
        FileHelper.getObjAndTextureFromCommonIyan3dPath(mContext, (int) gridView.getTag());
        final String userFolder = ((int) gridView.getTag() == Constants.OBJ_MODE) ? PathManager.LocalUserMeshFolder+"/" : PathManager.LocalImportedImageFolder+"/";
        final File f = new File(userFolder);
        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith(((int) gridView.getTag() == Constants.OBJ_MODE) ?"obj" : "png"))
                    return true;
                else
                    return false;
            }
        };
        File files[] = f.listFiles(filenameFilter);
        return files;
    }

    private void manageSelection(int position){
        if(this.gridView.getTag() == Constants.OBJ_MODE){
            objSelectedPosition = position;
            String fileName;
            if(position > 5) {
                fileName = FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position - 6].toString()));
                assetsDB.setAssetName(fileName);
                assetsDB.setTexture("white_texture");
                assetsDB.setAssetsId(123456);
                assetsDB.setx(1.0f);
                assetsDB.setY(1.0f);
                assetsDB.setZ(1.0f);
                ((EditorView) ((Activity) mContext)).renderManager.saveAsSGM(fileName, "white_texture", 123456, true, 1.0f, 1.0f, 1.0f);
            }
            else{
                assetsDB.setAssetsId(basicShapesId[position]);
                assetsDB.setAssetName(basicShapes[position]);
                assetsDB.setIsTempNode(true);
                assetsDB.setTexture("white_texture");
                assetsDB.setType(Constants.NODE_SGM);
                ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB,false);
            }
        }
        else if(this.gridView.getTag() == Constants.TEXTURE_MODE){
            if(position == 0){
                ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(this.gridView.getChildAt(0),null,Constants.TEXTURE_MODE);
                return;
            }
            else
                assetsDB.setTexture(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position-1].toString())));
        }
        importOBJ();
    }

    public void importOBJ(){
        if(this.gridView.getTag() == Constants.OBJ_MODE){
            if(objSelectedPosition > 5)
                assetsDB.setAssetsId(123456);
            else
                assetsDB.setAssetsId(basicShapesId[objSelectedPosition]);
        }
        ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB,false);
    }
}
