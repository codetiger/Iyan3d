package com.smackall.animator.AnimationEditorView;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ListView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

import smackall.animator.R;


/**
 * Created by Sabish.M on 14/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SelectItems {

    public void selectItemInArray(final Context context, float x , float y){
        final Dialog imported_object_dialog = new Dialog(context);
        imported_object_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        imported_object_dialog.setContentView(R.layout.import_object_list);
        imported_object_dialog.setCancelable(true);
        imported_object_dialog.setCanceledOnTouchOutside(true);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                imported_object_dialog.getWindow().setLayout(Constant.animationEditor.width / 6, (int) (Constant.animationEditor.height / 1.5));
                break;
            case "large":
                imported_object_dialog.getWindow().setLayout(Constant.animationEditor.width / 7, Constant.animationEditor.height / 2);
                break;
            case "xlarge":
                imported_object_dialog.getWindow().setLayout(Constant.animationEditor.width / 8, (int) (Constant.animationEditor.height / 2.5));
                break;
            case "undefined":
                imported_object_dialog.getWindow().setLayout(Constant.animationEditor.width / 8, (int) (Constant.animationEditor.height / 2.5));
                break;
        }

        Window window = imported_object_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.LEFT;
        wlp.dimAmount = 0.0f;
        wlp.x = Constant.animationEditor.width - (Constant.animationEditor.width / 8);
        wlp.y = (int) y;
        System.out.println("Y Value : " + y);

        window.setAttributes(wlp);

        final ListView imported_object_list_view = (ListView) imported_object_dialog.findViewById(R.id.imported_object_list);



        String[] values;
        int assetsCount= GL2JNILib.getAssetCount();
        values = new String[assetsCount];
        for (int i = 0; i < assetsCount; i++) {
            values[i] = GL2JNILib.getAssetList(i).toUpperCase();
        }
        imported_object_list_view.setAdapter(new AdaptorForSelectItems(context, values));

        imported_object_dialog.show();
    }
}

