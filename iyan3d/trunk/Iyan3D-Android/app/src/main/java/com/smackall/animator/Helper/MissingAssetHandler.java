package com.smackall.animator.Helper;

import android.content.Context;

import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

/**
 * Created by Sabish.M on 24/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class MissingAssetHandler {
    Context mContext;
    public MissingAssetHandler(Context context){
        this.mContext = context;
    }

    public boolean checkAssets(String filename , int nodeType){
        System.out.println("File Name : " + filename);
        switch (nodeType){
            case Constants.NODE_TEXT_SKIN:
            case Constants.NODE_TEXT:
                if(!FileHelper.checkValidFilePath(PathManager.LocalFontsFolder+"/"+filename))
                    if(!FileHelper.checkValidFilePath(PathManager.LocalUserFontFolder+"/"+filename))
                        return downloadMissingAsset(Constants.urlFont+filename,PathManager.LocalFontsFolder+"/"+filename);
                break;
            case Constants.NODE_SGM:
                if(!FileHelper.checkValidFilePath(PathManager.LocalMeshFolder+"/"+filename+".sgm")) {
                    downloadMissingAsset(Constants.urlForMesh + filename + ".sgm", PathManager.LocalMeshFolder + "/" + filename + ".sgm");
                   return downloadMissingAsset(Constants.urlForTexture + filename + ".png", PathManager.LocalMeshFolder + "/" + filename + "-cm.png");
                }
                break;
            case Constants.NODE_RIG:
                if(!FileHelper.checkValidFilePath(PathManager.LocalMeshFolder+"/"+filename+".sgr")) {
                    downloadMissingAsset(Constants.urlForMesh + filename + ".sgr", PathManager.LocalMeshFolder + "/" + filename + ".sgr");
                    return downloadMissingAsset(Constants.urlForTexture + filename + ".png", PathManager.LocalMeshFolder + "/" + filename + "-cm.png");
                }
                break;
            case Constants.NODE_PARTICLES:
                break;
            case Constants.NODE_IMAGE:
                if(!FileHelper.checkValidFilePath(PathManager.LocalImportedImageFolder+"/"+filename+".png")) {
                    return false;
                }
                break;
            case Constants.NODE_VIDEO:
                break;
        }
    return true;
    }

    public boolean downloadMissingAsset(String link,String path)
    {
        int count;
        try {
            URL url = new URL(link);
            URLConnection conection = url.openConnection();
            conection.connect();
            int lenghtOfFile = conection.getContentLength();
            InputStream input = new BufferedInputStream(url.openStream(), 8192);
            OutputStream output = new FileOutputStream(path);
            byte data[] = new byte[1024];
            long total = 0;
            while ((count = input.read(data)) != -1) {
                total += count;
                output.write(data, 0, count);
            }
            output.flush();
            output.close();
            input.close();

        } catch (Exception e) {
            return false;
        }
        return true;
    }
}
