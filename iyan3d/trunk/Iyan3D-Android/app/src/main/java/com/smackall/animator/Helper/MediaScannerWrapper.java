package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import java.io.File;

/**
 * Created by Sabish.M on 27/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class MediaScannerWrapper {
    // do the scanning
    public static void scan(Context mContext,String mPath) {
        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        intent.setData(Uri.fromFile(new File(mPath)));
        mContext.sendBroadcast(intent);
    }
}
