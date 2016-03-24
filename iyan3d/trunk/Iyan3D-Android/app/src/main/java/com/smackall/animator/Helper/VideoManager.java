package com.smackall.animator.Helper;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.AsyncTask;
import android.provider.MediaStore;

import com.smackall.animator.EditorView;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class VideoManager {

    Context mContext;

    ProgressDialog progress;

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
            FileHelper.copy(path,PathManager.LocalUserVideoFolder+"/"+FileHelper.getFileNameFromPath(path));
            ((EditorView)((Activity)mContext)).imageManager.savePng(ThumbnailUtils.createVideoThumbnail(path, MediaStore.Video.Thumbnails.MICRO_KIND), PathManager.LocalThumbnailFolder + "/" + FileHelper.getFileWithoutExt(path));
            return null;
        }

        public void onPostExecute(Void unused) {
            progress.dismiss();
            ((EditorView)((Activity)mContext)).videoSelection.notifyDataChanged();
        }
    }
}

