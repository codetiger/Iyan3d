package com.smackall.animator.AnimationAssetsView;

/**
 * Created by Sabish on 22/9/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.DownloadManager.AddToDownloadManager;
import com.smackall.animator.DownloadManager.DownloadManager;
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILibAssetView;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import smackall.animator.R;


/**
 * Created by android-developer on 26/8/15.
 */
public class CustomAdapterForAnimationSelection extends BaseAdapter {

    String [] result;
    Context mContext;
    String [] imageId;
    public static Timer timer;
    private static LayoutInflater inflater=null;

    public View rowView;

    AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    DownloadManagerClass downloadManager = new DownloadManagerClass(3);
    AnimationSelectionView animationSelectionView;

    public CustomAdapterForAnimationSelection(Context context, String[] prgmNameList, String[] prgmImages, Activity activity,AnimationSelectionView animationSelectionView) {
        // TODO Auto-generated constructor stub
        result = prgmNameList;
        mContext = context;
        imageId=prgmImages;
        inflater = ( LayoutInflater )activity.
                getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        this.animationSelectionView = animationSelectionView;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return result.length;
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

    public class Holder
    {
        TextView tv;
        ImageView img;
    }

    public int firstAnimDownload = 0;
    public static int currentAnimDownload = 0;
    public static int clickedPosition = 0;
    public static String animExtension = "";

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        Holder holder=new Holder();
        rowView = inflater.inflate(R.layout.gridview_content_for_assets_view, parent, false);
        rowView.getLayoutParams().width = Constant.width/8;
        rowView.getLayoutParams().height = (int) (Constant.height/4.5);

        holder.tv=(TextView) rowView.findViewById(R.id.textView1);
        holder.img=(ImageView) rowView.findViewById(R.id.imageView1);
        ProgressBar progressBar = (ProgressBar) rowView.findViewById(R.id.native_progress_bar);

        holder.tv.setText(result[position]);

        File imageFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/animations/"+
                Integer.toString(AnimationSelectionView.animAssetsId[position])+".png");

        if(!imageFile.exists() || imageFile.getTotalSpace() == 0) {

            holder.img.setVisibility(View.GONE);
            String urlForAnimImage = "http://iyan3dapp.com/appapi/animationImage/" + AnimationSelectionView.animAssetsId[position] + ".png";
            String fileName = AnimationSelectionView.animAssetsId[position] + ".png";
            String cacheDir =  Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/.cache/";
            Constant.mkDir(cacheDir);
            String toPath = Environment.getExternalStorageDirectory() + "/Android/data/"+mContext.getPackageName()+"/animations/";
            Constant.mkDir(cacheDir);
            Constant.mkDir(toPath);
            rowView.setId(addToDownloadManager.downloadAdd(mContext, urlForAnimImage, fileName, cacheDir, "low", downloadManager));

        }

        else{
            try {
                if(imageFile.exists())
                progressBar.setVisibility(View.GONE);
                holder.img.setImageBitmap(BitmapFactory.decodeFile(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/animations/" +
                        Integer.toString(AnimationSelectionView.animAssetsId[position]) + ".png"));

            }
            catch (Exception e){

            }
        }


        if (position == 0 && firstAnimDownload == 0) {
            firstAnimDownload++;
            animationUpdate(position,mContext,addToDownloadManager,downloadManager);
            animationSelectionView.publishBtnHandler(position);

        }


        rowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                    animationSelectionView.publishBtnHandler(position);
                    if (position != clickedPosition) {
                        if (AnimationSelectionView.animDBs.size() > clickedPosition) {
                            View gridPreviousSelected = AnimationSelectionView.gridView.getChildAt(clickedPosition);
                            LinearLayout gridHolderPreviousItem = (LinearLayout) gridPreviousSelected.findViewById(R.id.assets_list_elements_layout);
                            if (gridHolderPreviousItem != null)
                                gridHolderPreviousItem.setBackgroundDrawable(mContext.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
                        }
                        View gridSelected = AnimationSelectionView.gridView.getChildAt(position);
                        LinearLayout gridHolder = (LinearLayout) gridSelected.findViewById(R.id.assets_list_elements_layout);
                        if (gridHolder != null)
                            gridHolder.setBackgroundDrawable(mContext.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
                    }

                    downloadManager.cancel(currentAnimDownload);
                    animationUpdate(position,mContext,addToDownloadManager,downloadManager);
                    }

        });


        return rowView;
    }

    public static void animationUpdate(int position,Context mContext, AddToDownloadManager addToDownloadManager, DownloadManager downloadManager){
        System.out.println("Animation Update Called : Position : " + position + " ");
        AnimationSelectionView.anim_selection_like_count_text.setText(" "+String.valueOf(AnimationSelectionView.animDBs.get(position).getRating()));
        AnimationSelectionView.anim_selection_anim_date.setText("CREATED DATE \n" + String.valueOf(AnimationSelectionView.animDBs.get(position).getUploaded()));
        AnimationSelectionView.anim_selection_anim_name_text.setText(String.valueOf(AnimationSelectionView.animDBs.get(position).getAnimName()));
        AnimationSelectionView.anim_selection_created_by_text.setText("ANIMATED BY \n" +String.valueOf(AnimationSelectionView.animDBs.get(position).getUserName()));
        AnimationSelectionView.anim_selection_downloads_count.setText("DOWNLOADS : " + String.valueOf(AnimationSelectionView.animDBs.get(position).getDownloads()));

        String extention = null;
        if (Integer.parseInt(AnimationSelectionView.animDBs.get(position).getAnimType()) == 0)
            extention = ".sgra";
        if (Integer.parseInt(AnimationSelectionView.animDBs.get(position).getAnimType()) == 1)
            extention = ".sgta";
        animExtension = extention;
        clickedPosition = position;
        String animFolder = Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/animations/";
        Constant.mkDir(animFolder);

        File animFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + mContext.getPackageName() + "/animations/" +
                AnimationSelectionView.animDBs.get(position).getAnimAssetId() + extention);


        if (AnimationSelectionView.animAssetsId[position] != AnimationSelectionView.selectedAnimId) {
            System.out.println("Selected Animation Id Updated");
            AnimationSelectionView.selectedAnimId = Integer.parseInt(AnimationSelectionView.animDBs.get(position).getAnimAssetId());
            if (!animFile.exists() || animFile.getTotalSpace() == 0) {
                AnimationSelectionView.anim_selection_add_to_scene.setVisibility(View.INVISIBLE);
                GL2JNILibAssetView.stopAllAnimations();
                AnimationSelectionView.anim_selection_progressBar.setVisibility(View.VISIBLE);
                String urlForAnim = "http://iyan3dapp.com/appapi/animationFile/" + AnimationSelectionView.animDBs.get(position).getAnimAssetId()
                        + extention;
                String fileName = AnimationSelectionView.animDBs.get(position).getAnimAssetId()+extention;
                String from = Environment.getExternalStorageDirectory()+"/Android/data/"+mContext.getPackageName()+"/.cache/";
                Constant.mkDir(from);
                String to = Environment.getExternalStorageDirectory()+"/Android/data/"+mContext.getPackageName()+"/animations/";
                Constant.mkDir(to);
                currentAnimDownload =  addToDownloadManager.downloadAdd(mContext, urlForAnim, fileName, from, "low", downloadManager);
                if(!Constant.isOnline(mContext)) {
                    Constant.informDialog(mContext, "Failed connecting to the server! Please try again later.");
                }
            }
            else {
                AnimationSelectionView.anim_selection_add_to_scene.setVisibility(View.VISIBLE);
                AnimationSelectionView.anim_selection_progressBar.setVisibility(View.INVISIBLE);
                playAnimation(position);
            }
        }
    }

    public static void playAnimation(int position) {


        System.out.println("Applyed Position " + position);


        if (timer != null) {
            timer.cancel();
            timer = null;
        }

        GL2JNILibAssetView.stopAllAnimations();

        if (AnimationSelectionView.selectedAssetType == 4) {
            AnimationSelectionView.anim_selection_anim_comp_text.setVisibility(View.INVISIBLE);
            GL2JNILibAssetView.applyAnimationtoNode(Integer.parseInt(AnimationSelectionView.animDBs.get(position).getAnimAssetId()));
            timer = new Timer();
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                   // System.out.println("Animation Played");
                    GL2JNILibAssetView.playAnimation();
                }
            }, 0, 1000 / 24);
        } else {
            if (GL2JNILibAssetView.getBoneCount(AnimationSelectionView.selectedAssetId) != Integer.parseInt(AnimationSelectionView.animDBs.get(position).getBonecount())) {
                AnimationSelectionView.anim_selection_anim_comp_text.setVisibility(View.VISIBLE);
                AnimationSelectionView.anim_selection_progressBar.setVisibility(View.INVISIBLE);
                AnimationSelectionView.anim_selection_add_to_scene.setVisibility(View.INVISIBLE);
                AnimationSelectionView.anim_selection_anim_comp_text.setText("The Selected Aimation Item does not Match with the bone count of the selected asset Item.");

            }
            if (GL2JNILibAssetView.getBoneCount(AnimationSelectionView.selectedAssetId) == Integer.parseInt(AnimationSelectionView.animDBs.get(position).getBonecount())) {
                AnimationSelectionView.anim_selection_anim_comp_text.setVisibility(View.INVISIBLE);
                GL2JNILibAssetView.applyAnimationtoNode(Integer.parseInt(AnimationSelectionView.animDBs.get(position).getAnimAssetId()));
                AnimationSelectionView.anim_selection_progressBar.setVisibility(View.INVISIBLE);

                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        GL2JNILibAssetView.playAnimation();
                    }
                }, 0, 1000 / 24);

            }
        }
    }

}