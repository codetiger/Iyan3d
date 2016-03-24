package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.provider.MediaStore;

import com.smackall.animator.EditorView;

import java.io.File;
import java.io.FileOutputStream;

/**
 * Created by Sabish.M on 17/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImageManager {
    private Context mContext;
    private int viewType;
    public ImageManager(Context context){
        this.mContext = context;
    }

    public void getImageFromStorage(int viewType)
    {
        this.viewType = viewType;
        Intent i = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        ((Activity)mContext).startActivityForResult(i, Constants.IMAGE_IMPORT_RESPONSE);
    }

    public void startActivityForResult(Intent i, int requestCode,int resultCode)
    {
        if (requestCode == Constants.IMAGE_IMPORT_RESPONSE && resultCode == Activity.RESULT_OK && null != i) {
            Uri selectedImage = i.getData();
            String[] filePathColumn = { MediaStore.Images.Media.DATA };

            Cursor cursor = mContext.getContentResolver().query(selectedImage,
                    filePathColumn, null, null, null);
            if (cursor != null) {
                cursor.moveToFirst();
            }
            else
                return;

            int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
            String picturePath = cursor.getString(columnIndex);
            cursor.close();
            manageImageFile(picturePath);
        }
    }

    private void manageImageFile(String path){
        boolean exits = FileHelper.checkValidFilePath(path);
        if(exits){
            ((EditorView)((Activity)mContext)).showOrHideLoading(Constants.SHOW);
            Bitmap bmp = null;try {bmp = BitmapFactory.decodeFile(path);}catch (OutOfMemoryError e){UIHelper.informDialog(mContext,"Memory Out of range!");}
            if(bmp == null) return;
            savePng(bmp, PathManager.LocalThumbnailFolder + "/original" + FileHelper.getFileWithoutExt(path));
            bmp = null;
            makeThumbnail(path, "");
            scaleToSquare(path);
            if(viewType == Constants.IMPORT_IMAGES)
                ((EditorView)((Activity)mContext)).imageSelection.notifyDataChanged();
            else if(viewType == Constants.IMPORT_OBJ)
                ((EditorView)((Activity)mContext)).objSelection.notifyDataChanged();
            else if(viewType == Constants.CHANGE_TEXTURE_MODE)
                ((EditorView)((Activity)mContext)).textureSelection.notifyDataChanged();
        }
    }

    public void makeThumbnail(String path,String fileName)
    {
        Bitmap bmp = null;try {bmp = BitmapFactory.decodeFile(path);}catch (OutOfMemoryError e){UIHelper.informDialog(mContext,"Memory Out of range!");}
        if (bmp == null)
            return;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
        savePng(scaledBitmap,PathManager.LocalThumbnailFolder+"/"+((fileName.length() >0) ? fileName : FileHelper.getFileWithoutExt(path)));
        scaledBitmap = null;
    }

    public void makeThumbnail(String path)
    {
        Bitmap bmp = null;try {bmp = BitmapFactory.decodeFile(path);}catch (OutOfMemoryError e){UIHelper.informDialog(mContext,"Memory Out of range!");}
        if (bmp == null)
            return;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
        savePng(scaledBitmap,PathManager.LocalThumbnailFolder+"/"+FileHelper.getFileWithoutExt(path));
        scaledBitmap = null;
    }

    private void scaleToSquare(String path)
    {
        Bitmap bmp = null;try {bmp = BitmapFactory.decodeFile(path);}catch (OutOfMemoryError e){UIHelper.informDialog(mContext,"Memory Out of range!");}
        if(bmp == null) return;
        final int oriWidth = bmp.getWidth();
        final int oriHeight = bmp.getHeight();
        int maxSize = Math.max(oriHeight, oriWidth);
        int targetSize = 0;
        if(maxSize <= 128)
            targetSize = 128;
        else if(maxSize <= 256)
            targetSize = 256;
        else if(maxSize <= 512)
            targetSize = 512;
        else
            targetSize = 1024;
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, targetSize, targetSize, false);
        savePng(scaledBitmap,PathManager.LocalImportedImageFolder+"/"+FileHelper.getFileWithoutExt(path));
        scaledBitmap = null;
    }

    public void savePng(Bitmap bitmap,String filePath)
    {
        try {
            File temp = new File(filePath);
            FileOutputStream os = new FileOutputStream(temp+".png");
            bitmap.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
