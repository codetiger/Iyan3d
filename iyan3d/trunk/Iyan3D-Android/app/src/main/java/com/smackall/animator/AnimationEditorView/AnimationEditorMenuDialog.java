package com.smackall.animator.AnimationEditorView;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.smackall.animator.AnimationAssetsView.AnimationSelectionView;
import com.smackall.animator.AssetsViewController.AssetsViewController;
import com.smackall.animator.InAppPurchase.PremiumUpgrade;
import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILib;

import smackall.animator.R;


/**
 * Created by Sabish.M on 1/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class AnimationEditorMenuDialog {
    static PopupWindow popWindow;
    public static void menuDialog(final Context context, final Activity activity, View v) {

        final LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        final View menuDialog;

        menuDialog = inflater.inflate(R.layout.activity_menu_pop_up, null);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                popWindow = new PopupWindow(menuDialog, (int) ((Constant.animationEditor.width / 1.5) * 1.3), (int) ((Constant.animationEditor.height / 1.5) * 1.1), true);
                break;
            case "large":
               popWindow = new PopupWindow(menuDialog, (int) ((Constant.animationEditor.width / 2) * 1.3), (int) ((Constant.animationEditor.height / 2) * 1.1), true);
                break;
            case "xlarge":
                     popWindow = new PopupWindow(menuDialog, (int) ((Constant.animationEditor.width / 2.5) * 1.3), (int) ((Constant.animationEditor.height / 2.5) * 1.1), true);
                break;
            case "undefined":
                    popWindow = new PopupWindow(menuDialog, (int) ((Constant.animationEditor.width / 2.5) * 1.3), (int) ((Constant.animationEditor.height / 2.5) * 1.1), true);
                break;
        }

        popWindow.setOutsideTouchable(true);
        popWindow.setAnimationStyle(R.style.DialogAnimation);
        popWindow.showAtLocation(v, Gravity.BOTTOM, 0, 0);

        LinearLayout models_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_model_btn);
        LinearLayout images_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_images_btn);
        LinearLayout text_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_text_btn);
        LinearLayout animate_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_animate_btn);
        LinearLayout save_template_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_save_btn);
        LinearLayout image_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_image_export_btn);
        LinearLayout video_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_video_export_btn);
        LinearLayout gif_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_gif_export_btn);
        LinearLayout menu_btn = (LinearLayout) menuDialog.findViewById(R.id.menu_popup_menu_btn);
        LinearLayout light_btn = (LinearLayout) menuDialog.findViewById(R.id.light_btn);


        models_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILib.storeShaderProps();
                Intent modelImport = new Intent(activity, AssetsViewController.class);
                activity.startActivity(modelImport);
                Constant.FullScreencall(activity,context);
                popWindow.dismiss();
            }
        });
        images_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Constant.animationEditor.importImageFromLocal(" "," ",0,0);
                Intent intent = new Intent(activity, ImportImage.class);
                activity.startActivity(intent);
                popWindow.dismiss();
            }
        });
        text_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILib.storeShaderProps();
                Intent textAct = new Intent();
                textAct.setClass(activity, TextAssetsSelectionView.class);
                activity.startActivity(textAct);
                popWindow.dismiss();
            }
        });

        animate_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int selectedNodeId = Constant.animationEditor.selectedNodeId;
                int selectedNodeType = Constant.animationEditor.selectedNodeType;
                if (selectedNodeId == -1 || GL2JNILib.isJointSelected() || selectedNodeType == 5 || selectedNodeType == 7||
                        selectedNodeType ==0 || selectedNodeType == 1 || selectedNodeType == 6 || selectedNodeType == 2) {
                    InformationDialog.informationDialog(context, activity);
                    popWindow.dismiss();
                } else {
                    int assetId = GL2JNILib.getAssetId();
                    int selectedAssetType = GL2JNILib.getAssetType();
                    if (assetId == -1 || GL2JNILib.isJointSelected())
                        InformationDialog.informationDialog(context, activity);
                    else {
                        if (selectedAssetType == 3 || selectedAssetType == 4) {
                            GL2JNILib.storeShaderProps();
                            Intent animationAct = new Intent();
                            animationAct.putExtra("assetId", assetId);
                            animationAct.putExtra("assetType", selectedAssetType);
                            animationAct.setClass(activity, AnimationSelectionView.class);
                            activity.startActivity(animationAct);
                            //Constant.FullScreencall(activity, context);
                            popWindow.dismiss();
                        } else {
                            InformationDialog.informationDialog(context, activity);
                        }
                    }
                }
            }
        });
        image_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Constant.exportDialog = new ExportDialog();
                int resolution = GL2JNILib.getCameraResolution();
                Constant.exportDialog.exportDialog(context, activity, "image",resolution);
                Constant.FullScreencall(activity,context);
                popWindow.dismiss();
            }
        });
        video_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Constant.exportDialog = new ExportDialog();
                int resolution = GL2JNILib.getCameraResolution();
                Constant.exportDialog.exportDialog(context, activity, "video",resolution);
                Constant.FullScreencall(activity,context);
                popWindow.dismiss();
            }
        });
        gif_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Constant.exportDialog = new ExportDialog();
                int resolution = GL2JNILib.getCameraResolution();
                Constant.exportDialog.exportDialog(context, activity, "gif",resolution);
                Constant.FullScreencall(activity,context);
                popWindow.dismiss();
            }
        });
        save_template_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_RIG.getValue() && !GL2JNILib.isJointSelected()
                        ||Constant.animationEditor.selectedNodeId != -1 && Constant.animationEditor.selectedNodeType == AnimationEditor.NODE_RIG_OR_TEXT.NODE_TEXT.getValue() && !GL2JNILib.isJointSelected()) {
                    SaveTemplateDialog.saveTemplateDialog(context, activity, Constant.animationEditor.width, Constant.animationEditor.height, "anonymous@mail.com", "anonymous", Integer.toString(0), String.valueOf(Constant.animationEditor.selectedNodeType));
                }
                else {
                    SaveTemplateDialog.informationDialog(context, Constant.animationEditor.width, Constant.animationEditor.height);
                }
                popWindow.dismiss();
            }
        });
        light_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Constant.isPremium(context)){
                    int lightCount = GL2JNILib.addExtraLight();
                    if(lightCount < 5){
                        System.out.println("Light Added");
                        //Constant.animationEditor.importedItems.add("LIGHT" + lightCount);//Select List
                    }
                    else {
                        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
                        alertDialog.setTitle("Alert");
                        alertDialog.setMessage("Scene cannot contain more than five Lights");
                        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int which) {
                                        dialog.dismiss();
                                    }
                                });
                        alertDialog.show();
                    }
                    popWindow.dismiss();
                }
                else{
                    int lightcount=GL2JNILib.getLightcount();
                    if(lightcount<5){
                        Intent in= new Intent(context,PremiumUpgrade.class);
                        in.putExtra("actiontype","lights");
                        context.startActivity(in);
                    }
                    else{
                        AlertDialog alertDialog = new AlertDialog.Builder(Constant.animationEditor.context).create();
                        alertDialog.setTitle("Alert");
                        alertDialog.setMessage("Scene cannot contain more than five Lights");
                        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int which) {
                                        dialog.dismiss();
                                    }
                                });
                        alertDialog.show();
                    }
                    popWindow.dismiss();
                }



            }
        });
        menu_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Constant.FullScreencall(activity,context);
                popWindow.dismiss();
            }
        });

        popWindow.setOnDismissListener(new PopupWindow.OnDismissListener() {
            @Override
            public void onDismiss() {
                popWindow = null;
            }
        });

    }

    public void viewPicker(Context context, int x, int y,Activity activity) {
        final Dialog view_picker_dialog = new Dialog(context);
        view_picker_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        view_picker_dialog.setContentView(R.layout.list_item_for_animation_editor_views);
        view_picker_dialog.setCancelable(true);
        view_picker_dialog.setCanceledOnTouchOutside(true);

        switch (Constant.getScreenCategory(context)){
            case "normal":
                view_picker_dialog.getWindow().setLayout(Constant.width / 6, (int) (Constant.height / 1.5));
                break;
            case "large":
                view_picker_dialog.getWindow().setLayout(Constant.width / 7, Constant.height / 2);
                break;
            case "xlarge":
                view_picker_dialog.getWindow().setLayout(Constant.width / 8, (int) (Constant.height / 2.5));
                break;
            case "undefined":
                view_picker_dialog.getWindow().setLayout(Constant.width / 8, (int) (Constant.height / 2.5));
                break;
        }



        Window window = view_picker_dialog.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.LEFT;
        wlp.dimAmount = 0.0f;
        wlp.x = x;
        wlp.y = y;
        System.out.println("Y Value : " + y);

        window.setAttributes(wlp);

        TextView top_view = (TextView) view_picker_dialog.findViewById(R.id.top_view);
        TextView left_view = (TextView) view_picker_dialog.findViewById(R.id.left_view);
        TextView front_view = (TextView) view_picker_dialog.findViewById(R.id.front_view);
        TextView bottom_view = (TextView) view_picker_dialog.findViewById(R.id.bottom_view);
        TextView right_view = (TextView) view_picker_dialog.findViewById(R.id.right_view);
        TextView back_view = (TextView) view_picker_dialog.findViewById(R.id.back_view);
        TextView myview_text = (TextView) view_picker_dialog.findViewById(R.id.myview_text);

        view_picker_dialog.show();
        Constant.FullScreencall(activity, context);

        top_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILib.cameraTopView();
            }
        });
        left_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            GL2JNILib.cameraLeftView();
            }
        });
        front_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            GL2JNILib.cameraFrontView();
            }
        });
        bottom_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            GL2JNILib.cameraBottomView();
            }
        });
        right_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            GL2JNILib.cameraRightView();
            }
        });
        back_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GL2JNILib.cameraBackView();
            }
        });
    }

    public static void addAdditionalLights(){
        int lightCount = GL2JNILib.addExtraLight();
        if(lightCount < 5){
            System.out.println("Light Added");
            //Constant.animationEditor.importedItems.add("LIGHT" + lightCount);//Select List
        }
        else {

            System.out.println("Light Cannot be added");
        }
        if(popWindow != null && popWindow.isShowing())
            popWindow.dismiss();
    }

}

