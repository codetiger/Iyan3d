package com.smackall.animator;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.support.v7.widget.PopupMenu;
import android.view.MenuItem;
import android.view.View;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class InfoPopUp {

    private static final int ID_TUTORIAL  = 0;
    private static final int ID_SETTINGS = 1;
    private static final int ID_CONTACT = 2;
    private Context mContext;

    public void infoPopUpMenu(Context context, View v)
    {
        this.mContext = context;
        PopupMenu popup = new PopupMenu(context, v);
        popup.getMenuInflater().inflate(R.menu.info_props_menu, popup.getMenu());
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getOrder()) {
                    case ID_TUTORIAL:
                        openTutorial(mContext);
                        break;
                    case ID_SETTINGS:
                        openSettings(mContext);
                        break;
                    case ID_CONTACT:
                        openMail(mContext);
                        break;
                }
                return true;
            }
        });
        popup.show();
    }

    private void openTutorial(Context context)
    {
        String url = "https://www.iyan3dapp.com/tutorial-videos/";
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse(url));
        try {
            ((Activity)context).startActivity(i);
        }
        catch (ActivityNotFoundException e){

        }
    }

    private void openMail(Context context)
    {
        Intent email = new Intent(Intent.ACTION_SEND);
        email.setType("message/rfc822");
        email.putExtra(Intent.EXTRA_EMAIL,new String[] { "iyan3d@smackall.com" });
        email.putExtra(Intent.EXTRA_SUBJECT, "Feedback on Iyan3D 5.0 app ( Device Model " + Build.MANUFACTURER.toUpperCase() + " " + Build.MODEL + " , Android Version " + Build.VERSION.RELEASE + " )");
        try {
            ((Activity)context).startActivity(email);
        }
        catch (ActivityNotFoundException e){

        }
    }

    private void openSettings(Context context)
    {
        ((EditorView)((Activity)context)).settings.showSettings();
    }
}
