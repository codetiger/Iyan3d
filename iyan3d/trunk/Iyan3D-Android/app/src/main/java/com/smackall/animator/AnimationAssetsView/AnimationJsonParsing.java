package com.smackall.animator.AnimationAssetsView;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.preference.PreferenceManager;

import com.smackall.animator.common.Constant;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by Sabish on 26/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AnimationJsonParsing {
    Context mContext;
    Activity mActivity;
    String strForAnim;
    String whichViewIsCalled;
    AnimationDatabaseHandler animationDatabaseHandler;
    AnimationSelectionView animationSelectionView;

    public void loadDatabaseFromJson(Context context, Activity activity, String whichView, AnimationSelectionView animationSelectionView,AnimationDatabaseHandler animationDatabaseHandler) {
        mContext = context;
        mActivity = activity;
        whichViewIsCalled = whichView;
        this.animationSelectionView = animationSelectionView;
        this.animationDatabaseHandler = animationDatabaseHandler;
        System.out.println("Redownload Animation Json Executed");
        new JSONParseForAnim().execute();
    }

    private class JSONParseForAnim extends AsyncTask<String, String, JSONObject> {
        @Override
        protected JSONObject doInBackground(String... params) {
            strForAnim = "";
            DefaultHttpClient httpClient = new DefaultHttpClient();
            HttpGet get = new HttpGet("http://iyan3dapp.com/appapi/json/animationDetail.json");
            try {
                HttpResponse httpResponse = httpClient.execute(get);
                strForAnim = EntityUtils.toString(httpResponse.getEntity());
            } catch (Exception e) {
                System.out.println("Animation Json Parsing Failed");
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(JSONObject json) {

            Constant.animPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);
            Constant.animDBAdded = Constant.animPrefs.getBoolean("animAdded", true);

            if(strForAnim != null && strForAnim.length() !=0) {
                try {
                    JSONArray jArray = new JSONArray(strForAnim);

                    for (int n = 0; n < jArray.length(); n++) {
                        json = jArray.getJSONObject(n);
                        if (Constant.animDBAdded) {
                            animationDatabaseHandler.addAssets(new AnimDB(
                                    json.getString("id"), json.getString("name"), json.getString("keyword"),
                                    json.getString("userid"), json.getString("username"),
                                    json.getString("type"), json.getString("bonecount"),
                                    json.getInt("featuredindex"), json.getString("uploaded"),
                                    json.getInt("downloads"), json.getInt("rating")), "animation");
                        } else {

                            animationDatabaseHandler.updateAssets(new AnimDB(
                                    json.getString("id"), json.getString("name"), json.getString("keyword"),
                                    json.getString("userid"), json.getString("username"),
                                    json.getString("type"), json.getString("bonecount"),
                                    json.getInt("featuredindex"), json.getString("uploaded"),
                                    json.getInt("downloads"), json.getInt("rating")), "animassets");
                        }
                    }
                } catch (JSONException e) {
                    System.out.println("DaaBase Add Failed");

                    e.printStackTrace();
                } finally {
                    if(animationSelectionView != null)
                        animationSelectionView.loadAssetsFromDatabase("downloads", "AnimAssets");
                }
            }
            Constant.animPrefs.edit().putBoolean("animAdded", false).apply();
        }
    }

}