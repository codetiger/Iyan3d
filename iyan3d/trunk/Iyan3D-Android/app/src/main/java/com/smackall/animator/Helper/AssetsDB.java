package com.smackall.animator.Helper;

public class AssetsDB {
    String assetName;
    int assetsId;
    String hash;
    int iap;
    Integer id;
    String keywords;
    int nbones;
    String time;
    int type;
    int group;

    public AssetsDB() {
    }

    public AssetsDB(int n, int n2) {
        this.assetsId = n;
        this.type = n2;
    }

    public AssetsDB(Integer n, String string2, int n2, int n3, int n4, int n5, String string3, String string4, String string5) {
        this.id = n;
        this.assetName = string2;
        this.iap = n2;
        this.assetsId = n3;
        this.type = n4;
        this.nbones = n5;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
    }

    public AssetsDB(String string2, int n2, int n3, int n4, int n5, String string3, String string4, String string5,int group) {
        this.assetName = string2;
        this.iap = n2;
        this.assetsId = n3;
        this.type = n4;
        this.nbones = n5;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
        this.group = group;
    }

    public AssetsDB(String string2, int n, int n2, int n3, int n4, String string3, String string4, String string5) {
        this.assetName = string2;
        this.iap = n;
        this.assetsId = n2;
        this.type = n3;
        this.nbones = n4;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
    }

    public String getAssetName() {
        return this.assetName;
    }

    public int getAssetsId() {
        return this.assetsId;
    }

    public String getDateTime() {
        return this.time;
    }

    public String getHash() {
        return this.hash;
    }

    public int getID() {
        return this.id;
    }

    public int getIap() {
        return this.iap;
    }

    public String getKeywords() {
        return this.keywords;
    }

    public int getNbones() {
        return this.nbones;
    }

    public int getType() {
        return this.type;
    }

    public void setAssetName(String string2) {
        this.assetName = string2;
    }

    public void setAssetsId(int n) {
        this.assetsId = n;
    }

    public void setDateTime(String string2) {
        this.time = string2;
    }

    public void setHash(String string2) {
        this.hash = string2;
    }

    public void setID(int n) {
        this.id = n;
    }

    public void setIap(int n) {
        this.iap = n;
    }

    public void setKeywords(String string2) {
        this.keywords = string2;
    }

    public void setNbones(int n) {
        this.nbones = n;
    }

    public void setType(int n) {
        this.type = n;
    }

    public void setGroup(int n) {
        this.group = n;
    }
    public int getGroup() {
        return this.group;
    }
}

