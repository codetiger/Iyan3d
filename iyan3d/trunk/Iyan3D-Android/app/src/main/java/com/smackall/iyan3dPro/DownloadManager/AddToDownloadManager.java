package com.smackall.iyan3dPro.DownloadManager;

import android.content.Context;
import android.net.Uri;

import com.smackall.iyan3dPro.Adapters.AnimationSelectionAdapter;
import com.smackall.iyan3dPro.EditorView;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.R;
import com.smackall.iyan3dPro.TextSelection;

import java.util.Calendar;

/**
 * Created by Sabish.M on 23/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AddToDownloadManager {

    public boolean errorMessageShowing = false;
    public long lastErrorShowingTime = 0;
    long MAX_INTERVAL = 120000; //2 minutes in milli seconds
    private MyDownloadStatusListener myDownloadStatusListener = new MyDownloadStatusListener();
    private Context mContext;
    private TextSelection textSelection;
    private AnimationSelectionAdapter animationSelectionAdapter;

    public int downloadAdd(Context context, String url, String fileName, String destinationPath, DownloadRequest.Priority priority, DownloadManager downloadManager,
                           TextSelection textSelection, AnimationSelectionAdapter animationSelectionAdapter, String requiredFileName) {

        this.mContext = context;
        this.textSelection = textSelection;
        this.animationSelectionAdapter = animationSelectionAdapter;


        Uri uri = Uri.parse(url);
        Uri destinationUri = Uri.parse(destinationPath + ((requiredFileName != null) ? requiredFileName : fileName));

        DownloadRequest downloadRequest = new DownloadRequest(uri)
                .setDestinationURI(destinationUri).setPriority(priority)
                .setDownloadListener(myDownloadStatusListener);

        return downloadManager.add(downloadRequest);
    }


    class MyDownloadStatusListener implements DownloadStatusListener {
        @Override
        public void onDownloadComplete(int id) {

        }

        @Override
        public void onDownloadFailed(int id, int errorCode, String errorMessage) {
            if (errorCode != DownloadManager.ERROR_DOWNLOAD_CANCELLED && !errorMessageShowing && (Calendar.getInstance().getTimeInMillis() - lastErrorShowingTime) > MAX_INTERVAL) {
                lastErrorShowingTime = Calendar.getInstance().getTimeInMillis();
                errorMessageShowing = true;
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
                if (Constants.VIEW_TYPE == Constants.ANIMATION_VIEW && mContext != null && ((EditorView) mContext).animationSelection != null && ((EditorView) mContext).animationSelection.animationSelectionAdapter != null) {
                    ((EditorView) mContext).animationSelection.processCompleted = true;
                    ((EditorView) mContext).animationSelection.filePath = "";
                }
            }
        }

        @Override
        public void onProgress(int id, long totalBytes, long downloadedBytes, int progress) {
        }
    }
}
