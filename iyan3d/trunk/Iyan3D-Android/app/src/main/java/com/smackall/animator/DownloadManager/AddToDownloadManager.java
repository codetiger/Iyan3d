package com.smackall.animator.DownloadManager;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

import com.smackall.animator.Adapters.AnimationSelectionAdapter;
import com.smackall.animator.Adapters.AssetSelectionAdapter;
import com.smackall.animator.Adapters.TextSelectionAdapter;
import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.AssetsDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;

import java.io.File;
import java.io.FileOutputStream;
import java.util.Calendar;

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

    AssetSelectionAdapter assetSelectionAdapter;
    TextSelectionAdapter textSelectionAdapter;
    AnimationSelectionAdapter animationSelectionAdapter;
    public boolean errorMessageShowing = false;
    public long lastErrorShowingTime = 0;
    long MAX_INTERVAL = 120000; //2 minutes in milli seconds

    public int downloadAdd(Context context, String url, String fileName, String destinationPath,  DownloadRequest.Priority priority, DownloadManager downloadManager,
                           AssetSelectionAdapter assetSelectionAdapter,TextSelectionAdapter textSelectionAdapter,AnimationSelectionAdapter animationSelectionAdapter,String requiredFileName){

        this.mContext = context;
        this.priority = priority;
        this.fileName = fileName;
        this.assetSelectionAdapter = assetSelectionAdapter;
        this.textSelectionAdapter = textSelectionAdapter;
        this.animationSelectionAdapter = animationSelectionAdapter;


        Uri uri = Uri.parse(url);
        Uri destinationUri = Uri.parse(destinationPath + ((requiredFileName != null) ? requiredFileName : this.fileName));

        DownloadRequest downloadRequest =  new DownloadRequest(uri)
                .setDestinationURI(destinationUri).setPriority(this.priority)
                .setDownloadListener(myDownloadStatusListener);

        return downloadManager.add(downloadRequest);
    }

    class MyDownloadStatusListener implements DownloadStatusListener {
        @Override
        public void onDownloadComplete(int id){
            switch (Constants.VIEW_TYPE) {
                case Constants.PARTICLE_VIEW: {
                    try {
                        if (assetSelectionAdapter.meshDownloadId != id && assetSelectionAdapter.textureDownloadId != id && assetSelectionAdapter.jsonDownloadId != id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String fileName = assetSelectionAdapter.downloadingAssets.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder + "/" + fileName + ".png");
                            FileHelper.move(moveFrom, moveTo);
                            assetSelectionAdapter.notifyDataSetChanged();
                            break;
                        }
                        else if (assetSelectionAdapter.meshDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String ext = (assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getType() == 1) ? ".sgr" : ".sgm";
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ext);
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + ext);
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        }
                        else if (assetSelectionAdapter.textureDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW) {
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + "-cm.png");
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + "-cm.png");
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        }
                        else if(assetSelectionAdapter.jsonDownloadId == id && Constants.VIEW_TYPE == Constants.PARTICLE_VIEW){
                            String fileName = Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder + "/" + fileName + ".json");
                            File moveTo = new File(PathManager.LocalMeshFolder + "/" + fileName + ".json");
                            FileHelper.move(moveFrom, moveTo);
                            importAsset();
                        }
                    }
                    catch (NullPointerException e){
                        e.printStackTrace();
                    }
                }
                case Constants.ASSET_VIEW: {
                    try {
                        if(assetSelectionAdapter == null) break;
                        if (assetSelectionAdapter.meshDownloadId != id && assetSelectionAdapter.textureDownloadId != id && Constants.VIEW_TYPE == Constants.ASSET_VIEW) {
                            String fileName = assetSelectionAdapter.downloadingAssets.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder+"/" + fileName + ".png");
                            FileHelper.move(moveFrom,moveTo);
                            assetSelectionAdapter.notifyDataSetChanged();
                            break;
                        }
                        else if(assetSelectionAdapter.meshDownloadId == id && Constants.VIEW_TYPE == Constants.ASSET_VIEW){
                            String ext = (assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getType() == 1) ? ".sgr" : ".sgm";
                            String fileName =Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName + ext);
                            File moveTo = new File(PathManager.LocalMeshFolder+"/" + fileName+ext);
                            FileHelper.move(moveFrom,moveTo);
                            importAsset();
                        }
                        else if(Constants.VIEW_TYPE == Constants.ASSET_VIEW){
                            String fileName =Integer.toString(assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset).getAssetsId());
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName + "-cm.png");
                            File moveTo = new File(PathManager.LocalMeshFolder+"/" + fileName+"-cm.png");
                            FileHelper.move(moveFrom,moveTo);
                            importAsset();
                        }
                    }
                    catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored){
                        ignored.printStackTrace();
                    }
                    break;
                }
                case Constants.ANIMATION_VIEW: {
                    try {
                        if(animationSelectionAdapter == null) break;
                        if (animationSelectionAdapter.animDownloadId != id) {
                            String fileName = animationSelectionAdapter.downloadingAnimation.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder+"/" + fileName + ".png");
                            FileHelper.move(moveFrom,moveTo);
                            animationSelectionAdapter.notifyDataSetChanged();
                            break;
                        }
                        else{
                            int animId = animationSelectionAdapter.animDBs.get(animationSelectionAdapter.selectedId).getAnimAssetId();
                            String ext = ( animationSelectionAdapter.animDBs.get(animationSelectionAdapter.selectedId).getAnimType() == 0) ? ".sgra" : ".sgta";
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/"+animId+ext);
                            File moveTo = new File(PathManager.LocalAnimationFolder+"/"+animId+ext);
                            FileHelper.move(moveFrom,moveTo);
                            animationSelectionAdapter.importAnimation(animationSelectionAdapter.selectedId);
                        }
                    }
                    catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored){
                        ignored.printStackTrace();
                    }
                    break;
                }
                case Constants.TEXT_VIEW: {
                    if(textSelectionAdapter == null)break;
                    try {
                        String fileName = textSelectionAdapter.downloadingFonts.get(id).toString();
                        File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName);
                        File moveTo = new File(PathManager.LocalFontsFolder+"/" + fileName);
                        FileHelper.move(moveFrom,moveTo);
                        textSelectionAdapter.notifyDataSetChanged();
                    }
                    catch (NullPointerException | IndexOutOfBoundsException | ClassCastException ignored){
                        ignored.printStackTrace();
                    }
                    break;
                }
            }
        }

        @Override
        public void onDownloadFailed(int id, int errorCode, String errorMessage) {
            if(errorCode != DownloadManager.ERROR_DOWNLOAD_CANCELLED && !errorMessageShowing && (Calendar.getInstance().getTimeInMillis() - lastErrorShowingTime) > MAX_INTERVAL ) {
                lastErrorShowingTime = Calendar.getInstance().getTimeInMillis();
                errorMessageShowing = true;
                UIHelper.informDialog(mContext, "Please Check your network connection.");
                if(Constants.VIEW_TYPE == Constants.ANIMATION_VIEW && mContext!= null && ((EditorView)mContext).animationSelection != null && ((EditorView)mContext).animationSelection.animationSelectionAdapter != null) {
                    ((EditorView) mContext).animationSelection.processCompleted = true;
                    ((EditorView)mContext).animationSelection.filePath = "";
                }
            }
        }

        @Override
        public void onProgress(int id, long totalBytes, long downloadedBytes, int progress) {
        }
    }

    public String cropImage(String filePath,String toPath){
        if(bmp != null && !bmp.isRecycled())
        {
            bmp = null;
        }
        bmp = BitmapFactory.decodeFile(filePath);
        if(bmp == null)
            return null;

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
        try {
            File dumpFile = new File(filePath);
            FileOutputStream os = new FileOutputStream(dumpFile);
            img.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return filePath;
    }

    private void importAsset()
    {
        if(assetSelectionAdapter == null || assetSelectionAdapter.assetsDBs == null || assetSelectionAdapter.assetsDBs.size() <= assetSelectionAdapter.selectedAsset)
            return;
        AssetsDB assetsDB = assetSelectionAdapter.assetsDBs.get(assetSelectionAdapter.selectedAsset);
        String ext = (assetsDB.getType() == 1) ? ".sgr" : ".sgm";
        String mesh = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + ext;
        String texture = PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + "-cm.png";
        if(Constants.VIEW_TYPE == Constants.PARTICLE_VIEW && !FileHelper.checkValidFilePath(PathManager.LocalMeshFolder+"/"+assetsDB.getAssetsId() + ".json"))
            return;
        if(FileHelper.checkValidFilePath(mesh) && FileHelper.checkValidFilePath(texture)){
            ((EditorView)(Activity)mContext).showOrHideLoading(Constants.SHOW);
            assetsDB.setTexture(assetsDB.getAssetsId() + "-cm");
            assetsDB.setIsTempNode(true);
            ((EditorView)(Activity)mContext).renderManager.importAssets(assetsDB,false);
        }
    }
}
