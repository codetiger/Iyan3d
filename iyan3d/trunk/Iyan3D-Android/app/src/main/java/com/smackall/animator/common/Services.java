package com.smackall.animator.common;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * Created by Sabish.M on 30/11/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Services extends Service {


    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        System.out.println("Service Created");
    }

    @Override
    public void onDestroy() {
        System.out.println("Service Destroyed");
    }

    @Override
    public void onStart(Intent intent, int startid) {
        System.out.println("Service Started");
    }
}