package com.smackall.animator.AnimationEditorView;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;


/**
 * Created by Sabish.M on 10/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class ConfirmDialog {

    public static void deleteConfirmDialog(final Context context, final String type){

        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        final TextView msg = new TextView(context);
        if(type.equals("object")){
            alert.setMessage("Delete Object");
            msg.setText("Do you want to delete the object?");
        }
        if(type.equals("animation")){
            alert.setMessage("Delete Animation");
            msg.setText("Do you want to delete the Animation?");
        }

        alert.setView(msg);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (type.equals("object")) {
                    GL2JNILib.deleteObject();
                    //Constant.animationEditor.importedItems.remove(Constant.animationEditor.selectedNodeId);//Select List
                    Constant.animationEditor.selectedNodeType = 3;
                    Constant.animationEditor.selectedNodeId = -1;
                    ButtonHandler.buttonHanler();
                } else if (type.equals("animation")) {
                    boolean deleanim=GL2JNILib.deleteanimation();
                    if(!deleanim){
                        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
                        alertDialog.setTitle("Info");
                        alertDialog.setMessage("There is no Animation in this frame ");
                        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int which) {
                                        dialog.dismiss();
                                    }
                                });
                        alertDialog.show();

                    }

                }
                dialog.dismiss();
            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
            }
        });
            alert.create().show();
    }
}
