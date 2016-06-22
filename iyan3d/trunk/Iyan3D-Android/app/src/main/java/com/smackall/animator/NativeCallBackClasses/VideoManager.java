package com.smackall.animator.NativeCallBackClasses;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.provider.MediaStore;

import com.smackall.animator.EditorView;
import com.smackall.animator.FrameGrabber.FrameGrabber;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class VideoManager {

    Context mContext;

    ProgressDialog progress;
    private FrameGrabber mFrameGrabber;

    public VideoManager(Context mContext){
        this.mContext = mContext;
        progress = new ProgressDialog(mContext);
    }

    public void getVideoFromStorage()
    {
        Intent intent = new Intent();
        intent.setType("video/*");
        intent.setAction(Intent.ACTION_GET_CONTENT);
        ((Activity)mContext).startActivityForResult(Intent.createChooser(intent, "Select Video"), Constants.VIDEO_IMPORT_RESPONSE);
    }

    public void startActivityForResult(Intent i, int requestCode,int resultCode)
    {
        if (resultCode == Activity.RESULT_OK) {
            if (requestCode == Constants.VIDEO_IMPORT_RESPONSE) {
                Uri selectedImageUri = i.getData();
                String videoPath = getPath(selectedImageUri);
                if (videoPath != null) {
                    manageVideo(videoPath);
                }
            }
        }
    }

    public String getPath(Uri uri) {
        String[] projection = { MediaStore.Images.Media.DATA };
        Cursor cursor = ((Activity)mContext).managedQuery(uri, projection, null, null, null);
        if (cursor != null) {
            int column_index = cursor
                    .getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            return cursor.getString(column_index);
        } else
            return null;
    }

    private void manageVideo(String path){
        if(FileHelper.checkValidFilePath(path)){
            progress.setCanceledOnTouchOutside(false);
            progress.setMessage("Please Wait... Copying..");
            new Copy(path,progress).execute();
        }
    }

    private class Copy extends AsyncTask<Void, Void, Void> {
        String path;
        ProgressDialog progress;
        public Copy(String path,ProgressDialog progressDialog){
            this.path = path;
            this.progress = progressDialog;
        }

        public void onPreExecute() {
            progress.show();
        }

        public Void doInBackground(Void... unused) {
            FileHelper.copy(path, PathManager.LocalUserVideoFolder + "/" + FileHelper.getFileNameFromPath(path));
            ((EditorView)((Activity)mContext)).imageManager.savePng(ThumbnailUtils.createVideoThumbnail(path, MediaStore.Video.Thumbnails.MICRO_KIND), PathManager.LocalThumbnailFolder + "/" + FileHelper.getFileWithoutExt(path));
            return null;
        }

        public void onPostExecute(Void unused) {
            progress.dismiss();
//            ((EditorView)((Activity)mContext)).videoSelection.notifyDataChanged();// Video Restricted
        }
    }


    public byte[] getImageAtTime(String fileName, int frame,int width,int height)
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1)
            return null;

        String filePath = PathManager.LocalUserVideoFolder+"/"+fileName+".mp4";
        if(!FileHelper.checkValidFilePath(filePath))
            return null;
        int microSecondsForOneFrame = 1000000/24;
        long microSeconds = microSecondsForOneFrame * frame;
        Bitmap bmp = getFrameAtTimeByFrameGrabber(filePath, microSeconds,width,height);
        if (null != mFrameGrabber) {
            mFrameGrabber.release();
            mFrameGrabber = null;
        }
        if(bmp != null) {
            float bigSide = (bmp.getWidth() >= bmp.getHeight()) ? bmp.getWidth() : bmp.getHeight();
            int target;
            if (bigSide <= 128)
                target = 128;
            else if (bigSide <= 256)
                target = 256;
            else if (bigSide <= 512)
                target = 512;
            else
                target = 1024;
            Bitmap scaledBmp = Bitmap.createScaledBitmap(bmp, target, target, false);
            ByteArrayOutputStream blob = new ByteArrayOutputStream();
            scaledBmp.compress(Bitmap.CompressFormat.PNG, 0 /*ignored for PNG*/, blob);

            int bytes = scaledBmp.getByteCount();
            ByteBuffer buffer = ByteBuffer.allocate(bytes);
            scaledBmp.copyPixelsToBuffer(buffer);
            EditorView.updateDemo(blob.toByteArray());
            return buffer.array();
        }
        return null;
    }

    private Bitmap getFrameAtTimeByFrameGrabber(String path, long time,int width,int height) {
        mFrameGrabber = new FrameGrabber();
        mFrameGrabber.setDataSource(path);
        mFrameGrabber.setTargetSize(width, height);
        mFrameGrabber.init();
        return mFrameGrabber.getFrameAtTime(time);
    }
}