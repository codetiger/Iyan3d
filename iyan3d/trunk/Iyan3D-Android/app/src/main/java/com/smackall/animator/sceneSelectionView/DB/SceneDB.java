package com.smackall.animator.sceneSelectionView.DB;

/**
 * Created by Sabish on 05/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SceneDB {
    int _id;
    String sceneName;
    String image;
    String time;
    public SceneDB(){   }

    public SceneDB(int id, String sceneName, String  image, String time){
        this._id = id;
        this.sceneName = sceneName;
        this.image = image;
        this.time = time;
    }

    public SceneDB(String sceneName, String image, String time){
        this.sceneName = sceneName;
        this.image = image;
        this.time = time;
    }
    public int getID(){
        return this._id;
    }

    public void setID(int id){
        this._id = id;
    }

    public String getName(){
        return this.sceneName;
    }

    public void setName(String sceneName){
        this.sceneName = sceneName;
    }

    public String getImage(){
        return this.image;
    }

    public void setImage(String image){
        this.image = image;
    }

    public String getTime(){
        return this.time;
    }

    public void setTime(String time){
        this.time = time;
    }
}