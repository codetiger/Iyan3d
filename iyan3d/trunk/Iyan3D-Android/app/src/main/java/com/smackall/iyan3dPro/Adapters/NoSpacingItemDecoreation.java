package com.smackall.iyan3dPro.Adapters;

import android.graphics.Rect;
import android.support.v7.widget.RecyclerView;
import android.view.View;

import com.smackall.iyan3dPro.Helper.Constants;

/**
 * Created by Sabish.M on 7/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NoSpacingItemDecoreation extends RecyclerView.ItemDecoration {
    @Override
    public void getItemOffsets(Rect outRect, View view,
                               RecyclerView parent, RecyclerView.State state) {
        outRect.bottom = Constants.height / 20;
        outRect.left = 0;
        outRect.right = 0;
        outRect.top = Constants.height / 20;
    }
}

