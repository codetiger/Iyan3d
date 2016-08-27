package com.smackall.animator.Helper;

/**
 * Created by Sabish.M on 18/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImageDB {

    int nodeType;
    String name = "";
    int width;
    int height;
    int assetAddType;
    boolean isTempNode;
    int actionType = Constants.ACTION_TEXT_IMAGE_ADD;

    public ImageDB() {

    }

    public ImageDB(int nodeType, String name, int width, int height, int assetAddType, boolean isTempNode) {
        this.nodeType = nodeType;
        this.name = name;
        this.width = width;
        this.height = height;
        this.assetAddType = assetAddType;
        this.isTempNode = isTempNode;
    }

    public void setTempNode(boolean isTempNode) {
        this.isTempNode = isTempNode;
    }

    public int getNodeType() {
        return this.nodeType;
    }

    public void setNodeType(int nodeType) {
        this.nodeType = nodeType;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getWidth() {
        return this.width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return this.height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getAssetAddType() {
        return this.assetAddType;
    }

    public void setAssetAddType(int assetAddType) {
        this.assetAddType = assetAddType;
    }

    public boolean getIsTempNode() {
        return this.isTempNode;
    }

    public int getActionType() {
        return this.actionType;
    }

    public void setActionType(int actionType) {
        this.actionType = actionType;
    }
}