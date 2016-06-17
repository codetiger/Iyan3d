package com.smackall.animator.Helper;

import android.view.View;

/**
 * Created by Sabish.M on 28/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public interface CreditTask {

    void onCreditRequestCompleted(int credit,int premiumuser);
    void onCheckProgressCompleted(int progress,int taskId);
    void onTaskFileDownloadCompleted(HQTaskDB taskDB,View list,boolean downloadComplete);
    void finishExport(int frame,boolean status,String msg);
    void failed();
}
