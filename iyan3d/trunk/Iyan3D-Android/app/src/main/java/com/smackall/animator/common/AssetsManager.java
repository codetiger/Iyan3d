package com.smackall.animator.common;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Sabish on 11/09/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AssetsManager {



    public void copyFile(Context context, String filename, String md5Filename,String ext,String filePath) {
        AssetManager assetManager = context.getAssets();
        String filePathStr = null;
        if(filePath == null) {
            filePathStr = Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/scenes";
        }
        else {
            filePathStr = filePath;
        }
        File fileDir = new File(filePathStr);
        if(!fileDir.exists())
            fileDir.mkdir();

        InputStream in = null;
        OutputStream out = null;
        try {
            in = assetManager.open(filename);
            String newFileName = filePathStr+"/"+md5Filename+ext;
            out = new FileOutputStream(newFileName);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception e) {
        }
    }

    public static void copyAssetsDirToLocal(Context context) {
        AssetManager assetManager = context.getAssets();
        File defaultDirForFiles = new File("/data/data/com.smackall.animator/files/");
        if(!defaultDirForFiles.exists())
            defaultDirForFiles.mkdir();
        File defaultDirForAssets = new File("/data/data/com.smackall.animator/files/assets/");
        if(!defaultDirForAssets.exists())
            defaultDirForAssets.mkdir();

        String[] assets = null;
        try {
            assets = assetManager.list("");
        } catch (IOException e) {

        }

        if (assets != null) for (String filename : assets) {
            InputStream in = null;
            OutputStream out = null;
            try {
                in = assetManager.open(filename);
                File outFile = new File(Constant.defaultAssetsDir, filename);
                out = new FileOutputStream(outFile);
                copyFile(in, out);

            } catch(IOException e) {

            }
            finally {
                if (in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        // NOOP
                    }
                }
                if (out != null) {
                    try {
                        out.close();
                    } catch (IOException e) {
                        // NOOP
                    }
                }

            }
        }
    }
    private static void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1){
            out.write(buffer, 0, read);
        }
    }
}
