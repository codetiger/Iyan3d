package com.smackall.animator.AssetsViewController;


/**
 * Created by Sabish on 14/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AssetsDB {
    Integer id;
    String  assetName;
    String  iap;
    Integer assetsId;
    String  type;
    String  nbones;
    String  keywords;
    String  hash;
    String  time;

    public AssetsDB(){   }

    public AssetsDB(Integer id, String assetName, String iap, Integer assetsId, String type,
                   String nbones, String keywords, String hash,String time){

        this.id =               id;
        this.assetName =        assetName;
        this.iap =              iap;
        this.assetsId =         assetsId;
        this.type =             type;
        this.nbones =           nbones;
        this.keywords =         keywords;
        this.hash =             hash;
        this.time =             time;
    }

    public AssetsDB(String assetName, String iap,Integer assetsId, String type,
                    String nbones, String keywords, String hash,String time){

        this.assetName =        assetName;
        this.iap =              iap;
        this.assetsId =         assetsId;
        this.type =             type;
        this.nbones =           nbones;
        this.keywords =         keywords;
        this.hash =             hash;
        this.time =             time;
    }

    public AssetsDB(Integer assetsId, String type
                    ){
        this.assetsId =         assetsId;
        this.type =             type;

    }

    public int getID(){
        return this.id;
    }

    public void setID(int id){
        this.id = id;
    }

    public int getAssetsId(){
        return this.assetsId;
    }

    public void setAssetsId(int assetsId){
        this.assetsId = assetsId;
    }

    public String getAssetName(){
        return this.assetName;
    }

    public void setAssetName(String assetName){
        this.assetName = assetName;
    }

    public String getIap(){
        return this.iap;
    }

    public void setIap(String iap){
        this.iap = iap;
    }

    public String getType(){
        return this.type;
    }

    public void setType(String type){
        this.type = type;
    }


    public String getNbones(){
        return this.nbones;
    }

    public void setNbones(String nbones){
        this.nbones = nbones;
    }

    public String getKeywords(){
        return this.keywords;
    }

    public void setKeywords(String keywords){
        this.keywords = keywords;
    }

    public String getHash(){
        return this.hash;
    }

    public void setHash(String hash){
        this.hash = hash;
    }

    public String getDateTime(){
        return this.time;
    }

    public void setDateTime(String time){
        this.time = time;
    }

}