package com.smackall.animator.DownloadManager;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

import com.smackall.animator.Adapters.AnimationSelectionAdapter;
import com.smackall.animator.Adapters.AssetSelectionAdapter;
import com.smackall.animator.Adapters.TextSelectionAdapter;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

import java.io.File;
import java.io.FileOutputStream;

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

    public int downloadAdd(Context context, String url, String fileName, String destinationPath,  DownloadRequest.Priority priority, DownloadManager downloadManager,
                           AssetSelectionAdapter assetSelectionAdapter,TextSelectionAdapter textSelectionAdapter,AnimationSelectionAdapter animationSelectionAdapter){

        this.mContext = context;
        this.priority = priority;
        this.fileName = fileName;
        this.assetSelectionAdapter = assetSelectionAdapter;
        this.textSelectionAdapter = textSelectionAdapter;
        this.animationSelectionAdapter = animationSelectionAdapter;


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
            switch (Constants.VIEW_TYPE) {
                case Constants.ASSET_VIEW: {
                    try {
                        if(assetSelectionAdapter == null) break;
                        if (assetSelectionAdapter.meshDownloadId != id && assetSelectionAdapter.textureDownloadId != id) {
                            String fileName = assetSelectionAdapter.downloadingAssets.get(id).toString();
                            File moveFrom = new File(PathManager.LocalCacheFolder+"/" + fileName + ".png");
                            File moveTo = new File(PathManager.LocalThumbnailFolder+"/" + fileName + ".png");
                            FileHelper.move(moveFrom,moveTo);
                            assetSelectionAdapter.notifyDataSetChanged();
                            break;
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
        finally {
            return filePath;
        }
    }
}
