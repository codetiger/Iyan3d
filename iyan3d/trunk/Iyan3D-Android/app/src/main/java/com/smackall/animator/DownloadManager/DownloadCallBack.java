package com.smackall.animator.DownloadManager;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import com.smackall.animator.common.Constant;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

/**
 * Created by Sabish.M on 9/12/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class DownloadCallBack {

    String url, fileName, downloadPath;
    int type;
    String cacheDir;
    Context context;

    public void downloadCallBack(Context context,String url,String fileName,String downloadPath,int type){
        this.context = context;
        this.url = url;
        this.fileName = fileName;
        this.downloadPath = downloadPath;
        this.type = type;
        this.cacheDir = Environment.getExternalStorageDirectory()+"/Android/data/"+context.getPackageName()+"/.cache/";
        System.out.println("File Name Come here : " + this.fileName);
        downloadFileFromURL();
    }

        public void downloadFileFromURL() {
            int count;
            try {

                File cacheDir = new File(this.cacheDir);
                if(!cacheDir.exists())
                cacheDir.mkdir();

                URL url = new URL(this.url);
                URLConnection conection = url.openConnection();
                conection.connect();
                // getting file length
                int lenghtOfFile = conection.getContentLength();

                // input stream to read file - with 8k buffer
                InputStream input = new BufferedInputStream(url.openStream(), 8192);

                // Output stream to write file
                OutputStream output = new FileOutputStream(this.cacheDir+this.fileName);

                byte data[] = new byte[1024];

                long total = 0;

                while ((count = input.read(data)) != -1) {
                    total += count;
                    // publishing the progress....
                    // After this onProgressUpdate will be called

                    // writing data to file
                    output.write(data, 0, count);
                }

                // flushing output
                output.flush();

                // closing streams
                output.close();
                input.close();

            } catch (Exception e) {
                Constant.animationEditor.activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(Constant.informDialog == null)
                        Constant.informDialog( Constant.animationEditor.context, "Please connect with internet some resources files are missing need to download it.");
                    }
                });

                Log.e("Error: ", e.getMessage());
                return;
            }

            // dismiss the dialog after the file was downloaded
            File cacheFile = new File(this.cacheDir+this.fileName);
            if(!cacheFile.exists())
                return;
            File sgmFolder = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+this.context.getPackageName()+"/mesh");
            File fontFolder = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+this.context.getPackageName()+"/fonts");
            File animFolder = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+this.context.getPackageName()+"/animations");
            Constant.mkDir(sgmFolder);
            Constant.mkDir(fontFolder);
            Constant.mkDir(animFolder);

                if (this.type == Constant.NODE_TYPE.NODE_SGM.getValue()) {

                } else if (this.type == Constant.NODE_TYPE.NODE_RIG.getValue()) {
                    try {
                        Constant.copy(cacheFile,new File(Environment.getExternalStorageDirectory()+"/Android/data/"+this.context.getPackageName()+"/mesh/"+this.fileName));
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } else if (this.type == Constant.NODE_TYPE.NODE_OBJ.getValue()) {

                } else if (this.type == Constant.NODE_TYPE.NODE_TEXT.getValue()) {
                    try {
                        Constant.copy(cacheFile,new File(Environment.getExternalStorageDirectory()+"/Android/data/"+this.context.getPackageName()+"/fonts/"+this.fileName));
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } else if (this.type == Constant.NODE_TYPE.NODE_IMAGE.getValue()) {

            }
        }
}
