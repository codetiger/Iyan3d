package com.smackall.animator.Helper;

import android.app.Activity;
import android.os.Environment;

import java.io.File;

public class PathManager {
    public static String DefaultFilesDir = "";
    public static String LocalDataFolder = "";
    public static String LocalProjectFolder = "";
    public static String LocalAnimationFolder = "";
    public static String LocalThumbnailFolder = "";
    public static String LocalImportedImageFolder = "";
    public static String LocalMeshFolder = "";
    public static String LocalUserMeshFolder = "";
    public static String LocalCacheFolder = "";
    public static String LocalFontsFolder = "";
    public static String LocalScenesFolder = "";
    public static String LocalImportAndExport = "";
    public static String LocalUserFontFolder = "";
    public static String DefaultAssetsDir = "";
    public static String DefaultDatabaseDirectory = "";
    public static String Iyan3DDatabse = "";

    public static void initPaths(Activity activity) {
        DefaultFilesDir = String.valueOf(activity.getFilesDir());
        FileHelper.mkDir(DefaultFilesDir);
        LocalDataFolder = String.valueOf(activity.getExternalFilesDir(null));
        FileHelper.mkDir(LocalDataFolder);
        LocalProjectFolder = String.valueOf((activity.getExternalFilesDir(null) + "/projects"));
        FileHelper.mkDir(LocalProjectFolder);
        LocalAnimationFolder = String.valueOf((activity.getExternalFilesDir(null) + "/animations"));
        FileHelper.mkDir(LocalAnimationFolder);
        LocalThumbnailFolder = String.valueOf((activity.getExternalFilesDir(null) + "/images"));
        FileHelper.mkDir(LocalThumbnailFolder);
        LocalImportedImageFolder = String.valueOf((activity.getExternalFilesDir(null) + "/importedImages"));
        FileHelper.mkDir(LocalImportedImageFolder);
        LocalMeshFolder = String.valueOf((activity.getExternalFilesDir(null) + "/mesh"));
        FileHelper.mkDir(LocalMeshFolder);
        LocalUserMeshFolder = LocalMeshFolder + "/usermesh";
        FileHelper.mkDir(LocalUserMeshFolder);
        LocalCacheFolder = String.valueOf((activity.getExternalFilesDir(null) + "/.cache"));
        FileHelper.mkDir(LocalCacheFolder);
        LocalFontsFolder = String.valueOf((activity.getExternalFilesDir(null) + "/fonts"));
        FileHelper.mkDir(LocalFontsFolder);
        LocalUserFontFolder = String.valueOf((activity.getExternalFilesDir(null) + "/fonts/userFonts"));
        FileHelper.mkDir(LocalUserFontFolder);
        LocalScenesFolder = String.valueOf((activity.getExternalFilesDir(null) + "/scenes"));
        FileHelper.mkDir(LocalScenesFolder);
        LocalImportAndExport = String.valueOf((Environment.getExternalStorageDirectory() + "/iyan3d"));
        FileHelper.mkDir(LocalImportAndExport);
        DefaultDatabaseDirectory = DefaultFilesDir + "/databases";
        FileHelper.mkDir(DefaultDatabaseDirectory);
        DefaultAssetsDir = DefaultFilesDir + "/assets";
        FileHelper.mkDir(DefaultAssetsDir);
        Iyan3DDatabse = LocalDataFolder + "/iyan3d.db";
        PathManager.checkOldVersionProjectFilesFound(activity);
    }
    
    static void checkOldVersionProjectFilesFound(Activity activity) {
        File file;
        File file2;
        File file3;
        File file4;
        File file5;
        File file6;
        File file7;
        File file8;
        File file9;
        String string2 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/.cache/";
        String string3 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/mesh/usermesh/";
        String string4 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/mesh/";
        String string5 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/fonts/";
        String string6 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/fonts/userfonts/";
        String string7 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/images/";
        String string8 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/importedImages/";
        String string9 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/projects/";
        String string10 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/scenes/";
        String string11 = Environment.getExternalStorageDirectory() + "/Android/data/" + activity.getPackageName() + "/animations/";
        File file10 = new File(string2);
        if (file10.exists() && file10.isDirectory() && file10.list() != null) {
            for (String string12 : file10.list()) {
                FileHelper.move(string2 + string12, LocalCacheFolder + "/" + string12);
            }
        }
        if ((file4 = new File(string3)).exists() && file4.isDirectory() && file4.list() != null) {
            for (String string13 : file4.list()) {
                FileHelper.move(string3 + string13, LocalUserMeshFolder + "/" + string13);
            }
        }
        if ((file7 = new File(string4)).exists() && file7.isDirectory() && file7.list() != null) {
            for (String string14 : file7.list()) {
                FileHelper.move(string4 + string14, LocalMeshFolder + "/" + string14);
            }
        }
        if ((file5 = new File(string6)).exists() && file5.isDirectory() && file5.list() != null) {
            for (String string15 : file5.list()) {
                FileHelper.move(string6 + string15, LocalUserFontFolder + "/" + string15);
            }
        }
        if ((file9 = new File(string5)).exists() && file9.isDirectory() && file9.list() != null) {
            for (String string16 : file9.list()) {
                FileHelper.move(string5 + string16, LocalFontsFolder + "/" + file9);
            }
        }
        if ((file8 = new File(string8)).exists() && file8.isDirectory() && file8.list() != null) {
            for (String string17 : file8.list()) {
                FileHelper.move(string8 + string17, LocalImportedImageFolder + "/" + string17);
            }
        }
        if ((file2 = new File(string7)).exists() && file2.isDirectory() && file2.list() != null) {
            for (String string18 : file2.list()) {
                FileHelper.move(string7 + string18, LocalThumbnailFolder + "/" + string18);
            }
        }
        if ((file = new File(string9)).exists() && file.isDirectory() && file.list() != null) {
            for (String string19 : file.list()) {
                FileHelper.move(string9 + string19, LocalProjectFolder + "/" + string19);
            }
        }
        if ((file6 = new File(string10)).exists() && file6.isDirectory() && file6.list() != null) {
            for (String string20 : file6.list()) {
                FileHelper.move(string10 + string20, LocalScenesFolder + "/" + string20);
            }
        }
        if ((file3 = new File(string11)).exists() && file3.isDirectory() && file3.list() != null) {
            for (String string21 : file3.list()) {
                FileHelper.move(string11 + string21, LocalAnimationFolder + "/" + string21);
            }
        }
    }
}

