package com.smackall.animator.common;

/**
 * Created by Sabish on 31/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.content.Context;
import android.content.res.TypedArray;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;

import smackall.animator.R;


public class ImageAdapterForHelpView extends BaseAdapter
{
    Context mycontext = null;
    int galitembg = 0;
    String type;

    public ImageAdapterForHelpView(Context c, String type)
    {
        mycontext = c;
        this.type = type;
        TypedArray typArray = mycontext.obtainStyledAttributes(R.styleable.GalleryTheme);
        galitembg = typArray.getResourceId(R.styleable.GalleryTheme_android_galleryItemBackground, 0);
        typArray.recycle();
    }

    @Override
    public int getCount()
    {
        return Constant.helpImages(type).length;
    }

    @Override
    public Object getItem(int position)
    {
        return position;
    }

    @Override
    public long getItemId(int position)
    {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {

        ImageView imageview = new ImageView(mycontext);
        //imageview.setLayoutParams(new Gallery.LayoutParams(SceneSelectionHelpView.sizeOfHelpImages.getWidth(), SceneSelectionHelpView.sizeOfHelpImages.getHeight()));
        //imageview.setAdjustViewBounds(true);
        imageview.setImageResource(Constant.helpImages(type)[position]);

        //imageview.setScaleType(ImageView.ScaleType.CENTER);
        return imageview;
    }
}
