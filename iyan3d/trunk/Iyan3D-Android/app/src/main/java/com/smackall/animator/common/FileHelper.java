package com.smackall.animator.common;

import android.content.Context;
import android.os.Environment;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;

/**
 * Created by Sabish.M on 23/11/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FileHelper {

    public static void getFontsFromCommonIyan3dPath(Context context){
        final String commonFolder = Environment.getExternalStorageDirectory()+"/Iyan3d";
        Constant.mkDir(commonFolder);
        final String userFontFolder = Environment.getExternalStorageDirectory()+"/Android/data/com.smackall.animator/fonts/userfonts/";
        Constant.mkDir(userFontFolder);

        final File f = new File(commonFolder);

        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith("ttf") || filename.toLowerCase().endsWith("otf"))
                    return true;
                else
                    return false;
            }
        };
        File files[] = f.listFiles(filenameFilter);

        if(files != null) {
            for (int i = 0; i < files.length; i++) {
                File userFonts = new File(userFontFolder + files[i].getAbsolutePath().toString().substring(files[i].getAbsoluteFile().toString().lastIndexOf("/"), files[i].toString().length()));
                try {
                    Constant.copy(files[i].getAbsoluteFile(), userFonts);
                    files[i].delete();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            Constant.textAssetsSelectionView.updateMyFontsList();
        }
        else {
            System.out.println("File Helper Array is Null");
            Constant.informDialog(context, "Fonts not Found in Iyan3D Folder");
        }
    }

    public static void getObjAndTextureFromCommonIyan3dPath(Context context){
        final String commonFolder = Environment.getExternalStorageDirectory()+"/Iyan3d";
        Constant.mkDir(commonFolder);
        final String userMeshFolder = Environment.getExternalStorageDirectory()+"/Android/data/com.smackall.animator/mesh/usermesh/";
        Constant.mkDir(userMeshFolder);

        final File f = new File(commonFolder);

        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith("obj") || filename.toLowerCase().endsWith("png"))
                    return true;
                else
                    return false;
            }
        };
        File files[] = f.listFiles(filenameFilter);

        if(files != null) {
            for (int i = 0; i < files.length; i++) {
                File userFonts = new File(userMeshFolder + files[i].getAbsolutePath().toString().substring(files[i].getAbsoluteFile().toString().lastIndexOf("/"), files[i].toString().length()));
                try {
                    Constant.copy(files[i].getAbsoluteFile(), userFonts);
                    files[i].getAbsoluteFile().delete();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            Constant.importObjAndTexture.gridLoader(userMeshFolder);
        }
    else {
                System.out.println("File Helper Array is Null");
                Constant.informDialog(context, "Models not Found in Iyan3D Folder");
        }
    }
}
