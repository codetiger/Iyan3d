package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.EditText;
import android.widget.TextView;

import com.smackall.animator.AnimationAssetsView.AnimDB;
import com.smackall.animator.AnimationAssetsView.AnimationDatabaseHandler;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;
import com.smackall.animator.opengl.GL2JNILibAssetView;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by Sabish.M on 28/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SaveTemplateDialog {

    public static void saveTemplateDialog(final Context context, final Activity activity, int width, int height, final String userId, final String userName, final String boneCount, final String type) {

        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("NEW ANIMATION");

        final EditText input = new EditText(context);
        input.setHint("Animation Name");
        alert.setView(input);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (input.getText().toString().trim().length() == 0){
                    Constant.informDialog(context,"Animation name cannot be empty.");
                    dialog.dismiss();
                    return;
                }
                String templateName = String.valueOf(input.getText());
                if(Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_RIG.getValue() && !GL2JNILib.isJointSelected())
                    saveTemplateToLocalandDB(context, templateName, userId,  userName, type,Integer.toString(GL2JNILibAssetView.getBoneCount(0)));
                if(Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_TEXT.getValue() && !GL2JNILib.isJointSelected())
                    saveTemplateToLocalandDB(context, templateName, userId, userName,type, Integer.toString(0));
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }

    public static void informationDialog(Context context, int width, int height) {
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("INFORMATION");

        final TextView msg = new TextView(context);
        msg.setText("Please select a text or character to save Template.");
        alert.setView(msg);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }


    private static void saveTemplateToLocalandDB(Context context, String templateName, String userId, String userName,String type, String boneCount){
        System.out.println("My Animation Added Function Called");
        AnimationDatabaseHandler animationDatabaseHandler = new AnimationDatabaseHandler(context);
        animationDatabaseHandler.createMyAnimationTable(Constant.assetsDatabase);

        String time  = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());

        int myanimationCount = animationDatabaseHandler.getAssetsCount("MyAnimation");
        String aniamtionType = type;
        if(Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_RIG.getValue())
            aniamtionType = Constant.TYPE.NODE_RIG.getValue();
        if(Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_TEXT.getValue())
            aniamtionType = Constant.TYPE.NODE_TEXT.getValue();
        GL2JNILib.saveTemplate(Integer.toString(myanimationCount+80000));
        animationDatabaseHandler.addAssets(new AnimDB(Integer.toString(myanimationCount + 80000), templateName, templateName, userId, userName, aniamtionType, boneCount, 1, time, 0, 0, "0"), "myanimation");
    }
}

