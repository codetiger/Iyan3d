package com.smackall.animator.Helper;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish.M on 2/1/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class DatabaseHelper {

    private final String SCENE_TABLE = "Scenes";
    private final String SCENE_KEY_ID = "id";
    private final String SCENE_KEY_SCENE_NAME = "sceneName";
    private final String SCENE_KEY_IMAGE = "image";
    private final String SCENE_KEY_TIME = "time";

    private final String ANIM_TABLE_ANIMASSETS = "AnimAssets";
    private final String ANIM_TABLE_MYANIMASSETS = "MyAnimation";
    private final String ANIM_KEY_ID = "id";
    private final String ANIM_KEY_ASSETSID = "animAssetId";
    private final String ANIM_KEY_ANIM_NAME = "animName";
    private final String ANIM_KEY_KEYWORD = "keyword";
    private final String ANIM_KEY_USERID = "userid";
    private final String ANIM_KEY_USERNAME = "username";
    private final String ANIM_KEY_TYPE = "type";
    private final String ANIM_KEY_BONECOUNT = "bonecount";
    private final String ANIM_KEY_FEATUREDINDEX = "featuredindex";
    private final String ANIM_KEY_UPLOADEDTIME = "uploaded";
    private final String ANIM_KEY_DOWNLOADS = "downloads";
    private final String ANIM_KEY_RATING = "rating";
    private final String ANIM_PUBLISH_ID = "publishId";

    public final String ASSET_TABLE_ASSETS = "Assets";
    public final String ASSET_TABLE_MY_ASSETS = "MyLibrary";
    public final String ASSET_KEY_ID = "id";
    public final String ASSET_KEY_ASSET_NAME = "assetName";
    public final String ASSET_KEY_IAP = "iap";
    public final String ASSET_KEY_ASSETSID = "assetsId";
    public final String ASSET_KEY_TYPE = "type";
    public final String ASSET_KEY_NBONES = "nbones";
    public final String ASSET_KEY_KEYWORDS = "keywords";
    public final String ASSET_KEY_HASH = "hash";
    public final String ASSET_KEY_TIME = "time";
    public final String ASSET_KEY_GROUP = "groupId";




    public void createDataBase() {
        SQLiteDatabase SCENES_DATABASE = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        String CREATE_SCENES_TABLE = "CREATE TABLE IF NOT EXISTS " + SCENE_TABLE + "("
                + SCENE_KEY_ID + " INTEGER PRIMARY KEY," + SCENE_KEY_SCENE_NAME + " TEXT,"
                + SCENE_KEY_IMAGE + " TEXT," + SCENE_KEY_TIME + " TEXT" + ")";
        SCENES_DATABASE.execSQL(CREATE_SCENES_TABLE);
        if (SCENES_DATABASE.isOpen())
            SCENES_DATABASE.close();

        SQLiteDatabase animAssetDB = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_ANIMASSET_TABLE = "CREATE TABLE IF NOT EXISTS " + ANIM_TABLE_ANIMASSETS + "("
                + ANIM_KEY_ID + " INTEGER PRIMARY KEY," + ANIM_KEY_ASSETSID + " INTEGER,"
                + ANIM_KEY_ANIM_NAME + " TEXT," + ANIM_KEY_KEYWORD + " TEXT," + ANIM_KEY_USERID + " TEXT," + ANIM_KEY_USERNAME + " TEXT," + ANIM_KEY_TYPE + " INTEGER," +
                ANIM_KEY_BONECOUNT + " INTEGER," + ANIM_KEY_FEATUREDINDEX + " INTEGER," + ANIM_KEY_UPLOADEDTIME + " TEXT," +
                ANIM_KEY_DOWNLOADS + " INTEGER," + ANIM_KEY_RATING + " INTEGER," + ANIM_PUBLISH_ID + " INTEGER" + ")";
        animAssetDB.execSQL(CREATE_ANIMASSET_TABLE);
        if (animAssetDB.isOpen())
            animAssetDB.close();

        SQLiteDatabase myAnimDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_MYANIM_TABLE = "CREATE TABLE IF NOT EXISTS " + ANIM_TABLE_MYANIMASSETS + "("
                + ANIM_KEY_ID + " INTEGER PRIMARY KEY," + ANIM_KEY_ASSETSID + " INTEGER,"
                + ANIM_KEY_ANIM_NAME + " TEXT," + ANIM_KEY_KEYWORD + " TEXT," + ANIM_KEY_USERID + " TEXT," + ANIM_KEY_USERNAME + " TEXT," + ANIM_KEY_TYPE + " TEXT," +
                ANIM_KEY_BONECOUNT + " INTEGER," + ANIM_KEY_FEATUREDINDEX + " INTEGER," + ANIM_KEY_UPLOADEDTIME + " TEXT," +
                ANIM_KEY_DOWNLOADS + " INTEGER," + ANIM_KEY_RATING + " INTEGER," + ANIM_PUBLISH_ID + " INTEGER" + ")";
        myAnimDb.execSQL(CREATE_MYANIM_TABLE);
        if (myAnimDb.isOpen())
            myAnimDb.close();

        SQLiteDatabase modelAssetDB = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_MODELASSET_TABLE = "CREATE TABLE IF NOT EXISTS " + ASSET_TABLE_ASSETS + "("
                + ASSET_KEY_ID + " INTEGER PRIMARY KEY, " + ASSET_KEY_ASSET_NAME + " TEXT,"
                + ASSET_KEY_IAP + " INTEGER," + ASSET_KEY_ASSETSID + " INTEGER," + ASSET_KEY_TYPE + " INTEGER," + ASSET_KEY_NBONES + " INTEGER," +
                ASSET_KEY_KEYWORDS + " TEXT," + ASSET_KEY_HASH + " TEXT," + ASSET_KEY_TIME + " TEXT," + ASSET_KEY_GROUP + " INTEGER"+ ")";
        modelAssetDB.execSQL(CREATE_MODELASSET_TABLE);
        if (modelAssetDB.isOpen())
            modelAssetDB.close();

        SQLiteDatabase myModelAssetDB = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String CREATE_MYMODEL_TABLE = "CREATE TABLE IF NOT EXISTS " + "MyLibrary" + "("
                + ASSET_KEY_ID + " INTEGER PRIMARY KEY, " + ASSET_KEY_ASSET_NAME + " TEXT,"
                + ASSET_KEY_IAP + " INTEGER," + ASSET_KEY_ASSETSID + " INTEGER," + ASSET_KEY_TYPE + " INTEGER," + ASSET_KEY_NBONES + " INTEGER," +
                ASSET_KEY_KEYWORDS + " TEXT," + ASSET_KEY_HASH + " TEXT," + ASSET_KEY_TIME + " TEXT" + ")";
        myModelAssetDB.execSQL(CREATE_MYMODEL_TABLE);
        if (myModelAssetDB.isOpen())
            myModelAssetDB.close();

        checkOldDatabaseIsFoundThenCopyTableToNewDatabase();
    }

    public void addNewScene(SceneDB newScene) {
        SQLiteDatabase scenesDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(SCENE_KEY_SCENE_NAME, newScene.getName());
        values.put(SCENE_KEY_IMAGE, newScene.getImage());
        values.put(SCENE_KEY_TIME, newScene.getTime());
        scenesDatabase.insert(SCENE_TABLE, null, values);
        if (scenesDatabase.isOpen())
            scenesDatabase.close();
    }

    public int updateSceneDetails(SceneDB sceneDB) {
        SQLiteDatabase db = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(SCENE_KEY_ID, sceneDB.getID());
        values.put(SCENE_KEY_SCENE_NAME, sceneDB.getName());
        values.put(SCENE_KEY_IMAGE, sceneDB.getImage());
        values.put(SCENE_KEY_TIME, sceneDB.getTime());
        db.update(SCENE_TABLE, values, SCENE_KEY_ID+ " = ?", new String[]{String.valueOf(sceneDB.getID())});
        if (db.isOpen())
            db.close();
        values = null;
        return 0;
    }

    public List<SceneDB> getAllSceneDetailsWithSearch(String searchKeyWords) {
        List<SceneDB> sceneList = new ArrayList<SceneDB>();
        // Select All Query
        SQLiteDatabase sceneDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = null;
        String selectQuery;
        if (searchKeyWords.length() != 0) {
            Cursor c = sceneDatabase.rawQuery("SELECT  * FROM " + SCENE_TABLE + " WHERE " + SCENE_KEY_SCENE_NAME + " LIKE " + "'" + searchKeyWords + "%'", null);
            try {
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
            } catch (Exception e) {
                if (cursor != null)
                    cursor.close();
                if (sceneDatabase.isOpen())
                    sceneDatabase.close();
                return null;
            }
            if (cursor != null)
                cursor.close();
            if (sceneDatabase.isOpen())
                sceneDatabase.close();
            return sceneList;
        }
        return null;
    }

    public List<SceneDB> checkSceneAlreadyExist(String sceneName) {
        List<SceneDB> sceneList = new ArrayList<SceneDB>();
        // Select All Query
        SQLiteDatabase sceneDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = null;
        String selectQuery;
        if (sceneName.length() != 0) {
            Cursor c = sceneDatabase.rawQuery("SELECT  * FROM " + SCENE_TABLE + " WHERE " + SCENE_KEY_SCENE_NAME + " LIKE " + "'" + sceneName + "'", null);
            try {
                if (c.moveToFirst()) {
                    do {
                        SceneDB scenes = new SceneDB();
                        scenes.setID(Integer.parseInt(c.getString(0)));
                        scenes.setName(c.getString(1));
                        scenes.setImage(c.getString(2));
                        scenes.setTime(c.getString(3));
                        sceneList.add(scenes);

                    } while (c.moveToNext());
                }
            } catch (Exception e) {
                if (cursor != null)
                    cursor.close();
                if (sceneDatabase.isOpen())
                    sceneDatabase.close();
                return null;
            }
        }
        return null;
    }

    public List<SceneDB> getAllScenes() {
        List<SceneDB> sceneList = new ArrayList<SceneDB>();
        // Select All Query
        SQLiteDatabase sceneDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor;
        String selectQuery = "SELECT  * FROM " + SCENE_TABLE;
        cursor = sceneDatabase.rawQuery(selectQuery, null);
        try {
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
        } catch (Exception e) {
            if (cursor != null)
                cursor.close();
            if (sceneDatabase.isOpen())
                sceneDatabase.close();
            return null;
        }
        return sceneList;
    }

    public void deleteScene(String sceneName) {
        SQLiteDatabase sceneDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        sceneDatabase.delete("Scenes", "sceneName" + " = ?", new String[]{String.valueOf(sceneName)});
        if (sceneDatabase.isOpen())
            sceneDatabase.close();
    }

    public int getSceneCount() {
        String countQuery = "SELECT  * FROM " + SCENE_TABLE;
        SQLiteDatabase sceneDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = sceneDatabase.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        if (sceneDatabase.isOpen())
            sceneDatabase.close();
        return count;
    }

    public void addNewAnimationAssets(AnimDB animDB) {
        SQLiteDatabase aniamtionDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ANIM_KEY_ASSETSID, animDB.getAnimAssetId());
        values.put(ANIM_KEY_ANIM_NAME, animDB.getAnimName());
        values.put(ANIM_KEY_KEYWORD, animDB.getKeyword());
        values.put(ANIM_KEY_USERID, animDB.getUserid());
        values.put(ANIM_KEY_USERNAME, animDB.getUserName());
        values.put(ANIM_KEY_TYPE, animDB.getAnimType());
        values.put(ANIM_KEY_BONECOUNT, animDB.getBonecount());
        values.put(ANIM_KEY_FEATUREDINDEX, animDB.getFeaturedindex());
        values.put(ANIM_KEY_UPLOADEDTIME, animDB.getUploaded());
        values.put(ANIM_KEY_DOWNLOADS, animDB.getDownloads());
        values.put(ANIM_KEY_RATING, animDB.getRating());
        values.put(ANIM_PUBLISH_ID,animDB.getpublishedId());
        aniamtionDatabase.insert(ANIM_TABLE_ANIMASSETS, null, values);
        if (aniamtionDatabase.isOpen())
            aniamtionDatabase.close();
        values = null;
    }

    public void addNewMyAnimationAssets(AnimDB animDB) {
        SQLiteDatabase myAniamtionDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ANIM_KEY_ASSETSID, animDB.getAnimAssetId());
        values.put(ANIM_KEY_ANIM_NAME, animDB.getAnimName());
        values.put(ANIM_KEY_KEYWORD, animDB.getKeyword());
        values.put(ANIM_KEY_USERID, animDB.getUserid());
        values.put(ANIM_KEY_USERNAME, animDB.getUserName());
        values.put(ANIM_KEY_TYPE, animDB.getAnimType());
        values.put(ANIM_KEY_BONECOUNT, animDB.getBonecount());
        values.put(ANIM_KEY_FEATUREDINDEX, animDB.getFeaturedindex());
        values.put(ANIM_KEY_UPLOADEDTIME, animDB.getUploaded());
        values.put(ANIM_KEY_DOWNLOADS, animDB.getDownloads());
        values.put(ANIM_KEY_RATING, animDB.getRating());
        values.put(ANIM_PUBLISH_ID, animDB.getRating());
        myAniamtionDatabase.insert(ANIM_TABLE_MYANIMASSETS, null, values);
        if (myAniamtionDatabase.isOpen())
            myAniamtionDatabase.close();
        values = null;
    }

    public List<AnimDB> getAllAnimationDetailWithSearch(String table, String searchName, String animationType) {
        /** List for get Assets Detail **/
        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animationDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY); //Sql Database
        /** Condition for Search the Assets in Database **/
        if (searchName.length() != 0) {
            Cursor cursor = animationDatabase.query(table, new String[]{ANIM_KEY_ID,
                    ANIM_KEY_ASSETSID, ANIM_KEY_ANIM_NAME, ANIM_KEY_KEYWORD, ANIM_KEY_USERID, ANIM_KEY_USERNAME, ANIM_KEY_TYPE, ANIM_KEY_BONECOUNT,
                    ANIM_KEY_FEATUREDINDEX, ANIM_KEY_UPLOADEDTIME,
                    ANIM_KEY_DOWNLOADS, ANIM_KEY_RATING}, ANIM_KEY_ANIM_NAME + " LIKE " + "'" + searchName + "%'" + " and " + ANIM_KEY_TYPE + "= " + animationType, null, null, null, null);
            try {
                if (cursor.moveToFirst()) {
                    do {
                        AnimDB assets = new AnimDB();
                        assets.setID(Integer.parseInt(cursor.getString(0)));
                        assets.setAssetsId(cursor.getInt(1));
                        assets.setAnimName(cursor.getString(2));
                        assets.setKeyword(cursor.getString(3));
                        assets.setUserid(cursor.getString(4));
                        assets.setUserName(cursor.getString(5));
                        assets.setAnimType(cursor.getInt(6));
                        assets.setBonecount(cursor.getInt(7));
                        assets.setFeaturedindex(cursor.getInt(8));
                        assets.setUploaded(cursor.getString(9));
                        assets.setDownloads(cursor.getInt(10));
                        assets.setRating(cursor.getInt(11));
                        animList.add(assets);
                    } while (cursor.moveToNext());
                }
            } catch (Exception e) {
                if (cursor != null)
                    cursor.close();
                if (animationDatabase.isOpen())
                    animationDatabase.close();
                return null;
            }

            return animList;
        }
        if (animationDatabase.isOpen())
            animationDatabase.close();
        return null;
    }

    public List<AnimDB> getAnimationDetailsWithColumnName(String columnName, String animationType) {
        /** List for get Assets Detail **/
        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY); //Sql Database
        /** Condition for Search the Assets in Database **/
        if (columnName.length() != 0) {
            Cursor c = null;
            c = animDb.query(ANIM_TABLE_ANIMASSETS, new String[]{ANIM_KEY_ID,
                    ANIM_KEY_ASSETSID, ANIM_KEY_ANIM_NAME, ANIM_KEY_KEYWORD, ANIM_KEY_USERID, ANIM_KEY_USERNAME, ANIM_KEY_TYPE, ANIM_KEY_BONECOUNT, ANIM_KEY_FEATUREDINDEX,
                    ANIM_KEY_UPLOADEDTIME,
                    ANIM_KEY_DOWNLOADS, ANIM_KEY_RATING}, ANIM_KEY_TYPE + "= " + animationType, null, null, null, columnName + " DESC");
            try {
                if (c.moveToFirst()) {
                    do {
                        AnimDB assets = new AnimDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetsId(c.getInt(1));
                        assets.setAnimName(c.getString(2));
                        assets.setKeyword(c.getString(3));
                        assets.setUserid(c.getString(4));
                        assets.setUserName(c.getString(5));
                        assets.setAnimType(c.getInt(6));
                        assets.setBonecount(c.getInt(7));
                        assets.setFeaturedindex(c.getInt(8));
                        assets.setUploaded(c.getString(9));
                        assets.setDownloads(c.getInt(10));
                        assets.setRating(c.getInt(11));
                        animList.add(assets);
                    } while (c.moveToNext());
                }
            } catch (Exception e) {
                if (c != null)
                    c.close();
                if (animDb.isOpen())
                    animDb.close();
                return null;
            }
            c.close();
            if (animDb.isOpen())
                animDb.close();
            return animList;
        }

        if (animDb.isOpen())
            animDb.close();
        return null;
    }

    public List<AnimDB> getMyAnimationDetailsWithColumnName(String columnName, String animationType) {
        /** List for get Assets Detail **/
        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY); //Sql Database
        /** Condition for Search the Assets in Database **/
        if (columnName.length() != 0) {
            Cursor c = null;
            c = animDb.query(ANIM_TABLE_MYANIMASSETS, new String[]{ANIM_KEY_ID,
                    ANIM_KEY_ASSETSID, ANIM_KEY_ANIM_NAME, ANIM_KEY_KEYWORD, ANIM_KEY_USERID, ANIM_KEY_USERNAME, ANIM_KEY_TYPE, ANIM_KEY_BONECOUNT, ANIM_KEY_FEATUREDINDEX, ANIM_KEY_UPLOADEDTIME,
                    ANIM_KEY_DOWNLOADS, ANIM_KEY_RATING, ANIM_PUBLISH_ID}, ANIM_KEY_TYPE + "= " + animationType, null, null, null, columnName + " DESC");
            try {
                if (c.moveToFirst()) {
                    do {
                        AnimDB assets = new AnimDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetsId(c.getInt(1));
                        assets.setAnimName(c.getString(2));
                        assets.setKeyword(c.getString(3));
                        assets.setUserid(c.getString(4));
                        assets.setUserName(c.getString(5));
                        assets.setAnimType(c.getInt(6));
                        assets.setBonecount(c.getInt(7));
                        assets.setFeaturedindex(c.getInt(8));
                        assets.setUploaded(c.getString(9));
                        assets.setDownloads(c.getInt(10));
                        assets.setRating(c.getInt(11));
                        assets.setPublishedId(c.getInt(12));
                        animList.add(assets);
                    } while (c.moveToNext());
                }
            } catch (Exception e) {
                if (c != null)
                    c.close();
                if (animDb.isOpen())
                    animDb.close();
                return null;
            }
            c.close();
            if (animDb.isOpen())
                animDb.close();
            return animList;
        }
        if (animDb.isOpen())
            animDb.close();
        return null;
    }

    public List<AnimDB> getAllMyAnimation(int tableType,int type,String keyWord) {
        String querySQL = "";
        if(tableType == Constants.MYANIMATION_DOWNLOAD) {
            querySQL = "SELECT * FROM "+ANIM_TABLE_ANIMASSETS+" WHERE "+ANIM_KEY_TYPE+" = "+type;
        }else if(tableType == Constants.MYANIMATION_RATING) {
            querySQL = "SELECT * FROM "+ANIM_TABLE_ANIMASSETS+" WHERE "+ANIM_KEY_TYPE+" = "+type;
        } else if(tableType == Constants.MYANIMATION_TABLE) {
            querySQL = "SELECT * FROM "+ANIM_TABLE_MYANIMASSETS+" WHERE "+ANIM_KEY_TYPE+" = "+type;
        } else if(tableType == Constants.MYANIMATION_FEATURED) {
            querySQL = "SELECT * FROM "+ANIM_TABLE_ANIMASSETS+" WHERE "+ANIM_KEY_TYPE+" = "+type;
        } else if(tableType == Constants.MYANIMATION_RECENT) {
            querySQL = "SELECT * FROM "+ANIM_TABLE_ANIMASSETS+" WHERE "+ANIM_KEY_TYPE+" = "+type;
        }
        if(keyWord.length()>0) {
            switch (tableType) {
                case Constants.MYANIMATION_DOWNLOAD:
                    querySQL += " AND "+ANIM_KEY_KEYWORD+" LIKE '%"+ANIM_KEY_KEYWORD+"%' ORDER BY "+ANIM_KEY_DOWNLOADS+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_RATING:
                    querySQL += " AND "+ANIM_KEY_KEYWORD+" LIKE '%"+keyWord.toLowerCase()+"%' ORDER BY "+ANIM_KEY_RATING+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_FEATURED:
                    querySQL += " AND "+ANIM_KEY_KEYWORD+" LIKE '%"+keyWord.toLowerCase()+"%' ORDER BY "+ANIM_KEY_FEATUREDINDEX+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_TABLE:
                    querySQL = " AND "+ANIM_KEY_KEYWORD+" LIKE '%"+keyWord.toLowerCase()+"%'";
                    break;
                default:
                    break;
            }
        }else{
            switch (tableType) {
                case Constants.MYANIMATION_DOWNLOAD:
                    querySQL += " ORDER BY "+ANIM_KEY_DOWNLOADS+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_RATING:
                    querySQL += " ORDER BY "+ANIM_KEY_RATING+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_FEATURED:
                    querySQL += " ORDER BY "+ANIM_KEY_FEATUREDINDEX+" DESC LIMIT 50";
                    break;
                case Constants.MYANIMATION_RECENT:{
                    querySQL += " ORDER BY datetime("+ANIM_KEY_UPLOADEDTIME+") DESC LIMIT 50";
                    break;
                }
                default:
                    break;
            }
        }

        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        Cursor c = animDb.rawQuery(querySQL, null);
        try {
            if (c.moveToFirst()) {
                do {
                    AnimDB anim = new AnimDB();
                    anim.setID(Integer.parseInt(c.getString(0)));
                    anim.setAssetsId(c.getInt(1));
                    anim.setAnimName(c.getString(2));
                    anim.setKeyword(c.getString(3));
                    anim.setUserid(c.getString(4));
                    anim.setUserName(c.getString(5));
                    anim.setAnimType(c.getInt(6));
                    anim.setBonecount(c.getInt(7));
                    anim.setFeaturedindex(c.getInt(8));
                    anim.setUploaded(c.getString(9));
                    anim.setDownloads(c.getInt(10));
                    anim.setRating(c.getInt(11));
                    anim.setPublishedId(c.getInt(12));
                    animList.add(anim);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (animDb.isOpen())
                animDb.close();
            return null;
        }

        c.close();
        if (animDb.isOpen())
            animDb.close();
        return animList;
    }

    public int updateAnimationDetails(AnimDB animDB) {
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ANIM_KEY_ASSETSID, animDB.getAnimAssetId());
        values.put(ANIM_KEY_ANIM_NAME, animDB.getAnimName());
        values.put(ANIM_KEY_KEYWORD, animDB.getKeyword());
        values.put(ANIM_KEY_USERID, animDB.getUserid());
        values.put(ANIM_KEY_USERNAME, animDB.getUserName());
        values.put(ANIM_KEY_TYPE, animDB.getAnimType());
        values.put(ANIM_KEY_BONECOUNT, animDB.getBonecount());
        values.put(ANIM_KEY_FEATUREDINDEX, animDB.getFeaturedindex());
        values.put(ANIM_KEY_UPLOADEDTIME, animDB.getUploaded());
        values.put(ANIM_KEY_DOWNLOADS, animDB.getDownloads());
        values.put(ANIM_KEY_RATING, animDB.getRating());
        values.put(ANIM_PUBLISH_ID,animDB.getpublishedId());
        animDb.update(ANIM_TABLE_ANIMASSETS, values, ANIM_KEY_ASSETSID + " = ?", new String[]{String.valueOf(animDB.getAnimAssetId())});
        if (animDb.isOpen())
            animDb.close();
        values = null;
        return 0;
    }

    public int getAnimationCount() {
        String countQuery = "SELECT  * FROM " + ANIM_TABLE_ANIMASSETS;
        SQLiteDatabase animDB = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = animDB.rawQuery(countQuery, null);
        int count = cursor.getCount();
        if (cursor != null)
            cursor.close();
        if (animDB.isOpen())
            animDB.close();
        return count;
    }

    public int getMyAnimationCount() {
        String countQuery = "SELECT  * FROM " + ANIM_TABLE_MYANIMASSETS;
        SQLiteDatabase animDB = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = animDB.rawQuery(countQuery, null);
        int count = cursor.getCount();
        if (cursor != null)
            cursor.close();
        if (animDB.isOpen())
            animDB.close();
        return count;
    }

    public void addNewModelAssets(AssetsDB assetsDB) {
        SQLiteDatabase modelAssetDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ASSET_KEY_ASSET_NAME, assetsDB.getAssetName());
        values.put(ASSET_KEY_IAP, assetsDB.getIap());
        values.put(ASSET_KEY_ASSETSID, assetsDB.getAssetsId());
        values.put(ASSET_KEY_TYPE, assetsDB.getType());
        values.put(ASSET_KEY_NBONES, assetsDB.getNbones());
        values.put(ASSET_KEY_KEYWORDS, assetsDB.getKeywords());
        values.put(ASSET_KEY_HASH, assetsDB.getHash());
        values.put(ASSET_KEY_TIME, assetsDB.getDateTime());
        values.put(ASSET_KEY_GROUP,assetsDB.getGroup());
        modelAssetDatabase.insert(ASSET_TABLE_ASSETS, null, values);
        if (modelAssetDatabase.isOpen())
            modelAssetDatabase.close();
    }

    public void addNewMyModelAssets(AssetsDB assetsDB) {
        SQLiteDatabase modelAssetDatabase = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ASSET_KEY_ASSET_NAME, assetsDB.getAssetName());
        values.put(ASSET_KEY_IAP, assetsDB.getIap());
        values.put(ASSET_KEY_ASSETSID, assetsDB.getAssetsId());
        values.put(ASSET_KEY_TYPE, assetsDB.getType());
        values.put(ASSET_KEY_NBONES, assetsDB.getNbones());
        values.put(ASSET_KEY_KEYWORDS, assetsDB.getKeywords());
        values.put(ASSET_KEY_HASH, assetsDB.getHash());
        values.put(ASSET_KEY_TIME, assetsDB.getDateTime());
        modelAssetDatabase.insert(ASSET_TABLE_MY_ASSETS, null, values);
        if (modelAssetDatabase.isOpen())
            modelAssetDatabase.close();
    }

    public List<AssetsDB> getAllModelDetailWithSearch(String searchName) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = null;
        if (searchName.length() != 0) {
            cursor = assetsDb.rawQuery("SELECT  * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_ASSET_NAME + " LIKE " + "'" + searchName + "%'", null);
            try {
                if (cursor.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(cursor.getString(0)));
                        assets.setAssetName(cursor.getString(1));
                        assets.setIap(cursor.getInt(2));
                        assets.setAssetsId(cursor.getInt(3));
                        assets.setType(cursor.getInt(4));
                        assets.setNbones(cursor.getInt(5));
                        assets.setKeywords(cursor.getString(6));
                        assets.setHash(cursor.getString(7));
                        assets.setDateTime(cursor.getString(8));
                        assetsList.add(assets);
                    } while (cursor.moveToNext());
                }
            } catch (Exception e) {
                if (cursor != null)
                    cursor.close();
                if (assetsDb.isOpen())
                    assetsDb.close();
                return null;
            }
            cursor.close();
            if (assetsDb.isOpen())
                assetsDb.close();
        }
        if (assetsDb.isOpen())
            assetsDb.close();
        return null;
    }

    public List<AssetsDB> getAllMyModelDetailWithSearch(String searchName) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = null;
        if (searchName.length() != 0) {
            cursor = assetsDb.rawQuery("SELECT  * FROM " + ASSET_TABLE_MY_ASSETS + " WHERE " + ASSET_KEY_ASSET_NAME + " LIKE " + "'" + searchName + "%'", null);
            try {
                if (cursor.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(cursor.getString(0)));
                        assets.setAssetName(cursor.getString(1));
                        assets.setIap(cursor.getInt(2));
                        assets.setAssetsId(cursor.getInt(3));
                        assets.setType(cursor.getInt(4));
                        assets.setNbones(cursor.getInt(5));
                        assets.setKeywords(cursor.getString(6));
                        assets.setHash(cursor.getString(7));
                        assets.setDateTime(cursor.getString(8));
                        assetsList.add(assets);
                    } while (cursor.moveToNext());
                }
            } catch (Exception e) {
                if (cursor != null)
                    cursor.close();
                if (assetsDb.isOpen())
                    assetsDb.close();
                return null;
            }
            cursor.close();
            if (assetsDb.isOpen())
                assetsDb.close();
        }
        if (assetsDb.isOpen())
            assetsDb.close();
        return null;
    }

    public List<AssetsDB> getAllModelWithCategory(String categoryType) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        if (categoryType.length() != 0) {
            Cursor c = assetsDb.rawQuery("SELECT  * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " LIKE " + "'%" + categoryType + "%'", null);
            try {
                if (c.moveToFirst()) {
                    do {
                        AssetsDB assets = new AssetsDB();
                        assets.setID(Integer.parseInt(c.getString(0)));
                        assets.setAssetName(c.getString(1));
                        assets.setIap(c.getInt(2));
                        assets.setAssetsId(c.getInt(3));
                        assets.setType(c.getInt(4));
                        assets.setNbones(c.getInt(5));
                        assets.setKeywords(c.getString(6));
                        assets.setHash(c.getString(7));
                        assets.setDateTime(c.getString(8));
                        assetsList.add(assets);
                    } while (c.moveToNext());
                }
            } catch (Exception e) {
                if (c != null)
                    c.close();
                if (assetsDb.isOpen())
                    assetsDb.close();
                return null;
            }

            if (c != null)
                c.close();
            if (assetsDb.isOpen())
                assetsDb.close();
            return assetsList;
        }
        if (assetsDb.isOpen())
            assetsDb.close();
        return null;
    }

    public List<AssetsDB> getAllModelDetail(int type) {
        String querySQL = "";
        if(type == 5 || type == 6)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_GROUP + " = " + ((type == 5) ? 1 : 2);
        else if(type > 2)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " = " + type;
        else if(type == 2)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " = " + type +" AND " + ASSET_KEY_ASSETSID + " > 30000";
        else if(type == 1)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " = " + type +" AND " + ASSET_KEY_ASSETSID + " < 40000";
        else if(type == 0)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " = 1 OR " + ASSET_KEY_TYPE +" = 2 OR " + ASSET_KEY_TYPE + " = 3 ORDER BY datetime("+ASSET_KEY_TIME+") DESC";
        else if(type == 50)
            querySQL = "SELECT * FROM " + ASSET_TABLE_ASSETS + " WHERE " + ASSET_KEY_TYPE + " = "+type;

        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);

        Cursor c = assetsDb.rawQuery(querySQL, null);
        try {
            if (c.moveToFirst()) {
                do {
                    AssetsDB assets = new AssetsDB();
                    assets.setID(Integer.parseInt(c.getString(0)));
                    assets.setAssetName(c.getString(1));
                    assets.setIap(c.getInt(2));
                    assets.setAssetsId(c.getInt(3));
                    assets.setType(c.getInt(4));
                    assets.setNbones(c.getInt(5));
                    assets.setKeywords(c.getString(6));
                    assets.setHash(c.getString(7));
                    assets.setDateTime(c.getString(8));
                    assetsList.add(assets);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (assetsDb.isOpen())
                assetsDb.close();
            return null;
        }

        if (c != null)
            c.close();
        if (assetsDb.isOpen())
            assetsDb.close();
        return assetsList;
    }

    public List<AssetsDB> getAllMyModelDetail() {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        String selectQuery = "SELECT  * FROM " + ASSET_TABLE_MY_ASSETS;
        Cursor c = assetsDb.rawQuery(selectQuery, null);
        try {
            if (c.moveToFirst()) {
                do {
                    AssetsDB assets = new AssetsDB();
                    assets.setID(Integer.parseInt(c.getString(0)));
                    assets.setAssetName(c.getString(1));
                    assets.setIap(c.getInt(2));
                    assets.setAssetsId(c.getInt(3));
                    assets.setType(c.getInt(4));
                    assets.setNbones(c.getInt(5));
                    assets.setKeywords(c.getString(6));
                    assets.setHash(c.getString(7));
                    assets.setDateTime(c.getString(8));
                    assetsList.add(assets);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (assetsDb.isOpen())
                assetsDb.close();
            return null;
        }

        if (c != null)
            c.close();
        if (assetsDb.isOpen())
            assetsDb.close();
        return assetsList;
    }

    public int updateModelAssets(AssetsDB assetsDB) {
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        ContentValues values = new ContentValues();
        values.put(ASSET_KEY_ASSET_NAME, assetsDB.getAssetName());
        values.put(ASSET_KEY_IAP, assetsDB.getIap());
        values.put(ASSET_KEY_ASSETSID, assetsDB.getAssetsId());
        values.put(ASSET_KEY_TYPE, assetsDB.getType());
        values.put(ASSET_KEY_NBONES, assetsDB.getNbones());
        values.put(ASSET_KEY_KEYWORDS, assetsDB.getKeywords());
        values.put(ASSET_KEY_HASH, assetsDB.getHash());
        values.put(ASSET_KEY_TIME, assetsDB.getDateTime());
        values.put(ASSET_KEY_GROUP, assetsDB.getGroup());

        assetsDb.update(ASSET_TABLE_ASSETS, values, ASSET_KEY_ASSETSID + " = ?", new String[]{String.valueOf(assetsDB.getAssetsId())});
        if (assetsDb.isOpen())
            assetsDb.close();
        return 0;
    }

    public int getModelAssetCount() {
        String countQuery = "SELECT  * FROM " + ASSET_TABLE_ASSETS;
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = assetsDb.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        if (assetsDb.isOpen())
            assetsDb.close();
        return count;
    }

    public int getMYModelAssetCount() {
        String countQuery = "SELECT  * FROM " + ASSET_TABLE_MY_ASSETS;
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor = assetsDb.rawQuery(countQuery, null);
        int count = cursor.getCount();
        cursor.close();
        if (assetsDb.isOpen())
            assetsDb.close();
        return count;
    }

    public List<AssetsDB> getSingleModelDetail(String columnName, String keyWord) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor c = assetsDb.rawQuery("SELECT  * FROM " + ASSET_TABLE_ASSETS + " WHERE " + columnName + " LIKE " + "'" + keyWord + "'", null);
        try {
            if (c.moveToFirst()) {
                do {
                    AssetsDB assets = new AssetsDB();
                    assets.setID(Integer.parseInt(c.getString(0)));
                    assets.setAssetName(c.getString(1));
                    assets.setIap(c.getInt(2));
                    assets.setAssetsId(c.getInt(3));
                    assets.setType(c.getInt(4));
                    assets.setNbones(c.getInt(5));
                    assets.setKeywords(c.getString(6));
                    assets.setHash(c.getString(7));
                    assets.setDateTime(c.getString(8));
                    assetsList.add(assets);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (assetsDb.isOpen())
                assetsDb.close();
            return null;
        }
        if (c != null)
            c.close();
        if (assetsDb.isOpen())
            assetsDb.close();
        return assetsList;
    }

    public List<AnimDB> getSingleAnimationDetail(String columnName, String keyWord) {
        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor c = animDb.rawQuery("SELECT  * FROM " + ANIM_TABLE_ANIMASSETS + " WHERE " + columnName + " LIKE " + "'%" + keyWord + "%'", null);
        try {
            if (c.moveToFirst()) {
                do {
                    AnimDB assets = new AnimDB();
                    assets.setID(Integer.parseInt(c.getString(0)));
                    assets.setAssetsId(c.getInt(1));
                    assets.setAnimName(c.getString(2));
                    assets.setKeyword(c.getString(3));
                    assets.setUserid(c.getString(4));
                    assets.setUserName(c.getString(5));
                    assets.setAnimType(c.getInt(6));
                    assets.setBonecount(c.getInt(7));
                    assets.setFeaturedindex(c.getInt(8));
                    assets.setUploaded(c.getString(9));
                    assets.setDownloads(c.getInt(10));
                    assets.setRating(c.getInt(11));
                    animList.add(assets);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (animDb.isOpen())
                animDb.close();
        }
        if (c != null)
            c.close();
        if (animDb.isOpen())
            animDb.close();
        return animList;
    }

    public List<AnimDB> getSingleMyAnimationDetail(String columnName, String keyWord) {
        List<AnimDB> animList = new ArrayList<AnimDB>();
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor c = animDb.rawQuery("SELECT  * FROM " + ANIM_TABLE_MYANIMASSETS + " WHERE " + columnName + " LIKE " + "'%" + keyWord + "%'", null);
        try {
            if (c.moveToFirst()) {
                do {
                    AnimDB assets = new AnimDB();
                    assets.setID(Integer.parseInt(c.getString(0)));
                    assets.setAssetsId(c.getInt(1));
                    assets.setAnimName(c.getString(2));
                    assets.setKeyword(c.getString(3));
                    assets.setUserid(c.getString(4));
                    assets.setUserName(c.getString(5));
                    assets.setAnimType(c.getInt(6));
                    assets.setBonecount(c.getInt(7));
                    assets.setFeaturedindex(c.getInt(8));
                    assets.setUploaded(c.getString(9));
                    assets.setDownloads(c.getInt(10));
                    assets.setRating(c.getInt(11));
                    assets.setPublishedId(c.getInt(12));
                    animList.add(assets);
                } while (c.moveToNext());
            }
        } catch (Exception e) {
            if (c != null)
                c.close();
            if (animDb.isOpen())
                animDb.close();
        }
        if (c != null)
            c.close();
        if (animDb.isOpen())
            animDb.close();
        return animList;
    }

    public void checkOldDatabaseIsFoundThenCopyTableToNewDatabase() {
        String newDatabasepath = PathManager.Iyan3DDatabse;
        String oldSceneDatabase = "/data/data/com.smackall.animator/databases/iyan3dScenesDB";
        String oldAssetDatabase = "/data/data/com.smackall.animator/databases/iyan3dAssetsDB";
        String oldAnimDatabase = "/data/data/com.smackall.animator/databases/iyan3dAnimDB";

        if (FileHelper.checkValidFilePath(oldSceneDatabase)) {
            PathManager.Iyan3DDatabse = oldSceneDatabase;
            List<SceneDB> list = getAllScenes();
            PathManager.Iyan3DDatabse = newDatabasepath;
            if (list != null && list.size() > 0) {
                for (int i = 0; i < list.size(); ++i) {
                    this.addNewScene((SceneDB) list.get(i));
                }
            }
            File sceneDb = new File(oldSceneDatabase);
            sceneDb.delete();
        }
        if (FileHelper.checkValidFilePath(oldAssetDatabase)) {
            PathManager.Iyan3DDatabse = oldAssetDatabase;
            List<AssetsDB> list = getAllMyModelDetail();
            PathManager.Iyan3DDatabse = newDatabasepath;
            if (list != null && list.size() > 0) {
                for (int i = 0; i < list.size(); ++i) {
                    this.addNewMyModelAssets((AssetsDB) list.get(i));
                }
            }
            File AssetDb = new File(oldSceneDatabase);
            AssetDb.delete();
        }
        if (FileHelper.checkValidFilePath(oldAnimDatabase)) {
            PathManager.Iyan3DDatabse = oldAnimDatabase;
            List<AnimDB> list = getAllMyAnimation(9,1,"");
            PathManager.Iyan3DDatabse = newDatabasepath;
            if (list != null && list.size() > 0) {
                for (int i = 0; i < list.size(); ++i) {
                    this.addNewMyAnimationAssets((AnimDB) list.get(i));
                }
            }
            File AnimDb = new File(oldAnimDatabase);
            AnimDb.delete();
        }
    }
}
