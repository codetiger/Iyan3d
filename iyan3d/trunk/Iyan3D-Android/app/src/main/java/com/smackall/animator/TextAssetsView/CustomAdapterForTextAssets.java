package com.smackall.animator.TextAssetsView;

import android.app.Activity;
import android.content.Context;
import android.graphics.Typeface;
import android.os.Environment;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.common.Constant;

import java.io.File;

import smackall.animator.R;


/**
 * Created by Sabish on 26/8/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class CustomAdapterForTextAssets extends BaseAdapter {

    String [] fonts;
    Activity context;
    String [] fontName;
    int color;
    private static LayoutInflater inflater=null;

    AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    DownloadManagerClass downloadManager = new DownloadManagerClass(3);
    DownloadFonts downloadFonts = new DownloadFonts();

    public CustomAdapterForTextAssets(Activity activity) {
            // TODO Auto-generated constructor stub
        inflater = ( LayoutInflater )activity.
                getSystemService(Context.LAYOUT_INFLATER_SERVICE);

    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return fonts.length;
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    public static class Holder
    {
        TextView tv;
        TextView img;
    }

    int firstTextDownload = 0;
    public static int clickedPosition = 0;

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        Holder holder=new Holder();
        View rowView = inflater.inflate(R.layout.gridview_content_for_text_assets, parent, false);

        rowView.getLayoutParams().width = TextAssetsSelectionView.width/8;
        rowView.getLayoutParams().height = (int) (TextAssetsSelectionView.height/4.5);


        holder.tv=(TextView) rowView.findViewById(R.id.font_name);
        holder.tv.setMaxLines(1);
        holder.tv.setEllipsize(TextUtils.TruncateAt.END);
        holder.img=(TextView) rowView.findViewById(R.id.font_style);
        holder.img.setId(position);

        holder.tv.setText(fontName[position].substring(0, fontName[position].indexOf(".")));

        ProgressBar progressBarTextAssets = (ProgressBar) rowView.findViewById(R.id.progressBarTextAssets);


        String cacheDir = Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/.Cache/";
        Constant.mkDir(cacheDir);
        String fontDir = Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/fonts/";
        Constant.mkDir(fontDir);

        String fontNameWithExec = TextAssetsSelectionView.fontName[position].substring(TextAssetsSelectionView.fontName[position].length() - 4);
        File fontFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/fonts/"+TextAssetsSelectionView.fontName[position]);


        if(!fontFile.exists() || fontFile.getTotalSpace() == 0) {
            holder.img.setVisibility(View.GONE);
            String urlForFont = "http://iyan3dapp.com/appapi/font/" + TextAssetsSelectionView.assetsId[position] + fontNameWithExec;
            downloadFonts.downloadFontsFromUrl(context, position,
                    urlForFont,
                    TextAssetsSelectionView.fontName[position], cacheDir, fontDir);
            System.out.println("Font File Location : " + String.valueOf(fontFile));
        }

        else if(fontFile.exists()){
            System.out.println("Files Found");
            progressBarTextAssets.setVisibility(View.GONE);
            holder.img.setTextSize(TextAssetsSelectionView.width / 45);
            holder.img.setTextColor(color);
            holder.img.setText("Text");
            try{
                if(position == 0 && firstTextDownload == 0){
                    firstTextDownload++;
                    TextAssetsSelectionView.hashMap.put("fileName", TextAssetsSelectionView.fontName[0]);
                    TextAssetsSelectionView.textUpdater();
                }
                Typeface fontType = Typeface.createFromFile(fontFile);
                holder.img.setTypeface(fontType);
            }
            catch (Exception e){
                    e.printStackTrace();
            }
        }

        rowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                clickedPosition = position;
                // TODO Auto-generated method stub
                File textFile = new File(Environment.getExternalStorageDirectory()+"/Android/data/"+context.getPackageName()+"/fonts/" + TextAssetsSelectionView.fontName[position]);

                if(textFile.exists()) {
                    TextAssetsSelectionView.hashMap.put("fileName", TextAssetsSelectionView.fontName[position]);
                    TextAssetsSelectionView.textUpdater();
                }
            }
        });
        return rowView;
    }
}