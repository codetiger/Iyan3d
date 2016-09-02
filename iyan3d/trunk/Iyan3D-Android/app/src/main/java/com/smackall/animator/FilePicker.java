package com.smackall.animator;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Debug;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.TimingLogger;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.smackall.animator.Adapters.FilePickerAdapter;
import com.smackall.animator.Adapters.FilePickerRecycleAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

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
    Bitmap[] bitmaps = new Bitmap[10];
    AssetsDB assetsDB = new AssetsDB();

    public FilePicker(EditorView editorView){
        this.editorView = editorView;

        int width = ContextCompat.getDrawable(editorView,R.drawable.folder_icon).getIntrinsicWidth();
        int height = ContextCompat.getDrawable(editorView,R.drawable.folder_icon).getIntrinsicHeight();

        Bitmap bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cone)).getBitmap();
        bitmaps[0] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cube)).getBitmap();
        bitmaps[1] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.cylinder)).getBitmap();
        bitmaps[2] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.plane)).getBitmap();
        bitmaps[3] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.sphere)).getBitmap();
        bitmaps[4] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.torus)).getBitmap();
        bitmaps[5] = Bitmap.createScaledBitmap(bitmap,width,height,true);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.obj)).getBitmap();
        bitmaps[6] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.dae)).getBitmap();
        bitmaps[7] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.fbx)).getBitmap();
        bitmaps[8] = Bitmap.createScaledBitmap(bitmap,width,height,false);
        bitmap = ((BitmapDrawable)ContextCompat.getDrawable(editorView,R.drawable.threeds)).getBitmap();
        bitmaps[9] = Bitmap.createScaledBitmap(bitmap,width,height,false);
    }

    public void showFilePicker()
    {
        assetsDB.resetValues();
        editorView.showOrHideToolbarView(Constants.HIDE);
        insertPoint = (editorView.sharedPreferenceManager.getInt(editorView, "toolbarPosition") == 1) ?
                (ViewGroup) editorView.findViewById(R.id.leftView)
                : (ViewGroup) editorView.findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup) insertPoint.getParent()).getChildCount(); i++) {
            if (((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup) insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup) insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) editorView.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View filepicker = vi.inflate(R.layout.filepicker, insertPoint, false);
        insertPoint.addView(filepicker, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        insertPoint.findViewById(R.id.cancel).setOnClickListener(this);
        insertPoint.findViewById(R.id.add_to_scene).setOnClickListener(this);

        ListView recycleView = (ListView) insertPoint.findViewById(R.id.filePicker);
        String rootParent = Environment.getExternalStorageDirectory().getAbsolutePath().substring(0,
                Environment.getExternalStorageDirectory().getAbsolutePath().lastIndexOf("/"));
        recycleView.setAdapter(new FilePickerAdapter(editorView,getFileList(Environment.getExternalStorageDirectory().getAbsolutePath(),rootParent,null),bitmaps,rootParent,recycleView));


       // RecyclerView recycleView = (RecyclerView) insertPoint.findViewById(R.id.filePicker);
        //recycleView.setLayoutManager(new LinearLayoutManager(editorView,LinearLayoutManager.VERTICAL,false));
        //String rootParent = Environment.getExternalStorageDirectory().getAbsolutePath().substring(0,
          //      Environment.getExternalStorageDirectory().getAbsolutePath().lastIndexOf("/"));
//         recycleView.setAdapter(new FilePickerAdapter(editorView,getFileList(Environment.getExternalStorageDirectory().getAbsolutePath(),rootParent,null),bitmaps,rootParent,recycleView));
       // recycleView.setAdapter(new FilePickerRecycleAdapter(editorView,getFileList(Environment.getExternalStorageDirectory().getAbsolutePath(),rootParent,null),bitmaps,rootParent,recycleView));

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
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60001.sgm"));
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60002.sgm"));
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60003.sgm"));
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60004.sgm"));
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60005.sgm"));
            finalList.add(new File(PathManager.DefaultAssetsDir+"/60006.sgm"));
        }
        File[] list = new File(root).listFiles();
        for (File file:list) {

            if(file.isDirectory() && !file.getName().startsWith("."))
                folders.add(file.getName());
            else {
                String ext = FileHelper.getFileExt(file.getAbsoluteFile());
                if (ext.toLowerCase().endsWith("obj") ||
                        ext.toLowerCase().endsWith("3ds") ||
                        ext.toLowerCase().endsWith("dae") ||
                        ext.toLowerCase().endsWith("fbx"))
                    files.add(file.getName());
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

    public void importModel(String meshPath, boolean isTempNode) {
        String fileName = FileHelper.getFileNameFromPath(meshPath);

        assetsDB.resetValues();
        assetsDB.setAssetPath(meshPath);
        assetsDB.setAssetName(fileName);
        assetsDB.setTexture("-1");
        assetsDB.setX(1.0f);
        assetsDB.setY(1.0f);
        assetsDB.setZ(1.0f);
        assetsDB.setHasMeshColor((meshPath.toLowerCase().startsWith(PathManager.DefaultAssetsDir.toLowerCase())));
        assetsDB.setIsTempNode(isTempNode);
        assetsDB.setTexturePath(FileHelper.getFileLocation(meshPath)+"/");

        editorView.renderManager.importAssets(assetsDB);

        if(!isTempNode){
            HitScreens.EditorView(editorView);
            insertPoint.removeAllViews();
            editorView.showOrHideToolbarView(Constants.SHOW);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.cancel:
                HitScreens.EditorView(editorView);
                editorView.renderManager.removeTempNode();
                insertPoint.removeAllViews();
                editorView.showOrHideToolbarView(Constants.SHOW);
                break;
            case R.id.add_to_scene:
                if (FileHelper.checkValidFilePath(assetsDB.getAssetPath()))
                    importModel(assetsDB.getAssetPath(), false);

                break;
        }
    }
}
