package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.Display;
import android.view.WindowManager;

import com.smackall.animator.common.Constant;
import com.smackall.animator.common.RealPathUtil;

import java.io.File;
import java.io.FileOutputStream;

/**
 * Created by Sabish.M on 12/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ImportImage extends Activity{
    Bitmap bmp;
    Display d;
    Context context;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        d = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        context = this;
                openGallery();

    }

    private void openGallery()
    {
        Intent i = new Intent(
                Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        i.setType("image/*");
        try {
            startActivityForResult(i, 1);
        }
        catch (ActivityNotFoundException e){
            Constant.informDialog(context,"Gallery Not Found!");
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultcode, Intent intent)
    {
        super.onActivityResult(requestCode, resultcode, intent);

        if (requestCode == 1)
        {
            if (intent != null && resultcode == RESULT_OK)
            {
                Intent data = intent;
                Uri selectedImage = data.getData();
                String filePath;
                //filePath =   RealPathUtil.getPath(context, selectedImage);// getPathFromUri(selectedImage);
                //if(filePath == null)
                    filePath = getPathFromUri(selectedImage);

                if(filePath == null) {
                    Constant.informDialog(context, "Something Wrong! Try Again.");
                    return;
                }

                if(bmp != null && !bmp.isRecycled())
                {
                    bmp = null;
                }

                bmp = BitmapFactory.decodeFile(filePath);

                System.out.println("Bitmap Width : " + bmp.getWidth());
                System.out.println("Bitmap Height : " + bmp.getHeight());

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

                final Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, targetSize, targetSize, false);

                System.out.println("File Path : " + filePath);
                System.out.println("File Name : " + new File(filePath).getName());
                final String userData = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/importedImages/";
                Constant.mkDir(userData);
                int dotPos = new File(filePath).getName().lastIndexOf(".");
                final String fileNameWithoutExt = new File(filePath).getName().substring(0, dotPos);
                Constant.animationEditor.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        savePng(scaledBitmap, userData, fileNameWithoutExt, oriWidth, oriHeight);
                    }
                });
            }
            else
            {
               ImportImage.this.finish();
            }
        }
    }

     private void savePng( Bitmap img,String filePath, String fileName, int oriWidth, int oriHeight) {
         try {
             File dumpFile = new File(filePath, fileName + ".png");
             FileOutputStream os = new FileOutputStream(dumpFile);
             img.compress(Bitmap.CompressFormat.PNG, 50, os);
             os.flush();
             os.close();
         } catch (Exception e) {
             e.printStackTrace();
             return;
         }
         finally {
         }
         Constant.animationEditor.importImageFromLocal(filePath,fileName,oriWidth,oriHeight);
         ImportImage.this.finish();

     }

    private String getPathFromUri(Uri uri) {
        if (Build.VERSION.SDK_INT < 11)
            return RealPathUtil.getRealPathFromURI_BelowAPI11(context, uri);
        else if (Build.VERSION.SDK_INT < 19)
            return RealPathUtil.getRealPathFromURI_API11to18(context, uri);
        else
            return RealPathUtil.getRealPathFromURI_API19(context, uri);
    }
}
