package com.smackall.animator.InAppPurchase;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.smackall.animator.AnimationEditorView.AnimationEditorMenuDialog;
import com.smackall.animator.ImportAndRig.ImportObjAndTexture;
import com.smackall.animator.TextAssetsView.TextAssetsSelectionView;
import com.smackall.animator.common.Constant;
import com.smackall.animator.common.KeyMaker;

import java.io.FileNotFoundException;

import smackall.animator.R;

/**
 * Created by shankarganesh on 21/11/15.
 */
public class PremiumUpgrade extends Activity {
    Button premiumCancel,premiumUpgrade,premiumRestorePurchase,premiumViewAd;
    AlertDialog progressAlertDialog;
    IabHelper mHelper;
    private InterstitialAd mInterstitialAd;
    String base64EncodedPublicKey="MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAskNw3MwFyt81PHzlPKV3eYvPBi0pIWuvuu2xe9uE8CzsRFHkzWnzUP1IzZSQHXhp+nQcW8d2JualTRk2G4OPPnNSjUtK9HgPtKtIt8q7MDUfpwFSynzev6eNbleXYQW21HN8I1siFqg+8QA5XgKHLKwZEDZezQuTFEeIbjFprJ+YBkBVv4FXKWGENmZHF/YG0lLfdP3wtZQgZLESS4X55Gcaa3UJE9hjzt6Xvzr2C+h1kw/njC7lzxbfSjw2jto9a+5WL6/uC23Ikd7Ji2Ul+QIZqnSB7A6mGyIG6q1WslrMcp/E5Uj1iwonf4dBUCmwVThHrIFzrzeOoqHQka3TTQIDAQAB";
    IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener;
    Context mContext;
    String actionType;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.setFinishOnTouchOutside(false);
        setContentView(R.layout.premium_upgrade);
        Intent in =getIntent();
        actionType=in.getExtras().getString("actiontype");

        Log.d("Premium Upgrade","Action type: "+actionType);
        mContext = this;
        switch (Constant.getScreenCategory(this)){
            case "normal":
                this.getWindow().setLayout((int) (Constant.width/1.5), Constant.height);
                break;
            case "large":
                this.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "xlarge":
                this.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
            case "undefined":
                this.getWindow().setLayout(Constant.width / 2, (int) (Constant.height / 1.14285714286));
                break;
        }
        TextView feature_premium = (TextView) findViewById(R.id.featurs_premium);
        feature_premium.setText("1. Import Unlimited OBJ Models\n" +
                "\n"+
                "2. Use any Font Style to add 3D text.\n" +
                "\n"+
                "3. Render the Video in toon style.\n" +
                "\n"+
                "4. Add skeleton to any OBJ models\n" +
                "\n"+
                "5. Remove Watermark from any output Video.\n" +
                "\n"+
                "6. Add Multiple lights to your Scene.");
        TextView price_text = (TextView) findViewById(R.id.price_text);

        price_text.setText("INR 99.00");

        premiumCancel=(Button)findViewById(R.id.premium_cancel);
        premiumUpgrade=(Button)findViewById(R.id.premium_accept);

        mHelper = new IabHelper(this, "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAskNw3MwFyt81PHzlPKV3eYvPBi0pIWuvuu2xe9uE8CzsRFHkzWnzUP1IzZSQHXhp+nQcW8d2JualTRk2G4OPPnNSjUtK9HgPtKtIt8q7MDUfpwFSynzev6eNbleXYQW21HN8I1siFqg+8QA5XgKHLKwZEDZezQuTFEeIbjFprJ+YBkBVv4FXKWGENmZHF/YG0lLfdP3wtZQgZLESS4X55Gcaa3UJE9hjzt6Xvzr2C+h1kw/njC7lzxbfSjw2jto9a+5WL6/uC23Ikd7Ji2Ul+QIZqnSB7A6mGyIG6q1WslrMcp/E5Uj1iwonf4dBUCmwVThHrIFzrzeOoqHQka3TTQIDAQAB");
        premiumViewAd=(Button)findViewById(R.id.premium_view_ad);
        //mHelper = new IabHelper(this, base64EncodedPublicKey);
        // Create the InterstitialAd and set the adUnitId.
        mInterstitialAd = new InterstitialAd(this);
        // Defined in res/values/strings.xml
        mInterstitialAd.setAdUnitId("ca-app-pub-2186321854033194/9300739229");
        mHelper.enableDebugLogging(true);
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            @Override
            public void onIabSetupFinished(IabResult result) {
                if (!result.isSuccess()) {
                    Log.d("IAB", "Problem setting up In-app Billing: " + result);
                }
                if (result.isSuccess()) {
                    Log.d("IAB", "IAB SETUP SUCCESSfull " + result);
                }
            }
        });


        mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {

            @Override
            public void onIabPurchaseFinished(IabResult result, Purchase info) {
                System.out.println("Listener Working");
                System.out.println("Result : " + result);
                System.out.println("Purchase Info : " + info);
                if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_ERROR) {
                    Log.d("IAB", "Error purchasing: " + result);
                    if(KeyMaker.makeKey(mContext)){
                        try {
                            KeyMaker.writeToKeyFile(Constant.md5(Constant.deviceUniqueId + Constant.md5("Iyan3dPremium" + 0)), mContext);
                        } catch (FileNotFoundException e) {
                            System.out.println("Purchase Write Failed");
                            e.printStackTrace();
                        }
                        Constant.isPremium(mContext);
                    }
                    else {
                        System.out.println("Failed");
                    }

                }
                else if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_OK) {
                    Log.d("IAB", "Successfull purchase " +result);
                        if(KeyMaker.makeKey(mContext)){
                            try {
                                KeyMaker.writeToKeyFile(Constant.md5(Constant.deviceUniqueId + Constant.md5("Iyan3dPremium" + 1)), mContext);
                            } catch (FileNotFoundException e) {
                                System.out.println("Purchase Write Failed");
                                e.printStackTrace();
                            }
                            Constant.isPremium(mContext);
                        }
                    else {
                            System.out.println("Failed");
                        }
                    return;
                }

                else if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED) {
                   System.out.println("Already Purchased");
                    if(KeyMaker.makeKey(mContext)){
                        try {
                            KeyMaker.writeToKeyFile(Constant.md5(Constant.deviceUniqueId+Constant.md5("Iyan3dPremium"+1)),mContext);
                        } catch (FileNotFoundException e) {
                            System.out.println("Purchase Write Failed");
                            e.printStackTrace();
                        }
                        Constant.isPremium(mContext);
                    }
                    return;
                }
            }
        };


        premiumCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mInterstitialAd = null;
                PremiumUpgrade.this.finish();
            }
        });
        premiumUpgrade.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mInterstitialAd = null;
                promptForUpgrade();
            }
        });

        premiumViewAd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final AlertDialog.Builder alert = new AlertDialog.Builder(PremiumUpgrade.this);
                alert.setMessage("Please Wait");
                final ProgressBar progressBar = new ProgressBar(PremiumUpgrade.this);
                alert.setView(progressBar);
                Constant.FullScreencall(PremiumUpgrade.this, PremiumUpgrade.this);
                progressAlertDialog = alert.create();
                progressAlertDialog.show();
