package com.smackall.animator.ImportAndRig;

import android.app.Dialog;
import android.content.Context;
import android.util.TypedValue;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

import com.smackall.animator.common.Constant;

import smackall.animator.R;


/**
 * Created by Sabish.M on 15/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SelectBoneStructure {

    public static void selectboneStructureDialog(final Context context, final RigModel rigModel){
        final Dialog select_bone_dialog = new Dialog(context);
        select_bone_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        select_bone_dialog.setContentView(R.layout.select_bone_structure_layout);

        select_bone_dialog.setCancelable(true);
        select_bone_dialog.setCanceledOnTouchOutside(false);
        select_bone_dialog.getWindow().setLayout((Constant.width / 3), (int) (Constant.height / 1.5));


        TextView select_bone_dialog_head = (TextView) select_bone_dialog.findViewById(R.id.select_bone_dialog_head);
        TextView select_bone_sub_head= (TextView) select_bone_dialog.findViewById(R.id.select_bone_sub_head);


        select_bone_dialog_head.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.width/60);
        select_bone_sub_head.setTextSize(TypedValue.COMPLEX_UNIT_PX, Constant.width/60);

        Button single_bone_btn = (Button) select_bone_dialog.findViewById(R.id.single_bone_btn);
        Button human_bone_btn = (Button) select_bone_dialog.findViewById(R.id.human_bone_btn);
        Button cancel_btn = (Button) select_bone_dialog.findViewById(R.id.cancel_btn);

        select_bone_dialog.show();

        single_bone_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rigModel.backOrnext++;
                rigModel.addSingleBone();
                rigModel.buttonHandler("attachbone");
                Constant.FullScreencall(rigModel,context);
                select_bone_dialog.dismiss();
            }
        });

        human_bone_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rigModel.backOrnext++;
                rigModel.addSkeleton();
                rigModel.buttonHandler("attachbone");
                Constant.FullScreencall(rigModel,context);
                select_bone_dialog.dismiss();
            }
        });

        cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            select_bone_dialog.dismiss();

            }
        });

    }
}
