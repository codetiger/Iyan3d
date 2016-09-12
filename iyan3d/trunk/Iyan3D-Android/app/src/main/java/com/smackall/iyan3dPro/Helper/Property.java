package com.smackall.iyan3dPro.Helper;

import java.util.HashMap;

/**
 * Created by Sabish.M on 23/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Property {

    public int index;
    public int parentIndex;
    public int type;
    public String title;
    public String fileName;
    public int iconId;
    public String groupName;
    public HashMap<Integer, Property> subProps = new HashMap<>();
    public float valueX, valueY, valueZ, valueW;
    public boolean isEnabled;

    public Property() {

    }

    public void setProps(int index, int parentIndex, int type, String title, String fileName, int iconId, String groupName,
                         float valueX, float valueY, float valueZ, float valueW, boolean isEnabled) {
        this.index = index;
        this.parentIndex = parentIndex;
        this.type = type;
        this.title = title;
        this.fileName = fileName;
        this.iconId = iconId;
        this.groupName = groupName;
        this.valueX = valueX;
        this.valueY = valueY;
        this.valueZ = valueZ;
        this.valueW = valueW;
        this.isEnabled = isEnabled;
    }

    public void setSubProps(int index, int parentIndex, int type, String title, String fileName, int iconId, String groupName,
                            float valueX, float valueY, float valueZ, float valueW, boolean isEnabled) {

        Property subProps = new Property();
        subProps.index = index;
        subProps.parentIndex = parentIndex;
        subProps.type = type;
        subProps.title = title;
        subProps.fileName = fileName;
        subProps.iconId = iconId;
        subProps.groupName = groupName;
        subProps.valueX = valueX;
        subProps.valueY = valueY;
        subProps.valueZ = valueZ;
        subProps.valueW = valueW;
        subProps.isEnabled = isEnabled;

        this.subProps.put(index, subProps);
    }
}
