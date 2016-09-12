package com.smackall.iyan3dPro.Helper.Listeners;

import android.view.ViewGroup;

/**
 * Created by Sabish.M on 2/9/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public interface FilePickerListener {

    void FilePickerListenerCallback(String path, boolean isCanceled, boolean isTempNode, ViewGroup insertPoint);
}
