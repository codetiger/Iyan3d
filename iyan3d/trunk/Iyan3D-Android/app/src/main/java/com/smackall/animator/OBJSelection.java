package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;

import com.smackall.animator.Adapters.OBJSelectionAdapter;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

import java.io.File;
import java.io.FilenameFilter;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OBJSelection {

    Context mContext;
    DatabaseHelper db;
    OBJSelectionAdapter objSelectionAdapter;
    ViewGroup insertPoint;
    public OBJSelection(Context context,DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    public void showObjSelection(final int ViewMode)
    {
        ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView)((Activity)mContext)).sharedPreferenceManager.getInt(mContext,"toolbarPosition") == 1 ) ?
                (ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity)mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup)insertPoint.getParent()).getChildCount(); i++){
            if(((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup)insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = vi.inflate(R.layout.obj_view,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        final TextView infoLable = (TextView)v.findViewById(R.id.info_lable);
        final GridView gridView = (GridView)v.findViewById(R.id.obj_grid);
        gridView.setTag(ViewMode);
        infoLable.setText((gridView.getTag() == Constants.OBJ_MODE) ? "Add OBJ files in SD-Card/Iyan3D foler." : "Add Texture files in SD-Card/Iyan3D folder.");
        initAssetGrid(gridView);
        Button cancel = (Button)v.findViewById(R.id.cancel_obj);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                insertPoint.removeAllViews();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                objSelectionAdapter = null;
            }
        });
        v.findViewById(R.id.next_obj).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(gridView.getTag() == Constants.OBJ_MODE) {
                    gridView.setTag(Constants.TEXTURE_MODE);
                    ((Button)v).setText("Add To Scene");
                    objSelectionAdapter.files = null;
                    objSelectionAdapter.files = getFileList(gridView);
                    objSelectionAdapter.notifyDataSetChanged();
                    infoLable.setText((gridView.getTag() == Constants.OBJ_MODE) ? "Add OBJ files in SD-Card/Iyan3D foler." : "Add Texture files in SD-Card/Iyan3D folder.");
                }
                else {
                    insertPoint.removeAllViews();
                    ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.SHOW);
                    mContext = null;
                    db = null;
                    objSelectionAdapter = null;
                }
            }
        });
    }

    private void initAssetGrid(GridView gridView)
    {
        objSelectionAdapter = new OBJSelectionAdapter(mContext,gridView);
        objSelectionAdapter.files = getFileList(gridView);
        gridView.setAdapter(objSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    private File[] getFileList(final GridView gridView)
    {
        FileHelper.getObjAndTextureFromCommonIyan3dPath(mContext, (int) gridView.getTag());
        final String userFolder = ((int) gridView.getTag() == Constants.OBJ_MODE) ? PathManager.LocalUserMeshFolder+"/" : PathManager.LocalUserMeshFolder+"/";
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
}
