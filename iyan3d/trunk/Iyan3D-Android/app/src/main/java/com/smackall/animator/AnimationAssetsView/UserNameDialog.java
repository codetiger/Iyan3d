package com.smackall.animator.AnimationAssetsView;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.View;
import android.widget.EditText;

import com.smackall.animator.common.Constant;

/**
 * Created by Sabish.M on 30/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class UserNameDialog {

    public static void userNameDialog(final Context context,  int width, int height, final AnimationSelectionView animationSelectionView) {


        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("Display Name");

        final EditText input = new EditText(context);
        input.setHint("Enter Your Name Here");
        alert.setView(input);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (input.getText().toString().trim().length() == 0){
                    Constant.informDialog(context,"User name cannot be empty.");
                    dialog.dismiss();
                    return;
                }
                String username = String.valueOf(input.getText());
                Constant.showProgressDialog(animationSelectionView,context,"Please Wait! Publishing is Progress..",false);
                animationSelectionView.publishAnimation(username);
                dialog.dismiss();
            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                AnimationSelectionView.anim_publish_btn.setVisibility(View.VISIBLE);
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }
}
