package com.smackall.animator.Helper;

/**
 * Created by Sabish.M on 29/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class HQTaskDB {

    String date;
    int task;
    String name;
    int id;
    int taskType;
    int completed = Constants.TASK_PROGRESS;
    int progress = 0;
    String uniqueId = "";

    public HQTaskDB() {
    }

    public HQTaskDB(String date, String name, int task) {
        this.date = date;
        this.name = name;
        this.task = task;
    }

    public HQTaskDB(int id, String date, String name, int taskId, int taskType, int completed, int progress, String uniqueId) {
        this.id = id;
        this.date = date;
        this.name = name;
        this.task = taskId;
        this.completed = completed;
        this.progress = progress;
        this.uniqueId = uniqueId;
        this.taskType = taskType;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDate() {
        return this.date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public int getTask() {
        return this.task;
    }

    public void setTask(int task) {
        this.task = task;
    }

    public int getId() {
        return this.id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getTaskType() {
        return this.taskType;
    }

    public void setTaskType(int type) {
        this.taskType = type;
    }

    public int getCompleted() {
        return this.completed;
    }

    public void setCompleted(int isComplete) {
        this.completed = isComplete;
    }

    public int getProgress() {
        return this.progress;
    }

    public void setProgress(int progress) {
        this.progress = progress;
    }

    public String getUniqueId() {
        return this.uniqueId;
    }

    public void setUniqueId(String uniqueId) {
        this.uniqueId = uniqueId;
    }
}
