package com.smackall.iyan3dPro.Helper;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.RoundRectShape;
import android.os.Build;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;
import android.widget.TabWidget;
import android.widget.TextView;

/**
 * Created by Sabish.M on 21/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */

public class BadgeView extends TextView {

    public static final int POSITION_TOP_LEFT = 1;
    public static final int POSITION_TOP_RIGHT = 2;
    public static final int POSITION_BOTTOM_LEFT = 3;
    public static final int POSITION_BOTTOM_RIGHT = 4;
    public static final int POSITION_CENTER = 5;
    private static final int DEFAULT_MARGIN_DIP = 5;
    private static final int DEFAULT_LR_PADDING_DIP = 5;
    private static final int DEFAULT_CORNER_RADIUS_DIP = 8;
    private static final int DEFAULT_POSITION = 2;
    private static final int DEFAULT_BADGE_COLOR = Color.parseColor("#CCFF0000");
    private static final int DEFAULT_TEXT_COLOR = -1;
    private static Animation fadeIn;
    private static Animation fadeOut;
    private Context context;
    private View target;
    private int badgePosition;
    private int badgeMarginH;
    private int badgeMarginV;
    private int badgeColor;
    private boolean isShown;
    private ShapeDrawable badgeBg;
    private int targetTabIndex;

    public BadgeView(Context context) {
        this(context, (AttributeSet) null, 16842884);
    }

    public BadgeView(Context context, AttributeSet attrs) {
        this(context, attrs, 16842884);
    }

    public BadgeView(Context context, View target) {
        this(context, null, 16842884, target, 0);
    }

    public BadgeView(Context context, TabWidget target, int index) {
        this(context, null, 16842884, target, index);
    }

    public BadgeView(Context context, AttributeSet attrs, int defStyle) {
        this(context, attrs, defStyle, null, 0);
    }

    private BadgeView(Context context, AttributeSet attrs, int defStyle, View target, int tabIndex) {
        super(context, attrs, defStyle);
        this.init(context, target, tabIndex);
    }

    private void init(Context context, View target, int tabIndex) {
        this.context = context;
        this.target = target;
        this.targetTabIndex = tabIndex;
        this.badgePosition = 2;
        this.badgeMarginH = this.dipToPixels(5);
        this.badgeMarginV = this.badgeMarginH;
        this.badgeColor = DEFAULT_BADGE_COLOR;
        this.setTypeface(Typeface.DEFAULT_BOLD);
        int paddingPixels = this.dipToPixels(5);
        this.setPadding(paddingPixels, 0, paddingPixels, 0);
        this.setTextColor(-1);
        fadeIn = new AlphaAnimation(0.0F, 1.0F);
        fadeIn.setInterpolator(new DecelerateInterpolator());
        fadeIn.setDuration(200L);
        fadeOut = new AlphaAnimation(1.0F, 0.0F);
        fadeOut.setInterpolator(new AccelerateInterpolator());
        fadeOut.setDuration(200L);
        this.isShown = false;
        if (this.target != null) {
            this.applyTo(this.target);
        } else {
            this.show();
        }

    }

    private void applyTo(View target) {
        ViewGroup.LayoutParams lp = target.getLayoutParams();
        ViewParent parent = target.getParent();
        FrameLayout container = new FrameLayout(this.context);
        if (target instanceof TabWidget) {
            target = ((TabWidget) target).getChildTabViewAt(this.targetTabIndex);
            this.target = target;
            ((ViewGroup) target).addView(container, new ViewGroup.LayoutParams(-1, -1));
            this.setVisibility(View.GONE);
            container.addView(this);
        } else {
            ViewGroup group = (ViewGroup) parent;
            int index = group.indexOfChild(target);
            group.removeView(target);
            group.addView(container, index, lp);
            container.addView(target);
            this.setVisibility(View.GONE);
            container.addView(this);
            group.invalidate();
        }

    }

    public void show() {
        this.show(false, null);
    }

    public void show(boolean animate) {
        this.show(animate, fadeIn);
    }

    public void show(Animation anim) {
        this.show(true, anim);
    }

    public void hide() {
        this.hide(false, null);
    }

    public void hide(boolean animate) {
        this.hide(animate, fadeOut);
    }

