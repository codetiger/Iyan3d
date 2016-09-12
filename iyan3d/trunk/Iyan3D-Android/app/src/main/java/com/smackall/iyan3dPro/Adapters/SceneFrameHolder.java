package com.smackall.iyan3dPro.Adapters;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.iyan3dPro.R;

/**
 * Created by Sabish.M on 7/4/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SceneFrameHolder extends RecyclerView.ViewHolder {

    public ImageView imagepart, scene_props_btn;
    public TextView textpart;
    public View view;
    private Context context;

    public SceneFrameHolder(Context context, View itemView) {
        super(itemView);
        this.context = context;
        view = itemView;
        imagepart = (ImageView) itemView.findViewById(R.id.imagepart);
        scene_props_btn = (ImageView) itemView.findViewById(R.id.scene_props_btn);
        textpart = (TextView) itemView.findViewById(R.id.textpart);
    }
}
