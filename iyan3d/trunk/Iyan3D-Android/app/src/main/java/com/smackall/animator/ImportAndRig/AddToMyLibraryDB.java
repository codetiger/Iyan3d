package com.smackall.animator.ImportAndRig;

import android.content.Context;

import com.smackall.animator.AssetsViewController.AssetsDB;
import com.smackall.animator.AssetsViewController.AssetsViewDatabaseHandler;
import com.smackall.animator.common.Constant;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by Sabish.M on 16/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AddToMyLibraryDB {

    AssetsViewDatabaseHandler assetsViewDatabaseHandler;

    public int addToMyLibraryDatabase(Context context, String type ,String fileName, String textureFileName ){
        assetsViewDatabaseHandler = new AssetsViewDatabaseHandler(context);
        assetsViewDatabaseHandler.createMyLibraryTable(Constant.assetsDatabase);
        System.out.println("My Library asset Added Function Called");
        assetsViewDatabaseHandler.createMyLibraryTable(Constant.assetsDatabase);
        String name = "";
        String iap = "0";
        int assetsId = 0;
        String assetType = "";
        String nbones = "0";
        String keywords = "";
        String hash = "";
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
        if(type.equals("autorig")){
            name = "autorig" + Integer.toString(assetsViewDatabaseHandler.getAssetsCount("MyLibrary"));
            assetsId = (40000+assetsViewDatabaseHandler.getAssetsCount("MyLibrary"));
            assetType = "1";
            keywords = fileName+textureFileName+"autorig";
            hash = Constant.md5(fileName);
        }

        if(type.equals("obj")){
            name = "obj" + Integer.toString(assetsViewDatabaseHandler.getAssetsCount("MyLibrary"));
            assetsId = (20000+assetsViewDatabaseHandler.getAssetsCount("MyLibrary"));
            assetType = "6";
            keywords = fileName+textureFileName+"obj";
            hash = Constant.md5(fileName);
        }

        assetsViewDatabaseHandler.addAssets(new AssetsDB(name, iap,assetsId,assetType,nbones,keywords,hash,date
        ), "MyLibrary");

        return assetsId;
    }
}
