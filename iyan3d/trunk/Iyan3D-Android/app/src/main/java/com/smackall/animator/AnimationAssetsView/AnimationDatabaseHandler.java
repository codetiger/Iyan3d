package com.smackall.animator.AnimationAssetsView;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.smackall.animator.common.Constant;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish on 21/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 * */

public class
        AnimationDatabaseHandler extends SQLiteOpenHelper {

    private static final int DATABASE_VERSION = 1;
    public static final String DATABASE_NAME = "animAssetsDatabase";
    public static final String TABLE_ANIMASSETS= "AnimAssets";
    public static final String TABLE_MYANIMASSETS= "MyAnimation";
    public static final String KEY_ID = "id";
    public static final String KEY_ASSETSID = "animAssetId";
    public static final String KEY_ANIM_NAME = "animName";
    public static final String KEY_KEYWORD = "keyword";
    public static final String KEY_USERID = "userid";
    public static final String KEY_USERNAME = "username";
    public static final String KEY_TYPE = "type";
    public static final String KEY_BONECOUNT = "bonecount";
    public static final String KEY_FEATUREDINDEX = "featuredindex";
    public static final String KEY_UPLOADEDTIME = "uploaded";
    public static final String KEY_DOWNLOADS= "downloads";
    public static final String KEY_RATING= "rating";
    public static final String PUBLISH_ID = "publishId";

    Context mcontext;


    public AnimationDatabaseHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
        mcontext = context;
    }

    /** Creating Aniamtion Tables **/
    @Override
    public void onCreate(SQLiteDatabase animDb) {
        String CREATE_SCENES_TABLE = "CREATE TABLE IF NOT EXISTS " + TABLE_ANIMASSETS + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_ASSETSID + " TEXT,"
                + KEY_ANIM_NAME + " TEXT,"  + KEY_KEYWORD + " TEXT," + KEY_USERID + " TEXT," + KEY_USERNAME + " TEXT," + KEY_TYPE + " TEXT," +
                KEY_BONECOUNT + " TEXT," +  KEY_FEATUREDINDEX + " TEXT," +  KEY_UPLOADEDTIME + " TEXT," +
                KEY_DOWNLOADS + " INTEGER," + KEY_RATING + " TEXT" + ")";
        animDb.execSQL(CREATE_SCENES_TABLE);
    }

    /** Creating My Aniamtion Tables **/
    public void createMyAnimationTable(SQLiteDatabase animDB){
        animDB = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_SCENES_TABLE = "CREATE TABLE IF NOT EXISTS " + TABLE_MYANIMASSETS + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_ASSETSID + " TEXT,"
                + KEY_ANIM_NAME + " TEXT,"  + KEY_KEYWORD + " TEXT," + KEY_USERID + " TEXT," + KEY_USERNAME + " TEXT," + KEY_TYPE + " TEXT," +
                KEY_BONECOUNT + " TEXT," +  KEY_FEATUREDINDEX + " TEXT," +  KEY_UPLOADEDTIME + " TEXT," +
                KEY_DOWNLOADS + " INTEGER," + KEY_RATING + " TEXT," +  PUBLISH_ID + " TEXT" +")";
        animDB.execSQL(CREATE_SCENES_TABLE);
    }

    /** Upgrading database **/
    @Override
    public void onUpgrade(SQLiteDatabase animDB, int oldVersion, int newVersion) {
        animDB.execSQL("DROP TABLE IF EXISTS " + TABLE_ANIMASSETS);
        onCreate(animDB);
        animDB.close();
    }

    /** Add new Assets in Animation Table or My Animation table **/
    public void addAssets(AnimDB animDB, String tableName) {

        SQLiteDatabase assetsDb =  SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();
        values.put(KEY_ASSETSID, animDB.getAnimAssetId());
        values.put(KEY_ANIM_NAME, animDB.getAnimName());
        values.put(KEY_KEYWORD, animDB.getKeyword());
        values.put(KEY_USERID, animDB.getUserid());
        values.put(KEY_USERNAME, animDB.getUserName());
        values.put(KEY_TYPE, animDB.getAnimType());
        values.put(KEY_BONECOUNT, animDB.getBonecount());
        values.put(KEY_FEATUREDINDEX, animDB.getFeaturedindex());
        values.put(KEY_UPLOADEDTIME, animDB.getUploaded());
        values.put(KEY_DOWNLOADS, animDB.getDownloads());
        values.put(KEY_RATING, animDB.getRating());

        if(tableName.equals("animation"))
            assetsDb.insert(TABLE_ANIMASSETS, null, values);
        if(tableName.equals("myanimation")) {
            values.put(PUBLISH_ID, animDB.getRating());
            assetsDb.insert(TABLE_MYANIMASSETS, null, values);
        }
        /** Closing database connection **/
        assetsDb.close();
    }

    // Get assets detail in List in Animation Table or My Animation Table
    public List<AnimDB> getAllAssets(String searchName, String filter, String table, String animationType) {

        /** List for get Assets Detail **/
        List<AnimDB> animList = new ArrayList<AnimDB>();

        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY); //Sql Database

        /** Condition for Search the Assets in Database **/
        if (searchName.length() != 0 && filter == "") {

            Cursor c = animDb.query(table, new String[]{KEY_ID,
                    KEY_ASSETSID, KEY_ANIM_NAME, KEY_KEYWORD, KEY_USERID, KEY_USERNAME, KEY_TYPE, KEY_BONECOUNT, KEY_FEATUREDINDEX, KEY_UPLOADEDTIME,
                    KEY_DOWNLOADS, KEY_RATING}, KEY_ANIM_NAME + " LIKE " + "'"+searchName+"%'"+" and " + KEY_TYPE +"= " + animationType, null, null, null, null);
            try{
                if (c.moveToFirst()) {
                    do {
                        AnimDB assets = new AnimDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetsId(c.getString(1));
                        assets.setAnimName(c.getString(2));
                        assets.setKeyword(c.getString(3));
                        assets.setUserid(c.getString(4));
                        assets.setUserName(c.getString(5));
                        assets.setAnimType(c.getString(6));
                        assets.setBonecount(c.getString(7));
                        assets.setFeaturedindex(c.getInt(8));
                        assets.setUploaded(c.getString(9));
                        assets.setDownloads(c.getInt(10));
                        assets.setRating(c.getInt(11));

                        // Adding contact to list
                        animList.add(assets);
                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                animDb.close();
            }

        }
        else if(filter.length() != 0){

            System.out.println("Dublicate Function Called");
            Cursor c = null;
            if(table.startsWith("AnimAss")) {
                 c = animDb.query(table, new String[]{KEY_ID,
                        KEY_ASSETSID, KEY_ANIM_NAME, KEY_KEYWORD, KEY_USERID, KEY_USERNAME, KEY_TYPE, KEY_BONECOUNT, KEY_FEATUREDINDEX, KEY_UPLOADEDTIME,
                        KEY_DOWNLOADS, KEY_RATING}, KEY_TYPE + "= " + animationType, null, null, null, filter + " DESC");
            }
            else if(table.startsWith("MyAnimation")){
                c = animDb.query(table, new String[]{KEY_ID,
                        KEY_ASSETSID, KEY_ANIM_NAME, KEY_KEYWORD, KEY_USERID, KEY_USERNAME, KEY_TYPE, KEY_BONECOUNT, KEY_FEATUREDINDEX, KEY_UPLOADEDTIME,
                        KEY_DOWNLOADS, KEY_RATING, PUBLISH_ID}, KEY_TYPE + "= " + animationType, null, null, null, filter + " DESC");
            }

            try{
                if (c.moveToFirst()) {
                    do {
                        AnimDB assets = new AnimDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetsId(c.getString(1));
                        assets.setAnimName(c.getString(2));
                        assets.setKeyword(c.getString(3));
                        assets.setUserid(c.getString(4));
                        assets.setUserName(c.getString(5));
                        assets.setAnimType(c.getString(6));
                        assets.setBonecount(c.getString(7));
                        assets.setFeaturedindex(c.getInt(8));
                        assets.setUploaded(c.getString(9));
                        assets.setDownloads(c.getInt(10));
                        assets.setRating(c.getInt(11));
                        if(table.startsWith("MyAnimation")){
                            assets.setPublishedId(c.getString(12));
                        }
                        // Adding contact to list
                        animList.add(assets);
                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                animDb.close();
            }
        }
        // looping through all rows and adding to list
        // return contact list
        return animList;
    }

    // code to update the single contact
    public int updateAssets(AnimDB animDB, String table) {
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();

        values.put(KEY_ASSETSID, animDB.getAnimAssetId());
        values.put(KEY_ANIM_NAME, animDB.getAnimName());
        values.put(KEY_KEYWORD, animDB.getKeyword());
        values.put(KEY_USERID, animDB.getUserid());
        values.put(KEY_USERNAME, animDB.getUserName());
        values.put(KEY_TYPE, animDB.getAnimType());
        values.put(KEY_BONECOUNT, animDB.getBonecount());
        values.put(KEY_FEATUREDINDEX, animDB.getFeaturedindex());
        values.put(KEY_UPLOADEDTIME, animDB.getUploaded());
        values.put(KEY_DOWNLOADS, animDB.getDownloads());
        values.put(KEY_RATING, animDB.getRating());

        // updating row
        if(table.startsWith("animasset")) {
            return animDb.update(TABLE_ANIMASSETS, values, KEY_ASSETSID + " = ?", new String[]{String.valueOf(animDB.getAnimAssetId())});
        }
        if(table.startsWith("myanimation")) {
            return animDb.update(TABLE_MYANIMASSETS, values, KEY_ASSETSID + " = ?", new String[]{String.valueOf(animDB.getAnimAssetId())});
        }

        return 0;
    }

    // Deleting single contact
    public void deleteAssets(AnimDB animDBClass) {
        SQLiteDatabase animDB = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        animDB.delete(TABLE_ANIMASSETS, KEY_ID + " = ?", new String[] { String.valueOf(animDBClass.getID()) });
        animDB.close();
    }

    // Getting contacts Count
    public int getAssetsCount(String tableName) {
        String countQuery = "SELECT  * FROM " + tableName;
        SQLiteDatabase animDB = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = animDB.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        return count;
    }
}

