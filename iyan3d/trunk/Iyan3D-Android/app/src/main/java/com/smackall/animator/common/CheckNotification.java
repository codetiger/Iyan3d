package com.smackall.animator.common;

import android.app.Activity;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.TaskStackBuilder;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.support.v4.app.NotificationCompat;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONObject;

import smackall.animator.R;

/**
 * Created by Sabish.M on 10/12/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CheckNotification {

   static Context mContext;
   static Activity mActivity;
   static String strForAnim;
   static private NotificationManager mNotificationManager;
   static private int notificationID = 100;

   static SharedPreferences checkNotify;

    public static void loadNotificationUpdate(Context context, Activity activity) {
        checkNotify = context.getSharedPreferences(
                context.getPackageName(), Context.MODE_PRIVATE);
        mContext = context;
        mActivity = activity;
        new notificationCheck().execute();
    }

    private static class notificationCheck extends AsyncTask<String, String, JSONObject> {
        @Override
        protected JSONObject doInBackground(String... params) {
           strForAnim = "";
            DefaultHttpClient httpClient = new DefaultHttpClient();

            //HttpGet get = new HttpGet("http://thisana.com/text/temp.txt");
            HttpGet get = new HttpGet("http://iyan3dapp.com/appapi/information/update.txt");
            try {
                HttpResponse httpResponse = httpClient.execute(get);
                strForAnim = EntityUtils.toString(httpResponse.getEntity());
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(JSONObject json) {

            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN)
                displayNotification(strForAnim);
        }
    }

    static void displayNotification(String msg) {
        //System.out.println("Notification Msg " + msg );
        if(msg == null)
            return;

        String[] lines = msg.split(System.getProperty("line.separator"));
        if(lines[0].toLowerCase().equals("noupdate"))
            return;

        if(!lines[0].toLowerCase().equals("message"))
            return;

        System.out.println("Current Msg : " + checkNotify.getString("isAlready",""));

        if(!checkNotify.getString("isAlready","").equals(lines[1]))
                checkNotify.edit().putString("isAlready",lines[1]).apply();
        else
            return;

        NotificationCompat.Builder nBuilder = new NotificationCompat.Builder(
                mContext);
        nBuilder.setContentTitle(lines[1]);
        nBuilder.setContentText(lines[2]);
        nBuilder.setTicker(lines[3]);
        nBuilder.setAutoCancel(true);
        nBuilder.setSmallIcon(R.mipmap.ic_launcher);
        String url;
        if(lines.length >4) {
            NotificationCompat.InboxStyle inboxStyle = new NotificationCompat.InboxStyle();
            for(int i =0; i < lines.length-3; i++){
                inboxStyle.addLine(lines[1+i]);
            }
            nBuilder.setStyle(inboxStyle);
            url = lines[lines.length-1];
        }
        else{
            url = lines[3];
        }

        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse(url));

        TaskStackBuilder stackBuilder = null;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            stackBuilder = TaskStackBuilder.create(mContext);
            stackBuilder.addNextIntent(i);
            PendingIntent pendingIntent = stackBuilder.getPendingIntent(0,
                    PendingIntent.FLAG_UPDATE_CURRENT);
            nBuilder.setContentIntent(pendingIntent);
        }
        mNotificationManager = (NotificationManager)mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.notify(notificationID, nBuilder.build());
    }
}
