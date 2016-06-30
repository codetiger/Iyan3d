package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
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
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Locale;


/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelectionAdapter extends BaseAdapter {

    private static final int ID_DELETE = 0;

    private Context mContext;
    private GridView gridView;
    public File[] files;
    private String[] basicShapes = {"Cone","Cube","Cylinder","Plane","Sphere","Torus"};
    private int[] basicShapesId = {60001,60002,60003,60004,60005,60006};
    private int[] basicShapesDrawableId = {R.drawable.cone,R.drawable.cube,R.drawable.cylinder,R.drawable.plane,R.drawable.sphere,R.drawable.torus};
    private int objSelectedPosition = 0;
    private int textureSelectedPosition = 0;
    public AssetsDB assetsDB = new AssetsDB();
    int previousPosition = -1;

    public OBJSelectionAdapter(Context c,GridView gridView) {
        mContext = c;
        this.gridView = gridView;
    }

    @Override
    public int getCount() {
        this.files = getFileList(this.gridView);
        return ((this.files != null && this.files.length > 0) ? files.length : 0) + ((Integer.parseInt(gridView.getTag().toString()) == Constants.OBJ_MODE) ? 6 : 1);
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
        } else
            grid = (View)convertView;

        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                grid.getLayoutParams().height = this.gridView.getHeight()/4                                                                                                                                                                                                         ;
                break;
            default:
                grid.getLayoutParams().height = this.gridView.getHeight()/5;
                break;
        }

        ((ProgressBar) grid.findViewById(R.id.progress_bar)).setVisibility(View.INVISIBLE);
        if(position <= 5 && (Integer.parseInt(this.gridView.getTag().toString())) == Constants.OBJ_MODE){
            ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(basicShapesDrawableId[position]);
            ((TextView)grid.findViewById(R.id.assetLable)).setText(basicShapes[position]);
            grid.findViewById(R.id.props_btn).setVisibility(View.INVISIBLE);

        }
        else if(position > 5 && (Integer.parseInt(this.gridView.getTag().toString())) == Constants.OBJ_MODE ) {

            grid.findViewById(R.id.props_btn).setVisibility(View.VISIBLE);
            grid.findViewById(R.id.props_btn).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showObjModelAndTextureProps(v, position, gridView);
                }
            });

            if(files[position-6].exists()) {
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageResource(R.drawable.obj_file);
                ((TextView)grid.findViewById(R.id.assetLable)).setText(FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position-6].toString())));
            }
        }
        else if((Integer.parseInt(this.gridView.getTag().toString())) == Constants.TEXTURE_MODE){
            if(position == 0){
                ((ImageView) grid.findViewById(R.id.thumbnail)).setImageBitmap(BitmapFactory.decodeFile(PathManager.DefaultAssetsDir+"/white_texture.png"));
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
                        showObjModelAndTextureProps(v, position, gridView);
                    }
                });
            }
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
        if((Integer.parseInt(this.gridView.getTag().toString())) == Constants.OBJ_MODE){
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
                ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
                assetsDB.setAssetsId(basicShapesId[position]);
                assetsDB.setAssetName(basicShapes[position]);
                assetsDB.setIsTempNode(true);
                assetsDB.setTexture("white_texture");
                assetsDB.setType(Constants.NODE_SGM);
                ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB,false);
            }
        }
        else if((Integer.parseInt(this.gridView.getTag().toString())) == Constants.TEXTURE_MODE){
            textureSelectedPosition = position;
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
        ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
        if((Integer.parseInt(this.gridView.getTag().toString())) == Constants.OBJ_MODE){
            if(objSelectedPosition > 5)
                assetsDB.setAssetsId(123456);
            else
                assetsDB.setAssetsId(basicShapesId[objSelectedPosition]);
        }
        ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB,false);
        ((EditorView) ((Activity) mContext)).objSelection.modelImported = true;
    }

    private void showObjModelAndTextureProps(View view, final int position, final GridView gridView) {

        final PopupMenu popup = new PopupMenu(mContext, view);
        popup.getMenuInflater().inflate(R.menu.my_model_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case ID_DELETE:
                        String ext = ((Integer.parseInt(gridView.getTag().toString())) == Constants.OBJ_MODE) ? ".obj" : ".png";
                        String fileName = FileHelper.getFileWithoutExt(FileHelper.getFileNameFromPath(files[position - ((((Integer.parseInt(gridView.getTag().toString())) == Constants.OBJ_MODE)) ? 6 : 1)].toString()));

                        if ((Integer.parseInt(gridView.getTag().toString())) == Constants.TEXTURE_MODE) {
                            FileHelper.deleteFilesAndFolder(PathManager.LocalImportedImageFolder + "/" + fileName + ext);
                            FileHelper.deleteFilesAndFolder(PathManager.LocalThumbnailFolder + "/" + fileName + ext);
                            if (textureSelectedPosition == position) {
                                assetsDB.setTexture("white_texture");
                                ((EditorView) ((Activity) mContext)).renderManager.importAssets(assetsDB, false);
                                ((EditorView) ((Activity) mContext)).objSelection.modelImported = true;
                            }
                        } else if (((Integer.parseInt(gridView.getTag().toString())) == Constants.OBJ_MODE)) {
                            FileHelper.deleteFilesAndFolder(PathManager.LocalUserMeshFolder + "/" + fileName + ext);
                            if (objSelectedPosition == position) {
                                ((EditorView) mContext).renderManager.removeTempNode();
                                ((EditorView) ((Activity) mContext)).objSelection.modelImported = false;
                            }
                        }

                        files = null;
                        notifyDataSetChanged();
                        break;
                }
                return true;
            }
        });
        popup.show();
    }
}
