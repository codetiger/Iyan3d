package com.smackall.animator;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.SeekBar;
import android.widget.Switch;

import com.smackall.animator.Adapters.TextSelectionAdapter;
import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.TextDB;
import com.smackall.animator.Helper.UIHelper;

/**
 * Created by Sabish.M on 7/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class TextSelection implements View.OnClickListener,SeekBar.OnSeekBarChangeListener {

    private Context mContext;
    private DatabaseHelper db;
    TextSelectionAdapter textSelectionAdapter;
    private AddToDownloadManager addToDownloadManager;
    private DownloadManager downloadManager;
    ViewGroup insertPoint;
    View v;
    public TextDB textDB = new TextDB();

    public TextSelection(Context context,DatabaseHelper db,AddToDownloadManager addToDownloadManager,DownloadManager downloadManager){
        this.mContext = context;
        this.db = db;
        this.addToDownloadManager = addToDownloadManager;
        this.downloadManager = downloadManager;
    }

    public void showTextSelection()
    {
        ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.HIDE);
        insertPoint = (((EditorView)((Activity)mContext)).sharedPreferenceManager.getInt(mContext,"toolbarPosition") == 1 ) ?
                (ViewGroup) ((Activity)mContext).findViewById(R.id.leftView)
                : (ViewGroup) ((Activity)mContext).findViewById(R.id.rightView);

        for (int i = 0; i < ((ViewGroup)insertPoint.getParent()).getChildCount(); i++){
            if(((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag() != null && ((ViewGroup)insertPoint.getParent()).getChildAt(i).getTag().toString().equals("-1"))
                ((ViewGroup)insertPoint.getParent()).getChildAt(i).setVisibility(View.GONE);
        }
        insertPoint.setVisibility(View.VISIBLE);
        insertPoint.removeAllViews();
        LayoutInflater vi = (LayoutInflater) this.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        v = vi.inflate(R.layout.text_view,insertPoint,false);
        insertPoint.addView(v, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));

        initViews(v);
        GridView gridView = (GridView)v.findViewById(R.id.text_gridView);
        initTextGrid(gridView);
    }

    private void initViews(View v)
    {
        ((Button)v.findViewById(R.id.cancel_textView)).setOnClickListener(this);
        ((Button)v.findViewById(R.id.add_text_btn)).setOnClickListener(this);
        ((Button)v.findViewById(R.id.color_picker_btn)).setOnClickListener(this);
        ((Button)v.findViewById(R.id.withBone)).setOnClickListener(this);
        ((SeekBar)v.findViewById(R.id.bevalSlider)).setOnSeekBarChangeListener(this);
    }

    private void initTextGrid(GridView gridView)
    {
        textSelectionAdapter = new TextSelectionAdapter(mContext, db, gridView,this.addToDownloadManager,this.downloadManager);
        gridView.setAdapter(textSelectionAdapter);
        gridView.setNumColumns(3);
        gridView.setHorizontalSpacing(20);
        gridView.setVerticalSpacing(40);
        textSelectionAdapter.downloadFonts();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.color_picker_btn:
                ((EditorView)((Activity)mContext)).colorPicker.showColorPicker(v,null,Constants.TEXT_VIEW);
                break;
            case R.id.cancel_textView:
            case R.id.add_text_btn:
                textDB.setTempNode((v.getId() == R.id.cancel_textView));
                importText();
                ((EditorView)((Activity)mContext)).renderManager.removeTempNode();
                downloadManager.cancelAll();
                insertPoint.removeAllViews();
                ((EditorView)((Activity)mContext)).showOrHideToolbarView(Constants.SHOW);
                mContext = null;
                db = null;
                textSelectionAdapter = null;
                break;
            case R.id.withBone:
                importText();
                break;
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        textDB.setTempNode(true);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
       importText();
    }

    public void importText(){
        if(textDB.getFilePath().equals("-1")){ UIHelper.informDialog(mContext, "Please Choose Font Style."); return;}
        textDB.setAssetName(((EditText) v.findViewById(R.id.inputText)).getText().toString());
        textDB.setBevalValue(((SeekBar) v.findViewById(R.id.bevalSlider)).getProgress());
        textDB.setFontSize(10);
        textDB.setTextureName("-1");
        textDB.setTypeOfNode(((Switch) v.findViewById(R.id.withBone)).isChecked() ? Constants.ASSET_TEXT_RIG : Constants.ASSET_TEXT);
        ((EditorView)((Activity)mContext)).renderManager.importText(textDB);
    }
}
