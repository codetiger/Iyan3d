package com.smackall.animator;

import android.os.Build;
import android.util.Log;

import com.smackall.animator.Helper.PathManager;

/**
 * Created by Sabish.M on 4/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FFmpeg {

    static {
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avcodec");
        System.loadLibrary("avutil");
        System.loadLibrary("videokit");
    }

    public int ffmpegRunCommand(String command, String outFileName, String widthHeight) {
        if (command.isEmpty()) {
            return 1;
        }
        String[] cmd = new String[]{"ffmpeg", "-y", "-r", "24", "-i", PathManager.LocalCacheFolder + "/" + "%d.png", PathManager.RenderPath + "/" + outFileName + ".mp4"};
        for (String aCmd : cmd) System.out.println("Cmd : " + aCmd);
        return run(cmd.length, cmd);
    }

    public int checkFFmpeg()
    {
        String[] cmd = new String[]{"ffmpeg", "-y", "-r", "24", "-i", PathManager.RenderPath + "/imgs/" + "%d.png", PathManager.RenderPath + "/" + "dummy" + ".mp4"};
        for (String aCmd : cmd) System.out.println("Cmd : " + aCmd);
        return run(cmd.length, cmd);
    }

    public static native int run(int argc, String[] args);

}













