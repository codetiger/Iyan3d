package com.smackall.animator.sceneSelectionView.DB;

/**
 * Created by Sabish on 05/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.smackall.animator.common.Constant;

import java.util.ArrayList;
import java.util.List;

public class SceneSelectionDatabaseHandler extends SQLiteOpenHelper {
    private static final int DATABASE_VERSION = 1;
    private static final String DATABASE_NAME = "sceneManager";
    private static final String TABLE_SCENES= "Scenes";
    private static final String KEY_ID = "id";
    private static final String KEY_SCENE_NAME = "sceneName";
    private static final String KEY_IMAGE = "image";
    private static final String KEY_TIME = "time";

    public SceneSelectionDatabaseHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
        //3rd argument to be passed is CursorFactory instance
    }

    // Creating Tables
    @Override
    public void onCreate(SQLiteDatabase db) {
        db.close();
        String CREATE_SCENES_TABLE = "CREATE TABLE " + TABLE_SCENES + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_SCENE_NAME + " TEXT,"
                + KEY_IMAGE + " TEXT," + KEY_TIME + " TEXT" + ")";
        db.execSQL(CREATE_SCENES_TABLE);
    }

    // Upgrading database
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Drop older table if existed
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_SCENES);

        // Create tables again
        onCreate(db);
        db.close();
    }

    // code to add the new contact
    public void addScene(SceneDB scenes) {

        SQLiteDatabase db =  SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();
        values.put(KEY_SCENE_NAME, scenes.getName()); // Contact Name
        values.put(KEY_IMAGE, scenes.getImage()); // Contact Phone
        values.put(KEY_TIME, scenes.getTime());

        // Inserting Row
        db.insert(TABLE_SCENES, null, values);
        //2nd argument is String containing nullColumnHack
        db.close(); // Closing database connection
    }

    // code to get the single contact
     SceneDB getScene(String name) {
        SQLiteDatabase db = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        Cursor cursor = db.query(TABLE_SCENES, new String[]{KEY_ID,
                        KEY_SCENE_NAME, KEY_IMAGE, KEY_TIME}, KEY_SCENE_NAME + "=?",
                new String[]{String.valueOf(name)}, null, null, null, null);

        if (cursor != null)
            cursor.moveToFirst();

        SceneDB sceneDB = new SceneDB(Integer.parseInt(cursor.getString(0)),
                cursor.getString(1), cursor.getString(2), cursor.getString(3));

        cursor.close();
        // return contact
        return sceneDB;
    }

    // code to get all contacts in a list view
    public List<SceneDB> getAllScenes(String searchName, String newName) {
        List<SceneDB> sceneList = new ArrayList<SceneDB>();
        // Select All Query
        SQLiteDatabase db = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor;
        String selectQuery;
        if (searchName.length() != 0 && newName == "") {
            System.out.println("Search Function Called");
            Cursor c = db.rawQuery("SELECT  * FROM " + TABLE_SCENES + " WHERE " + KEY_SCENE_NAME + " LIKE " + "'"+searchName+"%'", null );
            try{
                if (c.moveToFirst()) {
                    do {
                        SceneDB scenes = new SceneDB();
                        scenes.setID(Integer.parseInt(c.getString(0)));
                        scenes.setName(c.getString(1));
                        scenes.setImage(c.getString(2));
                        scenes.setTime(c.getString(3));
                        // Adding contact to list
                        sceneList.add(scenes);
                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                db.close();
            }

        }
        else if(newName.length() != 0){
            Cursor c = db.rawQuery("SELECT  * FROM " + TABLE_SCENES + " WHERE " + KEY_SCENE_NAME + " LIKE " + "'"+newName+"'", null );
            System.out.println("Dublicate Function Called");
            try{
                if (c.moveToFirst()) {
                    do {
                        SceneDB scenes = new SceneDB();
                        scenes.setID(Integer.parseInt(c.getString(0)));
                        scenes.setName(c.getString(1));
                        scenes.setImage(c.getString(2));
                        scenes.setTime(c.getString(3));
                        // Adding contact to list
                        sceneList.add(scenes);

                    } while (c.moveToNext());
                }
            }
            finally {
                c.close();
                db.close();
            }
        }
        else {
            selectQuery = "SELECT  * FROM " + TABLE_SCENES;
            cursor = db.rawQuery(selectQuery, null);
            System.out.println("Function Called");
            try{
                if (cursor.moveToFirst()) {
                    do {
                        SceneDB scenes = new SceneDB();
                        scenes.setID(Integer.parseInt(cursor.getString(0)));
                        scenes.setName(cursor.getString(1));
                        scenes.setImage(cursor.getString(2));
                        scenes.setTime(cursor.getString(3));
                        // Adding contact to list
                        sceneList.add(scenes);
                    } while (cursor.moveToNext());
                }
            }
            finally {
                cursor.close();
                db.close();
            }
        }
        // looping through all rows and adding to list
        // return contact list
        System.out.println("Scene List Size " + sceneList.size());
        return sceneList;
    }

    // code to update the single contact
    public int updateScenes(SceneDB scenes) {
        SQLiteDatabase db = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        ContentValues values = new ContentValues();
        values.put(KEY_SCENE_NAME, scenes.getName());
        values.put(KEY_IMAGE, scenes.getImage());
        values.put(KEY_TIME, scenes.getTime());

        // updating row
        return db.update(TABLE_SCENES, values, KEY_ID + " = ?", new String[] { String.valueOf(scenes.getID()) });
    }

    // Deleting single contact
    public void deleteScene(SceneDB scenes) {
        SQLiteDatabase db = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        db.delete(TABLE_SCENES, KEY_ID + " = ?", new String[] { String.valueOf(scenes.getID()) });
        db.close();
    }

    // Getting contacts Count
    public int getScensCount() {
        String countQuery = "SELECT  * FROM " + TABLE_SCENES;
        SQLiteDatabase db = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = db.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        return count;
    }
}