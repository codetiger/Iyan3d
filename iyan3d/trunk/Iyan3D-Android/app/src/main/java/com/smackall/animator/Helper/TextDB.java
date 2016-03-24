package com.smackall.animator.Helper;

/**
 * Created by Sabish.M on 17/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TextDB {

    float red = 1.0f;
    float green = 1.0f;
    float blue = 1.0f;
    int typeOfNode;
    String textureName = "-1";
    String assetName = "TEXT";
    int fontSize = 10;
    int bevalValue;
    int assetAddType;
    String filePath = "-1";
    boolean isTempNode = true;

    public TextDB(){

    }

    public TextDB(float red,float green,float blue,int typeOfNode,String textureName,String assetName,int fontSize,int bevalValue,int assetAddType,String filePath,boolean isTempNode){
        this.red = red;
        this.green = green;
        this.blue = blue;
        this.typeOfNode = typeOfNode;
        this.textureName = textureName;
        this.assetName = assetName;
        this.fontSize = fontSize;
        this.bevalValue = bevalValue;
        this.assetAddType = assetAddType;
        this.filePath = filePath;
        this.isTempNode = isTempNode;
    }

    public void setRed(float red){
        this.red = red;
    }
    public void setGreen(float green){
        this.green = green;
    }
    public void setBlue(float blue){
        this.blue = blue;
    }
    public void setTypeOfNode(int typeOfNode){
        this.typeOfNode = typeOfNode;
    }
    public void setTextureName(String textureName){
        this.textureName = textureName;
    }
    public void setAssetName(String assetName){
        this.assetName = assetName;
    }
    public void setFontSize(int fontSize){
        this.fontSize = fontSize;
    }
    public void setBevalValue(int bevalValue){
        this.bevalValue = bevalValue;
    }
    public void setAssetAddType(int assetAddType){
        this.assetAddType = assetAddType;
    }
    public void setFilePath(String filePath){
        this.filePath = filePath;
    }
    public void setTempNode(boolean isTempNode){
        this.isTempNode = isTempNode;
    }

    public float getRed(){
        return this.red;
    }
    public float getGreen(){
        return this.green;
    }
    public float getBlue(){
        return blue;
    }
    public int getTypeOfNode(){
        return this.typeOfNode;
    }
    public String  getTextureName(){
        return this.textureName ;
    }
    public String getAssetName(){
        return this.assetName ;
    }
    public int getFontSize(){
        return this.fontSize ;
    }
    public int getBevalValue(){
        return this.bevalValue ;
    }
    public int getAssetAddType(){
        return this.assetAddType;
    }
    public String getFilePath(){
        return this.filePath;
    }
    public boolean getTempNode(){
        return this.isTempNode;
    }

}
