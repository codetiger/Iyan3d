package com.smackall.animator.DownloadManager;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.AnimationAssetsView.AnimDB;
import com.smackall.animator.AnimationAssetsView.AnimationDatabaseHandler;
import com.smackall.animator.AnimationAssetsView.AnimationSelectionView;
import com.smackall.animator.AnimationAssetsView.CustomAdapterForAnimationSelection;
import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.AssetsViewController.AssetsViewController;
import com.smackall.animator.AssetsViewController.AssetsViewDatabaseHandler;
import com.smackall.animator.AssetsViewController.CustomAdapterForAssetsSelection;
import com.smackall.animator.TextAssetsView.CustomAdapterForTextAssets;
import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILibAssetView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import smackall.animator.R;

/**
 * Created by Sabish.M on 23/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AddToDownloadManager {



    MyDownloadStatusListener myDownloadStatusListener = new MyDownloadStatusListener();

    Context mContext;
    DownloadRequest.Priority priority = DownloadRequest.Priority.NORMAL;

    String fileName = "";
    Bitmap bmp;


    public int downloadAdd(Context context, String url, String fileName, String destinationPath, String priority, DownloadManager downloadManager ){

        this.mContext = context;

        if(priority.equals("low"))
            this.priority = DownloadRequest.Priority.LOW;
        else if(priority.equals("high"))
            this.priority = DownloadRequest.Priority.HIGH;
        else
            this.priority = DownloadRequest.Priority.NORMAL;

        this.fileName = fileName;


        Uri uri = Uri.parse(url);
        Uri destinationUri = Uri.parse(destinationPath + this.fileName);

        DownloadRequest downloadRequest =  new DownloadRequest(uri)
                .setDestinationURI(destinationUri).setPriority(this.priority)
                .setDownloadListener(myDownloadStatusListener);

       return downloadManager.add(downloadRequest);
    }

    class MyDownloadStatusListener implements DownloadStatusListener {
        @Override
        public void onDownloadComplete(int id){

                switch (Constant.downloadViewType) {
                    case "assets":
                        if (CustomAdapterForAssetsSelection.currentMeshDownload != id && CustomAdapterForAssetsSelection.currentTextureDownload != id) {
                            for (int i = 0; i < AssetsViewController.assetsId.length; i++) {
                                if(AssetsViewController.gridView.getChildAt(i)==null)
                                    break;
                                if (AssetsViewController.gridView.getChildAt(i).getId() == id) {
                                    File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + AssetsViewController.assetsId[i] + ".png");
                                    File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/images/" + AssetsViewController.assetsId[i] + ".png");
                                    moveFrom.renameTo(moveTo);
                                    View viewItem = AssetsViewController.gridView.getChildAt(i);
                                    ImageView imageView = (ImageView) viewItem.findViewById(R.id.imageView1);
                                    ProgressBar progressBar = (ProgressBar) viewItem.findViewById(R.id.native_progress_bar);
                                    System.out.println("File Name " + String.valueOf(moveTo));
                                    imageView.setImageBitmap(BitmapFactory.decodeFile(String.valueOf(moveTo)));
                                    imageView.setVisibility(View.VISIBLE);
                                    progressBar.setVisibility(View.GONE);
                                    break;
                                }
                            }
                        }
                        if (id == CustomAdapterForAssetsSelection.currentTextureDownload && id != CustomAdapterForAssetsSelection.currentMeshDownload) {
                            System.out.println("Texture Download Completed");
                            File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + "-cm.png");
                            File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/mesh/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + "-cm.png");
                            File mesh = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/mesh/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + CustomAdapterForAssetsSelection.meshExtension);

                            moveFrom.renameTo(moveTo);

                            if (moveTo.exists() && mesh.exists()) {
                                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.GONE);
                                AssetsViewController.selectedItemId = AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition];
                                AssetsViewController.selectedItemId = AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetsId();
                                GL2JNILibAssetView.assetItemDisplay(AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetsId(), AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetName(), Integer.parseInt(AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getType()));
                                AssetsViewController.hideImportBtn(false);
                            }

                        }
                        if (id != CustomAdapterForAssetsSelection.currentTextureDownload && id == CustomAdapterForAssetsSelection.currentMeshDownload) {
                            System.out.println("Mesh Download Completed");
                            File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + CustomAdapterForAssetsSelection.meshExtension);
                            File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/mesh/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + CustomAdapterForAssetsSelection.meshExtension);
                            File texture = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/mesh/" + AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition] + "-cm.png");

                            moveFrom.renameTo(moveTo);
                            if (moveTo.exists() && texture.exists()) {
                                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.GONE);
                                AssetsViewController.selectedItemId = AssetsViewController.assetsId[CustomAdapterForAssetsSelection.clickedPosition];
                                AssetsViewController.selectedItemId = AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetsId();
                                GL2JNILibAssetView.assetItemDisplay(AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetsId(), AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getAssetName(), Integer.parseInt(AssetsViewController.assets.get(CustomAdapterForAssetsSelection.clickedPosition).getType()));
                                AssetsViewController.hideImportBtn(false);
                            }
                        }
                        break;
                    case "animation":
                        if (CustomAdapterForAnimationSelection.currentAnimDownload != id) {
                            for (int i = 0; i < AnimationSelectionView.animAssetsId.length; i++) {
                                if(AnimationSelectionView.gridView.getChildAt(i)==null)
                                    break;

                                if (AnimationSelectionView.gridView.getChildAt(i).getId() == id) {
                                    File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + AnimationSelectionView.animAssetsId[i] + ".png");
                                    File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/animations/" + AnimationSelectionView.animAssetsId[i] + ".png");
                                    View viewItem = AnimationSelectionView.gridView.getChildAt(i);
                                    ImageView imageView = (ImageView) viewItem.findViewById(R.id.imageView1);
                                    ProgressBar progressBar = (ProgressBar) viewItem.findViewById(R.id.native_progress_bar);
                                    System.out.println("File Name " + String.valueOf(moveTo));
                                    String cropedImagePath = cropImage(String.valueOf(moveFrom), String.valueOf(moveTo));
                                    if(cropedImagePath == null)
                                        break;
                                    imageView.setImageBitmap(BitmapFactory.decodeFile(cropedImagePath));
                                    imageView.setVisibility(View.VISIBLE);
                                    progressBar.setVisibility(View.GONE);
                                    break;
                                }
                            }
                        }
                        if (CustomAdapterForAnimationSelection.currentAnimDownload == id) {
                            System.out.println("Animation Download Completed");
                            File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + AnimationSelectionView.animDBs.get(CustomAdapterForAnimationSelection.clickedPosition).getAnimAssetId() + CustomAdapterForAnimationSelection.animExtension);
                            File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/animations/" + AnimationSelectionView.animDBs.get(CustomAdapterForAnimationSelection.clickedPosition).getAnimAssetId() + CustomAdapterForAnimationSelection.animExtension);
                            moveFrom.renameTo(moveTo);

                            if (moveTo.exists()) {
                                AnimationSelectionView.gridView.invalidateViews();
                                AnimationSelectionView.anim_selection_progressBar.setVisibility(View.GONE);
                                System.out.println("Download is Completed");
                                AnimationSelectionView.isDownloadCanceled = false;
                                AnimationSelectionView.anim_selection_add_to_scene.setVisibility(View.VISIBLE);
                                AnimationSelectionView.selectedAnimId = Integer.parseInt(AnimationSelectionView.animDBs.get(CustomAdapterForAnimationSelection.clickedPosition).getAnimAssetId());
                                CustomAdapterForAnimationSelection.playAnimation(CustomAdapterForAnimationSelection.clickedPosition);

                            }
                        }
                        break;
                    case "text":
                        System.out.println("Font Download Completed " + Integer.toString(id - 1));
                        File moveFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + TextAssetsSelectionView.fontName[0]);
                        File moveTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/fonts/" + TextAssetsSelectionView.fontName[0]);
                        if (moveFrom.exists()) {
                            System.out.println("File Moved Successfully");
                            moveFrom.renameTo(moveTo);

                        }
                        if (TextAssetsSelectionView.gridView.getChildAt(id - 1) == null)
                            break;
                        View viewItem = TextAssetsSelectionView.gridView.getChildAt(id - 1);

                        if (viewItem != null) {
                            TextView textInside = (TextView) viewItem.findViewById(R.id.font_style);
                            textInside.setVisibility(View.VISIBLE);
                            ProgressBar progressBarTextAssets = (ProgressBar) viewItem.findViewById(R.id.progressBarTextAssets);
                            progressBarTextAssets.setVisibility(View.GONE);
                            try {
                                Typeface fontFace = Typeface.createFromFile(moveTo);
                                textInside.setTypeface(fontFace);
                                textInside.setText("Text");
                                textInside.setTextSize(TextAssetsSelectionView.width / 45);
                                textInside.setTextColor(Color.WHITE);
                                if (CustomAdapterForTextAssets.clickedPosition == 0)
                                    if (id - 1 == 0) {
                                        TextAssetsSelectionView.hashMap.put("fileName", TextAssetsSelectionView.fontName[0]);
                                        TextAssetsSelectionView.textUpdater();
                                    }
                            } catch (Exception e) {
                                progressBarTextAssets.setVisibility(View.VISIBLE);
                                textInside.setVisibility(View.GONE);
                                System.out.println("Font set Failed");
                            }
                        } else {
                            System.out.println("View is Null");
                        }

                        break;


                    case "json":
                        if(Constant.assetJsonId == id){
                            String strForAssets = null;
                            JSONObject json;
                            AssetsViewDatabaseHandler assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(mContext);
                            try {
                                strForAssets = Constant.readFile(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + "animation");
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                            Constant.assetsPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);
                            Constant.assetsDBAdded = Constant.assetsPrefs.getBoolean("added", true);
                            if (strForAssets != null && strForAssets.length() != 0) {
                                try {
                                    JSONArray jArrayForAssets = new JSONArray(strForAssets);
                                    for (int n = 0; n < jArrayForAssets.length(); n++) {
                                        json = jArrayForAssets.getJSONObject(n);
                                        if (Constant.assetsDBAdded) {
                                            assetsViewDatabaseHandler.addAssets(new AssetsDB(
                                                    json.getString("name"), json.getString("iap"), json.getInt("id"),
                                                    json.getString("type"), json.getString("nbones"),
                                                    json.getString("keywords"), json.getString("hash"),
                                                    json.getString("datetime")
                                            ), "Assets");
                                        } else {
                                            assetsViewDatabaseHandler.updateAssets(new AssetsDB(
                                                    json.getString("name"), json.getString("iap"), json.getInt("id"),
                                                    json.getString("type"), json.getString("nbones"),
                                                    json.getString("keywords"), json.getString("hash"),
                                                    json.getString("datetime")
                                            ), "Assets");
                                        }
                                    }
                                }catch(JSONException e){
                                    System.out.println("DataBase Add Failed");
                                    e.printStackTrace();
                                }
                                finally{
                                    if(assetsViewDatabaseHandler.getAssetsCount(Constant.ASSETS_DB.ASSETS_TABLE.getValue()) != 0) {
                                        System.out.println("Assets Json Parsing Completed : " + assetsViewDatabaseHandler.getAssetsCount(Constant.ASSETS_DB.ASSETS_TABLE.getValue()));
                                    }
                                }

                                Constant.assetsPrefs.edit().putBoolean("added", false).apply();
                            }
                        }
                        if(Constant.animationJsonId == id){
                            String strForAnim = null;
                            JSONObject json;
                            AnimationDatabaseHandler animationDatabaseHandler = new AnimationDatabaseHandler(mContext);
                            try {
                                strForAnim = Constant.readFile(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/.cache/" + "animation");
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                            Constant.animPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);
                            Constant.animDBAdded = Constant.animPrefs.getBoolean("animAdded", true);

                            if(strForAnim != null && strForAnim.length() !=0) {
                                try {
                                    JSONArray jArray = new JSONArray(strForAnim);

                                    for (int n = 0; n < jArray.length(); n++) {
                                        json = jArray.getJSONObject(n);
                                        if (Constant.animDBAdded) {
                                            animationDatabaseHandler.addAssets(new AnimDB(
                                                    json.getString("id"), json.getString("name"), json.getString("keyword"),
                                                    json.getString("userid"), json.getString("username"),
                                                    json.getString("type"), json.getString("bonecount"),
                                                    json.getInt("featuredindex"), json.getString("uploaded"),
                                                    json.getInt("downloads"), json.getInt("rating")), "animation");
                                        } else {

                                            animationDatabaseHandler.updateAssets(new AnimDB(
                                                    json.getString("id"), json.getString("name"), json.getString("keyword"),
                                                    json.getString("userid"), json.getString("username"),
                                                    json.getString("type"), json.getString("bonecount"),
                                                    json.getInt("featuredindex"), json.getString("uploaded"),
                                                    json.getInt("downloads"), json.getInt("rating")), "animassets");
                                        }
                                    }
                                } catch (JSONException e) {
                                    System.out.println("DaaBase Add Failed");
                                    e.printStackTrace();
                                } finally {
                                    System.out.println("Animation Json Parsing Completed : " + animationDatabaseHandler.getAssetsCount("AnimAssets"));
                                }
                            }

                            Constant.animPrefs.edit().putBoolean("animAdded", false).apply();
                        }
                        break;
                }
        }

        @Override
        public void onDownloadFailed(int id, int errorCode, String errorMessage) {
                System.out.println("Download Error : " + errorMessage + " Download Id : " + id + " Download Error Code : " + errorCode);
        }

        @Override
        public void onProgress(int id, long totalBytes, long downloadedBytes, int progress) {


        }
    }

    public String cropImage(String filePath,String toPath){
        System.out.println("Crop Png Path : " + filePath);
        if(bmp != null && !bmp.isRecycled())
        {
            bmp = null;
        }
        bmp = BitmapFactory.decodeFile(filePath);
        if(bmp == null)
            return null;
        System.out.println("Bitmap Width : " + bmp.getWidth());
        System.out.println("Bitmap Height : " + bmp.getHeight());

        int oriWidth = bmp.getWidth();
        int oriHeight = bmp.getHeight();

        int maxSize = Math.max(oriHeight, oriWidth);
        int targetSize = 0;

        if(maxSize > 128) {
            targetSize = 128;
        }
        else
            targetSize = maxSize;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, targetSize, targetSize, false);
       return savePng(scaledBitmap, toPath);

    }

    private String savePng( Bitmap img,String filePath) {
        System.out.println("Save Png to Path : " + filePath);
        try {
            File dumpFile = new File(filePath);
            FileOutputStream os = new FileOutputStream(dumpFile);
            img.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        finally {
            return filePath;
        }
    }

}
