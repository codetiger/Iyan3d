package com.smackall.animator.ImportAndRig;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILibAutoRig;
import com.smackall.animator.sceneSelectionView.SceneSelectionView;

import java.util.Timer;
import java.util.TimerTask;


/**
 * Created by Sabish.M on 10/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AutoRigCloseConfirmDialog {

    public static void exitConfirmDialog(Context context, final Activity activity){

        final AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder
                .setMessage("Are you sure you want to exit?")
                .setCancelable(false)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    Constant.rigModel.renderer = false;
                    Constant.rigModel.glSurfaceViewAutoRigView.onPause();

                    final Timer timer = new Timer();
                    timer.schedule(new TimerTask() {
                        @Override
                        public void run() {
                            GL2JNILibAutoRig.deleteAutoRigScene();
                            Intent i = new Intent();
                            Constant.rigModel.glSurfaceViewAutoRigView.setWillNotDraw(true);
                            i.setClass(activity, SceneSelectionView.class);
                            activity.startActivity(i);
                            activity.finish();
                            timer.cancel();
                        }
                    }, 500);


                dialog.dismiss();
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        builder.create().show();

    }
}
