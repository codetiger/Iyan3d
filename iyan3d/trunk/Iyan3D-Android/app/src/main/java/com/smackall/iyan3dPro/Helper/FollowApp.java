package com.smackall.iyan3dPro.Helper;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.view.View;
import android.view.Window;

import com.smackall.iyan3dPro.R;

/**
 * Created by Sabish.M on 30/5/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */

public class FollowApp implements View.OnClickListener {
    Context mContext;
    Dialog follow;

    public FollowApp(Context mContext) {
        this.mContext = mContext;
    }

    public void showFollowOption() {
        Dialog follow = new Dialog(mContext);
        follow.requestWindowFeature(Window.FEATURE_NO_TITLE);
        follow.setContentView(R.layout.follow_us_social);
        follow.setCancelable(false);
        follow.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                follow.getWindow().setLayout(Constants.width / 2, (int) (Constants.height / 1.1));
                break;
            default:
                follow.getWindow().setLayout(Constants.width / 3, Constants.height / 2);
                break;
        }
        this.follow = follow;
        initView(follow);
        follow.show();
    }

    private void initView(Dialog follow) {
        follow.findViewById(R.id.facebook_follow).setOnClickListener(this);
        follow.findViewById(R.id.twitter_follow).setOnClickListener(this);
        follow.findViewById(R.id.google_follow).setOnClickListener(this);
        follow.findViewById(R.id.youtube_follow).setOnClickListener(this);
    }

    public void getOpenFacebookIntent(Context context) {
        try {
            context.getPackageManager().getPackageInfo("com.facebook.katana", 0);
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("fb://page/419066601594961")));
        } catch (Exception e) {
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.facebook.com/iyan3d")));
        }
    }

    public void getOpenTwitterIntent(Context context) {
        try {
            context.getPackageManager().getPackageInfo("com.twitter.android", 0);
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("twitter://user?user_id=114656838")));
        } catch (Exception e) {
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://twitter.com/SmackallGames")));
        }
    }

    public void getOpenGooglePlusIntent(Context context) {
        try {
            context.getPackageManager().getPackageInfo("com.google.android.apps.plus", 0);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse("https://plus.google.com/communities/108544248627724394883/"));
            intent.setPackage("com.google.android.apps.plus"); // don't open the browser, make sure it opens in Google+ app
            mContext.startActivity(intent);
        } catch (Exception e) {
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://plus.google.com/communities/108544248627724394883")));
        }
    }

    public void getOpenYoutubeIntent(Context context) {
        try {
            context.getPackageManager().getPackageInfo("com.google.android.youtube", 0);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse("https://www.youtube.com/user/SmackallGames"));
            intent.setPackage("com.google.android.youtube"); // don't open the browser, make sure it opens in Google+ app
            mContext.startActivity(intent);
        } catch (Exception e) {
            mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.youtube.com/user/SmackallGames")));
        }
    }

    @Override
    public void onClick(View v) {
        if (follow.isShowing())
            follow.dismiss();
        switch (v.getId()) {
            case R.id.facebook_follow:
                getOpenFacebookIntent(mContext);
                break;
            case R.id.twitter_follow:
                getOpenTwitterIntent(mContext);
                break;
            case R.id.google_follow:
                getOpenGooglePlusIntent(mContext);
                break;
            case R.id.youtube_follow:
                getOpenYoutubeIntent(mContext);
                break;
        }
    }
}
