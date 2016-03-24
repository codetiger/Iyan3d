package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridView;

import com.smackall.animator.Adapters.ImageSelectionAdapter;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.ImageDB;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImageSelection {

    private Context mContext;
    private ImageSelectionAdapter imageSelectionAdapter;
    ViewGroup insertPoint;
    public ImageDB imageDB = new ImageDB();


    public ImageSelection(Context context){
        this.mContext = context;
    }

    public void showImageSelection()
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
        View v = vi.inflate(R.layout.image_selection,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
        GridView gridView = (GridView)v.findViewById(R.id.image_grid);
        initImagesGrid(gridView);
        Button cancel = (Button)v.findViewById(R.id.cancel_image);
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                insertPoint.removeAllViews();
                ((EditorView) ((Activity) mContext)).renderManager.removeTempNode();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                imageSelectionAdapter = null;
            }
        });
        v.findViewById(R.id.add_image).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                imageDB.setTempNode(false);
                importImage();
                insertPoint.removeAllViews();
                ((EditorView) ((Activity) mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                imageSelectionAdapter = null;
            }
        });
        v.findViewById(R.id.import_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((EditorView)((Activity)mContext)).imageManager.getImageFromStorage(Constants.IMPORT_IMAGES);
            }
        });
    }

    private void initImagesGrid(GridView gridView)
    {
        imageSelectionAdapter = new ImageSelectionAdapter(mContext,gridView);
        gridView.setAdapter(imageSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
    }

    public void notifyDataChanged(){
        imageSelectionAdapter.notifyDataSetChanged();
    }

    public void importImage(){
        ((EditorView)((Activity)mContext)).renderManager.importImage(imageDB);
    }
}
