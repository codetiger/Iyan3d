package com.smackall.animator.Helper;

/**
 * Created by Sabish.M on 14/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NewsFeedItem {

    private int id;
    private int type;
    private String title;
    private String message;
    private String url;
    private String thumbnail_url;

    public NewsFeedItem() {

    }

    public NewsFeedItem(int id, int type, String title, String message, String url, String thumbnailurl) {
        this.id = id;
        this.type = type;
        this.title = title;
        this.message = message;
        this.url = url;
        this.thumbnail_url = thumbnailurl;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getThumbnailUrl() {
        return thumbnail_url;
    }

    public void setThumbnailUrl(String thumbnailUrl) {
        this.thumbnail_url = thumbnailUrl;
    }
}