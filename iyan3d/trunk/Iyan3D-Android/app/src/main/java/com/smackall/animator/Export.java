package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.view.Window;
import android.widget.LinearLayout;

import com.smackall.animator.Helper.Constants;

/**
 * Created by Sabish.M on 12/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class Export implements View.OnClickListener {
    private Context mContext;

    public Export(Context mContext){
        this.mContext = mContext;
    }

    public void showExport(int type)
    {
        Dialog export = new Dialog(mContext);
        export.requestWindowFeature(Window.FEATURE_NO_TITLE);
        export.setContentView(R.layout.export);
        export.setCancelable(false);
        export.setCanceledOnTouchOutside(false);
        export.getWindow().setLayout(Constants.width / 2, (int) (Constants.height / 1.14285714286));
        export.show();
        initViews(export);
    }

    private void initViews(Dialog export)
    {
        ((LinearLayout)export.findViewById(R.id.color_picket_btn)).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.color_picket_btn:
                ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(v,null,Constants.RENDERING_VIEW);
                break;
        }
    }
}
