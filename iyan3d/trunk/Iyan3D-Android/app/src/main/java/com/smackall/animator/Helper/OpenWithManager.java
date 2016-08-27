package com.smackall.animator.Helper;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;

import java.io.File;

/**
 * Created by Sabish.M on 22/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class OpenWithManager {

    public static String handleOpenWithFile(Uri uri, Context mContext) {
        if (uri == null) return null;
        String path = new File(uri.getPath()).getAbsolutePath();
        if (path.toLowerCase().endsWith("obj")) {
            return handleObjFile(path);
        } else if (path.toLowerCase().endsWith("ttf")) {
            return handleTtfAndOtfFile(path);
        } else if (path.toLowerCase().endsWith("otf")) {
            return handleTtfAndOtfFile(path);
        } else {
            return handleImageFile(uri, mContext);
        }

    }

    public static String handleObjFile(String path) {
        if (FileHelper.checkValidFilePath(path)) {
            FileHelper.copy(path, PathManager.LocalUserMeshFolder + "/" + FileHelper.getFileNameFromPath(path));
            return path;
        }
        return null;
    }

    public static String handleTtfAndOtfFile(String path) {
        if (FileHelper.checkValidFilePath(path)) {
            FileHelper.copy(path, PathManager.LocalUserFontFolder + "/" + FileHelper.getFileNameFromPath(path));
            return path;
        }
        return null;
    }

    public static String handleImageFile(Uri path, Context mContext) {
        String imagePath = getPathFromUri(path, mContext);
        if (imagePath == null || imagePath.toLowerCase().equals("null")) return null;
        if (FileHelper.checkValidFilePath(imagePath)) {
            FileHelper.copy(imagePath, PathManager.LocalImportedImageFolder + "/" + FileHelper.getFileNameFromPath(imagePath));
            return imagePath;
        }
        return null;
    }

    public static String getPathFromUri(Uri uri, Context mContext) {
        String[] filePathColumn = {MediaStore.Images.Media.DATA};

        Cursor cursor = mContext.getContentResolver().query(uri,
                filePathColumn, null, null, null);
        if (cursor != null) {
            cursor.moveToFirst();
        } else
            return null;

        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        String picturePath = cursor.getString(columnIndex);
        cursor.close();
        return picturePath;
    }
}
