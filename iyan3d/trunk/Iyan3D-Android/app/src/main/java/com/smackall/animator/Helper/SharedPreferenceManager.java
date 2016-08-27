package com.smackall.animator.Helper;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by Sabish.M on 4/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SharedPreferenceManager {

    String APP_PREF = "APP_PREF";

    public void setData(Context context, String key, int value) {
        SharedPreferences.Editor editor = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE).edit();
        editor.putInt(key, value);
        editor.apply();
    }

    public void setData(Context context, String key, long value) {
        SharedPreferences.Editor editor = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE).edit();
        editor.putLong(key, value);
        editor.apply();
    }

    public void setData(Context context, String key, String value) {
        SharedPreferences.Editor editor = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE).edit();
        editor.putString(key, value);
        editor.apply();
    }

    public void setData(Context context, String key, float value) {
        SharedPreferences.Editor editor = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE).edit();
        editor.putFloat(key, value);
        editor.apply();
    }

    public void setData(Context context, String key, boolean value) {
        SharedPreferences.Editor editor = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE).edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    public String getString(Context context, String key) {
        SharedPreferences prefs = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE);
        return prefs.getString(key, null);
    }

    public int getInt(Context context, String key) {
        SharedPreferences prefs = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE);
        return prefs.getInt(key, 0);
    }

    public float getFloat(Context context, String key) {
        SharedPreferences prefs = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE);
        return prefs.getFloat(key, 0);
    }

    public long getLong(Context context, String key) {
        SharedPreferences prefs = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE);
        return prefs.getLong(key, 0);
    }

    public boolean getBool(Context context, String key) {
        SharedPreferences prefs = context.getSharedPreferences(APP_PREF, Context.MODE_PRIVATE);
        return prefs.getBoolean(key, false);
    }
}
