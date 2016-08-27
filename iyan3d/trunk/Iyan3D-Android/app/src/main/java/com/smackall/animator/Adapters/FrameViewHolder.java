package com.smackall.animator.Adapters;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.smackall.animator.R;


/**
 * Created by Sabish.M on 9/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class FrameViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

    public TextView tvName;
    public View view;
    private Context context;

    public FrameViewHolder(Context context, View itemView) {
        super(itemView);
        this.context = context;
        view = itemView;
        tvName = (TextView) itemView.findViewById(R.id.frame_text);
        itemView.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        Toast.makeText(context, tvName.getText().toString(), Toast.LENGTH_SHORT).show();
    }
}
