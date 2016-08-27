package com.smackall.animator.Helper;

public class AnimDB {
    private int animAssetId;
    private String animName;
    private int boneCount;
    private int downloads;
    private int featuredIndex;
    private Integer id;
    private String keyword;
    private int publishedId;
    private int rating;
    private int type;
    private String uploaded;
    private String userName;
    private String userId;

    public AnimDB() {
    }

    public AnimDB(int n, String string2, String string3, String string4, String string5, int n2, int n3, int n4, String string6, int n5, int n6) {
        this.animAssetId = n;
        this.animName = string2;
        this.keyword = string3;
        this.userId = string4;
        this.userName = string5;
        this.type = n2;
        this.boneCount = n3;
        this.featuredIndex = n4;
        this.uploaded = string6;
        this.downloads = n5;
        this.rating = n6;
    }

    public AnimDB(int n, String string2, String string3, String string4, String string5, int n2, int n3, Integer n4, String string6, Integer n5, int n6, int n7) {
        this.animAssetId = n;
        this.animName = string2;
        this.keyword = string3;
        this.userId = string4;
        this.userName = string5;
        this.type = n2;
        this.boneCount = n3;
        this.featuredIndex = n4;
        this.uploaded = string6;
        this.downloads = n5;
        this.rating = n6;
        this.publishedId = n7;
    }

    public AnimDB(Integer n, int n2, String string2, String string3, String string4, String string5, int n3, int n4, int n5, String string6, int n6, int n7, int n8) {
        this.id = n;
        this.animAssetId = n2;
        this.animName = string2;
        this.keyword = string3;
        this.userId = string4;
        this.userName = string5;
        this.type = n3;
        this.boneCount = n4;
        this.featuredIndex = n5;
        this.uploaded = string6;
        this.downloads = n6;
        this.rating = n7;
        this.publishedId = n8;
    }

    public int getAnimAssetId() {
        return this.animAssetId;
    }

    public String getAnimName() {
        return this.animName;
    }

    public void setAnimName(String string2) {
        this.animName = string2;
    }

    public int getAnimType() {
        return this.type;
    }

    public void setAnimType(int n) {
        this.type = n;
    }

    public int getBoneCount() {
        return this.boneCount;
    }

    public void setBoneCount(int n) {
        this.boneCount = n;
    }

    public int getDownloads() {
        return this.downloads;
    }

    public void setDownloads(int n) {
        this.downloads = n;
    }

    public int getFeaturedIndex() {
        return this.featuredIndex;
    }

    public void setFeaturedIndex(int n) {
        this.featuredIndex = n;
    }

    public int getID() {
        return this.id;
    }

    public void setID(int n) {
        this.id = n;
    }

    public String getKeyword() {
        return this.keyword;
    }

    public void setKeyword(String string2) {
        this.keyword = string2;
    }

    public int getRating() {
        return this.rating;
    }

    public void setRating(int n) {
        this.rating = n;
    }

    public String getUploaded() {
        return this.uploaded;
    }

    public void setUploaded(String string2) {
        this.uploaded = string2;
    }

    public String getUserName() {
        return this.userName;
    }

    public void setUserName(String string2) {
        this.userName = string2;
    }

    public String getUserId() {
        return this.userId;
    }

    public void setUserId(String string2) {
        this.userId = string2;
    }

    public int getPublishedId() {
        return this.publishedId;
    }

    public void setPublishedId(int n) {
        this.publishedId = n;
    }

    public void setAssetsId(int n) {
        this.animAssetId = n;
    }
}

