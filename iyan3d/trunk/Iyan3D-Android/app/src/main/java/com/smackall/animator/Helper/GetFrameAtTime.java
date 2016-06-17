package com.smackall.animator.Helper;

import android.graphics.Bitmap;
import android.media.MediaMetadataRetriever;
import android.os.Environment;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class GetFrameAtTime {
    public Bitmap getFrameAtTime(int frameC) {
        Bitmap frame = null;
        MediaMetadataRetriever mediaMetadata = new MediaMetadataRetriever();
        try {
            mediaMetadata.setDataSource(Environment.getExternalStorageDirectory() + "/105.mp4");
            frame = null;
            frame = mediaMetadata.getFrameAtTime(frameC);
        } catch (Exception e) {
        }
        return frame;
    }
}
