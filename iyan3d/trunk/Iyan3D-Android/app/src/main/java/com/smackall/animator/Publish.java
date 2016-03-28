package com.smackall.animator;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;

import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.File;
import java.io.IOException;

/**
 * Created by Sabish.M on 24/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Publish {
    private Context mContext;
    public Publish(Context context){
        this.mContext = context;
    }

    public void publishAnimation(int position)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new PublishAnimation(mContext, 1).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new PublishAnimation(mContext, 1).execute();
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
                httppost = new HttpPost("https://www.iyan3dapp.com/appapi/publish.php");
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

                builder.addPart("userid", new StringBody("anonymous"));
                builder.addPart("uniqueId", new StringBody("sadhsa454as547d8w7w94da4d5"));
                builder.addPart("email", new StringBody("anonymous@mail.com"));
                builder.addPart("username", new StringBody("anonymous"));
                builder.addPart("asset_name",new StringBody("Testing"));
                builder.addPart("keyword",new StringBody("Testing"));
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
            AnimDB animDB = ((EditorView)((Activity)mContext)).animationSelection.animationSelectionAdapter.animDBs.get(0);
            animDB.setPublishedId(id);
            ((EditorView)((Activity)mContext)).db.updateMyAnimationDetails(animDB);
            UIHelper.informDialog(mContext, "Animation Published Successfully.");
            ((FrameLayout)((Activity)mContext).findViewById(R.id.publishFrame)).setVisibility(View.GONE);
            ((EditorView)((Activity)mContext)).animationSelection.animationSelectionAdapter.notifyDataSetChanged();
        }
    }
}
