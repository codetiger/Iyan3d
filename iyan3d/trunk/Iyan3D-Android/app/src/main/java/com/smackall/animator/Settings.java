package com.smackall.animator;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.RemoteException;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.Switch;

import com.android.vending.billing.IInAppBillingService;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.opengl.GL2JNILib;

import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by Sabish.M on 8/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Settings implements CompoundButton.OnCheckedChangeListener , View.OnClickListener {

    Context mContext;
    SharedPreferenceManager sp;

    RadioButton  toolbar_left, toolbar_right,preview_small,preview_large,frame_count,frame_duration,preview_left_bottom,preview_left_top,preview_right_bottom,preview_right_top;
    Switch multi_select;
    Button restore_btn,done_btn;
    ImageView img_toolbar_left,img_toolbar_right,img_preview_small,img_preview_large,preview_left_bottom_btn,preview_left_top_btn,preview_right_bottom_btn,preview_right_top_btn;
    LinearLayout frame_count_btn,frame_duration_btn;
    boolean Ui;

    public IInAppBillingService mService;
    HashMap<String, String> map;


    public Settings(Context context,SharedPreferenceManager sp,IInAppBillingService service){
        this.mContext = context;
        this.sp = sp;
        this.mService = service;

    }

    public void showSettings() {
        HitScreens.SettingsView(mContext);
        if(map != null) map.clear();
        final Dialog dialog = new Dialog(mContext);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.setContentView(R.layout.settings_view);

        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                dialog.getWindow().setLayout((int) (Constants.width), Constants.height);
                break;
            default:
                dialog.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height);
                break;
        }

        dialog.setCanceledOnTouchOutside(false);
        initButtons(dialog);
        toolbar_left.setOnCheckedChangeListener(this);
        toolbar_right.setOnCheckedChangeListener(this);
        preview_small.setOnCheckedChangeListener(this);
        preview_large.setOnCheckedChangeListener(this);
        frame_count.setOnCheckedChangeListener(this);
        frame_duration.setOnCheckedChangeListener(this);
        preview_left_bottom.setOnCheckedChangeListener(this);
        preview_left_top.setOnCheckedChangeListener(this);
        preview_right_bottom.setOnCheckedChangeListener(this);
        preview_right_top.setOnCheckedChangeListener(this);
        multi_select.setOnCheckedChangeListener(this);

        img_toolbar_left.setOnClickListener(this);
        img_toolbar_right.setOnClickListener(this);
        img_preview_small.setOnClickListener(this);
        img_preview_large.setOnClickListener(this);
        preview_left_bottom_btn.setOnClickListener(this);
        preview_left_top_btn.setOnClickListener(this);
        preview_right_bottom_btn.setOnClickListener(this);
        preview_right_top_btn.setOnClickListener(this);
        frame_count_btn.setOnClickListener(this);
        frame_duration_btn.setOnClickListener(this);


        done_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HitScreens.EditorView(mContext);
                if(Constants.currentActivity == 1) {
                    ((EditorView) ((Activity) mContext)).swapViews();
                    ((EditorView) ((Activity) mContext)).renderManager.cameraPosition(Constants.height - ((FrameLayout) ((Activity)mContext).findViewById(R.id.glView)).getHeight());
                    Constants.VIEW_TYPE = Constants.EDITOR_VIEW;
                }
                dialog.dismiss();
            }
        });

        restore_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //queryForItemAvailable();
                try {
                    if ((Constants.currentActivity == 0) ? ((SceneSelection) (Activity) mContext).userDetails.signInType <= 0 : ((EditorView) (Activity) mContext).userDetails.signInType <= 0) {
                        informDialog(mContext, "Please SignIn to continue.");
                        done_btn.performClick();
                        return;
                    }
                }
                catch (ClassCastException ignored){return;}
                restorePurchase();
            }
        });

        settingsBtnHandler();
        dialog.show();
        Constants.VIEW_TYPE = Constants.SETTINGS_VIEW;
    }

    private void initButtons(Dialog dialog)
    {
        toolbar_left = (RadioButton)dialog.findViewById(R.id.toolBar_left_radio);
        toolbar_right = (RadioButton)dialog.findViewById(R.id.toolBar_Right_radio);
        preview_small = (RadioButton)dialog.findViewById(R.id.preview_small_radio);
        preview_large = (RadioButton)dialog.findViewById(R.id.preview_Large_radio);
        frame_count = (RadioButton)dialog.findViewById(R.id.frame_count_radio);
        frame_duration = (RadioButton)dialog.findViewById(R.id.frame_duration_radio);
        preview_left_bottom = (RadioButton)dialog.findViewById(R.id.preview_left_bottom_radio);
        preview_left_top = (RadioButton)dialog.findViewById(R.id.preview_left_top_radio);
        preview_right_bottom = (RadioButton)dialog.findViewById(R.id.preview_right_bottom_radio);
        preview_right_top = (RadioButton)dialog.findViewById(R.id.preview_right_top_radio);
        multi_select = (Switch)dialog.findViewById(R.id.mutiSelect_switch);

        img_toolbar_left = (ImageView)dialog.findViewById(R.id.img_toolbar_left);
        img_toolbar_right = (ImageView)dialog.findViewById(R.id.img_toobar_right);
        img_preview_small =(ImageView)dialog.findViewById(R.id.img_preview_small);
        img_preview_large =(ImageView)dialog.findViewById(R.id.img_preview_large);
        preview_left_bottom_btn=(ImageView)dialog.findViewById(R.id.img_left_bottom_btn);
        preview_left_top_btn=(ImageView)dialog.findViewById(R.id.img_left_top_tn);
        preview_right_bottom_btn=(ImageView)dialog.findViewById(R.id.img_right_bottom_btn);
        preview_right_top_btn=(ImageView)dialog.findViewById(R.id.img_right_top_btn);
        frame_count_btn = (LinearLayout)dialog.findViewById(R.id.frame_count_btn);
        frame_duration_btn = (LinearLayout)dialog.findViewById(R.id.frame_duration_btn);
        done_btn = (Button)dialog.findViewById(R.id.settings_done_btn);
        restore_btn = (Button)dialog.findViewById(R.id.restore_btn);

    }

    private void settingsBtnHandler()
    {
        Ui = true;
        int toolBarPosition = this.sp.getInt(mContext,"toolbarPosition");
        int frameType = this.sp.getInt(mContext,"frameType");
        int previewPosition = this.sp.getInt(mContext,"previewPosition");
        int multiSelect = this.sp.getInt(mContext,"multiSelect");
        int previewSize = this.sp.getInt(mContext,"previewSize");


        toolbar_left.setChecked((toolBarPosition == Constants.TOOLBAR_LEFT));
        toolbar_right.setChecked((toolBarPosition == Constants.TOOLBAR_RIGHT));

        preview_small.setChecked((previewSize == Constants.PREVIEW_SMALL));
        preview_large.setChecked((previewSize == Constants.PREVIEW_LARGE));

        frame_count.setChecked((frameType == Constants.FRAME_COUNT) ? true : false);
        frame_duration.setChecked((frameType == Constants.FRAME_DURATION) ? true : false);

        preview_left_bottom.setChecked((previewPosition == Constants.PREVIEW_LEFT_BOTTOM));
        preview_left_top.setChecked((previewPosition == Constants.PREVIEW_LEFT_TOP));
        preview_right_bottom.setChecked((previewPosition == Constants.PREVIEW_RIGHT_BOTTOM));
        preview_right_top.setChecked((previewPosition == Constants.PREVIEW_RIGHT_TOP));

        multi_select.setChecked((multiSelect == 1));
        Ui = false;
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if(Ui)return;
        int toolBarPreviewPosition = this.sp.getInt(mContext,"previewPosition");
        switch (buttonView.getId()){
            case R.id.toolBar_left_radio:
                sp.setData(mContext,"toolbarPosition",(isChecked) ? 0 : 1);
                break;
            case R.id.toolBar_Right_radio:
                sp.setData(mContext,"toolbarPosition",(isChecked) ? 1 : 0);
                break;
            case R.id.preview_small_radio:
                sp.setData(mContext,"previewSize",(isChecked) ? 0 : 1);
                break;
            case R.id.preview_Large_radio:
                sp.setData(mContext,"previewSize",(isChecked) ? 1 : 0);
                break;
            case R.id.frame_count_radio:
                this.sp.setData(mContext,"frameType",(isChecked) ? 0 : 1);
                break;
            case R.id.frame_duration_radio:
                this.sp.setData(mContext,"frameType",(isChecked) ? 1 : 0);
                break;
            case R.id.preview_left_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 0 : toolBarPreviewPosition);
                break;
            case R.id.preview_left_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 1 : toolBarPreviewPosition);
                break;
            case R.id.preview_right_bottom_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 2 :toolBarPreviewPosition);
                break;
            case R.id.preview_right_top_radio:
                this.sp.setData(mContext, "previewPosition", (isChecked) ? 3 : toolBarPreviewPosition);
                break;
            case R.id.mutiSelect_switch:
                this.sp.setData(mContext, "multiSelect", multi_select.isChecked() ? 1 : 0);
                break;
        }
        settingsBtnHandler();
    }

    @Override
    public void onClick(View v) {
        int toolBarPreviewPosition = this.sp.getInt(mContext,"previewPosition");
        switch (v.getId()){
            case R.id.img_toolbar_left:
                sp.setData(mContext,"toolbarPosition",0);
                break;
            case R.id.img_toobar_right:
                sp.setData(mContext,"toolbarPosition",1);
                break;
            case R.id.img_preview_small:
                sp.setData(mContext,"previewSize",0);
                break;
            case R.id.img_preview_large:
                sp.setData(mContext,"previewSize",1);
                break;
            case R.id.frame_count_btn:
                this.sp.setData(mContext,"frameType",0);
                break;
            case R.id.frame_duration_btn:
                this.sp.setData(mContext,"frameType",1);
                break;
            case R.id.img_left_bottom_btn:
                this.sp.setData(mContext,  "previewPosition",0);
                break;
            case R.id.img_left_top_tn:
                this.sp.setData(mContext,  "previewPosition", 1);
                break;
            case R.id.img_right_bottom_btn:
                this.sp.setData(mContext,  "previewPosition", 2);
                break;
            case R.id.img_right_top_btn:
                this.sp.setData(mContext,  "previewPosition", 3);
                break;
        }
        settingsBtnHandler();
    }


    private void restorePurchase() {
        if (mService == null) {
            return;
        }
        Bundle restore = null;
        try {
            restore = mService.getPurchases(3, mContext.getPackageName(), "inapp", null);
            int responseCode = restore.getInt("RESPONSE_CODE");
            if (responseCode == 0) {
                ArrayList<String> ownedSkus = restore.getStringArrayList("INAPP_PURCHASE_ITEM_LIST");
                ArrayList<String> purchaseDataList = restore.getStringArrayList("INAPP_PURCHASE_DATA_LIST");
                ArrayList<String> signatureList = restore.getStringArrayList("INAPP_DATA_SIGNATURE_LIST");
                String continuationToken = restore.getString("INAPP_CONTINUATION_TOKEN");
                if (purchaseDataList != null && signatureList != null && ownedSkus != null) {
                    for (int i = 0; i < purchaseDataList.size(); ++i) {
                        String sku = ownedSkus.get(i);
                        if(sku.equals("premium")){
                            String json = purchaseDataList.get(i);
                            String signature = signatureList.get(i);
                            new VerifyRestorePurchase(json,signature).execute();
                            return;
                        }
                    }
                    UIHelper.informDialog(mContext,"No Purchases found.");
                }
                else{
                    UIHelper.informDialog(mContext,"No Purchases found.");
                }
            }
            else{
                UIHelper.informDialog(mContext,"No Purchases found.");
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void result(int requestCode, int resultCode, Intent data){
        if (requestCode == 1001) {
            int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
            String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
            String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");
            if (resultCode == Activity.RESULT_OK) {
                try {
                    JSONObject jo = new JSONObject(purchaseData);
                    String sku = jo.getString("productId");
                }
                catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void queryForItemAvailable()
    {
        ArrayList<String> skuList = new ArrayList<String> ();
        skuList.add("basicrecharge");
        skuList.add("mediumrecharge");
        skuList.add("megarecharge");
        Bundle querySkus = new Bundle();
        querySkus.putStringArrayList("ITEM_ID_LIST", skuList);
        try {
            Bundle skuDetails = mService.getSkuDetails(3,
                    mContext.getPackageName(), "inapp", querySkus);
            int response = skuDetails.getInt("RESPONSE_CODE");
            if (response == 0) {
                ArrayList<String> responseList
                        = skuDetails.getStringArrayList("DETAILS_LIST");
                if(responseList != null){
                    for (String thisResponse : responseList) {
                        JSONObject object = new JSONObject(thisResponse);
                        String sku = object.getString("productId");
                        String price = object.getString("price");
                    }
                }
            }
        } catch (RemoteException | JSONException e) {
            e.printStackTrace();
        }
    }

    private class VerifyRestorePurchase extends AsyncTask<Integer, Integer, String> {
        String json;
        String signature;
        String response;
        public VerifyRestorePurchase(String json,String signature){
            this.json = json;
            this.signature = signature;
        }
        @Override
        protected String doInBackground(Integer... params) {
            String uniqueId = ((Constants.currentActivity == 0) ? ((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId);
            String url = GL2JNILib.verifyRestorePurchase();
            HttpClient httpClient = new DefaultHttpClient();
            HttpPost httpPost = new HttpPost(url);
            org.apache.http.entity.mime.MultipartEntity builder = new org.apache.http.entity.mime.MultipartEntity();
            try {
                builder.addPart("uniqueid",new StringBody(uniqueId));
                builder.addPart("signed_data",new StringBody(json));
                builder.addPart("signature",new StringBody(signature));
                httpPost.setEntity(builder);
                ResponseHandler<String> handler = new BasicResponseHandler();
                response = httpClient.execute(httpPost,handler);
            } catch (IOException e) {
                e.printStackTrace();
                UIHelper.informDialog(mContext,"Please Check your network connection.");
                return null;
            }
            return response;
        }
        protected void onPostExecute(String st) {
            if(response != null){
                jsonToMap(st);
                int result = Integer.parseInt(map.get("result"));
                String msg = map.get("message");
                if(result == 1){
                    UIHelper.informDialog(mContext,msg);
                }
            }
            else
                System.out.println("Response is null");
        }
    }

    private void jsonToMap(String t){
        try {
            if(map != null)
                map.clear();
            map = new HashMap<String, String>();
            JSONObject jObject = new JSONObject(t);
            Iterator<?> keys = jObject.keys();
            while (keys.hasNext()) {
                String key = (String) keys.next();
                String value = jObject.getString(key);
                map.put(key, value);
            }
        }
        catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public static void informDialog(final Context context, final String msg){
        ((Activity)context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder informDialog = new AlertDialog.Builder(context);
                informDialog
                        .setMessage(msg)
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface informDialog, int id) {
                                informDialog.dismiss();
                                try {
                                    if (Constants.currentActivity == 0) {
                                        ((SceneSelection) context).settings.done_btn.performClick();
                                        ((SceneSelection) context).showLogIn(((Activity) context).findViewById(R.id.login_btn));
                                    } else {
                                        ((EditorView) context).settings.done_btn.performClick();
                                        ((EditorView) context).showLogin(((Activity) context).findViewById(R.id.login));
                                    }
                                }
                                catch (ClassCastException ignored){return;}
                            }
                        });
                informDialog.create();
                try {
                    informDialog.show();
                } catch (WindowManager.BadTokenException e) {
                    e.printStackTrace();
                }
            }
        });
    }
}

