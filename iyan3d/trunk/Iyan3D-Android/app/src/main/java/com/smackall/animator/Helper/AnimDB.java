package com.smackall.animator.Helper;

public class AnimDB {
    int animAssetId;
    String animName;
    int bonecount;
    int downloads;
    int featuredindex;
    Integer id;
    String keyword;
    int publishedId;
    int rating;
    int type;
    String uploaded;
    String userName;
    String userid;

    public AnimDB() {
    }

    public AnimDB(int n, String string2, String string3, String string4, String string5, int n2, int n3, int n4, String string6, int n5, int n6) {
        this.animAssetId = n;
        this.animName = string2;
        this.keyword = string3;
        this.userid = string4;
        this.userName = string5;
        this.type = n2;
        this.bonecount = n3;
        this.featuredindex = n4;
        this.uploaded = string6;
        this.downloads = n5;
        this.rating = n6;
    }

    public AnimDB(int n, String string2, String string3, String string4, String string5, int n2, int n3, Integer n4, String string6, Integer n5, int n6, int n7) {
        this.animAssetId = n;
        this.animName = string2;
        this.keyword = string3;
        this.userid = string4;
        this.userName = string5;
        this.type = n2;
        this.bonecount = n3;
        this.featuredindex = n4;
        this.uploaded = string6;
        this.downloads = n5;
        this.rating = n6;
        this.publishedId = n7;
    }

    public AnimDB(Integer n, int n2, String string2, String string3, String string4, String string5, int n3, int n4, int n5, String string6, int n6, int n7) {
        this.id = n;
        this.animAssetId = n2;
        this.animName = string2;
        this.keyword = string3;
        this.userid = string4;
        this.userName = string5;
        this.type = n3;
        this.bonecount = n4;
        this.featuredindex = n5;
        this.uploaded = string6;
        this.downloads = n6;
        this.rating = n7;
    }

    public int getAnimAssetId() {
        return this.animAssetId;
    }

    public String getAnimName() {
        return this.animName;
    }

    public int getAnimType() {
        return this.type;
    }

    public int getBonecount() {
        return this.bonecount;
    }

    public int getDownloads() {
        return this.downloads;
    }

    public int getFeaturedindex() {
        return this.featuredindex;
    }

    public int getID() {
        return this.id;
    }

    public String getKeyword() {
        return this.keyword;
    }

    public int getRating() {
        return this.rating;
    }

    public String getUploaded() {
        return this.uploaded;
    }

    public String getUserName() {
        return this.userName;
    }

    public String getUserid() {
        return this.userid;
    }

    public int getpublishedId() {
        return this.publishedId;
    }

    public void setAnimName(String string2) {
        this.animName = string2;
    }

    public void setAnimType(int n) {
        this.type = n;
    }

    public void setAssetsId(int n) {
        this.animAssetId = n;
    }

    public void setBonecount(int n) {
        this.bonecount = n;
    }

    public void setDownloads(int n) {
        this.downloads = n;
    }

    public void setFeaturedindex(int n) {
        this.featuredindex = n;
    }

    public void setID(int n) {
        this.id = n;
    }

    public void setKeyword(String string2) {
        this.keyword = string2;
    }

    public void setPublishedId(int n) {
        this.publishedId = n;
    }

    public void setRating(int n) {
        this.rating = n;
    }

    public void setUploaded(String string2) {
        this.uploaded = string2;
    }

    public void setUserName(String string2) {
        this.userName = string2;
    }

    public void setUserid(String string2) {
        this.userid = string2;
    }
}

