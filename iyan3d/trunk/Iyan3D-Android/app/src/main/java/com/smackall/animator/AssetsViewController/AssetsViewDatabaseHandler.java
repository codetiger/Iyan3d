package com.smackall.animator.AssetsViewController;

/**
 * Created by Sabish on 14/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 * */

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.smackall.animator.common.Constant;

import java.util.ArrayList;
import java.util.List;


public class AssetsViewDatabaseHandler extends SQLiteOpenHelper {

    private static final int DATABASE_VERSION = 1;
    public static final String DATABASE_NAME = "assetsDatabase";
    public static final String TABLE_ASSETS= "Assets";
    public static final String TABLE_MY_ASSETS= "MyLibrary";
    public static final String KEY_ID = "id";
    public static final String KEY_ASSET_NAME = "assetName";
    public static final String KEY_IAP = "iap";
    public static final String KEY_ASSETSID = "assetsId";
    public static final String KEY_TYPE = "type";
    public static final String KEY_NBONES = "nbones";
    public static final String KEY_KEYWORDS = "keywords";
    public static final String KEY_HASH = "hash";
    public static final String KEY_TIME = "time";

    Context mcontext;


    public AssetsViewDatabaseHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
        mcontext = context;
        //3rd argument to be passed is CursorFactory instance
    }

    // Creating Tables
    @Override
    public void onCreate(SQLiteDatabase assetsDb) {
        String CREATE_SCENES_TABLE = "CREATE TABLE IF NOT EXISTS " + TABLE_ASSETS + "("
                + KEY_ID + " INTEGER PRIMARY KEY, " + KEY_ASSET_NAME + " TEXT,"
                + KEY_IAP + " TEXT,"  + KEY_ASSETSID + " INTEGER," + KEY_TYPE + " TEXT," + KEY_NBONES + " TEXT," +
                KEY_KEYWORDS + " TEXT," +  KEY_HASH + " TEXT," +  KEY_TIME + " TEXT" + ")";
        assetsDb.execSQL(CREATE_SCENES_TABLE);
    }

    public void createMyLibraryTable(SQLiteDatabase assetsDb){
        assetsDb =  SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_SCENES_TABLE = "CREATE TABLE IF NOT EXISTS " + "MyLibrary" + "("
                + KEY_ID + " INTEGER PRIMARY KEY, " + KEY_ASSET_NAME + " TEXT,"
                + KEY_IAP + " TEXT,"  + KEY_ASSETSID + " INTEGER," + KEY_TYPE + " TEXT," + KEY_NBONES + " TEXT," +
                KEY_KEYWORDS + " TEXT," +  KEY_HASH + " TEXT," +  KEY_TIME + " TEXT" + ")";
        assetsDb.execSQL(CREATE_SCENES_TABLE);
    }

    // Upgrading database
    @Override
    public void onUpgrade(SQLiteDatabase assetsDb, int oldVersion, int newVersion) {
        // Drop older table if existed
        assetsDb.execSQL("DROP TABLE IF EXISTS " + TABLE_ASSETS);
        // Create tables again
        onCreate(assetsDb);
        assetsDb.close();
    }

    // code to add the new contact
    public void addAssets(AssetsDB assetsDB, String tableName) {

        SQLiteDatabase assetsDb =  SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();
        values.put(KEY_ASSET_NAME, assetsDB.getAssetName());
        values.put(KEY_IAP, assetsDB.getIap());
        values.put(KEY_ASSETSID, assetsDB.getAssetsId());
        values.put(KEY_TYPE, assetsDB.getType());
        values.put(KEY_NBONES, assetsDB.getNbones());
        values.put(KEY_KEYWORDS, assetsDB.getKeywords());
        values.put(KEY_HASH, assetsDB.getHash());
        values.put(KEY_TIME, assetsDB.getDateTime());

        // Inserting Row
        if(tableName.equals("Assets"))
        assetsDb.insert(TABLE_ASSETS, null, values);
        if(tableName.equals("MyLibrary"))
            assetsDb.insert(TABLE_MY_ASSETS, null, values);
        //2nd argument is String containing nullColumnHack
        assetsDb.close(); // Closing database connection
    }

    // code to get the single contact
    SQLiteDatabase getAssets(String name) {
        SQLiteDatabase assetsDB = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        Cursor cursor = assetsDB.query(TABLE_ASSETS, new String[]{KEY_ID,
                        KEY_ASSET_NAME, KEY_IAP, KEY_ASSETSID, KEY_TYPE, KEY_NBONES, KEY_KEYWORDS,KEY_HASH,KEY_TIME }, KEY_ASSET_NAME + "=?",
                new String[]{String.valueOf(name)}, null, null, null, null);

        if (cursor != null)
            cursor.moveToFirst();

        AssetsDB assetsDB1 = new AssetsDB(Integer.parseInt(cursor.getString(0)),
                cursor.getString(1), cursor.getString(2), cursor.getInt(3),
                cursor.getString(4), cursor.getString(5), cursor.getString(6), cursor.getString(7), cursor.getString(8));

        cursor.close();
        // return contact
        return assetsDB;
    }

    // code to get all contacts in a list view
    public List<AssetsDB> getAllAssets(String searchName, String type, String tableName) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        // Select All Query
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor;
        String selectQuery;
        if (searchName.length() != 0 && type == "") {
            System.out.println("Search Function Called");
            Cursor c = assetsDb.rawQuery("SELECT  * FROM " + tableName + " WHERE " + KEY_ASSET_NAME + " LIKE " + "'"+searchName+"%'", null );
            try{
                if (c.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetName(c.getString(1));
                        assets.setIap(c.getString(2));
                        assets.setAssetsId(c.getInt(3));
                        assets.setType(c.getString(4));
                        assets.setNbones(c.getString(5));
                        assets.setKeywords(c.getString(6));
                        assets.setHash(c.getString(7));
                        assets.setDateTime(c.getString(8));

                        // Adding contact to list
                        assetsList.add(assets);
                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                assetsDb.close();
            }

        }
        else if(type.length() != 0){
            Cursor c = assetsDb.rawQuery("SELECT  * FROM " + tableName + " WHERE " + KEY_TYPE + " LIKE " + "'%"+type+"%'", null );
            System.out.println("Dublicate Function Called");
            try{
                if (c.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetName(c.getString(1));
                        assets.setIap(c.getString(2));
                        assets.setAssetsId(c.getInt(3));
                        assets.setType(c.getString(4));
                        assets.setNbones(c.getString(5));
                        assets.setKeywords(c.getString(6));
                        assets.setHash(c.getString(7));
                        assets.setDateTime(c.getString(8));
                        // Adding contact to list
                        assetsList.add(assets);
                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                assetsDb.close();
            }
        }
        else {
            selectQuery = "SELECT  * FROM " + tableName;
            cursor = assetsDb.rawQuery(selectQuery, null);
            System.out.println("Function Called");
            try{
                if (cursor.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(cursor.getString(0)));
                        assets.setAssetName(cursor.getString(1));
                        assets.setIap(cursor.getString(2));
                        assets.setAssetsId(cursor.getInt(3));
                        assets.setType(cursor.getString(4));
                        assets.setNbones(cursor.getString(5));
                        assets.setKeywords(cursor.getString(6));
                        assets.setHash(cursor.getString(7));
                        assets.setDateTime(cursor.getString(8));
                        // Adding contact to list
                        assetsList.add(assets);
                    } while (cursor.moveToNext());
                }
            }
            finally {
                cursor.close();
                assetsDb.close();
            }
        }
        // looping through all rows and adding to list
        // return contact list
        return assetsList;
    }

    // code to update the single contact
    public int updateAssets(AssetsDB assetsDB, String tableName) {
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();

        values.put(KEY_ASSET_NAME, assetsDB.getAssetName()); // Contact Name
        values.put(KEY_IAP, assetsDB.getIap()); // Contact Phone
        values.put(KEY_ASSETSID, assetsDB.getAssetsId());
        values.put(KEY_TYPE, assetsDB.getType());
        values.put(KEY_NBONES, assetsDB.getNbones());
        values.put(KEY_KEYWORDS, assetsDB.getKeywords());
        values.put(KEY_HASH, assetsDB.getHash());
        values.put(KEY_TIME, assetsDB.getDateTime());

        // updating row
        return assetsDb.update(tableName, values, KEY_ASSETSID + " = ?", new String[] { String.valueOf(assetsDB.getAssetsId()) });
    }

    // Deleting single contact
    public void deleteAssets(AssetsDB assetsDB) {
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        assetsDb.delete(TABLE_ASSETS, KEY_ID + " = ?", new String[] { String.valueOf(assetsDB.getID()) });
        assetsDb.close();
    }

    // Getting contacts Count
    public int getAssetsCount(String tableName) {
        String countQuery = "SELECT  * FROM " + tableName;
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = assetsDb.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        return count;
    }
}