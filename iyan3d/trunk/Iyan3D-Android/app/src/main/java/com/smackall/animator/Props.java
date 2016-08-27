package com.smackall.animator;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.v4.content.ContextCompat;
import android.support.v7.view.ContextThemeWrapper;
import android.support.v7.widget.AppCompatButton;
import android.support.v7.widget.AppCompatRadioButton;
import android.support.v7.widget.AppCompatSeekBar;
import android.support.v7.widget.SwitchCompat;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CustomViews.CustomRadioButton;
import com.smackall.animator.Helper.Listeners.CustomListeners;
import com.smackall.animator.Helper.Property;
import com.smackall.animator.Helper.PropsViewGenerator;
import com.smackall.animator.Helper.SGNodeOptionsMap;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.NativeCallBackClasses.NativeCallBacks;
import com.smackall.animator.opengl.GL2JNILib;

import java.util.Map;

/**
 * Created by Sabish.M on 23/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Props {

    public EditorView editorView;
    public Dialog dialog;
    public FrameLayout props_frame;
    Context mContext;

    public Props(Context mContext) {
        this.mContext = mContext;
        this.editorView = ((EditorView) mContext);
    }

    public void showProps(View v, MotionEvent event) {
        Dialog props = new Dialog(new ContextThemeWrapper(mContext,R.style.AppCompat));
        props.requestWindowFeature(Window.FEATURE_NO_TITLE);
        props.setContentView(R.layout.props);
        props.setCancelable(false);
        props.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                props.getWindow().setLayout((int) (Constants.width / 1.3), (int) (Constants.height / 1.3));
                break;
            default:
                props.getWindow().setLayout(Constants.width / 2, Constants.height / 2);
                break;
        }

        Window window = props.getWindow();
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
        dialog = props;
        props_frame = (FrameLayout) props.findViewById(R.id.props_frame);
        props.show();
        getMap();

        getPropsView(editorView.sgNodeOptionsMap.getSortedMap(editorView.sgNodeOptionsMap.nodeProps), false, false);
        HitScreens.PropsView(mContext, (GL2JNILib.getSelectedNodeId() != -1) ? GL2JNILib.getNodeType(GL2JNILib.getSelectedNodeId()) : -1);
        props.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                HitScreens.EditorView(mContext);
            }
        });
    }

    private void getMap() {
        NativeCallBacks nativeCallBacks = editorView.nativeCallBacks;
        SGNodeOptionsMap sgNodeOptionsMap = editorView.sgNodeOptionsMap;
        if (nativeCallBacks == null || sgNodeOptionsMap == null) return;
        sgNodeOptionsMap.clearNodeProps();
        if (GL2JNILib.getSelectedNodeId() != -1)
            GL2JNILib.getNodePropsMapWithNodeId(nativeCallBacks, GL2JNILib.getSelectedNodeId());
        else
            GL2JNILib.getSceneProps(nativeCallBacks);
    }

    public SeekBar getSlider(int progress, float weight) {
        AppCompatSeekBar seekBar = new AppCompatSeekBar(mContext);
        LinearLayout.LayoutParams a = new LinearLayout.LayoutParams((weight != 0) ? 0 : LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT, weight);
        seekBar.setLayoutParams(a);
        seekBar.setProgress(progress);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            seekBar.setElevation(3.0f);
        }
        return seekBar;
    }

    public LinearLayout getLinearLayout(int orientation, float weightSum, int gravity, LinearLayout.LayoutParams params) {
        LinearLayout a = new LinearLayout(mContext);
        a.setLayoutParams(params);
        a.setOrientation(orientation);
        a.setWeightSum(weightSum);
        a.setGravity(gravity);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            a.setElevation(3.0f);
        }
        return a;
    }

    public TextView getTextView(float weight, String chr, int color, int gravity, int[] layoutParms) {
        TextView t = new TextView(mContext);
        t.setText(chr);
        t.setTextColor(color);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(layoutParms[0], layoutParms[1], weight);
        t.setLayoutParams(params);
        t.setGravity(gravity);
        t.setMaxLines(1);
        t.setEllipsize(TextUtils.TruncateAt.END);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            t.setElevation(3.0f);
        }
        return t;
    }

    public Button getIconTypeButton(float weight, String chr, int color, int gravity, Drawable icon) {
        AppCompatButton b = new AppCompatButton(mContext);
        b.setText("   " + chr);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT, weight);
        b.setLayoutParams(params);
        b.setGravity(gravity);
        b.setTextColor(color);
        b.setMaxLines(1);
        b.setEllipsize(TextUtils.TruncateAt.END);
        b.setCompoundDrawablesWithIntrinsicBounds(icon, null, null, null);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            b.setElevation(3.0f);
        }
        return b;
    }

    public RadioGroup getRadioGroup(int gravity, int orientation) {
        RadioGroup r = new RadioGroup(mContext);
        r.setGravity(gravity);
        r.setOrientation(orientation);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            r.setElevation(3.0f);
        }
        return r;
    }

    public CustomRadioButton getRadioButton(String chr, int color, boolean isChecked, LinearLayout.LayoutParams params) {
        CustomRadioButton r = new CustomRadioButton(mContext);
        r.setText(chr);
        r.setTextColor(color);
        r.setLayoutParams(params);
        r.setChecked(isChecked);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            r.setElevation(3.0f);
        }
        return r;
    }

    public SwitchCompat getSwitch(boolean isChecked, LinearLayout.LayoutParams params) {
        SwitchCompat s = new SwitchCompat(mContext);
        s.setLayoutParams(params);
        s.setChecked(isChecked);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            s.setElevation(3.0f);
        }
        return s;
    }

    public ImageView getImageView() {
        ImageView imageView = new ImageView(mContext);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        imageView.setLayoutParams(params);
        imageView.setAdjustViewBounds(true);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            imageView.setElevation(3.0f);
        }
        return imageView;
    }

    public void getPropsView(Map<Integer, Property> nodeProps, boolean isAnimated, boolean isSubProps) {
        LinearLayout topOfTheView = new LinearLayout(mContext);
        LinearLayout.LayoutParams ltParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        topOfTheView.setLayoutParams(ltParams);
        topOfTheView.setOrientation(LinearLayout.VERTICAL);
        topOfTheView.setBackgroundColor(Color.WHITE);

        ScrollView scrollView = new ScrollView(mContext);
        ScrollView.LayoutParams sParams = new ScrollView.LayoutParams(ScrollView.LayoutParams.MATCH_PARENT, ScrollView.LayoutParams.WRAP_CONTENT);
        scrollView.setLayoutParams(sParams);
        scrollView.setPadding(5, 5, 5, 5);

        LinearLayout insideScrollView = new LinearLayout(mContext);
        LinearLayout.LayoutParams ltParams1 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        insideScrollView.setOrientation(LinearLayout.VERTICAL);
        insideScrollView.setLayoutParams(ltParams1);

        scrollView.addView(insideScrollView);


        if (isSubProps) {
            LinearLayout header = new LinearLayout(mContext);
            LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            header.setLayoutParams(params);
            header.setOrientation(LinearLayout.HORIZONTAL);
            header.setGravity(Gravity.CENTER | Gravity.LEFT);
            Drawable drawable = ContextCompat.getDrawable(mContext, R.drawable.arrow_left);
            Button b = getIconTypeButton(1.0f, mContext.getResources().getString(R.string.back), Color.BLACK, Gravity.LEFT | Gravity.CENTER, drawable);
            b.setText(" " + mContext.getResources().getString(R.string.back));

            Animation toRight = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f,
                    Animation.RELATIVE_TO_PARENT, +1.0f,
                    Animation.RELATIVE_TO_PARENT, 0.0f,
                    Animation.RELATIVE_TO_PARENT, 0.0f);
            toRight.setDuration(500);
            toRight.setInterpolator(new AccelerateDecelerateInterpolator());

            b.setOnClickListener(new CustomListeners(toRight, topOfTheView, true, this));

            header.addView(b);
            topOfTheView.addView(header);
        }

        topOfTheView.addView(scrollView);

        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                topOfTheView.setMinimumHeight((int) (Constants.height / 1.3));
                break;
            default:
                topOfTheView.setMinimumHeight(Constants.height / 2);
                break;
        }

        //For Prevent from events passed through the front view
        topOfTheView.setClickable(true);

        props_frame.addView(topOfTheView);
        PropsViewGenerator propsAdapter = new PropsViewGenerator(mContext, insideScrollView, this, nodeProps, isSubProps);
        propsAdapter.getView();

        if (isAnimated) {
            Animation fromRight = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, +1.0f,
                    Animation.RELATIVE_TO_PARENT, 0.0f,
                    Animation.RELATIVE_TO_PARENT, 0.0f,
                    Animation.RELATIVE_TO_PARENT, 0.0f);
            fromRight.setDuration(500);
            fromRight.setInterpolator(new AccelerateDecelerateInterpolator());
            topOfTheView.setAnimation(fromRight);
            topOfTheView.animate().start();
        }
    }
}


