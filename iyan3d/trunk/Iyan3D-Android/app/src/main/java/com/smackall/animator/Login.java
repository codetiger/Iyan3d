package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;

import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.LoggingBehavior;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.login.widget.LoginButton;
import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.TwitterButton;
import com.smackall.animator.Helper.UIHelper;
import com.twitter.sdk.android.Twitter;
import com.twitter.sdk.android.core.Callback;
import com.twitter.sdk.android.core.Result;
import com.twitter.sdk.android.core.TwitterException;
import com.twitter.sdk.android.core.TwitterSession;
import com.twitter.sdk.android.core.models.User;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Login implements View.OnClickListener, GoogleApiClient.OnConnectionFailedListener {

    Context mContext;
    CallbackManager callbackManager;
    LoginButton facebookLoginBtn;
    GoogleApiClient mGoogleApiClient;
    TwitterButton twitterButton;
    HashMap<String, String> map;
    private Dialog login;
    private UserDetails userDetails;
    private FacebookCallback<LoginResult> callback = new FacebookCallback<LoginResult>() {
        @Override
        public void onSuccess(LoginResult loginResult) {
            GraphRequest request = GraphRequest.newMeRequest(
                    loginResult.getAccessToken(),
                    new GraphRequest.GraphJSONObjectCallback() {
                        @Override
                        public void onCompleted(JSONObject object, GraphResponse response) {
                            jsonToMap(response.getJSONObject());
                            userDetails.uniqueId = map.get("id");
                            userDetails.userName = map.get("name");
                            userDetails.mail = map.get("email");
                            userDetails.signInType = Constants.FACEBOOK_SIGNIN;
                            userDetails.setUserDetails();
                        }
                    });
            Bundle parameters = new Bundle();
            parameters.putString("fields", "id,name,email,gender");
            request.setParameters(parameters);
            request.executeAsync();
        }

        @Override
        public void onCancel() {
        }

        @Override
        public void onError(FacebookException e) {
            UIHelper.informDialog(mContext, mContext.getString(R.string.unable_to_sign_int));

        }
    };

    public Login(Context context) {
        this.mContext = context;
    }

    public void showLogin(View v, MotionEvent event) {
        if (map != null)
            map.clear();

        try {
            String className = mContext.getClass().getSimpleName();
            userDetails = className.toLowerCase().equals("sceneselection") ? ((SceneSelection) mContext).userDetails : ((EditorView) mContext).userDetails;
        } catch (ClassCastException ignored) {
            return;
        }

        Dialog login = new Dialog(mContext);
        login.requestWindowFeature(Window.FEATURE_NO_TITLE);
        login.setContentView(R.layout.signin);
        login.setCancelable(false);
        login.setCanceledOnTouchOutside(true);

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                login.getWindow().setLayout(Constants.width / 2, (int) (Constants.height / 1.1));
                break;
            default:
                login.getWindow().setLayout(Constants.width / 3, Constants.height / 2);
                break;
        }

        Window window = login.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.START;
        wlp.dimAmount = 0.0f;
        if (event != null) {
            wlp.x = (int) event.getX();
            wlp.y = (int) event.getY();
        } else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);
        this.login = login;
        initViews(login);

        login.show();

        twitterButton = new TwitterButton(mContext);
        LinearLayout a = (LinearLayout) login.findViewById(R.id.twitterBtnHolder);
        a.addView(twitterButton);
        twitterButton.setVisibility(View.GONE);
        twitter();
    }

    public void initAllSignSdk() {

        FacebookSdk.sdkInitialize(mContext);
        callbackManager = CallbackManager.Factory.create();
        FacebookSdk.addLoggingBehavior(LoggingBehavior.REQUESTS);

        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestEmail()
                .build();

        mGoogleApiClient = new GoogleApiClient.Builder(mContext)
                .addApi(Auth.GOOGLE_SIGN_IN_API, gso)
                .addOnConnectionFailedListener(this)
                .build();
    }

    private void initViews(Dialog login) {
        login.findViewById(R.id.sign_in_button).setOnClickListener(this);
        login.findViewById(R.id.dummyFacebookBtn).setOnClickListener(this);
        login.findViewById(R.id.dummyTwitterbtn).setOnClickListener(this);
        facebookLoginBtn = (LoginButton) login.findViewById(R.id.facebook);
        facebookLoginBtn.setReadPermissions(Arrays.asList(
                "public_profile", "email", "user_friends"));
        facebookLoginBtn.registerCallback(callbackManager, callback);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.sign_in_button:
                google();

                break;
            case R.id.dummyFacebookBtn:
                facebookLoginBtn.performClick();
                break;
            case R.id.dummyTwitterbtn:
                twitterButton.performClick();
                break;
        }
        if (login != null && login.isShowing())
            login.dismiss();
    }

    public void twitter() {
        twitterButton.setCallback(new Callback() {
            @Override
            public void success(Result result) {
                loginTwitter(result);
            }

            @Override
            public void failure(TwitterException exception) {
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
            }
        });
    }

    public void loginTwitter(Result<TwitterSession> result) {
        TwitterSession session = result.data;
        Twitter.getApiClient(session).getAccountService().verifyCredentials(true, false, new Callback<User>() {
            @Override
            public void failure(TwitterException e) {
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
                //If any error occurs handle it here
            }

            @Override
            public void success(Result<User> userResult) {
                User user = userResult.data;
                String profileImage = user.profileImageUrl.replace("_normal", "");
                userDetails.uniqueId = user.idStr;
                userDetails.userName = user.name;
                userDetails.signInType = Constants.TWITTER_SIGNIN;
                userDetails.setUserDetails();
            }
        });
    }

    private void google() {
        Intent signInIntent = Auth.GoogleSignInApi.getSignInIntent(mGoogleApiClient);
        ((Activity) mContext).startActivityForResult(signInIntent, Constants.GOOGLE_SIGNIN_REQUESTCODE);
    }

    public void googleSignInActivityResult(int requestCode, int resultCode, Intent data) {
        if (data != null) {
            GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);
            handleGoogleSignInResult(result);
        }
    }

    @Override
    public void onConnectionFailed(ConnectionResult connectionResult) {
        UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
    }

    private void handleGoogleSignInResult(GoogleSignInResult result) {
        if (result.isSuccess()) {
            mGoogleApiClient.connect();
            GoogleSignInAccount acct = result.getSignInAccount();
            if (acct != null && userDetails != null) {
                userDetails.uniqueId = acct.getId();
                userDetails.userName = acct.getDisplayName();
                userDetails.mail = acct.getEmail();
                userDetails.signInType = Constants.GOOGLE_SIGNIN;
                userDetails.setUserDetails();
            } else
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
        }
    }

    public void logOut() {

        try {
            String className = mContext.getClass().getSimpleName();
            if (className.toLowerCase().equals("sceneselection")) {
                ((SceneSelection) mContext).userDetails.resetUserDetails();
            } else
                ((EditorView) mContext).userDetails.resetUserDetails();
        } catch (ClassCastException ignored) {
            return;
        }
        try {
            Twitter.getSessionManager().clearActiveSession();
            Twitter.logOut();
            LoginManager.getInstance().logOut();

            if (mGoogleApiClient.isConnected())
                Auth.GoogleSignInApi.signOut(mGoogleApiClient).setResultCallback(new ResultCallback<Status>() {
                    @Override
                    public void onResult(@NonNull Status status) {

                    }
                });
            if (mGoogleApiClient.isConnected())
                Auth.GoogleSignInApi.revokeAccess(mGoogleApiClient).setResultCallback(new ResultCallback<Status>() {
                    @Override
                    public void onResult(@NonNull Status status) {

                    }
                });
        } catch (IllegalStateException e) {
            e.printStackTrace();
        }
        if (login != null && login.isShowing())
            login.dismiss();
    }

    private void jsonToMap(JSONObject t) {
        if (t == null) return;
        try {
            if (map != null)
                map.clear();
            map = new HashMap<String, String>();
            Iterator<?> keys = t.keys();

            while (keys.hasNext()) {
                String key = (String) keys.next();
                String value = t.getString(key);
                map.put(key, value);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}
