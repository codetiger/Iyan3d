package com.smackall.animator.common;

import android.app.Activity;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.AsyncTask;
import android.os.Environment;
import android.preference.PreferenceManager;

import com.smackall.animator.AnimationAssetsView.AnimDB;
import com.smackall.animator.AnimationAssetsView.AnimationDatabaseHandler;
import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.AssetsViewController.AssetsViewDatabaseHandler;
import com.smackall.animator.sceneSelectionView.DB.SceneDB;
import com.smackall.animator.sceneSelectionView.DB.SceneSelectionDatabaseHandler;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by Sabish on 21/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class WholeDatabaseCreator {

    static Context mContext;
    static Activity mActivity;

    static AssetsManager assetsManager;
    static SQLiteDatabase sceneDatabase;
    private static final String TABLE_SCENES = "Scenes";
    private static final String KEY_ID = "id";
    private static final String KEY_SCENE_NAME = "sceneName";
    private static final String KEY_IMAGE = "image";
    private static final String KEY_TIME = "time";

    static AssetsViewDatabaseHandler assetsViewDatabaseHandler;
    static String strForAssets = null;


    static AnimationDatabaseHandler animationDatabaseHandler;
    static String strForAnim = null;

    public static DefaultHttpClient assetsJsonConnection = new DefaultHttpClient() , animationJsonConnection = new DefaultHttpClient();


    public static void backGroundTaskLoader(Context context, Activity activity){
        mContext = context;
        mActivity = activity;
        sceneDatabaseLoader();
    }


    private static void sceneDatabaseLoader(){

        assetsManager = new AssetsManager();
        try{
            File dir = new File(Constant.sceneDatabaseDirectory);
            if(!dir.exists())
                dir.mkdirs();

            File sourceDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/scenes/");
            if(!sourceDir.exists()) {
                sourceDir.mkdirs();

                assetsManager.copyFile(mContext, "Stay Hungry Stay Foolish.png", md5("Stay Hungry Stay Foolish"),".png",null);
            }

            sceneDatabase = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

            String CREATE_CONTACTS_TABLE = "CREATE TABLE " + TABLE_SCENES + "("
                    + KEY_ID + " INTEGER PRIMARY KEY," + KEY_SCENE_NAME + " TEXT,"
                    + KEY_IMAGE + " TEXT," + KEY_TIME + " TEXT"+ ")";
            sceneDatabase.execSQL(CREATE_CONTACTS_TABLE);
        }
        catch (SQLiteException e) {
        }
        SceneSelectionDatabaseHandler db;
        db = new SceneSelectionDatabaseHandler(mContext);
        if(db.getScensCount() == 0) {
            String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
            db.addScene(new SceneDB("Stay Hungry Stay Foolish", md5("Stay Hungry Stay Foolish"), date));
            AssetsManager assetsManager = new AssetsManager();
            assetsManager.copyFile(mContext, "sample.png", Constant.md5("Stay Hungry Stay Foolish"), ".png", null);
            assetsManager.copyFile(mContext, "sample.sgb",Constant.md5("Stay Hungry Stay Foolish"),".sgb",Environment.getExternalStorageDirectory()+"/Android/data/"+mContext.getPackageName()+"/projects");
        }

        assetsDatabaseLoader();
    }

    private static String md5(String s) {

        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i=0; i<messageDigest.length; i++)
                hexString.append(Integer.toHexString(0xFF & messageDigest[i]));
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return "";
    }

    private static void assetsDatabaseLoader(){

        try {
            File dir = new File(Constant.assetsDatabaseDirectory);
            if (!dir.exists())
                dir.mkdirs();

            File sourceDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/images/");
            if (!sourceDir.exists()) {
                sourceDir.mkdirs();
            }

            Constant.assetsDatabase = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(mContext);
            assetsViewDatabaseHandler.onCreate(Constant.assetsDatabase);
        } catch (SQLiteException e) {
        }
        finally {
            loadAnimDatabaseFromJson(mContext, mActivity);
            loadDatabaseFromJson(mContext, mActivity);
        }

    }


    private static  void loadDatabaseFromJson(Context context, Activity activity){
            mContext = context;
            mActivity = activity;
        new JSONParseForAssets().execute();

    }

    private static class JSONParseForAssets extends AsyncTask<String, String, JSONObject> {
        @Override
        protected JSONObject doInBackground(String... params) {
            strForAssets = "";
            try {

                HttpGet get = new HttpGet("http://iyan3dapp.com/appapi/json/assetsDetail.json");
                HttpParams httpParameters = new BasicHttpParams();
                int timeoutConnection = 4000;
                HttpConnectionParams.setConnectionTimeout(httpParameters, timeoutConnection);
                int timeoutSocket = 6000;
                HttpConnectionParams.setSoTimeout(httpParameters, timeoutSocket);

                HttpResponse httpResponse = assetsJsonConnection.execute(get);
                strForAssets = EntityUtils.toString(httpResponse.getEntity());
                //strForAssets =  Constant.readFile(Environment.getExternalStorageDirectory() + "/Iyan3d/assetsDetail.txt");
            } catch (Exception e) {
                System.out.println("Connection TimeOut");
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
                    if(assetsViewDatabaseHandler.getAssetsCount(Constant.ASSETS_DB.ASSETS_TABLE.getValue()) != 0) {
                        System.out.println("Assets Json Parsing Completed : " + assetsViewDatabaseHandler.getAssetsCount(Constant.ASSETS_DB.ASSETS_TABLE.getValue()));
                    }
                }

                Constant.assetsPrefs.edit().putBoolean("added", false).apply();
            }
        }
    }

    private static void loadAnimDatabaseFromJson(Context context, Activity activity){
        mContext = context;
        mActivity = activity;

        try {
            File dir = new File(Constant.assetsDatabaseDirectory);
            if (!dir.exists())
                dir.mkdirs();

            File sourceDir = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/images/");
            if (!sourceDir.exists()) {
                sourceDir.mkdirs();
            }

            Constant.animDatabase = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
            animationDatabaseHandler = new AnimationDatabaseHandler(mContext);
            animationDatabaseHandler.onCreate(Constant.animDatabase);
        } catch (SQLiteException e) {
        }

        //new JSONParseForAnim().execute();

    }

    private static class JSONParseForAnim extends AsyncTask<String, String, JSONObject> {
        @Override
        protected JSONObject doInBackground(String... params) {

            JSONObject json = null;
            strForAnim = "";
            try {
                HttpGet get = new HttpGet("http://iyan3dapp.com/appapi/json/animationDetail.json");
                HttpParams httpParameters = new BasicHttpParams();
                int timeoutConnection = 4000;
                HttpConnectionParams.setConnectionTimeout(httpParameters, timeoutConnection);
                int timeoutSocket = 6000;
                HttpConnectionParams.setSoTimeout(httpParameters, timeoutSocket);
                HttpResponse httpResponse = animationJsonConnection.execute(get);
                strForAnim = EntityUtils.toString(httpResponse.getEntity());

                //strForAnim =  Constant.readFile(Environment.getExternalStorageDirectory()+"/Iyan3d/animationDetail.txt");

            } catch (Exception e) {
                System.out.println("Connection TimeOut");
            }
            finally {

            }

            System.out.println("url for Animation : " + "http://iyan3dapp.com/appapi/json/animationDetail.json");

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
                    System.out.println("Animation Json Parsing Completed : " + animationDatabaseHandler.getAssetsCount("AnimAssets"));
                }
            }

            Constant.animPrefs.edit().putBoolean("animAdded", false).apply();
        }
    }

}
