package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteStatement;
import android.os.AsyncTask;
import android.os.Build;
import android.os.SystemClock;

import com.smackall.animator.EditorView;
import com.smackall.animator.LoadingActivity;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish.M on 4/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DownloadHelper {

    public static int ADD = 0;
    public static int UPDATE = 1;
    String url;
    Context context;

    String className = "";

    public void jsonParse(String url, DatabaseHelper db, Context context, SharedPreferenceManager sp, int jsonType) {
        this.url = url;
        this.context = context;
        className = this.context.getClass().getSimpleName();
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
            return jParser.getJSONFromUrl(url);
        }

        @Override
        protected void onPostExecute(final String json) {
            super.onPostExecute(json);
            if (json != null) {
                doUpdateDatabase(context, jsonType, sp, json, db);
            } else {
                UIHelper.informDialog(context,"Check your network connection");
                if(className.toLowerCase().equals("editorview")){

                }
                else {
                    ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                    if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                        ((LoadingActivity) ((Activity) context)).startSceneSelection();
                    }
                }
            }
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

    public void doUpdateDatabase(Context context, int jsonType, SharedPreferenceManager sp, String json, DatabaseHelper db) {
        if (jsonType == Constants.ASSET_JSON) {
            int addType = sp.getInt(context, "jsonAssetUpdated");
            if (json.length() != 0) {
                try {
                    JSONArray jArrayForAssets = new JSONArray(json);
                    List<AssetsDB> assetDbs = new ArrayList<>();
                    for (int n = 0; n < jArrayForAssets.length(); n++) {
                        JSONObject jsonObject = jArrayForAssets.getJSONObject(n);
                        assetDbs.add(new AssetsDB(
                                jsonObject.getString("name"), jsonObject.getInt("iap"), jsonObject.getInt("id"),
                                jsonObject.getInt("type"), jsonObject.getInt("nbones"),
                                jsonObject.getString("keywords"), jsonObject.getString("hash"),
                                jsonObject.getString("datetime"), jsonObject.getInt("group")));
                    }
                    insertIntoDatabase(assetDbs, null, (addType == ADD), true);
                } catch (JSONException e) {
                    e.printStackTrace();
                    UIHelper.informDialog(context,"Check your network connection");
                    sp.setData(context, "jsonAssetUpdated", 0);
                    if(className.toLowerCase().equals("editorview")){

                    }
                    else {
                        ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                        if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                            ((LoadingActivity) ((Activity) context)).startSceneSelection();
                        }
                    }
                }
                sp.setData(context, "jsonAssetUpdated", 1);
            }
        } else if (jsonType == Constants.ANIMATION_JSON) {
            int addType = sp.getInt(context, "jsonAnimationUpdated");
            if (json.length() != 0) {
                try {
                    JSONArray jArrayForAssets = new JSONArray(json);
                    List<AnimDB> animdDbs = new ArrayList<>();
                    for (int n = 0; n < jArrayForAssets.length(); n++) {
                        JSONObject jsonObject = jArrayForAssets.getJSONObject(n);
                        animdDbs.add(new AnimDB(
                                jsonObject.getInt("id"), jsonObject.getString("name"), jsonObject.getString("keyword"),
                                jsonObject.getString("userid"), jsonObject.getString("username"),
                                jsonObject.getInt("type"), jsonObject.getInt("bonecount"),
                                jsonObject.getInt("featuredindex"), jsonObject.getString("uploaded"),
                                jsonObject.getInt("downloads"), jsonObject.getInt("rating"), 0));
                    }
                    insertIntoDatabase(null, animdDbs, (addType == ADD), false);
                } catch (JSONException e) {
                    e.printStackTrace();
                    UIHelper.informDialog(context,"Check your network connection");
                    sp.setData(context, "jsonAnimationUpdated", 0);
                    if(className.toLowerCase().equals("editorview")){

                    }
                    else {
                        ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                        if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                            ((LoadingActivity) ((Activity) context)).startSceneSelection();
                        }
                    }
                }
                sp.setData(context, "jsonAnimationUpdated", 1);
            }
        }
    }

    private void insertIntoDatabase(List<AssetsDB> assetDbs, List<AnimDB> animsDbs, boolean isInsertMethod, boolean isAsset) {
        if(!isInsertMethod) {updateDatabase(assetDbs,animsDbs,isInsertMethod,isAsset); return;}
        if (isAsset) {
            SQLiteDatabase modelAssetDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            String
                    sql = "Insert or Replace into " + DatabaseHelper.ASSET_TABLE_ASSETS + "(" + DatabaseHelper.ASSET_KEY_ASSET_NAME + "," +
                    DatabaseHelper.ASSET_KEY_IAP + "," + DatabaseHelper.ASSET_KEY_ASSETSID + "," +
                    DatabaseHelper.ASSET_KEY_TYPE + "," + DatabaseHelper.ASSET_KEY_NBONES + "," +
                    DatabaseHelper.ASSET_KEY_KEYWORDS + "," + DatabaseHelper.ASSET_KEY_HASH + "," +
                    DatabaseHelper.ASSET_KEY_TIME + "," + DatabaseHelper.ASSET_KEY_GROUP + ") values(?,?,?,?,?,?,?,?,?)";

            SQLiteStatement statement = modelAssetDatabase.compileStatement(sql);
            modelAssetDatabase.beginTransaction();
            for (AssetsDB assetDb : assetDbs) {
                statement.clearBindings();
                statement.bindString(1, assetDb.getAssetName());
                statement.bindString(2, String.valueOf(assetDb.getIap()));
                statement.bindString(3, String.valueOf(assetDb.getAssetsId()));
                statement.bindString(4, String.valueOf(assetDb.getType()));
                statement.bindString(5, String.valueOf(assetDb.getNbones()));
                statement.bindString(6, assetDb.getKeywords());
                statement.bindString(7, assetDb.getHash());
                statement.bindString(8, assetDb.getDateTime());
                statement.bindString(9, String.valueOf(assetDb.getGroup()));
                statement.execute();
            }
            modelAssetDatabase.setTransactionSuccessful();
            modelAssetDatabase.endTransaction();
            if (modelAssetDatabase.isOpen())
                modelAssetDatabase.close();
            if(className.toLowerCase().equals("editorview")){
                if(isAsset)
                    try{
                        if(context == null || ((EditorView)context).assetSelection == null || ((EditorView)context).assetSelection.assetSelectionAdapter == null)
                            return;
                        int position = ((EditorView)context).assetSelection.category.getSelectedItemPosition();
                        ((EditorView)context).assetSelection.onSpinnerItemSelected(position);
                    }catch (ClassCastException ignored){}
            }
            else {
                ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                    ((LoadingActivity) ((Activity) context)).startSceneSelection();
                }
            }
        } else {
            SQLiteDatabase aniamtionDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            String sql = "Insert or Replace into " + DatabaseHelper.ANIM_TABLE_ANIMASSETS + "(" + DatabaseHelper.ANIM_KEY_ASSETSID + "," +
                    DatabaseHelper.ANIM_KEY_ANIM_NAME + "," + DatabaseHelper.ANIM_KEY_KEYWORD + "," +
                    DatabaseHelper.ANIM_KEY_USERID + "," + DatabaseHelper.ANIM_KEY_USERNAME + "," +
                    DatabaseHelper.ANIM_KEY_TYPE + "," + DatabaseHelper.ANIM_KEY_BONECOUNT + "," +
                    DatabaseHelper.ANIM_KEY_FEATUREDINDEX + "," + DatabaseHelper.ANIM_KEY_UPLOADEDTIME + "," +
                    DatabaseHelper.ANIM_KEY_DOWNLOADS + "," + DatabaseHelper.ANIM_KEY_RATING + "," + DatabaseHelper.ANIM_PUBLISH_ID + ") values(?,?,?,?,?,?,?,?,?,?,?,?)";

            SQLiteStatement statement = aniamtionDatabase.compileStatement(sql);
            aniamtionDatabase.beginTransaction();
            for (AnimDB animDb : animsDbs) {
                statement.clearBindings();
                statement.bindString(1, String.valueOf(animDb.getAnimAssetId()));
                statement.bindString(2, animDb.getAnimName());
                statement.bindString(3, animDb.getKeyword());
                statement.bindString(4, animDb.getUserid());
                statement.bindString(5, animDb.getUserName());
                statement.bindString(6, String.valueOf(animDb.getAnimType()));
                statement.bindString(7, String.valueOf(animDb.getBonecount()));
                statement.bindString(8, String.valueOf(animDb.getFeaturedindex()));
                statement.bindString(9, animDb.getUploaded());
                statement.bindString(10, String.valueOf(animDb.getDownloads()));
                statement.bindString(11, String.valueOf(animDb.getRating()));
                statement.bindString(12, String.valueOf(animDb.getpublishedId()));
                statement.execute();
            }
            aniamtionDatabase.setTransactionSuccessful();
            aniamtionDatabase.endTransaction();
            if (aniamtionDatabase.isOpen())
                aniamtionDatabase.close();
            if(className.toLowerCase().equals("editorview")){
                    try{
                        if(context == null || ((EditorView)context).animationSelection == null || ((EditorView)context).animationSelection.animationSelectionAdapter== null)
                            return;
                        int position = ((EditorView)context).animationSelection.category.getSelectedItemPosition();
                        ((EditorView)context).animationSelection.onSpinnerItemSelected(position);
                    }catch (ClassCastException ignored){}
            }
            else {
                ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                    ((LoadingActivity) ((Activity) context)).startSceneSelection();
                }
            }
        }
    }

    private void updateDatabase(List<AssetsDB> assetDbs, List<AnimDB> animsDbs, boolean isInsertMethod, boolean isAsset)
    {
        if(isAsset){
            SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            assetsDb.beginTransaction();
            try {
                for (AssetsDB assetsDB : assetDbs) {
                    ContentValues values = new ContentValues();
                    values.put(DatabaseHelper.ASSET_KEY_ASSET_NAME, assetsDB.getAssetName());
                    values.put(DatabaseHelper.ASSET_KEY_IAP, assetsDB.getIap());
                    values.put(DatabaseHelper.ASSET_KEY_ASSETSID, assetsDB.getAssetsId());
                    values.put(DatabaseHelper.ASSET_KEY_TYPE, assetsDB.getType());
                    values.put(DatabaseHelper.ASSET_KEY_NBONES, assetsDB.getNbones());
                    values.put(DatabaseHelper.ASSET_KEY_KEYWORDS, assetsDB.getKeywords());
                    values.put(DatabaseHelper.ASSET_KEY_HASH, assetsDB.getHash());
                    values.put(DatabaseHelper.ASSET_KEY_TIME, assetsDB.getDateTime());
                    values.put(DatabaseHelper.ASSET_KEY_GROUP, assetsDB.getGroup());
                    int value = assetsDb.update(DatabaseHelper.ASSET_TABLE_ASSETS, values, DatabaseHelper.ASSET_KEY_ASSETSID + " = ?", new String[]{String.valueOf(assetsDB.getAssetsId())});
                    if(value == 0)
                        assetsDb.insert(DatabaseHelper.ASSET_TABLE_ASSETS,null,values);
                }
                assetsDb.setTransactionSuccessful();
            } finally {
                assetsDb.endTransaction();
            }
            if (assetsDb.isOpen())
                assetsDb.close();
            if(className.toLowerCase().equals("editorview")){
                if(isAsset)
                    try{
                        if(context == null || ((EditorView)context).assetSelection == null || ((EditorView)context).assetSelection.assetSelectionAdapter == null)
                            return;
                        int position = ((EditorView)context).assetSelection.category.getSelectedItemPosition();
                        ((EditorView)context).assetSelection.onSpinnerItemSelected(position);
                    }catch (ClassCastException ignored){}
            }
            else {
                ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                    ((LoadingActivity) ((Activity) context)).startSceneSelection();
                }
            }
        }
        else{
            SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            animDb.beginTransaction();
            try {
                for (AnimDB animDB : animsDbs) {
                    ContentValues values = new ContentValues();
                    values.put(DatabaseHelper.ANIM_KEY_ASSETSID, animDB.getAnimAssetId());
                    values.put(DatabaseHelper.ANIM_KEY_ANIM_NAME, animDB.getAnimName());
                    values.put(DatabaseHelper.ANIM_KEY_KEYWORD, animDB.getKeyword());
                    values.put(DatabaseHelper.ANIM_KEY_USERID, animDB.getUserid());
                    values.put(DatabaseHelper.ANIM_KEY_USERNAME, animDB.getUserName());
                    values.put(DatabaseHelper.ANIM_KEY_TYPE, animDB.getAnimType());
                    values.put(DatabaseHelper.ANIM_KEY_BONECOUNT, animDB.getBonecount());
                    values.put(DatabaseHelper.ANIM_KEY_FEATUREDINDEX, animDB.getFeaturedindex());
                    values.put(DatabaseHelper.ANIM_KEY_UPLOADEDTIME, animDB.getUploaded());
                    values.put(DatabaseHelper.ANIM_KEY_DOWNLOADS, animDB.getDownloads());
                    values.put(DatabaseHelper.ANIM_KEY_RATING, animDB.getRating());
                    values.put(DatabaseHelper.ANIM_PUBLISH_ID, animDB.getpublishedId());
                    int value = animDb.update(DatabaseHelper.ANIM_TABLE_ANIMASSETS, values, DatabaseHelper.ANIM_KEY_ASSETSID + " = ?", new String[]{String.valueOf(animDB.getAnimAssetId())});
                    if(value == 0)
                        animDb.insert(DatabaseHelper.ANIM_TABLE_ANIMASSETS,null,values);
                }
                animDb.setTransactionSuccessful();

            } finally {
                animDb.endTransaction();
            }
            if (animDb.isOpen())
                animDb.close();
            if(className.toLowerCase().equals("editorview")){
                if(isAsset)
                    try{
                        if(context == null || ((EditorView)context).animationSelection == null || ((EditorView)context).animationSelection.animationSelectionAdapter== null)
                            return;
                        int position = ((EditorView)context).animationSelection.category.getSelectedItemPosition();
                        ((EditorView)context).animationSelection.onSpinnerItemSelected(position);
                    }catch (ClassCastException ignored){}
            }
            else {
                ((LoadingActivity) ((Activity) context)).loadingBar.setProgress(((LoadingActivity) ((Activity) context)).loadingBar.getProgress() + 25);
                if (((LoadingActivity) ((Activity) context)).loadingBar.getProgress() >= 100) {
                    ((LoadingActivity) ((Activity) context)).startSceneSelection();
                }
            }
        }
    }
}