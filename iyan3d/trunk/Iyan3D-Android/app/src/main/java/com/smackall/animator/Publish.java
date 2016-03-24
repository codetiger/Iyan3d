package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.util.Log;

import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;

import org.apache.http.HttpEntity;
import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.HttpMultipartMode;
import org.apache.http.entity.mime.MultipartEntityBuilder;
import org.apache.http.entity.mime.content.FileBody;
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
        ResponseHandler<String> responseHandler = new BasicResponseHandler();
        String response;
        Context mContext;
        int position;
        public PublishAnimation(Context context,int position){
            this.mContext = context;
            this.position = position;
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                httpclient = new DefaultHttpClient();
                httppost = new HttpPost("https://www.iyan3dapp.com/appapi/publish.php");
                MultipartEntityBuilder builder = MultipartEntityBuilder.create();
                builder.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);

                String anim = Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId());
                String img = PathManager.LocalThumbnailFolder+"/"+((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId()+".png";

                if(FileHelper.checkValidFilePath(anim)){
                    File animationFile= new File(anim);
                    FileBody fb = new FileBody(animationFile);
                    builder.addPart("animationFile", fb);
                } else {
                    Log.e("Error", "File Not Found");
                }
                if(FileHelper.checkValidFilePath(img)){
                    File animationimgFile= new File(img);
                    FileBody fb = new FileBody(animationimgFile);
                    builder.addPart("animationImgFile", fb);
                }
                else {
                    Log.e("Error", "File Not Found");
                }

                builder.addTextBody("userid", "anonymous");
                builder.addTextBody("uniqueId", "sadhsa454as547d8w7w94da4d5");
                builder.addTextBody("email", "anonymous@mail.com");
                builder.addTextBody("username", "anonymous");
                builder.addTextBody("asset_name","Testing");
                builder.addTextBody("keyword","Testing");
                builder.addTextBody("bonecount", Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getBonecount()));
                builder.addTextBody("asset_id", Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimAssetId()));
                builder.addTextBody("type", Integer.toString(((EditorView) ((Activity) mContext)).animationSelection.animationSelectionAdapter.animDBs.get(position).getAnimType()));

                HttpEntity yourEntity = builder.build();
                httppost.setEntity(yourEntity);

                response = httpclient.execute(httppost,responseHandler);

            } catch (IOException e) {
                e.printStackTrace();
            }
            return response;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(String s) {
            System.out.println("Response : " + s);
            super.onPostExecute(s);
        }
    }
}
