package com.smackall.animator.AssetsViewController;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.preference.PreferenceManager;

import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.common.Constant;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by Sabish on 25/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AssetsJsonParsing {
    Context mContext;
    Activity mActivity;
    String strForAssets;
    AssetsViewDatabaseHandler assetsViewDatabaseHandler;
    AssetsViewController assetsViewController;
    TextAssetsSelectionView textAssetsSelectionView;
    String whichViewIsCalled;

    public void loadDatabaseFromJson(Context context, Activity activity, String whichView,
                                     AssetsViewDatabaseHandler assetsViewDatabaseHandler,AssetsViewController assetsViewController,TextAssetsSelectionView textAssetsSelectionView){
        mContext = context;
        mActivity = activity;
        whichViewIsCalled = whichView;
        this.assetsViewDatabaseHandler = assetsViewDatabaseHandler;
        this.assetsViewController = assetsViewController;
        this.textAssetsSelectionView = textAssetsSelectionView;
        new JSONParseForAssets().execute();
    }

    private class JSONParseForAssets extends AsyncTask<String, String, JSONObject> {
        @Override
        protected JSONObject doInBackground(String... params) {
            strForAssets = "";
            DefaultHttpClient httpClient = new DefaultHttpClient();
            HttpGet get = new HttpGet("http://iyan3dapp.com/appapi/json/assetsDetail.json");
            try {
                HttpResponse httpResponse = httpClient.execute(get);
                strForAssets = EntityUtils.toString(httpResponse.getEntity());
            } catch (Exception e) {
                System.out.println("Assets Json Parsing Failed");
                    e.printStackTrace();
            }
            System.out.println("Url for Assets : " + "http://iyan3dapp.com/appapi/json/assetsDetail.json" );
            return null;
        }

        @Override
        protected void onPostExecute(JSONObject json) {

            Constant.assetsPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);
            Constant.assetsDBAdded = Constant.assetsPrefs.getBoolean("added", true);
            if (strForAssets != null && strForAssets.length() != 0) {
                try {
                    JSONArray jArrayForAssets = new JSONArray(strForAssets);
                    for (int n = 0; n < jArrayForAssets.length(); n++) {
                        json = jArrayForAssets.getJSONObject(n);
                        if (Constant.assetsDBAdded) {
                            assetsViewDatabaseHandler.addAssets(new AssetsDB(
                                    json.getString("name"), json.getString("iap"), json.getInt("id"),
                                    json.getString("type"), json.getString("nbones"),
                                    json.getString("keywords"), json.getString("hash"),
                                    json.getString("datetime")
                            ), "Assets");
                        } else {
                            assetsViewDatabaseHandler.updateAssets(new AssetsDB(
                                    json.getString("name"), json.getString("iap"), json.getInt("id"),
                                    json.getString("type"), json.getString("nbones"),
                                    json.getString("keywords"), json.getString("hash"),
                                    json.getString("datetime")
                            ), "Assets");
                        }
                    }
                }catch(JSONException e){
                    System.out.println("DataBase Add Failed");
                    e.printStackTrace();
                }
                finally{
                        System.out.println("Assets Json Parsing Completed : " + assetsViewDatabaseHandler.getAssetsCount("Assets"));

                    if(assetsViewDatabaseHandler.getAssetsCount("Assets") != 0) {
                        if (whichViewIsCalled.equals("AssetsView")) {
                           assetsViewController.loadAssetsFromDatabase("1");
                        }
                        if (whichViewIsCalled.equals("TextView")) {
                            textAssetsSelectionView.loadAssetsFromDatabase("50");
                        }
                    }
                }

                Constant.assetsPrefs.edit().putBoolean("added", false).apply();
            }
        }
    }
}
