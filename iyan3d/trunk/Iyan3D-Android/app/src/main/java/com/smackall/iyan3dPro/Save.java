package com.smackall.iyan3dPro;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.text.InputType;
import android.widget.EditText;

import com.smackall.iyan3dPro.Helper.AnimDB;
import com.smackall.iyan3dPro.Helper.Constants;
import com.smackall.iyan3dPro.Helper.DatabaseHelper;
import com.smackall.iyan3dPro.Helper.FileHelper;
import com.smackall.iyan3dPro.Helper.PathManager;
import com.smackall.iyan3dPro.Helper.UIHelper;
import com.smackall.iyan3dPro.opengl.GL2JNILib;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * Created by Sabish.M on 23/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Save {

    private Context mContext;
    private DatabaseHelper db;

    public Save(Context context, DatabaseHelper db) {
        this.mContext = context;
        this.db = db;
    }

    public void enterNameForAnimation() {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mContext.getString(R.string.save_your_animation_as_template));
        final EditText input = new EditText(mContext);
        input.setHint(mContext.getString(R.string.animation_name));
        input.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(input);
        builder.setPositiveButton(mContext.getString(R.string.ok), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                saveAnimation(input.getText().toString());
            }
        });
        builder.setNegativeButton(mContext.getString(R.string.cancel), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        builder.show();
    }

    public void saveAnimation(final String name) {
        if (name.length() == 0) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.animation_name_empty));
            return;
        }
        if (FileHelper.isItHaveSpecialChar(name)) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.animation_name_special_char));
            return;
        }
        final int assetId = 80000 + db.getNextAnimationAssetId();
        final int type = (GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) == Constants.NODE_TEXT_SKIN) ? 1 : 0;
        ((EditorView) mContext).glView.queueEvent(new Runnable() {
            @Override
            public void run() {
                GL2JNILib.saveAnimation(((EditorView) mContext).nativeCallBacks, assetId, name, type);
            }
        });
    }

    public void addToDatabase(final boolean status, final String name, final int type) {
        ((Activity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (status) {
                    final int assetId = 80000 + db.getNextAnimationAssetId();
                    AnimDB animDB = new AnimDB();
                    animDB.setAssetsId(assetId);
                    animDB.setAnimName(name);
                    animDB.setPublishedId(0);
                    animDB.setRating(0);
                    animDB.setBoneCount(GL2JNILib.jointSize(GL2JNILib.getSelectedNodeId()));
                    animDB.setAnimType(type);
                    animDB.setUserId(((EditorView) mContext).userDetails.uniqueId);
                    animDB.setUserName(((EditorView) mContext).userDetails.userName);
                    animDB.setKeyword(((EditorView) mContext).userDetails.userName);
                    animDB.setUploaded(new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date()));
                    db.addNewMyAnimationAssets(animDB);
                    ((EditorView) mContext).imageManager.makeThumbnail(PathManager.LocalAnimationFolder + "/" + assetId + ".png");
                    UIHelper.informDialog(mContext, mContext.getString(R.string.animation_save_success));
                    showMyAnimationList();
                } else
                    System.out.println("Error");
            }
        });
    }

    public void showMyAnimationList() {
        if (((EditorView) mContext).animationSelection == null)
            ((EditorView) mContext).animationSelection = new AnimationSelection(mContext, db, ((EditorView) mContext).sharedPreferenceManager);
        Constants.VIEW_TYPE = Constants.ANIMATION_VIEW;
        ((EditorView) mContext).animationSelection.showAnimationSelection();
    }
}
