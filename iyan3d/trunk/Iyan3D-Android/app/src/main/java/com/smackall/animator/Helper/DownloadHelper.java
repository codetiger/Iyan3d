package com.smackall.animator.Helper;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by Sabish.M on 4/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DownloadHelper {

    public static int ADD = 0;
    public static int UPDATE = 1;
    String url;

    public void jsonParse(String url, DatabaseHelper db, Context context, SharedPreferenceManager sp, int jsonType) {
        this.url = url;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            new JSONParse(db, context, sp, jsonType).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        } else {
            new JSONParse(db, context, sp, jsonType).execute();
        }
    }

    private class JSONParse extends AsyncTask<Integer, Integer, String> {
        DatabaseHelper db;
        Context context;
        SharedPreferenceManager sp;
        int jsonType;

        public JSONParse(DatabaseHelper dbhelper, Context context, SharedPreferenceManager sp, int jsonType) {
            this.db = dbhelper;
            this.context = context;
            this.sp = sp;
            this.jsonType = jsonType;
        }

        @Override
        protected String doInBackground(Integer... params) {
            JSONParser jParser = new JSONParser();
            // Getting JSON from URL
            String json = jParser.getJSONFromUrl(url);
            return json;
        }

        @Override
        protected void onPostExecute(String json) {
            if (json != null) {
                if (jsonType == Constants.ASSET_JSON)
                    assetToDatabase(json, this.db, this.context, this.sp);
                else if (jsonType == Constants.ANIMATION_JSON)
                    animToDatabase(json, this.db, this.context, this.sp);
            }
            super.onPostExecute(json);
        }

        @Override
        protected void onPreExecute() {
            // TODO Auto-generated method stub
            super.onPreExecute();
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            // TODO Auto-generated method stub

            super.onProgressUpdate(values[0]);
        }
    }

    private void assetToDatabase(String jsonStr, DatabaseHelper db, Context context, SharedPreferenceManager sp) {
        int addType = sp.getInt(context, "jsonAssetUpdated");

        if (jsonStr != null && jsonStr.length() != 0) {
            try {
                JSONArray jArrayForAssets = new JSONArray(jsonStr);
                for (int n = 0; n < jArrayForAssets.length(); n++) {
                    JSONObject jsonObject = jArrayForAssets.getJSONObject(n);
                    if (addType == ADD) {
                        db.addNewModelAssets(new AssetsDB(
                                jsonObject.getString("name"), jsonObject.getInt("iap"), jsonObject.getInt("id"),
                                jsonObject.getInt("type"), jsonObject.getInt("nbones"),
                                jsonObject.getString("keywords"), jsonObject.getString("hash"),
                                jsonObject.getString("datetime"), jsonObject.getInt("group")
                        ));
                    } else if (addType == UPDATE) {
                        db.updateModelAssets(new AssetsDB(
                                jsonObject.getString("name"), jsonObject.getInt("iap"), jsonObject.getInt("id"),
                                jsonObject.getInt("type"), jsonObject.getInt("nbones"),
                                jsonObject.getString("keywords"), jsonObject.getString("hash"),
                                jsonObject.getString("datetime"), jsonObject.getInt("group")
                        ));
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
                sp.setData(context,"jsonAssetUpdated",0);
            }
            sp.setData(context,"jsonAssetUpdated",1);
        }
    }

    private void animToDatabase(String jsonStr, DatabaseHelper db, Context context, SharedPreferenceManager sp) {
        int addType = sp.getInt(context, "jsonAnimationUpdated");
        if (jsonStr != null && jsonStr.length() != 0) {
            try {
                JSONArray jArrayForAssets = new JSONArray(jsonStr);
                for (int n = 0; n < jArrayForAssets.length(); n++) {
                    JSONObject jsonObject = jArrayForAssets.getJSONObject(n);
                    if (addType == ADD) {
                        db.addNewAnimationAssets(new AnimDB(
                                jsonObject.getInt("id"), jsonObject.getString("name"), jsonObject.getString("keyword"),
                                jsonObject.getString("userid"), jsonObject.getString("username"),
                                jsonObject.getInt("type"), jsonObject.getInt("bonecount"),
                                jsonObject.getInt("featuredindex"), jsonObject.getString("uploaded"),
                                jsonObject.getInt("downloads"), jsonObject.getInt("rating"),0));
                    } else if (addType == UPDATE) {
                        db.updateAnimationDetails(new AnimDB(
                                jsonObject.getInt("id"), jsonObject.getString("name"), jsonObject.getString("keyword"),
                                jsonObject.getString("userid"), jsonObject.getString("username"),
                                jsonObject.getInt("type"), jsonObject.getInt("bonecount"),
                                jsonObject.getInt("featuredindex"), jsonObject.getString("uploaded"),
                                jsonObject.getInt("downloads"), jsonObject.getInt("rating"),0));
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
                sp.setData(context,"jsonAnimationUpdated", 0);
            }
            sp.setData(context, "jsonAnimationUpdated", 1);
        }
    }
}