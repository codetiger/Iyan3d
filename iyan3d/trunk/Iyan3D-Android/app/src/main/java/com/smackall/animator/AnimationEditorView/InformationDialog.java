package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.TextView;


/**
 * Created by Sabish.M on 8/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class InformationDialog {

    public static void informationDialog(Context context , Activity activity){

        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("INFORMATION");

        final TextView msg = new TextView(context);
        msg.setText("Please select a text or character to apply animation.");
        alert.setView(msg);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
            }
        });

            AlertDialog alertDialog = alert.create();
            alertDialog.show();
        }
    }

