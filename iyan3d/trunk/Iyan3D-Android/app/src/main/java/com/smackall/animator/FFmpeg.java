package com.smackall.animator;

import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.PathManager;

/**
 * Created by Sabish.M on 4/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FFmpeg {

    static {
        try {
            System.loadLibrary("videokit");
        } catch (ExceptionInInitializerError | UnsatisfiedLinkError | Exception e) {
            e.printStackTrace();
            Constants.forceToJCodec = true;
        }
    }

    public static native int run(int argc, String[] args);

    public int ffmpegRunCommand(String startImage, String outFileName) {
        String[] cmd = new String[]{"ffmpeg", "-y", "-r", "24", "-start_number", startImage, "-i", PathManager.LocalCacheFolder + "/" + "%d.png", "-vcodec", "mpeg4", "-q:v", "1", PathManager.RenderPath + "/" + outFileName + ".mp4"};
        return run(cmd.length, cmd);
    }
}













