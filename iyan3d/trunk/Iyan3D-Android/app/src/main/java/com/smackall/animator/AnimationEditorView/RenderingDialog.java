package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.videokit.api.android.SequenceEncoder;

import org.jcodec.common.NIOUtils;
import org.jcodec.common.SeekableByteChannel;
import org.jcodec.containers.mp4.Brand;
import org.jcodec.containers.mp4.TrackType;
import org.jcodec.containers.mp4.muxer.MP4Muxer;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import smackall.animator.R;


/**
 * Created by Sabish.M on 17/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class RenderingDialog {
    Activity activity;
    ProgressBar rendering_progress_bar;
    int progress = 0;
    int frameCount = 0;
    boolean isCanceled = false;
    boolean waterMark = false;
    int shaderType;
    //int min, max;
    TextView rendering_value;
    ImageView rendering_images_view;

    ProgressBar videoGenerateProgress;
    ImageView saveVideo;
    String date = "";
    String iyan3dVideoDir;
    int imageCount = 1;
    boolean lastImage = false;

    public void renderingDialogFunction(final Context context, Activity activity, boolean waterMark, int shaderType, int minFrame, int maxFrame, final String type,int resolution ) {

        this.activity = activity;
        this.waterMark = waterMark;
        this.shaderType = shaderType;
        //this.min = minFrame;
        //this.max = maxFrame;
        final Dialog rendering_dialog = new Dialog(context);
        rendering_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        rendering_dialog.setContentView(R.layout.rendering_dialog);
        rendering_dialog.setCancelable(false);
        rendering_dialog.setCanceledOnTouchOutside(false);

        rendering_value= (TextView) rendering_dialog.findViewById(R.id.rendering_value);

        Button rendering_dialog_cancel = (Button) rendering_dialog.findViewById(R.id.rendering_dialog_cancel);

        rendering_images_view = (ImageView) rendering_dialog.findViewById(R.id.rendering_iumages_view);

        rendering_progress_bar= (ProgressBar) rendering_dialog.findViewById(R.id.rendering_progress_bar);
        videoGenerateProgress = (ProgressBar) rendering_dialog.findViewById(R.id.videoGenerateProgress);

        saveVideo = (ImageView) rendering_dialog.findViewById(R.id.save_video_btn);

        switch (Constant.getScreenCategory(context)) {
            case "normal":
                rendering_dialog.getWindow().setLayout(Constant.width, Constant.height);
                break;
            case "large":
                rendering_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "xlarge":
                rendering_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "undefined":
                rendering_dialog.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
        }
        rendering_progress_bar.setMax(((maxFrame - minFrame) + 1));

        rendering_value.setTextSize(Constant.width / 60);
        rendering_value.setText("0/" + ((maxFrame - minFrame) + 1));
        frameCount = minFrame;
        File cacheDir = new File(Constant.defaultCacheDir);
        Constant.deleteDir(cacheDir);
        Constant.mkDir(Constant.defaultCacheDir);
        date =  new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
        rendering_dialog.show();
        String iyan3dDir = iyan3dVideoDir = Environment.getExternalStorageDirectory()+"/Iyan3d/";
        Constant.mkDir(iyan3dDir);
        iyan3dVideoDir = Environment.getExternalStorageDirectory()+"/Iyan3d/render/";
        Constant.mkDir(iyan3dVideoDir);

        exportRenderingImages(type, context, rendering_dialog,minFrame,maxFrame,resolution);

        rendering_dialog_cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isCanceled = true;
                GL2JNILib.exportingCompleted();
                rendering_dialog.dismiss();
                Constant.renderingDialog = null;
            }
        });
    }

    private void exportRenderingImages(final String type, final Context context, final Dialog renderingDialog, final int min, final int max, final int resolution){

        SequenceEncoder encoder = null;

        if (type.equals("video"))
            encoder = createVideo(type,context,renderingDialog);


        final SequenceEncoder finalEncoder = encoder;
        (new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while (!Thread.interrupted())
                            try {
                                progress++;
                                if (type.equals("video"))
                                    GL2JNILib.renderFrame(frameCount, shaderType, waterMark, Constant.defaultCacheDir + progress + ".png",resolution);
                                else {
                                    GL2JNILib.renderFrame(frameCount, shaderType, waterMark, iyan3dVideoDir + Constant.replaceSpecialCharFromString(date) + ".png",resolution);
                                }
                                frameCount++;
                                if (isCanceled) {
                                    break;
                                }
                                activity.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        if (type.equals("video")) {
                                            rendering_images_view.setImageBitmap(decodeSampledBitmapFromFile((Constant.defaultCacheDir + (progress - 1) + ".png"), rendering_images_view.getWidth(), rendering_images_view.getHeight()));
                                            if(progress == 2) {
                                                encodeImage(finalEncoder,renderingDialog,context,min,max);
                                            }
                                        } else {
                                            rendering_images_view.setImageBitmap(BitmapFactory.decodeFile(iyan3dVideoDir + Constant.replaceSpecialCharFromString(date) + ".png"));
                                            if (progress >= ((max - min) + 1)) {
                                                if (!(type.equals("video")))
                                                    videoCreatedAlert(context,"image",renderingDialog);
                                            }
                                        }

                                        rendering_value.setText(progress + "/" + ((max - min) + 1));
                                        rendering_progress_bar.setProgress(progress);
                                    }
                                });
                                if (progress >= ((max - min) + 1)) {
                                    activity.runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            //rendering_value.setText("Please Wait! Encoding Process...");
                                            videoGenerateProgress.setVisibility(View.VISIBLE);
                                        }
                                    });
                                    break;
                                }


                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                    }
                })).start();
    }

    public static Bitmap decodeSampledBitmapFromFile(String imageFilePath,
                                                         int reqWidth, int reqHeight) {

        // First decode with inJustDecodeBounds=true to check dimensions
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        Bitmap bitmap = BitmapFactory.decodeFile(imageFilePath, options);
        bitmap = null;
        // Calculate inSampleSize
        options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);

        // Decode bitmap with inSampleSize set
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeFile(imageFilePath, options);
    }

    public static int calculateInSampleSize(
            BitmapFactory.Options options, int reqWidth, int reqHeight) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        int inSampleSize = 1;

        if (height > reqHeight || width > reqWidth) {

            final int halfHeight = height / 2;
            final int halfWidth = width / 2;

            // Calculate the largest inSampleSize value that is a power of 2 and keeps both
            // height and width larger than the requested height and width.
            while ((halfHeight / inSampleSize) > reqHeight
                    && (halfWidth / inSampleSize) > reqWidth) {
                inSampleSize *= 2;
            }
        }

        return inSampleSize;
    }

    private SequenceEncoder createVideo(final String type, Context context, final Dialog renderingDialog){
//        activity.runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//                videoGenerateProgress.setVisibility(View.VISIBLE);
//                if(type.equals("video"))
//                rendering_value.setText("Making Video");
//                else
//                    rendering_value.setText("Rendering Images");
//            }
//        });
     return createVideoFromImages(type, context, renderingDialog);
    }

    private SequenceEncoder createVideoFromImages(final String type,final Context context, final Dialog renderingDialog) {
        if(type.equals("video")) {
            SeekableByteChannel ch;
            MP4Muxer muxer;
            try {
                File file = new File(iyan3dVideoDir +Constant.replaceSpecialCharFromString(date)+ ".mp4");
                SequenceEncoder encoder = new SequenceEncoder(file);
                ch = NIOUtils.writableFileChannel(file);
                muxer = new MP4Muxer(ch, Brand.MP4);
                muxer.addTrack(TrackType.VIDEO, 25);

                return  encoder;
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    videoGenerateProgress.setVisibility(View.INVISIBLE);
                    videoCreatedAlert(context, type, renderingDialog);
                }
            });
        }
        return  null;
    }

    private void encodeImage(final SequenceEncoder encoder, final Dialog renderingDialog, final Context context, final int min, final int max)
    {
        final SequenceEncoder finalEncoder = encoder;


        (new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while (imageCount <= ((max - min) + 2)) {

                            File imageFile = new File(Constant.defaultCacheDir + imageCount + ".png");
                            if (imageCount <= ((max - min) + 1)) {
                                try {
                                    while (!imageFile.exists() && !isCanceled) {
                                        if(isCanceled)
                                            break;
                                        System.out.println("Image File Not Found : " + imageCount);
                                        Thread.sleep(10);
                                    }
                                    Bitmap bitmap1 = getBitmap(imageFile);
                                    if (bitmap1 != null) {
                                        if(!isCanceled)
                                        finalEncoder.encodeImage(bitmap1);
                                        imageCount++;
                                        bitmap1 = null;
                                    }

                                } catch (IOException e) {
                                    e.printStackTrace();
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }
                            } else {
                                try {
                                    if(!isCanceled)
                                    encoder.finish();

                                    activity.runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            //rendering_value.setText("Video Created");
                                            videoGenerateProgress.setVisibility(View.INVISIBLE);
                                            videoCreatedAlert(context, "video", renderingDialog);
                                        }
                                    });
                                    break;
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }

                            if(isCanceled && progress >= 24) {
                                try {
                                    encoder.finish();
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                break;
                            }

                        }
                    }
                })).start();
    }

    public void videoCreatedAlert(final Context context, final String type, final Dialog renderingDialog){
        videoGenerateProgress.setVisibility(View.INVISIBLE);
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("Process Completed");

        final TextView msg = new TextView(context);
        if(type.equals("video"))
        msg.setText("Video was successfully saved in your gallery.");
        else
        msg.setText("Image was successfully saved in your gallery.");
        alert.setView(msg);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                renderingDialog.dismiss();
                GL2JNILib.exportingCompleted();
                    dialog.dismiss();
                    dialog = null;
                    Constant.renderingDialog = null;
            }
        });
        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }

    public Bitmap getBitmap(File bitmapPath){
        BitmapFactory.Options bmOptions = new BitmapFactory.Options();
        bmOptions.inJustDecodeBounds = false;
        //bmOptions.inPreferredConfig = Bitmap.Config.ARGB_8888;
        bmOptions.inDither = true;
        Bitmap bitmap = BitmapFactory.decodeFile(bitmapPath.getAbsolutePath(), bmOptions);
        return bitmap;
    }
}