    public void hide(Animation anim) {
        this.hide(true, anim);
    }

    public void toggle() {
        this.toggle(false, null, null);
    }

    public void toggle(boolean animate) {
        this.toggle(animate, fadeIn, fadeOut);
    }

    public void toggle(Animation animIn, Animation animOut) {
        this.toggle(true, animIn, animOut);
    }

    private void show(boolean animate, Animation anim) {
        if (this.getBackground() == null) {
            if (this.badgeBg == null) {
                this.badgeBg = this.getDefaultBackground();
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                this.setBackground(this.badgeBg);
            } else
                this.setBackgroundDrawable(this.badgeBg);
        }

        this.applyLayoutParams();
        if (animate) {
            this.startAnimation(anim);
        }

        this.setVisibility(View.VISIBLE);
        this.isShown = true;
    }

    private void hide(boolean animate, Animation anim) {
        this.setVisibility(View.GONE);
        if (animate) {
            this.startAnimation(anim);
        }

        this.isShown = false;
    }

    private void toggle(boolean animate, Animation animIn, Animation animOut) {
        if (this.isShown) {
            this.hide(animate && animOut != null, animOut);
        } else {
            this.show(animate && animIn != null, animIn);
        }

    }

    private int increment(int offset) {
        CharSequence txt = this.getText();
        int i;
        if (txt != null) {
            try {
                i = Integer.parseInt(txt.toString());
            } catch (NumberFormatException var5) {
                i = 0;
            }
        } else {
            i = 0;
        }

        i += offset;
        this.setText(String.valueOf(i));
        return i;
    }

    public int decrement(int offset) {
        return this.increment(-offset);
    }

    private ShapeDrawable getDefaultBackground() {
        int r = this.dipToPixels(8);
        float[] outerR = new float[]{(float) r, (float) r, (float) r, (float) r, (float) r, (float) r, (float) r, (float) r};
        RoundRectShape rr = new RoundRectShape(outerR, null, null);
        ShapeDrawable drawable = new ShapeDrawable(rr);
        drawable.getPaint().setColor(this.badgeColor);
        return drawable;
    }

    private void applyLayoutParams() {
        android.widget.FrameLayout.LayoutParams lp = new android.widget.FrameLayout.LayoutParams(-2, -2);
        switch (this.badgePosition) {
            case 1:
                lp.gravity = 51;
                lp.setMargins(this.badgeMarginH, this.badgeMarginV, 0, 0);
                break;
            case 2:
                lp.gravity = 53;
                lp.setMargins(0, this.badgeMarginV, this.badgeMarginH, 0);
                break;
            case 3:
                lp.gravity = 83;
                lp.setMargins(this.badgeMarginH, 0, 0, this.badgeMarginV);
                break;
            case 4:
                lp.gravity = 85;
                lp.setMargins(0, 0, this.badgeMarginH, this.badgeMarginV);
                break;
            case 5:
                lp.gravity = 17;
                lp.setMargins(0, 0, 0, 0);
        }

        this.setLayoutParams(lp);
    }

    public View getTarget() {
        return this.target;
    }

    public boolean isShown() {
        return this.isShown;
    }

    public int getBadgePosition() {
        return this.badgePosition;
    }

    public void setBadgePosition(int layoutPosition) {
        this.badgePosition = layoutPosition;
    }

    public int getHorizontalBadgeMargin() {
        return this.badgeMarginH;
    }

    public int getVerticalBadgeMargin() {
        return this.badgeMarginV;
    }

    public void setBadgeMargin(int badgeMargin) {
        this.badgeMarginH = badgeMargin;
        this.badgeMarginV = badgeMargin;
    }

    public void setBadgeMargin(int horizontal, int vertical) {
        this.badgeMarginH = horizontal;
        this.badgeMarginV = vertical;
    }

    public int getBadgeBackgroundColor() {
        return this.badgeColor;
    }

    public void setBadgeBackgroundColor(int badgeColor) {
        this.badgeColor = badgeColor;
        this.badgeBg = this.getDefaultBackground();
    }

    private int dipToPixels(int dip) {
        Resources r = this.getResources();
        float px = TypedValue.applyDimension(1, (float) dip, r.getDisplayMetrics());
        return (int) px;
    }
}

