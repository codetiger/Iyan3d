package com.smackall.iyan3dPro;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.smackall.iyan3dPro.Adapters.FilePickerAdapter;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.FileHelper;
import com.smackall.iyan3dPro.Helper.Listeners.FilePickerListener;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by Sabish.M on 31/8/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FilePicker implements View.OnClickListener {

    EditorView editorView;
    ViewGroup insertPoint;
    Bitmap[] bitmaps = new Bitmap[12];
    List<File> topList;
    String[] extensions;
    FilePickerListener filePickerListener;
    String pickedPath = "-1";

    public FilePicker(EditorView editorView){
        this.editorView = editorView;

        int width = ContextCompat.getDrawable(editorView,R.drawable.folder_icon).getIntrinsicWidth();
        int height = ContextCompat.getDrawable(editorView,R.drawable.folder_icon).getIntrinsicHeight();

        Bitmap bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cone)).getBitmap();
        bitmaps[0] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cube)).getBitmap();
        bitmaps[1] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cylinder)).getBitmap();
        bitmaps[2] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.plane)).getBitmap();
        bitmaps[3] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.sphere)).getBitmap();
        bitmaps[4] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.torus)).getBitmap();
        bitmaps[5] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.obj)).getBitmap();
        bitmaps[6] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.dae)).getBitmap();
        bitmaps[7] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.fbx)).getBitmap();
        bitmaps[8] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.threeds)).getBitmap();
        bitmaps[9] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable) ContextCompat.getDrawable(editorView, R.drawable.ttf)).getBitmap();
        bitmaps[10] = Bitmap.createScaledBitmap(bitmap, width, height, false);
        bitmap = ((BitmapDrawable) ContextCompat.getDrawable(editorView, R.drawable.otf)).getBitmap();
        bitmaps[11] = Bitmap.createScaledBitmap(bitmap, width, height, false);
    }

    public View showFilePicker(String[] extensions, List<File> topList, ViewGroup insertPoint, FilePickerListener filePickerListener, boolean withCancelAndOkBtn)
    {
        this.extensions = extensions;
        this.filePickerListener = filePickerListener;
        this.insertPoint = insertPoint;
        this.topList = topList;
        this.pickedPath = "-1";

        editorView.showOrHideToolbarView(Constants.HIDE);

        LayoutInflater vi = (LayoutInflater) editorView.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View filepicker = vi.inflate(R.layout.filepicker, this.insertPoint, false);

        for (int i = 0; i < ((ViewGroup) this.insertPoint.getParent()).getChildCount(); i++) {
            if (((ViewGroup) this.insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) this.insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) this.insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }

        this.insertPoint.setVisibility(View.VISIBLE);
        this.insertPoint.removeAllViews();

        this.insertPoint.addView(filepicker, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        if (withCancelAndOkBtn) {
            this.insertPoint.findViewById(R.id.cancel).setOnClickListener(this);
            this.insertPoint.findViewById(R.id.add_to_scene).setOnClickListener(this);
        } else {
            this.insertPoint.findViewById(R.id.cancel).setVisibility(View.GONE);
            this.insertPoint.findViewById(R.id.add_to_scene).setVisibility(View.GONE);
        }
        ListView recycleView = (ListView) filepicker.findViewById(R.id.filePicker);
        String rootParent = Environment.getExternalStorageDirectory().getAbsolutePath().substring(0,
                Environment.getExternalStorageDirectory().getAbsolutePath().lastIndexOf("/"));
        recycleView.setAdapter(new FilePickerAdapter(editorView,getFileList(Environment.getExternalStorageDirectory().getAbsolutePath(),rootParent,null),bitmaps,rootParent,recycleView));

        return filepicker;
    }

    public List<File> getFileList(String root,String parentPath,FilePickerAdapter filePickerAdapter)
    {
        List<File> finalList = new ArrayList<>();
        finalList.add(new File(parentPath));

        String rootParent = Environment.getExternalStorageDirectory().getAbsolutePath().substring(0,
                Environment.getExternalStorageDirectory().getAbsolutePath().lastIndexOf("/"));

        if(finalList.get(0).getAbsolutePath().toLowerCase().equals(rootParent)){
            finalList.remove(0);
            if(filePickerAdapter != null)
                filePickerAdapter.parentPathRemoved = true;
        }
        else if(filePickerAdapter != null)
            filePickerAdapter.parentPathRemoved = false;

        List<String> folders = new ArrayList<>();
        List<String> files = new ArrayList<>();

        if(root.toLowerCase().equals(Environment.getExternalStorageDirectory().getAbsolutePath().toLowerCase())){
            for (File file : topList) {
                finalList.add(file);
            }
        }
        File[] list = new File(root).listFiles();
        for (File file:list) {
            if(file.isDirectory() && !file.getName().startsWith("."))
                folders.add(file.getName());
            else if (file.getName().contains(".")) {
                String ext = FileHelper.getFileExt(file.getAbsoluteFile());
                for (int i = 0; i < extensions.length; i++) {
                    if (ext.toLowerCase().endsWith(extensions[i]) ||
                            ext.toLowerCase().endsWith(extensions[i]) ||
                            ext.toLowerCase().endsWith(extensions[i]) ||
                            ext.toLowerCase().endsWith(extensions[i])) {
                        files.add(file.getName());
                        i = extensions.length;
                    }
                }
            }
        }

        Collections.sort(folders);
        Collections.sort(files);

        for (String name : files){
            finalList.add(new File(root+"/"+name));
        }
        for (String folder : folders){
            finalList.add(new File(root+"/"+folder));
        }
        return finalList;
    }

    public FilePickerListener getFilePickerListener() {
        return filePickerListener;
    }

    public ViewGroup getInsertPoint() {
        return insertPoint;
    }

    public void setPickedPath(String pickedPath) {
        this.pickedPath = pickedPath;
    }

    public String[] getExtensions() {
        return extensions;
    }

    public List<File> getTopList() {
        return topList;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cancel:
                this.filePickerListener.FilePickerListenerCallback(pickedPath, true, true, insertPoint);
                break;
            case R.id.add_to_scene:
                this.filePickerListener.FilePickerListenerCallback(pickedPath, false, false, insertPoint);
                break;
        }
    }
}
