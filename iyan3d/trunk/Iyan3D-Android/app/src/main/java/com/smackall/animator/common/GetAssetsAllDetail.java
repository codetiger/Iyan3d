package com.smackall.animator.common;

import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.smackall.animator.AnimationAssetsView.AnimDB;
import com.smackall.animator.AssetsViewController.AssetsDB;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish.M on 1/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class GetAssetsAllDetail {

    public List<AnimDB> getAnimAssets(String filterValue, String value, String table) {
        List<AnimDB> animList = new ArrayList<AnimDB>();
        // Select All Query
        SQLiteDatabase animDb = SQLiteDatabase.openDatabase(Constant.animDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor;
        String selectQuery;
        System.out.println("Search Function Called");
        Cursor c = animDb.rawQuery("SELECT  * FROM " + table + " WHERE " + filterValue + " LIKE " + "'%" + value + "%'", null);
        try {
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
        } finally {
            c.close();
            animDb.close();
        }
        // looping through all rows and adding to list
        // return contact list
        return animList;
    }

    public List<AssetsDB> getAssetsDetail(String filterValue, String value, String tablename) {
        List<AssetsDB> assetsList = new ArrayList<AssetsDB>();
        // Select All Query
        SQLiteDatabase assetsDb = SQLiteDatabase.openDatabase(Constant.assetsDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);
        Cursor cursor;
        String selectQuery;

            System.out.println("Search Function Called");
            Cursor c = assetsDb.rawQuery("SELECT  * FROM " + tablename + " WHERE " + filterValue + " LIKE " + "'" + value + "'", null);
            try {
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
            } finally {
                c.close();
                assetsDb.close();
            }

        return assetsList;
    }
}

