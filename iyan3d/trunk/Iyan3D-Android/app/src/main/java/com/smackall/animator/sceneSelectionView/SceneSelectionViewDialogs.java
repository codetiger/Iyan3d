package com.smackall.animator.sceneSelectionView;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.database.sqlite.SQLiteDatabase;
import android.os.Environment;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.smackall.animator.common.AssetsManager;
import com.smackall.animator.common.Constant;
import com.smackall.animator.sceneSelectionView.DB.SceneDB;
import com.smackall.animator.sceneSelectionView.DB.SceneSelectionDatabaseHandler;
import com.smackall.animator.sceneSelectionView.newCoverFlow.SceneSelectionCoverFlow;
import com.smackall.animator.sceneSelectionView.newCoverFlow.ViewGroupExampleAdapter;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import smackall.animator.R;


/**
 * Created by Sabish on 11/09/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class SceneSelectionViewDialogs {

    int width = Constant.width;
    int height = Constant.height;

    AssetsManager assetsManager;

    List<SceneDB> scenes;


    public void newSceneDialog(final Context context, final Activity activity, final SceneSelectionDatabaseHandler db) {
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("Enter Scene Name");

        final EditText input = new EditText(context);
        input.setHint("Scene Name");
        alert.setView(input);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {

                if (input.getText().toString().trim().length() != 0) {
                    scenes = db.getAllScenes("", input.getText().toString().trim());


                    if (scenes.size() != 0) {
                        sceneAlreadyExitsDialog(context, activity);
                        System.out.println("Duplicate Found");
                    } else {

                        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());

                        assetsManager = new AssetsManager();

                        assetsManager.copyFile(context, "Stay Hungry Stay Foolish.png", Constant.md5(input.getText().toString().trim()),".png",null);
                        // takeScreenShot(context, activity, Environment.getExternalStorageDirectory() + "/iyan3d/sources/" + SceneSelectionView.md5(newSceneEditText.getText().toString()) + ".png");
                        db.addScene(new SceneDB(input.getText().toString().trim(), Constant.md5(input.getText().toString().trim()), date));
                        scenes = db.getAllScenes("", "");
                        ViewGroupExampleAdapter.sceneImages = new String[scenes.size()];
                        ViewGroupExampleAdapter.sceneNames = new String[scenes.size()];
                        ViewGroupExampleAdapter.sceneCreatedTime = new String[scenes.size()];

                        for (int i = 0; i < db.getScensCount(); i++) {
                            ViewGroupExampleAdapter.sceneNames[i] = scenes.get(i).getName();
                            ViewGroupExampleAdapter.sceneImages[i] = scenes.get(i).getImage();
                            ViewGroupExampleAdapter.sceneCreatedTime[i] = scenes.get(i).getTime();
                        }

                        SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) activity.findViewById(R.id.sceneSelectionCoverFlow);
                        fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());
                        fancyCoverFlow.setSelection(db.getScensCount() - 1);
                        Constant.FullScreencall(activity, context);
                        dialog.dismiss();
                    }
                }
                else {
                    dialog.dismiss();
                    Constant.informDialog(context, "Scene name Cannot be empty.");
                }

                if (db.getScensCount() > 0) {
                    Constant.sceneSelectionView.clone_btn.setVisibility(View.VISIBLE);
                    Constant.sceneSelectionView.delete_btn.setVisibility(View.VISIBLE);
                }

            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                Constant.FullScreencall(activity, context);
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();

    }



    public void sceneAlreadyExitsDialog(final Context context, final Activity activity) {
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);

        alert.setMessage("MESSAGE");

        final TextView textView = new TextView(context);
        textView.setText("Scene Name already found! try another one.");
        alert.setView(textView);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Constant.FullScreencall(activity,context);
                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = alert.create();
        alertDialog.show();

    }

    public void deleteSceneDialog(final Context context, final Activity activity, final SceneSelectionDatabaseHandler db) {
        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("Delete Scene");

        final TextView msg = new TextView(context);
        msg.setText("Do you want to delete the scene?");
        alert.setView(msg);

        alert.setPositiveButton("YES", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {

                SQLiteDatabase sceneDatabase;

                sceneDatabase = SQLiteDatabase.openDatabase(Constant.sceneDatabaseLocation, null, SQLiteDatabase.CREATE_IF_NECESSARY);

                File image = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/scenes/" + Constant.md5(ViewGroupExampleAdapter.sceneNames[Constant.currentItemID]) + ".png");
                File projectFile = new File(Environment.getExternalStorageDirectory() + "/Android/data/"+context.getPackageName()+"/projects/" + Constant.md5(ViewGroupExampleAdapter.sceneNames[Constant.currentItemID]) + ".sgb");
                if(projectFile.exists())
                    projectFile.delete();

                image.delete();

                sceneDatabase.delete("Scenes", "sceneName" + " = ?", new String[]{String.valueOf(ViewGroupExampleAdapter.sceneNames[Constant.currentItemID])});
                scenes = db.getAllScenes("", "");
                ViewGroupExampleAdapter.sceneImages = new String[db.getScensCount()];
                ViewGroupExampleAdapter.sceneNames = new String[db.getScensCount()];
                ViewGroupExampleAdapter.sceneCreatedTime = new String[db.getScensCount()];

                for (int i = 0; i < db.getScensCount(); i++) {
                    System.out.println("Scene Names :" + scenes.get(i).getName());
                    System.out.println("Scene Image :" + scenes.get(i).getImage());
                    System.out.println("Scene Time  :" + scenes.get(i).getTime());

                    ViewGroupExampleAdapter.sceneNames[i] = scenes.get(i).getName();
                    ViewGroupExampleAdapter.sceneImages[i] = scenes.get(i).getImage();
                    ViewGroupExampleAdapter.sceneCreatedTime[i] = scenes.get(i).getTime();
                }

                SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) activity.findViewById(R.id.sceneSelectionCoverFlow);
                fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());
                fancyCoverFlow.setSelection(Constant.currentItemID - 1);
                Constant.FullScreencall(activity, context);
                dialog.dismiss();
                if(db.getScensCount() <=0){
                    Constant.sceneSelectionView.clone_btn.setVisibility(View.INVISIBLE);
                    Constant.sceneSelectionView.delete_btn.setVisibility(View.INVISIBLE);
                }
            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                Constant.FullScreencall(activity,context);
                dialog.dismiss();
            }
        });
        if(db.getScensCount() !=0){
            AlertDialog alertDialog = alert.create();
            alertDialog.show();
        }


    }

    public void cloneSceneDialog(final Context context, final Activity activity, final SceneSelectionDatabaseHandler db) {

        final AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage("Clone Scene");

        final EditText input = new EditText(context);
        input.setHint("Scene Name");
        alert.setView(input);

        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {

                if (input.getText().toString().trim().length() != 0) {
                    scenes = db.getAllScenes("", input.getText().toString().trim());

                    if (scenes.size() != 0) {
                        sceneAlreadyExitsDialog(context, activity);
                        System.out.println("Dublicate Found");
                    } else {

                        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());

                        //assetsManager = new AssetsManager();

                        // assetsManager.copyFile(context, "Stay Hungry Stay Foolish.png", Constant.md5(input.getText().toString().trim()));
                        scenes = db.getAllScenes("", "");
                        File thumpnailFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/scenes/" + scenes.get(Constant.currentItemID).getImage() + ".png");
                        File thumpnailTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/scenes/" + Constant.md5(input.getText().toString().trim()) + ".png");

                        File projectFrom = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/projects/" + scenes.get(Constant.currentItemID).getImage() + ".sgb");
                        File projectTo = new File(Environment.getExternalStorageDirectory() + "/Android/data/" + context.getPackageName() + "/projects/" + Constant.md5(input.getText().toString().trim()) + ".sgb");

                            try {
                                if(thumpnailFrom.exists())
                                    Constant.copy(thumpnailFrom, thumpnailTo);
                                if(projectFrom.exists())
                                    Constant.copy(projectFrom, projectTo);
                                }
                            catch (IOException e) {
                                    e.printStackTrace();
                            }

                        // takeScreenShot(context, activity, Environment.getExternalStorageDirectory() + "/iyan3d/sources/" + SceneSelectionView.md5(newSceneEditText.getText().toString()) + ".png");
                        db.addScene(new SceneDB(input.getText().toString().trim(), Constant.md5(input.getText().toString().trim()), date));
                        scenes = db.getAllScenes("", "");
                        ViewGroupExampleAdapter.sceneImages = new String[scenes.size()];
                        ViewGroupExampleAdapter.sceneNames = new String[scenes.size()];
                        ViewGroupExampleAdapter.sceneCreatedTime = new String[scenes.size()];

                        for (int i = 0; i < db.getScensCount(); i++) {
                            ViewGroupExampleAdapter.sceneNames[i] = scenes.get(i).getName();
                            ViewGroupExampleAdapter.sceneImages[i] = scenes.get(i).getImage();
                            ViewGroupExampleAdapter.sceneCreatedTime[i] = scenes.get(i).getTime();
                        }

                        SceneSelectionCoverFlow fancyCoverFlow = (SceneSelectionCoverFlow) activity.findViewById(R.id.sceneSelectionCoverFlow);
                        fancyCoverFlow.setAdapter(new ViewGroupExampleAdapter());
                        fancyCoverFlow.setSelection(db.getScensCount() - 1);
                        Constant.FullScreencall(activity, context);
                        dialog.dismiss();
                    }
                } else {
                    dialog.dismiss();
                    Constant.informDialog(context, "Scene name Cannot be empty.");
                }
            }

        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                Constant.FullScreencall(activity, context);
                dialog.dismiss();
            }
        });


            AlertDialog alertDialog = alert.create();
            alertDialog.show();
    }
}
