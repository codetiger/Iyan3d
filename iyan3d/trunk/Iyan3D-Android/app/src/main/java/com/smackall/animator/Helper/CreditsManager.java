package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;

import com.smackall.animator.EditorView;
import com.smackall.animator.SceneSelection;
import com.smackall.animator.UserDetails;
import com.smackall.animator.opengl.GL2JNILib;

import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by Sabish.M on 28/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CreditsManager {

    Context mContext;
    HashMap<String, String> map;
    private UserDetails userDetails;

    public CreditsManager(Context mContext){
        this.mContext = mContext;
        try {
            if (Constants.currentActivity == 0) {
                userDetails = ((SceneSelection) ((Activity) mContext)).userDetails;
            } else
                userDetails = ((EditorView) ((Activity) mContext)).userDetails;
        }
        catch (ClassCastException ignored){}
    }

    public void getCreditsForUniqueId(CreditTask context)
    {
        String uniqueId = userDetails.uniqueId;
        String name = userDetails.userName;
        String email = userDetails.mail;
        String signInType = Integer.toString(userDetails.signInType-1);
        String hwVersion = userDetails.hwversion;
        String osVersion = userDetails.osversion;
        if(uniqueId.length() < 5)
            return;
        new getCredis(uniqueId,name,email,signInType,hwVersion,osVersion,context).execute();
    }

    private class getCredis extends AsyncTask<Integer, Integer, String> {
        String uniqueId;
        String name;
        String email;
        String signInType;
        String response;
        String hwversion;
        String osversion;
        CreditTask task;

        public getCredis(String uniqueId,String name,String email,String signInType,String hwversion,String osversion,CreditTask context){
            this.uniqueId =  uniqueId;
            this.name =  name;
            this.email =  email;
            this.signInType =  signInType;
            this.hwversion = hwversion;
            this.osversion = osversion;
            this.task = context;
        }
        @Override
        protected String doInBackground(Integer... params) {
            String url = GL2JNILib.Credits()+"?uniqueid="+uniqueId+"&username="+name+"&email="+email+"&hwversion="+
                    hwversion+"&osversion="+osversion+"&deviceid="+Constants.deviceUniqueId+"&signintype="+signInType;
            HttpClient httpClient = new DefaultHttpClient();
            HttpGet httpGet = new HttpGet(url.replace(" ","%20"));
            try {
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpClient.execute(httpGet,handler);
            } catch (IOException e) {
                e.printStackTrace();
                UIHelper.informDialog(mContext,"Please Check your network connection.");
                return null;
            }
            return response;
        }
        protected void onPostExecute(String st) {
            if(st != null && st.length() >5) {
                jsonToMap(st);
                this.task.onCreditRequestCompleted(Integer.parseInt(map.get("credits")),Integer.parseInt(map.get("premium")));
            }
            else{
                this.task.failed();
            }
        }
    }

    private void jsonToMap(String t){
        try {
            if(map != null)
                map.clear();
            map = new HashMap<String, String>();
            JSONObject jObject = new JSONObject(t);
            Iterator<?> keys = jObject.keys();
            while (keys.hasNext()) {
                String key = (String) keys.next();
                String value = jObject.getString(key);
                map.put(key, value);
            }
        }
        catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void getTaskProgress(CreditTask context,int taskId){
        new getTaskProgress(taskId,context).execute();
    }

    private class getTaskProgress extends AsyncTask<Integer, Integer, String> {

        CreditTask task;
        int taskId;
        String response;

        public getTaskProgress(int taskId,CreditTask context){
            this.taskId = taskId;
            this.task = context;
        }
        @Override
        protected String doInBackground(Integer... params) {
            String url = GL2JNILib.CheckProgress()+"?taskid="+taskId;
            HttpClient httpClient = new DefaultHttpClient();
            HttpGet httpGet = new HttpGet(url);
            try {
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpClient.execute(httpGet,handler);
            } catch (IOException e) {
                e.printStackTrace();
                UIHelper.informDialog(mContext,"Please Check your network connection.");
                return null;
            }
            return response;
        }
        protected void onPostExecute(String st) {
            if(st != null && st.length() >0) {
                int progress = (int) Float.parseFloat(st);
                task.onCheckProgressCompleted(progress,taskId);
            }
        }
    }

    public void useOrRechargeCredits(int credits,String usageType,String signed_data,String signature,int frame,CreditTask task)
    {
        new UseOrRechargeCredits(credits,usageType,signed_data,signature,frame,task).execute();
    }

    private class UseOrRechargeCredits extends AsyncTask<Integer, Integer, String> {

        String usageType;
        int credits;
        String signed_data;
        String signature;
        String response;
        CreditTask task;
        int frame;

        public UseOrRechargeCredits(int credits,String usageType,String signed_data,String signature,int frame,CreditTask task){
            this.credits = credits;
            this.usageType = usageType;
            this.signed_data = signed_data;
            this.signature = signature;
            this.task = task;
            this.frame = frame;
        }
        @Override
        protected String doInBackground(Integer... params) {
            String uniqueId = userDetails.uniqueId;
            HttpClient httpClient = new DefaultHttpClient();
            HttpPost httpPost = new HttpPost(GL2JNILib.UseOrRecharge());
            try {
                MultipartEntity multipartEntity = new MultipartEntity();
                multipartEntity.addPart("uniqueid",new StringBody(uniqueId));
                multipartEntity.addPart("signed_data",new StringBody(signed_data));
                multipartEntity.addPart("signature",new StringBody(signature));
                multipartEntity.addPart("usagetype",new StringBody(usageType));
                multipartEntity.addPart("credits",new StringBody(Integer.toString(credits)));
                httpPost.setEntity(multipartEntity);
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpClient.execute(httpPost,handler);
            } catch (IOException e) {
                e.printStackTrace();
                UIHelper.informDialog(mContext,"Please Check your network connection.");
                return null;
            }
            return response;
        }
        protected void onPostExecute(String st) {
            if(st == null) {
                task.finishExport(frame, false,"");
                return;
            }
            jsonToMap(st);
            int result = Integer.parseInt(map.get("result"));
            String msg = map.get("message");
            if(result > 0)
                task.finishExport(frame,true,msg);
            else task.finishExport(frame,false,msg);
        }
    }
}
