package com.smackall.animator.Helper;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.view.WindowManager;

/**
 * Created by Sabish.M on 5/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UIHelper {

    public static void informDialog(final Context context, final String msg){
        ((Activity)context).runOnUiThread(new Runnable() {
            @Override
            public void run() {

                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                dialog.dismiss();
                            }
                        });
                informDialog.create();
                try {
                    informDialog.show();
                } catch (WindowManager.BadTokenException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public static int getScreenCategory(Context context){
        if ((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_LARGE) {
            return Constants.SCREEN_LARGE;
        }
        else if((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_NORMAL){
            return Constants.SCREEN_NORMAL;
        }
        else if((context.getResources().getConfiguration().screenLayout &
                Configuration.SCREENLAYOUT_SIZE_MASK) ==
                Configuration.SCREENLAYOUT_SIZE_XLARGE){
            return Constants.SCREEN_XLARGE;
        }
        else {
            return Constants.SCREEN_UNDEFINED;
        }
    }

}
