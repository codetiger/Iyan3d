/*
 * Decompiled with CFR 0_110.
 * 
 * Could not load the following classes:
 *  java.io.File
 *  java.io.FileInputStream
 *  java.io.FileOutputStream
 *  java.lang.Exception
 *  java.lang.Object
 *  java.lang.String
 */
package com.smackall.animator.Helper;

import android.content.Context;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class FileHelper {
    public static boolean checkValidFilePath(File file) {
        return file.exists();
    }

    public static boolean checkValidFilePath(String string2) {
        return new File(string2).exists();
    }

    /*
     * Enabled aggressive block sorting
     * Enabled unnecessary exception pruning
     * Enabled aggressive exception aggregation
     */
    public static boolean copy(File file, File file2) {
        try {
            FileInputStream fileInputStream = new FileInputStream(file);
            FileOutputStream fileOutputStream = new FileOutputStream(file2);
            byte[] arrby = new byte[1024];
            do {
                int n;
                if ((n = fileInputStream.read(arrby)) <= 0) {
                    fileInputStream.close();
                    fileOutputStream.close();
                    return true;
                }
                fileOutputStream.write(arrby, 0, n);
            } while (true);
        }
        catch (Exception var4_6) {
            return false;
        }
    }

    /*
     * Enabled aggressive block sorting
     * Enabled unnecessary exception pruning
     * Enabled aggressive exception aggregation
     */
    public static boolean copy(String string2, String string3) {
        File file = new File(string2);
        File file2 = new File(string3);
        try {
            FileInputStream fileInputStream = new FileInputStream(file);
            FileOutputStream fileOutputStream = new FileOutputStream(file2);
            byte[] arrby = new byte[1024];
            do {
                int n;
                if ((n = fileInputStream.read(arrby)) <= 0) {
                    fileInputStream.close();
                    fileOutputStream.close();
                    return true;
                }
                fileOutputStream.write(arrby, 0, n);
            } while (true);
        }
        catch (Exception var6_8) {
            return false;
        }
    }

    public static void deleteFilesAndFolder(File file) {
        if (file.exists()) {
            if (file.isDirectory() && file.list() != null) {
                String[] arrstring = file.list();
                int n = arrstring.length;
                for (int i = 0; i < n; ++i) {
                    FileHelper.deleteFilesAndFolder(new File(file, arrstring[i]));
                }
            }
            file.delete();
        }
    }

    public static void deleteFilesAndFolder(String string2) {
        File file = new File(string2);
        if (file.exists()) {
            if (file.isDirectory() && file.list() != null) {
                String[] arrstring = file.list();
                int n = arrstring.length;
                for (int i = 0; i < n; ++i) {
                    FileHelper.deleteFilesAndFolder(String.valueOf((Object)new File(file, arrstring[i])));
                }
            }
            file.delete();
        }
    }

    public static String getFileExt(File file) {
        String string2 = file.getAbsolutePath();
        return string2.substring(1 + string2.lastIndexOf("."), string2.length());
    }

    public static String getFileExt(String string2) {
        return string2.substring(1 + string2.lastIndexOf("."), string2.length());
    }

    public static String getFileNameFromPath(String path){
        return path.substring(path.lastIndexOf("/"), path.length());
    }

    public static String getFileWithoutExt(File file) {
        String string2 = file.getAbsolutePath();
        return string2.substring(1 + string2.lastIndexOf("/"),  string2.lastIndexOf("."));
    }

    public static String getFileWithoutExt(String string2) {
        return string2.substring(1 + string2.lastIndexOf("/"), string2.lastIndexOf("."));
    }

    public static void mkDir(File file) {
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void mkDir(String string2) {
        File file = new File(string2);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static boolean move(File file, File file2) {
        if (file.exists()) {
            file.renameTo(file2);
            return true;
        }
        return false;
    }

    public static boolean move(String string2, String string3) {
        File file = new File(string2);
        File file2 = new File(string3);
        if (file.exists()) {
            file.renameTo(file2);
            return true;
        }
        return false;
    }

    public static String md5(String s) {

        try {
            // Create MD5 Hash
            MessageDigest digest = MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i=0; i<messageDigest.length; i++)
                hexString.append(Integer.toHexString(0xFF & messageDigest[i]));
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return "";
    }

    public static boolean isItHaveSpecialChar(String input){
        String charecters =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 1234567890";
        String[] charArray = input.split("");
        for(int i =0; i < charArray.length; i++){
            if(!charecters.contains(charArray[i]))
                return true;
        }
        return false;
    }

    public static void getObjAndTextureFromCommonIyan3dPath(Context context,final  int mode){
        final String commonFolder = PathManager.LocalImportAndExport+"/";
        final String userMeshFolder = PathManager.LocalUserMeshFolder+"/";

        final File f = new File(commonFolder);

        FilenameFilter filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                if(filename.toLowerCase().endsWith((mode == Constants.OBJ_MODE) ? "obj" :"png"))
                    return true;
                else
                    return false;
            }
        };
        File files[] = f.listFiles(filenameFilter);

        if(files != null) {
            for (int i = 0; i < files.length; i++) {
                File file = new File(userMeshFolder + getFileNameFromPath(files[i].getAbsolutePath()));
                copy(files[i].getAbsoluteFile(), file);
                files[i].getAbsoluteFile().delete();
            }
        }
    }
}

