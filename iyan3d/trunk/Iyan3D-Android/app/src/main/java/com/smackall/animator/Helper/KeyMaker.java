package com.smackall.animator.Helper;

import android.content.Context;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * Created by Sabish.M on 25/11/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class KeyMaker {

    public static boolean makeKey(Context context) {
        File shared_prefs = new File("data/data/" + context.getPackageName() + "/shared_prefs/");
        FileHelper.mkDir(shared_prefs);
        File key = new File("data/data/" + context.getPackageName() + "/shared_prefs/key");
        try {
            if (!key.exists())
                key.createNewFile();
            return true;

        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    public static String readFromKeyFile(Context context) throws IOException {
        File key = new File("data/data/" + context.getPackageName() + "/shared_prefs/key");
        if (!key.exists())
            return null;
        int length = (int) key.length();

        byte[] bytes = new byte[length];

        FileInputStream in = new FileInputStream(key);
        try {
            in.read(bytes);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        } finally {
            in.close();
        }

        return new String(bytes);
    }

}
