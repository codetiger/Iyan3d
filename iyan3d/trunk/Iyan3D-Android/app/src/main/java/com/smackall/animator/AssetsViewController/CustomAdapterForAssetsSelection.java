package com.smackall.animator.AssetsViewController;

/**
 * Created by Sabish on 27/8/15.
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
import com.smackall.animator.DownloadManager.DownloadManagerClass;
import com.smackall.animator.common.Constant;
import com.smackall.animator.opengl.GL2JNILibAssetView;

import java.io.File;

import smackall.animator.R;

public class CustomAdapterForAssetsSelection extends BaseAdapter {

    String [] result;
    Context context;
    String [] imageId;
    private static LayoutInflater inflater=null;

    public static View rowView;

    AddToDownloadManager addToDownloadManager = new AddToDownloadManager();
    DownloadManagerClass downloadManager = new DownloadManagerClass(3);

    public CustomAdapterForAssetsSelection(AssetsViewController mainActivity, String[] prgmNameList, String[] prgmImages, Activity activity) {
        // TODO Auto-generated constructor stub
        context=mainActivity;
        inflater = ( LayoutInflater ) activity.
                getSystemService(Context.LAYOUT_INFLATER_SERVICE);
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
        LinearLayout gridLayout;
    }

    public  int firstAssetDownload = 0;
    public static int currentTextureDownload = Constant.unDefined;
    public static int currentMeshDownload = Constant.unDefined;
    public static int clickedPosition = 0;
    public static String meshExtension = "";
    int previousClickPosition = 0;

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        Holder holder=new Holder();
        rowView = inflater.inflate(R.layout.gridview_content_for_assets_view, parent, false);

        rowView.getLayoutParams().width = AssetsViewController.width/8;
        rowView.getLayoutParams().height = (int) (AssetsViewController.height/4.5);


        holder.tv=(TextView) rowView.findViewById(R.id.textView1);
        holder.img=(ImageView) rowView.findViewById(R.id.imageView1);
        holder.gridLayout = (LinearLayout) rowView.findViewById(R.id.assets_list_elements_layout);
        ProgressBar progressBar = (ProgressBar) rowView.findViewById(R.id.native_progress_bar);

        holder.tv.setText(result[position]);

        File imageFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/images/"+
                Integer.toString(AssetsViewController.assetsId[position])+".png");

        if(!imageFile.exists()) {

           holder.img.setVisibility(View.GONE);

            String urlForImage = "http://www.iyan3dapp.com/appapi/128images/" + AssetsViewController.assetsId[position] + ".png";
            String fileName = AssetsViewController.assetsId[position] + ".png";
            String cacheDir =  Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/.cache/";
            Constant.mkDir(cacheDir);

            String toPath = Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/images/";
            Constant.mkDir(toPath);

            rowView.setId(addToDownloadManager.downloadAdd(context, urlForImage, fileName, cacheDir, "low", downloadManager));
        }
        else{
            try {
                if(imageFile.exists()) {
                    progressBar.setVisibility(View.GONE);
                    holder.img.setImageBitmap(BitmapFactory.decodeFile(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/images/" +
                            Integer.toString(AssetsViewController.assetsId[position]) + ".png"));
                }
            }
            catch (Exception e){

            }
        }

        if(position == 0 && firstAssetDownload == 0){
            holder.gridLayout.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
            firstAssetDownload++;
            clickedPosition = 0;
            AssetsViewController.assets_view_selection_name.setText(result[position]);
            String extension = null;
            if (AssetsViewController.assets.get(position).getType().equals("1"))
                extension = ".sgr";
           else if (AssetsViewController.assets.get(position).getType().equals("2")
                    || AssetsViewController.assets.get(position).getType().equals("3"))
                extension = ".sgm";
            else if(AssetsViewController.assets.get(position).getType().equals("6"))
                extension = ".obj";

            meshExtension = extension;
            File texture = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/mesh/" +
                    Integer.toString(AssetsViewController.assetsId[position]) + "-cm.png");
            File mesh = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/mesh/" +
                    Integer.toString(AssetsViewController.assetsId[position]) + extension);

            if (!mesh.exists() || mesh.getTotalSpace() == 0 || !texture.exists() || texture.getTotalSpace() == 0) {
                AssetsViewController.assets_view_add_to_scene_btn.setVisibility(View.INVISIBLE);
                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.VISIBLE);
                String urlForTexture = "http://iyan3dapp.com/appapi/meshtexture/" + AssetsViewController.assetsId[position] + ".png";
                String fileNameTexture = AssetsViewController.assetsId[position] + "-cm.png";
                String cacheDir =  Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/.cache/";
                Constant.mkDir(cacheDir);
                String toPath = Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/mesh/";
                Constant.mkDir(toPath);

                currentTextureDownload = addToDownloadManager.downloadAdd(context, urlForTexture, fileNameTexture, cacheDir, "high", downloadManager);

                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.VISIBLE);

                String urlForMesh = "http://iyan3dapp.com/appapi/mesh/" + AssetsViewController.assetsId[position] + extension;

                String fileName = AssetsViewController.assetsId[position] + extension;

                currentMeshDownload = addToDownloadManager.downloadAdd(context, urlForMesh, fileName, cacheDir, "high", downloadManager);
            }
            else {
                AssetsViewController.assets_view_add_to_scene_btn.setVisibility(View.VISIBLE);
                AssetsViewController.selectedItemId = AssetsViewController.assets.get(position).getAssetsId();
                System.out.println("First Asset Id : " + AssetsViewController.assets.get(position).getAssetsId() );
                GL2JNILibAssetView.assetItemDisplay(AssetsViewController.assets.get(position).getAssetsId(), AssetsViewController.assets.get(position).getAssetName(), Integer.parseInt(AssetsViewController.assets.get(position).getType()));
            }
        }

        rowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                AssetsViewController.assets_view_selection_name.setText(result[position]);
                if (position != clickedPosition) {
                    View gridSelected = AssetsViewController.gridView.getChildAt(position);
                    LinearLayout gridHolder = (LinearLayout) gridSelected.findViewById(R.id.assets_list_elements_layout);
                    if(gridHolder!=null)
                    gridHolder.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_pressed));
                    if(AssetsViewController.assetsId.length > clickedPosition) {
                        View gridPreviousSelected = AssetsViewController.gridView.getChildAt(clickedPosition);
                        LinearLayout gridHolderPreviousItem = (LinearLayout) gridPreviousSelected.findViewById(R.id.assets_list_elements_layout);
                        if (gridHolderPreviousItem != null)
                            gridHolderPreviousItem.setBackgroundDrawable(context.getResources().getDrawable(R.drawable.border_grid_frames_non_pressed));
                    }
                }


                downloadManager.cancel(currentMeshDownload);
                downloadManager.cancel(currentTextureDownload);
                clickedPosition = position;

                String meshDirStr = Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/mesh/";
                Constant.mkDir(meshDirStr);

                File texture = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/mesh/" +
                        Integer.toString(AssetsViewController.assetsId[position]) + "-cm.png");

                String extension = "";
                if (AssetsViewController.assets.get(position).getType().equals("1"))
                    extension = ".sgr";
                else if (AssetsViewController.assets.get(position).getType().equals("2")
                        || AssetsViewController.assets.get(position).getType().equals("3"))
                    extension = ".sgm";
                else if (AssetsViewController.assets.get(position).getType().equals("6"))
                    extension = ".obj";
                File mesh = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/mesh/" +
                        Integer.toString(AssetsViewController.assetsId[position]) + extension);
                meshExtension = extension;


                if (AssetsViewController.assetsId[position] != AssetsViewController.selectedItemId) {
                    AssetsViewController.selectedItemId = AssetsViewController.assets.get(position).getAssetsId();
                    if (mesh.exists() && mesh.getTotalSpace() != 0 && texture.exists() && texture.getTotalSpace() != 0) {
                            AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.GONE);
                            System.out.println("Download Completed File Found");
                            System.out.println("Assets Type : " + AssetsViewController.assets.get(position).getType());
                            System.out.println("Assets Id : " + AssetsViewController.assets.get(position).getAssetsId());
                            System.out.println("Assets Name : " + AssetsViewController.assets.get(position).getAssetName());
                            GL2JNILibAssetView.assetItemDisplay(AssetsViewController.assets.get(position).getAssetsId(), AssetsViewController.assets.get(position).getAssetName(), Integer.parseInt(AssetsViewController.assets.get(position).getType()));
                            AssetsViewController.hideImportBtn(false);
                        }
                        else{
                        if(!Constant.isOnline(context)) {
                            Constant.informDialog(context, "Failed connecting to the server! Please try again later.");
                        }

                        if (!texture.exists() || texture.getTotalSpace() == 0) {
                                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.VISIBLE);
                                String urlForTexture = "http://iyan3dapp.com/appapi/meshtexture/" + AssetsViewController.assetsId[position] + ".png";
                                String fileName = AssetsViewController.assetsId[position] + "-cm.png";
                                String cacheDir =  Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/.cache/";
                                Constant.mkDir(cacheDir);
                                String toPath = Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/mesh/";
                                Constant.mkDir(toPath);
                                currentTextureDownload = addToDownloadManager.downloadAdd(context, urlForTexture, fileName, cacheDir, "high", downloadManager);
                                AssetsViewController.hideImportBtn(true);
                            }


                            if (!mesh.exists() || mesh.getTotalSpace() == 0) {
                                AssetsViewController.assetsViewCenterProgressbar.setVisibility(View.VISIBLE);
                                String urlForMesh = "http://iyan3dapp.com/appapi/mesh/" + AssetsViewController.assetsId[position] + extension;
                                String fileName = AssetsViewController.assetsId[position] + extension;
                                String cacheDir =  Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/.cache/";
                                currentMeshDownload = addToDownloadManager.downloadAdd(context, urlForMesh, fileName, cacheDir, "high", downloadManager);
                                AssetsViewController.hideImportBtn(true);
                            }
                        }

                }
            }
        });
        return rowView;
    }

}