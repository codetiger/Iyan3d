package com.smackall.animator.AnimationAssetsView;

/**
 * Created by Sabish on 22/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AnimDB {

        Integer id;
        String  animAssetId;
        String  animName;
        String  keyword;
        String  userid;
        String  userName;
        String  type;
        String  bonecount;
        int  featuredindex;
        String  uploaded;
        int  downloads;
        int  rating;
        String publishedId;

        public AnimDB(){   }

        public AnimDB(Integer id, String animAssetId, String animName, String keyword, String userid, String userName,
                        String type, String bonecount, Integer featuredindex,String uploaded,
                      Integer downloads, Integer rating){

            this.id =               id;
            this.animAssetId =    animAssetId;
            this.animName =       animName;
            this.keyword =        keyword;
            this.userid =         userid;
            this.userName =       userName;
            this.type =           type;
            this.bonecount =      bonecount;
            this.featuredindex =  featuredindex;
            this.uploaded =       uploaded;
            this.downloads =      downloads;
            this.rating =         rating;
        }

        public AnimDB(String animAssetId, String animName, String keyword, String userid, String userName,
                  String type, String bonecount, Integer featuredindex,String uploaded,
                      Integer downloads, Integer rating){

            this.animAssetId =    animAssetId;
            this.animName =       animName;
            this.keyword =        keyword;
            this.userid =         userid;
            this.userName =       userName;
            this.type =           type;
            this.bonecount =      bonecount;
            this.featuredindex =  featuredindex;
            this.uploaded =       uploaded;
            this.downloads =      downloads;
            this.rating =         rating;
        }

    public AnimDB(String animAssetId, String animName, String keyword, String userid, String userName,
                  String type, String bonecount, Integer featuredindex,String uploaded,
                  Integer downloads, Integer rating, String publishedId){

        this.animAssetId =    animAssetId;
        this.animName =       animName;
        this.keyword =        keyword;
        this.userid =         userid;
        this.userName =       userName;
        this.type =           type;
        this.bonecount =      bonecount;
        this.featuredindex =  featuredindex;
        this.uploaded =       uploaded;
        this.downloads =      downloads;
        this.rating =         rating;
        this.publishedId = publishedId;
    }



        public int getID(){
            return this.id;
        }

        public void setID(int id){
            this.id = id;
        }

        public String getAnimAssetId(){
            return this.animAssetId;
        }

        public void setAssetsId(String animAssetId){
            this.animAssetId = animAssetId;
        }

        public String getAnimName(){
            return this.animName;
        }

        public void setAnimName(String animName){
            this.animName = animName;
        }

        public String getKeyword(){
            return this.keyword;
        }

        public void setKeyword(String keyword){
            this.keyword = keyword;
        }

        public String getUserName(){
            return this.userName;
        }

        public void setUserName(String userName){
            this.userName = userName;
        }

    public String getUserid(){
        return this.userid;
    }

    public void setUserid(String userid){
        this.userid = userid;
    }

        public String getAnimType(){
            return this.type;
        }

        public void setAnimType(String type){
            this.type = type;
        }

        public String getBonecount(){
            return this.bonecount;
        }

        public void setBonecount(String bonecount){
            this.bonecount = bonecount;
        }

        public int getFeaturedindex(){
            return this.featuredindex;
        }

        public void setFeaturedindex(int featuredindex){
            this.featuredindex = featuredindex;
        }

        public String getUploaded(){
            return this.uploaded;
        }

        public void setUploaded(String uploaded){
            this.uploaded = uploaded;
        }

        public int getDownloads(){
            return this.downloads;
        }

        public void setDownloads(int downloads){
        this.downloads = downloads;
    }

        public int getRating(){
            return this.rating;
        }

        public void setRating(int rating){
        this.rating = rating;
    }

        public String getpublishedId(){
            return this.publishedId;
        }
        public void setPublishedId(String publishedId){
            this.publishedId = publishedId;
        }

    }

