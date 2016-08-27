package com.smackall.animator.Helper;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Typeface;
import android.os.Build;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;

import com.twitter.sdk.android.core.Callback;
import com.twitter.sdk.android.core.TwitterCore;
import com.twitter.sdk.android.core.TwitterSession;
import com.twitter.sdk.android.core.identity.TwitterAuthClient;

import java.lang.ref.WeakReference;

import io.fabric.sdk.android.Fabric;
import io.fabric.sdk.android.services.common.CommonUtils;

/**
 * Created by Sabish.M on 1/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TwitterButton extends Button {
    static final String TAG = "Twitter";
    static final String ERROR_MSG_NO_ACTIVITY = "TwitterLoginButton requires an activity. Override getActivity to provide the activity for this button.";
    final WeakReference<Activity> activityRef;
    volatile TwitterAuthClient authClient;
    OnClickListener onClickListener;
    Callback<TwitterSession> callback;

    public TwitterButton(Context context) {
        this(context, null);
    }

    public TwitterButton(Context context, AttributeSet attrs) {
        this(context, attrs, 16842824);
    }

    public TwitterButton(Context context, AttributeSet attrs, int defStyle) {
        this(context, attrs, defStyle, null);
    }

    TwitterButton(Context context, AttributeSet attrs, int defStyle, TwitterAuthClient authClient) {
        super(context, attrs, defStyle);
        this.activityRef = new WeakReference(this.getActivity());
        this.authClient = authClient;
        this.setupButton();
        this.checkTwitterCoreAndEnable();
    }

    private void setupButton() {
        Resources res = this.getResources();
        super.setCompoundDrawablesWithIntrinsicBounds(ContextCompat.getDrawable(this.getContext(), com.twitter.sdk.android.core.R.drawable.tw__ic_logo_default), null, null, null);
        super.setCompoundDrawablePadding(res.getDimensionPixelSize(com.twitter.sdk.android.core.R.dimen.tw__login_btn_drawable_padding));
        super.setText(com.twitter.sdk.android.core.R.string.tw__login_btn_txt);
        super.setTextColor(ContextCompat.getColor(this.getContext(), com.twitter.sdk.android.core.R.color.tw__solid_white));
        super.setTextSize(0, (float) res.getDimensionPixelSize(com.twitter.sdk.android.core.R.dimen.tw__login_btn_text_size));
        super.setTypeface(Typeface.DEFAULT_BOLD);
        super.setPadding(res.getDimensionPixelSize(com.twitter.sdk.android.core.R.dimen.tw__login_btn_left_padding), 0, res.getDimensionPixelSize(com.twitter.sdk.android.core.R.dimen.tw__login_btn_right_padding), 0);
        super.setBackgroundResource(com.twitter.sdk.android.core.R.drawable.tw__login_btn);
        super.setOnClickListener(new TwitterButton.LoginClickListener());
        if (Build.VERSION.SDK_INT >= 21) {
            super.setAllCaps(false);
        }

    }

    public Callback<TwitterSession> getCallback() {
        return this.callback;
    }

    public void setCallback(Callback<TwitterSession> callback) {
        if (callback == null) {
            throw new IllegalArgumentException("Callback cannot be null");
        } else {
            this.callback = callback;
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == this.getTwitterAuthClient().getRequestCode()) {
            this.getTwitterAuthClient().onActivityResult(requestCode, resultCode, data);
        }

    }

    protected Activity getActivity() {
        if (this.getContext() instanceof Activity) {
            return (Activity) this.getContext();
        } else if (this.isInEditMode()) {
            return null;
        } else {
            throw new IllegalStateException("TwitterLoginButton requires an activity. Override getActivity to provide the activity for this button.");
        }
    }

    public void setOnClickListener(OnClickListener onClickListener) {
        this.onClickListener = onClickListener;
    }

    TwitterAuthClient getTwitterAuthClient() {
        if (this.authClient == null) {
            Class var1 = TwitterButton.class;
            synchronized (TwitterButton.class) {
                if (this.authClient == null) {
                    this.authClient = new TwitterAuthClient();
                }
            }
        }
        return this.authClient;
    }

    private void checkTwitterCoreAndEnable() {
        if (!this.isInEditMode()) {
            try {
                TwitterCore.getInstance();
            } catch (IllegalStateException var2) {
                Fabric.getLogger().e("Twitter", var2.getMessage());
                this.setEnabled(false);
            }

        }
    }

    private class LoginClickListener implements OnClickListener {
        private LoginClickListener() {
        }

        public void onClick(View view) {
            this.checkCallback(TwitterButton.this.callback);
            this.checkActivity(TwitterButton.this.activityRef.get());
            TwitterButton.this.getTwitterAuthClient().authorize(TwitterButton.this.activityRef.get(), TwitterButton.this.callback);
            if (TwitterButton.this.onClickListener != null) {
                TwitterButton.this.onClickListener.onClick(view);
            }

        }

        private void checkCallback(Callback callback) {
            if (callback == null) {
                CommonUtils.logOrThrowIllegalStateException("Twitter", "Callback must not be null, did you call setCallback?");
            }

        }

        private void checkActivity(Activity activity) {
            if (activity == null || activity.isFinishing()) {
                CommonUtils.logOrThrowIllegalStateException("Twitter", "TwitterLoginButton requires an activity. Override getActivity to provide the activity for this button.");
            }

        }
    }
}