//                progressAlertDialog.setOnCancelListener(new DialogInterface.OnCancelListener() {
//                    @Override
//                    public void onCancel(DialogInterface dialog) {
//                        mInterstitialAd = null;
//                        PremiumUpgrade.this.finish();

//                    }
//                });
                requestNewAd();
            }
        });
        // Show the ad if it's ready. Otherwise toast and restart the game.


        mInterstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdClosed() {
//                Constant.isTempPremium=true;
                finish();
                if(actionType.equalsIgnoreCase("watermark")){
                    Constant.exportDialog.disableWaterwark();
                    actionType="";
                }
                if(actionType.equalsIgnoreCase("toonshader")){
                    Constant.exportDialog.enableToonShader();
                    actionType="";
                }
                if(actionType.equalsIgnoreCase("importobj")){
                    actionType="";
                    Intent intent = new Intent();
                    intent.setClass(PremiumUpgrade.this, ImportObjAndTexture.class);
                    startActivity(intent);
                }
                if(actionType.equalsIgnoreCase("lights")){
                    actionType="";
                    AnimationEditorMenuDialog.addAdditionalLights();
                }
                if(actionType.equalsIgnoreCase("myfonts")){
                    String caller     = getIntent().getStringExtra("class");
//                    Class callerClass=null;
//                    try {
//                        callerClass = Class.forName(caller);
//                    } catch (ClassNotFoundException e) {
//                        e.printStackTrace();
//                    }
                    Activity act=TextAssetsSelectionView.mActivity;
                    ((TextAssetsSelectionView)act).myFontstab();
                    actionType="";
                }
            }

            @Override
            public void onAdFailedToLoad(int errorCode) {
                Log.d("Premium Upgrader", "Error Code: " + errorCode);
                if(AdRequest.ERROR_CODE_INTERNAL_ERROR==errorCode){
                    AlertDialog alertDialog = new AlertDialog.Builder(PremiumUpgrade.this).create();
                    alertDialog.setTitle("Alert");
                    alertDialog.setMessage("Problem Loading Video...Please try later!!");
                    alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int which) {
                                    dialog.dismiss();
                                    progressAlertDialog.dismiss();
                                }
                            });
                    alertDialog.show();
                }
                else if(AdRequest.ERROR_CODE_NETWORK_ERROR==errorCode){
                    AlertDialog alertDialog = new AlertDialog.Builder(PremiumUpgrade.this).create();
                    alertDialog.setTitle("Alert");
                    alertDialog.setMessage("Please Check Your Internet Connection");
                    alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int which) {
                                    dialog.dismiss();
                                    progressAlertDialog.dismiss();
                                }
                            });
                    alertDialog.show();
                }
                super.onAdFailedToLoad(errorCode);

            }

            @Override
            public void onAdLoaded() {
                progressAlertDialog.dismiss();
                showInterstitial();
                super.onAdLoaded();
            }

            @Override
            public void onAdOpened() {
                super.onAdOpened();
            }
        });



    }


    private void requestNewAd() {
       String m_androidId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        AdRequest adRequest = new AdRequest.Builder()
               // .addTestDevice("F2558610D7A6B0639B05FC515C6F9989")
//                .addTestDevice("928C8D8E3DEF2331307CDB1E4F26106C")
                .build();

        mInterstitialAd.loadAd(adRequest);
    }
    private void showInterstitial() {

        if (mInterstitialAd != null && mInterstitialAd.isLoaded()) {
            mInterstitialAd.show();
        } else {
            Toast.makeText(this, "Check Network ", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (mHelper.handleActivityResult(requestCode, resultCode, data))
            super.onActivityResult(requestCode, resultCode, data);

    }


    private void promptForUpgrade() {
        mHelper.launchPurchaseFlow(PremiumUpgrade.this, "premium", 1001,mPurchaseFinishedListener, "iyan3d");
    }

    @Override
    public void onBackPressed(){
        //Do nothing
    }
}
