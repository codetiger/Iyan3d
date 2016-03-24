package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.text.InputType;
import android.widget.EditText;

import com.smackall.animator.Helper.AnimDB;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by Sabish.M on 23/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Save {

    private Context mContext;
    private DatabaseHelper db;
    public Save(Context context,DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    public void enterNameForAnimation()
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle("Save your Animation as template");
        final EditText input = new EditText(mContext);
        input.setHint("Animation Name");
        input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
        builder.setView(input);
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                saveAnimation(input.getText().toString());
            }
        });
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        builder.show();
    }

    public void saveAnimation(final String name)
    {
        if(name.length() == 0){ UIHelper.informDialog(mContext,"Animation name cannot be empty."); return;}
        if(FileHelper.isItHaveSpecialChar(name)) {UIHelper.informDialog(mContext,"Animation Name cannot contain any special characters.");return;}
        final int assetId = 80000 + db.getNextAnimationAssetId();
        final int type = (GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_TEXT_SKIN) ? 1 : 0;
        ((EditorView)((Activity)mContext)).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.saveAnimation(Save.this,assetId,name,type);
            }
        });
    }

    public void addToDatabase(final boolean status, final String name, final int type)
    {
        ((Activity)mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (status) {
                    final int assetId = 80000 + db.getNextAnimationAssetId();
                    AnimDB animDB = new AnimDB();
                    animDB.setAssetsId(assetId);
                    animDB.setAnimName(name);
                    animDB.setPublishedId(0);
                    animDB.setRating(0);
                    animDB.setBonecount(GL2JNILib.jointSize(GL2JNILib.getSelectedNodeId()));
                    animDB.setAnimType(type);
                    animDB.setUserid("");
                    animDB.setUserName("Anonymous");
                    animDB.setKeyword("");
                    animDB.setUploaded(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date()));
                    db.addNewMyAnimationAssets(animDB);
                    ((EditorView)((Activity)mContext)).imageManager.makeThumbnail(PathManager.LocalAnimationFolder+"/"+assetId+".png");
                    UIHelper.informDialog(mContext,"Animation Saved Successfully.");
                }
                else
                    System.out.println("Error");
            }
        });
    }
}
