package com.smackall.animator.ImportAndRig;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;

import java.io.File;

import smackall.animator.R;


/**
 * Created by Sabish.M on 15/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CustomAdapterForTextureImportGrid extends BaseAdapter{
        String [] textureFile;
        Context context;
        private static LayoutInflater inflater=null;
        public static View rowView;

        public CustomAdapterForTextureImportGrid(ImportObjAndTexture mainActivity, String[] textureFiles) {
            // TODO Auto-generated constructor stub
            this.textureFile=textureFiles;
            context=mainActivity;
            inflater = ( LayoutInflater ) context.
                    getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @Override
        public int getCount() {
            // TODO Auto-generated method stub
            return textureFile.length;
        }

        @Override
        public Object getItem(int position) {
            // TODO Auto-generated method stub
            return position;
        }

        @Override
        public long getItemId(int position) {
            // TODO Auto-generated method stub
            return position;
        }

    int previous = Constant.unDefined;
    int current = 0;
    boolean firstTime = true;

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        View row = convertView;
        RecordHolder holder = null;
        final String userMesh = Environment.getExternalStorageDirectory()+"/Android/data/" +context.getPackageName()+"/mesh/usermesh/";

        if (row == null) {
            LayoutInflater inflater = ((Activity) context).getLayoutInflater();
            row = inflater.inflate(R.layout.grid_content_for_obj_import, parent, false);

            holder = new RecordHolder();
            holder.txtTitle = (TextView) row.findViewById(R.id.obj_grid_content_text);
            holder.imageItem = (ImageView) row.findViewById(R.id.obj_grid_content_image);
            holder.linearLayout = (LinearLayout) row.findViewById(R.id.texture_grid_bg);
            holder.progressBar = (ProgressBar) row.findViewById(R.id.native_progress_bar);
            row.setTag(holder);
        } else {
            holder = (RecordHolder) row.getTag();
        }

        row.getLayoutParams().width = Constant.width/8;
        row.getLayoutParams().height = (int) (Constant.height/4.5);
        holder.progressBar.setVisibility(View.INVISIBLE);

        final File imageFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/images/"+
              textureFile[position]);

        holder.imageItem.setImageBitmap(BitmapFactory.decodeFile(String.valueOf(imageFile)));

        holder.txtTitle.setText(textureFile[position]);
        holder.txtTitle.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.width / 60);

        if(position == 0 && firstTime){
            current = position;
            holder.linearLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_pressed));
            ImportObjAndTexture.texturePath = userMesh+textureFile[0];
            ImportObjAndTexture.textureFileName = textureFile[0];
            previous = current;
            firstTime = false;
            ImportObjAndTexture.isImported = ImportObjAndTexture.texturePath.length() != 0 && ImportObjAndTexture.objPath.length() != 0 && ImportObjAndTexture.texturePath != null && ImportObjAndTexture.objPath != null;
        }

        row.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (position != previous) {
                    current = position;
                    View gridSelected = ImportObjAndTexture.texture_import_grid.getChildAt(current);
                    LinearLayout gridHolder = (LinearLayout) gridSelected.findViewById(R.id.texture_grid_bg);
                    gridHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_pressed));


                    View gridPreviousSelected = ImportObjAndTexture.texture_import_grid.getChildAt(previous);
                    LinearLayout gridPreviousHolder = (LinearLayout) gridPreviousSelected.findViewById(R.id.texture_grid_bg);
                    gridPreviousHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.grid_layout_non_pressed));
                    previous = current;
                }

                ImportObjAndTexture.texturePath = userMesh+textureFile[position];
                ImportObjAndTexture.textureFileName = textureFile[position];

                ImportObjAndTexture.isImported = ImportObjAndTexture.texturePath.length() != 0 && ImportObjAndTexture.objPath.length() != 0 && ImportObjAndTexture.texturePath != null && ImportObjAndTexture.objPath != null;
            }
        });
        return row;
    }

    static class RecordHolder {
        TextView txtTitle;
        ImageView imageItem;
        LinearLayout linearLayout;
        ProgressBar progressBar;

    }
}



