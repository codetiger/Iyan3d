package com.smackall.animator.ImportAndRig;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.TextView;

import com.smackall.animator.opengl.GL2JNILibAutoRig;


/**
 * Created by Sabish.M on 17/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class WarningDialog {

    public static void warningConfirmDialog(Context context, final RigModel rigModel){

        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("WARNING");

        final TextView msg = new TextView(context);
        msg.setText("Are you sure you want to go back to previous mode? By pressing Yes all your changes will be discarded.");
        alert.setView(msg);

        alert.setPositiveButton("YES", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                rigModel.backOrnext--;
                rigModel.buttonHandler("preview");
                GL2JNILibAutoRig.prevBtnAction();
                dialog.dismiss();
            }
        });

        alert.setNegativeButton("NO", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();

    }
}
