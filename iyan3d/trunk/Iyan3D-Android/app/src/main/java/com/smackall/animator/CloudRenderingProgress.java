package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.os.RemoteException;
import android.support.v4.content.ContextCompat;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.android.vending.billing.IInAppBillingService;
import com.smackall.animator.Adapters.RenderingProgressAdapter;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CreditTask;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.HQTaskDB;
import com.smackall.animator.Helper.MediaScannerWrapper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class CloudRenderingProgress implements View.OnClickListener,CreditTask {

    private Context mContext;
    private RenderingProgressAdapter adapter;
    private DatabaseHelper db;
    private Dialog dialog;

    public IInAppBillingService mService;
    HashMap<String, String> map;

    public CloudRenderingProgress(Context context,DatabaseHelper db,IInAppBillingService service){
        this.mContext = context;
        this.db = db;
        this.mService = service;
    }

    public void showCloudRenderingProgress(View v, MotionEvent event)
    {
        Dialog cloud_rendering = new Dialog(mContext);
        cloud_rendering.requestWindowFeature(Window.FEATURE_NO_TITLE);
        cloud_rendering.setContentView(R.layout.cloud_rendering);
        cloud_rendering.setCancelable(false);
        cloud_rendering.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType){
            case Constants.SCREEN_NORMAL:
                cloud_rendering.getWindow().setLayout((int) (Constants.width / 1.5), (int) (Constants.height));
                break;
            default:
                cloud_rendering.getWindow().setLayout(Constants.width / 3, (int) (Constants.height / 1.25));
                break;
        }
        Window window = cloud_rendering.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity= Gravity.TOP | Gravity.START;
        wlp.dimAmount=0.0f;
        if(event != null) {
            wlp.x = (int)event.getX();
            wlp.y = (int)event.getY();
        }
        else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        initViews(cloud_rendering);
        dialog = cloud_rendering;
        try {
            String className = mContext.getClass().getSimpleName();
            if (className.toLowerCase().equals("sceneselection"))
                ((SceneSelection) ((Activity) mContext)).creditsManager.getCreditsForUniqueId(CloudRenderingProgress.this);
            else
                ((EditorView) ((Activity) mContext)).creditsManager.getCreditsForUniqueId(CloudRenderingProgress.this);
        }
        catch (ClassCastException ignored){
            return;
        }
        cloud_rendering.show();
    }

    private void initViews(Dialog cloudRendering)
    {
        ((Button)cloudRendering.findViewById(R.id.add5K)).setOnClickListener(this);
        ((Button)cloudRendering.findViewById(R.id.add20K)).setOnClickListener(this);
        ((Button)cloudRendering.findViewById(R.id.add50K)).setOnClickListener(this);
        ((Button)cloudRendering.findViewById(R.id.sign_out)).setOnClickListener(this);
        ((Button)cloudRendering.findViewById(R.id.my_account)).setOnClickListener(this);
        showLoginTypeAndUserName(cloudRendering);
        adapter = new RenderingProgressAdapter(mContext,db);
        initList(cloudRendering);

    }

    private void showLoginTypeAndUserName(Dialog cloudRendering)
    {
        UserDetails userDetails = null;
        String className = mContext.getClass().getSimpleName();

        try{
            if (className.toLowerCase().equals("sceneselection"))
                userDetails =((SceneSelection) ((Activity) mContext)).userDetails;
            else
                userDetails = ((EditorView) ((Activity) mContext)).userDetails;
        }
        catch (ClassCastException ignored){}
        Drawable drawable = null;
        if(userDetails == null) return;
        switch (userDetails.signInType){
            case Constants.GOOGLE_SIGNIN:
                drawable = ContextCompat.getDrawable(mContext,R.drawable.gplus);
                break;
            case Constants.FACEBOOK_SIGNIN:
                drawable = ContextCompat.getDrawable(mContext,R.drawable.fb);
                break;
            case Constants.TWITTER_SIGNIN:
                drawable = ContextCompat.getDrawable(mContext,R.drawable.twit);
                break;
        }
        if(drawable != null)
            ((ImageView)cloudRendering.findViewById(R.id.loginTypeImage)).setImageDrawable(drawable);
            ((TextView)cloudRendering.findViewById(R.id.userName)).setText(userDetails.userName);

    }


    private void initList(Dialog dialog)
    {
        ((ListView)dialog.findViewById(R.id.progress_list)).setAdapter(adapter);
        adapter.updateTaskStatus();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.add5K:
                purchaseCredits("basicrecharge");
                break;
            case R.id.add20K:
                purchaseCredits("mediumrecharge");
                break;
            case R.id.add50K:
                purchaseCredits("megarecharge");
                break;
            case R.id.sign_out:
                try {
                    String className = mContext.getClass().getSimpleName();
                    if (className.toLowerCase().equals("sceneselection"))
                        ((SceneSelection) ((Activity) mContext)).login.logOut();
                    else
                        ((EditorView) ((Activity) mContext)).login.logOut();
                }
                catch (ClassCastException ignored){}
                break;
            case R.id.my_account:
                String url = "https://www.iyan3dapp.com/cms/";
                Intent i = new Intent(Intent.ACTION_VIEW);
                i.setData(Uri.parse(url));
                try {
                    ((Activity)mContext).startActivity(i);
                }
                catch (ActivityNotFoundException ignored){

                }
                break;
        }
        dialog.dismiss();
    }

    @Override
    public void onCreditRequestCompleted(int credit,int premiumUser) {
        ((ProgressBar)dialog.findViewById(R.id.progressBar)).setVisibility(View.GONE);
        ((TextView)dialog.findViewById(R.id.credit_lable)).setVisibility(View.VISIBLE);
        ((TextView)dialog.findViewById(R.id.credit_lable)).setText(String.format(Locale.getDefault(),"%d", credit));
    }

    @Override
    public void onCheckProgressCompleted(int progress, int taskId) {

    }

    @Override
    public void onTaskFileDownloadCompleted(HQTaskDB taskDB, View list, boolean downloadComplete) {

    }

    @Override
    public void finishExport(int frame, boolean status,String msg) {
        UIHelper.informDialog(mContext,msg);
    }

    @Override
    public void failed() {

    }

    private void purchaseCredits(String type){
        if(mService == null){
            return;
        }

        try {
            Bundle buyIntentBundle = mService.getBuyIntent(3, mContext.getPackageName(),type, "inapp", FileHelper.randomString(32));
            PendingIntent pendingIntent = buyIntentBundle.getParcelable("BUY_INTENT");
            if(pendingIntent != null) {
                ((Activity) mContext).startIntentSenderForResult(pendingIntent.getIntentSender(),
                        1002, new Intent(), 0, 0,
                        0);
            }
        } catch (RemoteException | IntentSender.SendIntentException e) {
            e.printStackTrace();
        }
    }

    public void purchaseResult(int requestCode, int resultCode, Intent data){
        if (requestCode == 1002 && Activity.RESULT_OK == resultCode && data != null) {
            int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
            String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
            String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");
            jsonToMap(purchaseData);
            String token = map.get("purchaseToken");
            try {
                int response = mService.consumePurchase(3, mContext.getPackageName(), token);
                if (response == 0) {
                    try {
                        JSONObject jo = new JSONObject(purchaseData);
                        String sku = jo.getString("productId");
                        int credit = 0;
                        if(sku.equals("basicrecharge"))
                            credit = 5000;
                        else if(sku.equals("mediumrecharge"))
                            credit = 25000;
                        else if(sku.equals("megarecharge"))
                            credit = 50000;
                        try {
                            String className = mContext.getClass().getSimpleName();
                            if (className.toLowerCase().equals("sceneselection"))
                                ((SceneSelection) (Activity) mContext).creditsManager.useOrRechargeCredits(credit, "RECHARGE", purchaseData, dataSignature, 0, CloudRenderingProgress.this);
                            else
                                ((EditorView) (Activity) mContext).creditsManager.useOrRechargeCredits(credit, "RECHARGE", purchaseData, dataSignature, 0, CloudRenderingProgress.this);
                        }catch (ClassCastException ignored){}
                    }
                    catch (JSONException e) {

                        e.printStackTrace();
                    }
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
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
}
