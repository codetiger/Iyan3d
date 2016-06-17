package com.smackall.animator;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import com.google.android.gms.gcm.GoogleCloudMessaging;
import com.google.android.gms.iid.InstanceID;
import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.HQTaskDB;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;

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

    GoogleCloudMessaging gcmObj;

    public Publish(Context context){
        this.mContext = context;
    }

    public void publishAnimation(int position)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new PublishAnimation(mContext, position).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new PublishAnimation(mContext, position).execute();
        }
    }

    private class PublishAnimation extends AsyncTask<String,String,String> {
        HttpClient httpclient;
        HttpPost httppost;
        String response;
        Context mContext;
        int position;
        ProgressDialog asyncDialog;
        public PublishAnimation(Context context,int position){
            this.mContext = context;
            this.position = position;
            asyncDialog = new ProgressDialog(context);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                httpclient = new DefaultHttpClient();
                httppost = new HttpPost(GL2JNILib.PublishAnimation());
                org.apache.http.entity.mime.MultipartEntity builder = new org.apache.http.entity.mime.MultipartEntity();
                String ext =(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimType() == 0) ? ".sgra" : ".sgta";
                String anim = PathManager.LocalAnimationFolder+"/"+ Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId())+ext;
                String img = PathManager.LocalThumbnailFolder+"/"+((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId()+".png";

                if(FileHelper.checkValidFilePath(anim) && FileHelper.checkValidFilePath(img)){
                    File animationFile= new File(anim);
                    FileBody animation = new FileBody(animationFile);
                    builder.addPart("animationFile", animation);
                    File animationimgFile= new File(img);
                    FileBody thumbnail = new FileBody(animationimgFile);
                    builder.addPart("animationImgFile", thumbnail);
                } else {
                    Log.e("Error", "File Not Found");
                    asyncDialog.dismiss();
                    UIHelper.informDialog(mContext,"Something Wrong.. Try again.");
                    return null;
                }

                builder.addPart("userid", new StringBody(((EditorView)((Activity)mContext)).userDetails.uniqueId));
                builder.addPart("uniqueId", new StringBody(((EditorView)((Activity)mContext)).userDetails.uniqueId));
                builder.addPart("email", new StringBody(((EditorView)((Activity)mContext)).userDetails.mail));
                builder.addPart("username", new StringBody(((EditorView)((Activity)mContext)).userDetails.userName));
                builder.addPart("asset_name",new StringBody(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimName()));
                builder.addPart("keyword",new StringBody(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getKeyword()));
                builder.addPart("bonecount", new StringBody(Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getBonecount())));
                builder.addPart("asset_id", new StringBody(Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId())));
                builder.addPart("type", new StringBody(Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimType())));
                httppost.setEntity(builder);
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpclient.execute(httppost,handler);
            } catch (IOException e) {
                e.printStackTrace();
                asyncDialog.dismiss();
                UIHelper.informDialog(mContext, "Something Wrong.. Try again.");
                return null;
            }
            return response;
        }

        @Override
        protected void onPreExecute() {
            asyncDialog.setMessage("Animation Publish progress..Please wait..");
            asyncDialog.show();
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(String s) {
            int id = 0;
            updateDataBase((s != null) ? Integer.parseInt(s) : id , position);
            asyncDialog.dismiss();
            super.onPostExecute(s);
        }
    }

    private void updateDataBase(int id , int position)
    {
        if(id > 0){
            AnimDB animDB = ((EditorView)((Activity)mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position);
            animDB.setPublishedId(id);
            ((EditorView)((Activity)mContext)).db.updateMyAnimationDetails(animDB);
            UIHelper.informDialog(mContext, "Animation Published Successfully.");
            ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
            ((EditorView)((Activity)mContext)).animationSelection.animationSelectionAdapter.notifyDataSetChanged();
        }
    }


    public void uploadHQRender(int startFrame,int endFrame,int credits,int x,int y)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new UploadHQRender(mContext,startFrame,endFrame,credits,x,y).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new UploadHQRender(mContext,startFrame,endFrame,credits,x,y).execute();
        }
    }

    private class UploadHQRender extends AsyncTask<String,String,String> {
        HttpClient httpclient;
        HttpPost httppost;
        String response;
        Context mContext;
        ProgressDialog asyncDialog;
        int startFrame,endFrame,x,y,credits;
        public UploadHQRender(Context context,int startFrame,int endFrame,int credits,int x,int y){
            this.mContext = context;
            this.startFrame = startFrame;
            this.endFrame = endFrame;
            this.credits = credits;
            this.x = x; this.y = y;
            asyncDialog = new ProgressDialog(context);
        }
        @Override
        protected String doInBackground(String... params) {
            try {
                httpclient = new DefaultHttpClient();
                httppost = new HttpPost(GL2JNILib.RenderTask());
                org.apache.http.entity.mime.MultipartEntity builder = new org.apache.http.entity.mime.MultipartEntity();
                String zip = PathManager.LocalCacheFolder+"/index.zip";
                if(FileHelper.checkValidFilePath(zip)){
                    File renderFile = new File(zip);
                    FileBody renderFileBody = new FileBody(renderFile,"image/png");
                    builder.addPart("renderFile", renderFileBody);
                } else {
                    Log.e("Error", "File Not Found");
                    asyncDialog.dismiss();
                    UIHelper.informDialog(mContext,"Something Wrong.. Try again.");
                    return null;
                }

//                String id = "";
//                String regId = "";
//                if (gcmObj == null) {
//                    gcmObj = GoogleCloudMessaging
//                            .getInstance(mContext);
//                }
//                regId = gcmObj
//                        .register("328259754555");
//                id = regId;

                String token = "";
                if(Constants.checkPlayServices(mContext)) {
                    InstanceID instanceID = InstanceID.getInstance(mContext);
                    try {
                        token = instanceID.getToken(mContext.getString(R.string.gcm_defaultSenderIdOwn),
                                GoogleCloudMessaging.INSTANCE_ID_SCOPE, null);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                builder.addPart("userid", new StringBody(((EditorView) ((Activity) mContext)).userDetails.uniqueId));
                builder.addPart("startFrame", new StringBody(Integer.toString(startFrame)));
                builder.addPart("endFrame", new StringBody(Integer.toString(endFrame)));
                builder.addPart("width", new StringBody(Integer.toString(x)));
                builder.addPart("height", new StringBody(Integer.toString(y)));
                builder.addPart("credits", new StringBody(Integer.toString(credits)));
                builder.addPart("pushid", new StringBody(token));
                builder.addPart("os", new StringBody("2"));
                httppost.setEntity(builder);
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpclient.execute(httppost, handler);
            } catch (IOException e) {
                e.printStackTrace();
                asyncDialog.dismiss();
                UIHelper.informDialog(mContext, "Something Wrong.. Try again.");
                return null;
            }
            return response;
        }
        @Override
        protected void onPreExecute() {
            asyncDialog.setMessage("Please wait..");
            asyncDialog.show();
            super.onPreExecute();
        }
        @Override
        protected void onPostExecute(String s) {
            if(s != null) {
                UIHelper.informDialog(mContext, "Uploaded successfully. You can see the progress of your render in your profile view.");
                addTaskToDb(s,startFrame,endFrame);
            }
            asyncDialog.dismiss();
            super.onPostExecute(s);
        }
    }

    private void addTaskToDb(String response,int min,int max){
        int taskId = Integer.parseInt(response);
        HQTaskDB hqTaskDB = new HQTaskDB();
        hqTaskDB.setTask(taskId);
        hqTaskDB.setName(((EditorView) ((Activity) mContext)).projectName);
        hqTaskDB.setTaskType((min == max) ? Constants.EXPORT_IMAGES : Constants.EXPORT_VIDEO);
        String date = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault()).format(new Date());
        hqTaskDB.setDate(date);
        String uniqueId = ((EditorView)((Activity)mContext)).userDetails.uniqueId;
        hqTaskDB.setUniqueId(uniqueId);
        ((EditorView)((Activity)mContext)).db.addNewTask(hqTaskDB);
        if(((EditorView)((Activity)mContext)).export.dialog != null && ((EditorView)((Activity)mContext)).export.dialog.isShowing())
            ((EditorView)((Activity)mContext)).export.dialog.dismiss();
    }
}
