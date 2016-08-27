package com.smackall.animator;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.util.Log;
import android.view.View;

import com.google.android.gms.gcm.GoogleCloudMessaging;
import com.google.android.gms.iid.InstanceID;
import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.FileUploader;
import com.smackall.animator.Helper.HQTaskDB;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;


/**
 * Created by Sabish.M on 24/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Publish {

    private Context mContext;

    public Publish(Context context) {
        this.mContext = context;
    }

    public void publishAnimation(int position) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new PublishAnimation(mContext, position).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new PublishAnimation(mContext, position).execute();
        }
    }

    private void updateDataBase(int id, int position) {
        if (id > 0) {
            AnimDB animDB = ((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position);
            animDB.setPublishedId(id);
            ((EditorView) mContext).db.updateMyAnimationDetails(animDB);
            UIHelper.informDialog(mContext, mContext.getString(R.string.animation_publish_successfully));
            ((Activity) mContext).findViewById(R.id.publishFrame).setVisibility(View.GONE);
            ((EditorView) mContext).animationSelection.animationSelectionAdapter.notifyDataSetChanged();
        }
    }

    public void uploadHQRender(int startFrame, int endFrame, int credits, int x, int y) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new UploadHQRender(mContext, startFrame, endFrame, credits, x, y).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new UploadHQRender(mContext, startFrame, endFrame, credits, x, y).execute();
        }
    }

    private void addTaskToDb(String response, int min, int max) {
        int taskId = Integer.parseInt(response);
        HQTaskDB hqTaskDB = new HQTaskDB();
        hqTaskDB.setTask(taskId);
        hqTaskDB.setName(((EditorView) mContext).projectName);
        hqTaskDB.setTaskType((min == max) ? Constants.EXPORT_IMAGES : Constants.EXPORT_VIDEO);
        String date = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault()).format(new Date());
        hqTaskDB.setDate(date);
        String uniqueId = ((EditorView) mContext).userDetails.uniqueId;
        hqTaskDB.setUniqueId(uniqueId);
        ((EditorView) mContext).db.addNewTask(hqTaskDB);
        if (((EditorView) mContext).export.dialog != null && ((EditorView) mContext).export.dialog.isShowing()) {
            ((EditorView) mContext).export.dialog.dismiss();
            Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
        }
    }

    private class PublishAnimation extends AsyncTask<String, String, String> {

        String response;
        Context mContext;
        int position;
        ProgressDialog asyncDialog;

        public PublishAnimation(Context context, int position) {
            this.mContext = context;
            this.position = position;
            asyncDialog = new ProgressDialog(context);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                String charset = "UTF-8";
                String ext = (((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
                String anim = PathManager.LocalAnimationFolder + "/" + Integer.toString(((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId()) + ext;
                String img = PathManager.LocalThumbnailFolder + "/" + ((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId() + ".png";

                FileUploader multipart = new FileUploader(GL2JNILib.PublishAnimation(), charset, "POST");
                if (FileHelper.checkValidFilePath(anim) && FileHelper.checkValidFilePath(img)) {
                    File animationFile = new File(anim);
                    multipart.addFilePart("animationFile", animationFile);
                    File animationimgFile = new File(img);
                    multipart.addFilePart("animationImgFile", animationimgFile);
                } else {
                    if (asyncDialog != null && asyncDialog.isShowing())
                        asyncDialog.dismiss();
                    UIHelper.informDialog(mContext, mContext.getString(R.string.somthingwrongmsg));
                    return null;
                }

                multipart.addFormField("userid", (((EditorView) mContext).userDetails.uniqueId));
                multipart.addFormField("uniqueId", (((EditorView) mContext).userDetails.uniqueId));
                multipart.addFormField("email", (((EditorView) mContext).userDetails.mail));
                multipart.addFormField("username", (((EditorView) mContext).userDetails.userName));
                multipart.addFormField("asset_name", (((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimName()));
                multipart.addFormField("keyword", (((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getKeyword()));
                multipart.addFormField("bonecount", (Integer.toString(((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getBoneCount())));
                multipart.addFormField("asset_id", (Integer.toString(((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId())));
                multipart.addFormField("type", (Integer.toString(((EditorView) mContext).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimType())));
                response = multipart.finish();
            } catch (IOException e) {

                e.printStackTrace();
                asyncDialog.dismiss();
                UIHelper.informDialog(mContext, mContext.getString(R.string.somthingwrongmsg));
                return null;
            }
            return response;
        }

        @Override
        protected void onPreExecute() {
            asyncDialog.setMessage(mContext.getString(R.string.animation_publish_progress));
            asyncDialog.show();
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(String s) {
            int id = 0;
            updateDataBase((s != null) ? Integer.parseInt(s) : id, position);
            if (asyncDialog != null && asyncDialog.isShowing())
                asyncDialog.dismiss();
            super.onPostExecute(s);
        }
    }

    private class UploadHQRender extends AsyncTask<String, String, String> {
        String response;
        Context mContext;
        ProgressDialog asyncDialog;
        int startFrame, endFrame, x, y, credits;

        public UploadHQRender(Context context, int startFrame, int endFrame, int credits, int x, int y) {
            this.mContext = context;
            this.startFrame = startFrame;
            this.endFrame = endFrame;
            this.credits = credits;
            this.x = x;
            this.y = y;
            asyncDialog = new ProgressDialog(context);
            asyncDialog.setCanceledOnTouchOutside(false);
            asyncDialog.setCancelable(false);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                String charset = "UTF-8";

                FileUploader multipart = new FileUploader(GL2JNILib.RenderTask(), charset, "POST");

                String zip = PathManager.LocalCacheFolder + "/index.zip";
                if (FileHelper.checkValidFilePath(zip)) {
                    multipart.addFilePart("renderFile", new File(zip));
                } else {
                    Log.e("Error", "File Not Found");
                    if (asyncDialog != null && asyncDialog.isShowing())
                        asyncDialog.dismiss();
                    UIHelper.informDialog(mContext, mContext.getString(R.string.somthingwrongmsg));
                    return null;
                }

                String token = "";
                if (Constants.checkPlayServices(mContext)) {
                    InstanceID instanceID = InstanceID.getInstance(mContext);
                    try {
                        token = instanceID.getToken(mContext.getString(R.string.gcm_defaultSenderIdOwn),
                                GoogleCloudMessaging.INSTANCE_ID_SCOPE, null);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                multipart.addFormField("userid", ((EditorView) mContext).userDetails.uniqueId);
                multipart.addFormField("startFrame", (Integer.toString(startFrame)));
                multipart.addFormField("endFrame", (Integer.toString(endFrame)));
                multipart.addFormField("width", (Integer.toString(x)));
                multipart.addFormField("height", (Integer.toString(y)));
                multipart.addFormField("credits", (Integer.toString(credits)));
                multipart.addFormField("pushid", (token));
                multipart.addFormField("os", ("2"));
                response = multipart.finish();
            } catch (IOException e) {
                e.printStackTrace();
                asyncDialog.dismiss();
                UIHelper.informDialog(mContext, mContext.getString(R.string.somthingwrongmsg));
                return null;
            }
            return response;
        }

        @Override
        protected void onPreExecute() {
            asyncDialog.setMessage(mContext.getString(R.string.please_wait));
            asyncDialog.show();
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(String s) {
            if (s != null) {
                UIHelper.informDialog(mContext, mContext.getString(R.string.upload_success_msg));
                addTaskToDb(s, startFrame, endFrame);
            }
            if (asyncDialog != null && asyncDialog.isShowing())
                asyncDialog.dismiss();
            super.onPostExecute(s);
        }
    }
}
