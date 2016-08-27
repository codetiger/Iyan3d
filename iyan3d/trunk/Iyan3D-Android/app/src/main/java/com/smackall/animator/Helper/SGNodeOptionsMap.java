package com.smackall.animator.Helper;

import android.content.Context;

import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by Sabish.M on 23/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SGNodeOptionsMap {

    public Map<Integer, Property> nodeProps = new HashMap<>();
    Context mContext;

    public SGNodeOptionsMap(Context mContext) {
        this.mContext = mContext;
    }

    public void clearNodeProps() {
        if (nodeProps != null)
            nodeProps.clear();
    }

    public Map<Integer, Property> getSortedMap(Map<Integer, Property> nodeProps) {
        Map<Integer, Property> treeMap = new TreeMap<Integer, Property>(new Comparator<Integer>() {
            @Override
            public int compare(Integer lhs, Integer rhs) {
                return lhs.compareTo(rhs);
            }
        });

        treeMap.putAll(nodeProps);
        return treeMap;
    }

    public void addSGNodeProperty(int index, int parentIndex, int type, String title, String fileName, int iconId, String groupName,
                                  float valueX, float valueY, float valueZ, float valueW, boolean isEnabled) {
        if (parentIndex != Constants.UNDEFINED) {
            if (nodeProps.containsKey(parentIndex)) {
                Property property = nodeProps.get(parentIndex);
                property.setSubProps(index, parentIndex, type, title, fileName, iconId, groupName,
                        valueX, valueY, valueZ, valueW, isEnabled);
            }
        } else {
            Property property = new Property();
            property.setProps(index, parentIndex, type, title, fileName, iconId, groupName,
                    valueX, valueY, valueZ, valueW, isEnabled);
            nodeProps.put(index, property);
        }
    }
}
