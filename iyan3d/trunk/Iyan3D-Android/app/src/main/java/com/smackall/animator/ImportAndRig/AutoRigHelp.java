package com.smackall.animator.ImportAndRig;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.common.GalleryActivityForHelp;
import com.smackall.animator.common.ImageAdapterForHelpView;

import smackall.animator.R;


/**
 * Created by Sabish.M on 15/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class AutoRigHelp extends Activity {

        LinearLayout count_layout;
        TextView page_text[];
        int count = 0;
        public static LinearLayout sizeOfHelpImages;


        TextView scene_selection_view_helpImages_text;
        Button scene_selection_view_moreInfo_btn, scene_selection_view_done_btn;

        @Override
        protected void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            this.requestWindowFeature(Window.FEATURE_NO_TITLE);
            setContentView(R.layout.scene_selection_view_help);
            this.setFinishOnTouchOutside(false);
            Constant.FullScreencall(AutoRigHelp.this,this);

            switch (Constant.getScreenCategory(this)){
                case "normal":
                    this.getWindow().setLayout(Constant.width, Constant.height);
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

            sizeOfHelpImages = (LinearLayout) findViewById(R.id.scene_selection_view_help_images_layout);

            scene_selection_view_helpImages_text = (TextView) findViewById(R.id.scene_selection_help_helpImages_text);

            scene_selection_view_done_btn = (Button) findViewById(R.id.scene_selection_help_done_btn);
            scene_selection_view_moreInfo_btn = (Button) findViewById(R.id.scene_selection_help_more_info_btn);


            scene_selection_view_done_btn.setPadding(Constant.width / 50, Constant.height / 75, Constant.width / 50, Constant.height / 75);
            scene_selection_view_moreInfo_btn.setPadding(Constant.width / 50, Constant.height / 75, Constant.width / 50, Constant.height / 75);

            GalleryActivityForHelp mGallery = (GalleryActivityForHelp) findViewById(R.id.mygallery01);


            mGallery.setAdapter(new ImageAdapterForHelpView(this, "autorig"));
            mGallery.setScrollingEnabled(true);
            count_layout = (LinearLayout) findViewById(R.id.image_count);

            count = mGallery.getAdapter().getCount();
            page_text = new TextView[count];
            for (int i = 0; i < count; i++)
            {
                page_text[i] = new TextView(this);
                page_text[i].setText(R.string.text_with_bullet);
                page_text[i].setGravity(Gravity.CENTER);
                page_text[i].setTextSize(TypedValue.COMPLEX_UNIT_PX, (float) (Constant.width/25.6));
                page_text[i].setTypeface(null, Typeface.BOLD);
                page_text[i].setTextColor(android.graphics.Color.GRAY);
                count_layout.addView(page_text[i]);
            }
            mGallery.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> parent, View view,
                                           int pos, long id) {
                    // TODO Auto-generated method stub
                    for (int i = 0; i < count; i++) {
                        page_text[i]
                                .setTextColor(android.graphics.Color.GRAY);
                    }
                    page_text[pos]
                            .setTextColor(Color.BLACK);
                }

                @Override
                public void onNothingSelected(AdapterView<?> arg0) {
                }
            });

            scene_selection_view_done_btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    AutoRigHelp.this.finish();
                }
            });

            scene_selection_view_moreInfo_btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Uri uri = Uri.parse("http://www.iyan3dapp.com/");
                    Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                    startActivity(intent);
                }
            });
        }
    }



