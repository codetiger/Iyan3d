package com.smackall.animator.Adapters;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImageSelectionAdapter extends BaseAdapter {

    private Context context;
    private Cursor cursor;
    private int columnIndex;
    int imageID;

    public ImageSelectionAdapter(Context localContext,Cursor cursor,int columnIndex) {
        context = localContext;
        this.cursor = cursor;
        this.columnIndex = columnIndex;
    }

    public int getCount() {
        return cursor.getCount();
    }
    public Object getItem(int position) {
        return position;
    }
    public long getItemId(int position) {
        return position;
    }
    public View getView(final int position, View convertView, ViewGroup parent) {
        ImageView picturesView;
        if (convertView == null) {
            picturesView = new ImageView(context);
            // Move cursor to current position
            cursor.moveToPosition(position);
            // Get the current value for the requested column
            imageID = cursor.getInt(columnIndex);
            // Set the content of the image based on the provided URI
            picturesView.setImageURI(Uri.withAppendedPath(
                    MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "" + imageID));
            picturesView.setScaleType(ImageView.ScaleType.FIT_XY);
            picturesView.setPadding(10, 10, 10, 10);
            picturesView.setLayoutParams(new GridView.LayoutParams(100, 100));
        }
        else {
            picturesView = (ImageView)convertView;
        }

        picturesView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                cursor.moveToPosition(position);
                // Get the current value for the requested column
                imageID = cursor.getInt(columnIndex);
            }
        });
        return picturesView;
    }



    public String getRealPathFromURI(Context context, Uri contentUri) {
        Cursor cursor = null;
        try {
            String[] proj = { MediaStore.Images.Media.DATA };
            cursor = context.getContentResolver().query(contentUri,  proj, null, null, null);
            int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            return cursor.getString(column_index);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }
}
