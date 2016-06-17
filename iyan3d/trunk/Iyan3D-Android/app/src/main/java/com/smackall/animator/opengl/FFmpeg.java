package com.smackall.animator.opengl;

/**
 * Created by Sabish.M on 26/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FFmpeg {
    static {
                System.loadLibrary("libavutil.so");
                System.loadLibrary("libswscale.so");
                System.loadLibrary("libavcodec.so");
                System.loadLibrary("libavformat.so");
                System.loadLibrary("libffmpeg_mediametadataretriever_jni.so");
    }

    public static native void setDataSource(String path) throws IllegalArgumentException;;
    public static native byte[] getFrameAtTime(long time, int option);
    public static native byte[] getEmbeddedPicture();
    public static native Object extractMetadata(String key);
    public static native void release();
    public static native void lfinalize();
    public static native void linit();
    public static native void lsetup();
}
