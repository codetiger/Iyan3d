package com.smackall.iyan3dPro.Adapters;

import android.graphics.Bitmap;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.smackall.iyan3dPro.EditorView;
import com.smackall.iyan3dPro.R;

import java.io.File;
import java.util.List;

/**
 * Created by Sabish.M on 31/8/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FilePickerAdapter extends BaseAdapter /*RecyclerView.Adapter<FilePickerAdapter.FilePickerViewHolder> */{

    private EditorView editorView;
    List<File> files;
    public boolean parentPathRemoved = true;
    String rootParent;
    Bitmap cone, cube, cylinder, plane, sphere, torus, obj, dae, fbx, threeds, ttf, otf;
    ListView recyclerView;

    public FilePickerAdapter(EditorView editorView, List<File> files,Bitmap[] bitmaps , String rootParent,ListView recyclerView) {
        this.editorView = editorView;
        this.files = files;
        this.cone = bitmaps[0];
        this.cube = bitmaps[1];
        this.cylinder = bitmaps[2];
        this.plane = bitmaps[3];
        this.sphere = bitmaps[4];
        this.torus = bitmaps[5];
        this.obj = bitmaps[6];
        this.dae = bitmaps[7];
        this.fbx = bitmaps[8];
        this.threeds = bitmaps[9];
        this.ttf = bitmaps[10];
        this.otf = bitmaps[11];
        this.rootParent = rootParent;
        this.recyclerView = recyclerView;
    }

    @Override
    public int getCount() {
        return files.size();
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
    public View getView(int position, View convertView, ViewGroup parent) {
        final int viewPosition = position;
        final View grid;
        if (convertView == null) {
            LayoutInflater inflater = editorView.getLayoutInflater();
            grid = inflater.inflate(R.layout.filepicker_cell, parent, false);
        } else {
            grid = convertView;
        }

        TextView tvName = (TextView) grid.findViewById(R.id.name);
        ImageView imageView = (ImageView) grid.findViewById(R.id.folder_icon);
        String name = files.get(viewPosition).getName();
        tvName.setText(name);

        if (!parentPathRemoved && viewPosition == 0 && !files.get(viewPosition).getAbsolutePath().toLowerCase().equals(rootParent)){
            grid.setVisibility(View.VISIBLE);
            tvName.setText("..");
        }
        else
            grid.setVisibility(View.VISIBLE);

        if(files.get(viewPosition).isDirectory()){
            imageView.setImageDrawable(ContextCompat.getDrawable(editorView,R.drawable.folder_icon));
        }
        else{
            if (viewPosition < editorView.filePicker.getTopList().size() &&
                    files.get(viewPosition).getAbsolutePath().endsWith(".sgm") &&
                    rootParent.toLowerCase().equals(Environment.getExternalStorageDirectory().getAbsolutePath().toLowerCase().substring(
                    0,Environment.getExternalStorageDirectory().getAbsolutePath().lastIndexOf("/")))){

                tvName.setText(name.substring(0,name.lastIndexOf(".")));
                imageView.setImageBitmap((viewPosition == 0) ? cone :
                        (viewPosition == 1) ? cube :
                                (viewPosition == 2) ? cylinder :
                                        (viewPosition == 3) ? plane :
                                                (viewPosition == 4) ? sphere : torus);

                tvName.setText((viewPosition == 0) ? "Cone" :
                        (viewPosition == 1) ? "Cube" :
                                (viewPosition == 2) ? "Cylinder" :
                                        (viewPosition == 3) ? "Plane" :
                                                (viewPosition == 4) ? "Sphere": "Torus");
            }
            else {
                String ext = files.get(viewPosition).getAbsolutePath().substring(files.get(viewPosition).getAbsolutePath()
                        .length() - 4, files.get(viewPosition).getAbsolutePath().length());
                imageView.setImageBitmap(
                        (ext.toLowerCase().equals(".fbx") ? fbx :
                                ext.toLowerCase().equals(".3ds") ? threeds :
                                        ext.toLowerCase().equals(".dae") ? dae :
                                                ext.toLowerCase().equals(".obj") ? obj :
                                                        ext.toLowerCase().equals(".ttf") ? ttf : otf));
            }
        }

        grid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(files.get(viewPosition).isDirectory()){
                    String rootPath = files.get(viewPosition).getAbsolutePath();
                    String parentPath = files.get(viewPosition).getAbsolutePath().substring(0,files.get(viewPosition).getAbsolutePath().lastIndexOf("/"));
                    files.clear();
                    files = editorView.filePicker.getFileList(rootPath,parentPath,FilePickerAdapter.this);
                    notifyDataSetChanged();
                } else {
                    editorView.filePicker.setPickedPath(files.get(viewPosition).getAbsolutePath());
                    editorView.filePicker.getFilePickerListener().FilePickerListenerCallback(files.get(viewPosition).getAbsolutePath(), false, true,
                            editorView.filePicker.getInsertPoint());
                }
            }
        });

        return grid;
    }
}
