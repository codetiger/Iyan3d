package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.CreditTask;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.HQTaskDB;
import com.smackall.animator.Helper.MediaScannerWrapper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.Preview;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;
import com.smackall.animator.opengl.GL2JNILib;

import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.Locale;

/**
 * Created by Sabish.M on 29/3/16.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */

public class RenderingProgressAdapter extends BaseAdapter implements CreditTask {

    Context mContext;
    DatabaseHelper db;
    boolean downloadPending = false;

    public RenderingProgressAdapter(Context context, DatabaseHelper db){
        this.mContext = context;
        this.db = db;
    }

    @Override
    public int getCount() {
        String className = mContext.getClass().getSimpleName();
        String uniqueId = ((className.toLowerCase().equals("sceneselection")) ?((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId);
        return ((db.getAllTasks(uniqueId) != null) ? db.getAllTasks(uniqueId).size(): 0);
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        final View list;
        if(convertView==null){
            LayoutInflater inflater=((Activity)mContext).getLayoutInflater();
            list=inflater.inflate(R.layout.cloud_progress_cell, parent, false);

        }else{
            list = (View)convertView;
        }

        String className = mContext.getClass().getSimpleName();
        String uniqueId = ((className.toLowerCase().equals("sceneselection") ?((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId));
        HQTaskDB taskDB = db.getAllTasks(uniqueId).get(position);
                ((TextView) list.findViewById(R.id.taskName)).setText(taskDB.getName());
        ((TextView)list.findViewById(R.id.progress_date)).setText(taskDB.getDate());
        if(taskDB.getTaskType() == Constants.EXPORT_IMAGES) {
            if(FileHelper.checkValidFilePath(PathManager.RenderPath+"/"+taskDB.getTask()+".png")) {
                ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "Open"));
            }
            else{
                ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "In Queue"));
                if (taskDB.getProgress() >= 100)
                    ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "Download"));
            }
        }
        else {
            if(FileHelper.checkValidFilePath(PathManager.RenderPath+"/"+taskDB.getTask()+".mp4")) {
                ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "Open"));
            }
            else {
                if (taskDB.getProgress() > 0)
                    ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%d%s", taskDB.getProgress(), "%"));
                else
                    ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "In Queue"));
                if (taskDB.getProgress() >= 100)
                    ((TextView) list.findViewById(R.id.task_precentage)).setText(String.format(Locale.getDefault(), "%s", "Download"));
            }
        }
        list.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                downloadTaskFile(position,list);
            }
        });
        return list;
    }

    private void downloadTaskFile(int position,View list){
        String className = mContext.getClass().getSimpleName();
        String uniqueId = ((className.toLowerCase().equals("sceneselection") ?((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId));
        HQTaskDB task = db.getAllTasks(uniqueId).get(position);
        if(task.getCompleted() == Constants.TASK_PROGRESS || downloadPending)
            return;
        String ext = (task.getTaskType() == Constants.EXPORT_IMAGES) ? ".png" : ".mp4";
        String path = PathManager.RenderPath+"/"+task.getTask()+ext;
        if(FileHelper.checkValidFilePath(path)){
            Intent i = new Intent(((Activity)mContext),Preview.class);
            i.putExtra("path",FileHelper.getFileWithoutExt(path));
            ((Activity)mContext).startActivity(i);
        }
        else{
            ((TextView)list.findViewById(R.id.task_precentage)).setVisibility(View.GONE);
            ((ProgressBar)list.findViewById(R.id.downloadProgress)).setVisibility(View.VISIBLE);
            String uhash = task.getTask()+"-"+((className.toLowerCase().equals("sceneselection") ?((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId));
            uhash = FileHelper.md5(uhash);
            String url = GL2JNILib.TaskDownload()+"?taskid="+task.getTask()+"&uhash="+uhash+"&action=0";
            downloadPending = true;
            new DownloadTaskCompleteFile(url,path,this,task,list).execute();
        }
    }

    public void updateTaskStatus()
    {
        String className = mContext.getClass().getSimpleName();
        String uniqueId = ((className.toLowerCase().equals("sceneselection") ?((SceneSelection)((Activity)mContext)).userDetails.uniqueId : ((EditorView)((Activity)mContext)).userDetails.uniqueId));
        for (int i = 0; i < ((db.getAllTasks(uniqueId) != null) ?db.getAllTasks(uniqueId).size(): 0); i++){
            if(db.getAllTasks(uniqueId).get(i).getCompleted() != Constants.TASK_COMPLETED){
                try {
                    if (className.toLowerCase().equals("sceneselection"))
                        ((SceneSelection) ((Activity) mContext)).creditsManager.getTaskProgress(RenderingProgressAdapter.this, db.getAllTasks(uniqueId).get(i).getTask());
                    else
                        ((EditorView) ((Activity) mContext)).creditsManager.getTaskProgress(RenderingProgressAdapter.this, db.getAllTasks(uniqueId).get(i).getTask());
                }
                catch (ClassCastException ignored){return;}
            }
        }
        notifyDataSetChanged();
    }

    @Override
    public void onCreditRequestCompleted(int credit, int premiumUser) {

    }

    @Override
    public void onCheckProgressCompleted(int progress, int taskId) {
        HQTaskDB taskDB = db.getTaskWithTaskId(taskId).get(0);
        if(taskDB != null){
            taskDB.setProgress(progress);
            taskDB.setCompleted((progress >= 100) ? Constants.TASK_COMPLETED : Constants.TASK_PROGRESS);
            db.updateTask(taskDB);
        }
        notifyDataSetChanged();
    }

    @Override
    public void onTaskFileDownloadCompleted(final HQTaskDB taskDB, final View list, final boolean downloadCompleted) {
        (((Activity)mContext)).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (downloadCompleted) {
                    UIHelper.informDialog(mContext, ((taskDB.getTaskType() == Constants.EXPORT_IMAGES) ? "Image" : "Video") + " Successfully saved in your SDCard/Iyan3D/Render");
                    String ext = (taskDB.getTaskType() == Constants.EXPORT_IMAGES) ? ".png" : ".mp4";
                    MediaScannerWrapper.scan(mContext,PathManager.RenderPath + "/" + taskDB.getName() + ext);
                } else
                    UIHelper.informDialog(mContext, "Cannot download file. Check your internet connection.");
                ((TextView) list.findViewById(R.id.task_precentage)).setVisibility(View.VISIBLE);
                ((ProgressBar) list.findViewById(R.id.downloadProgress)).setVisibility(View.GONE);
                downloadPending = false;
                notifyDataSetChanged();
            }
        });
    }

    @Override
    public void finishExport(int frame, boolean status,String msg) {

    }

    @Override
    public void failed() {

    }

    private class DownloadTaskCompleteFile extends AsyncTask<String, Void, Boolean> {
        String link;
        String path;
        CreditTask task;
        HQTaskDB taskDB;
        View list;
        public DownloadTaskCompleteFile(String link,String path,CreditTask creditTask,HQTaskDB taskDB,View list){
            this.link = link;
            this.path = path;
            this.task = creditTask;
            this.taskDB = taskDB;
            this.list = list;
        }
        @Override
        protected Boolean doInBackground(String... urls) {
            return  downloadTaskCompleteFile(link,path);
        }
        // onPostExecute displays the results of the AsyncTask.
        @Override
        protected void onPostExecute(Boolean result) {
            task.onTaskFileDownloadCompleted(taskDB,list,result);
        }
    }

    public boolean downloadTaskCompleteFile(String link,String path)
    {
        int count;
        try {
            URL url = new URL(link);
            URLConnection conection = url.openConnection();
            conection.connect();
            int lenghtOfFile = conection.getContentLength();
            InputStream input = new BufferedInputStream(url.openStream(), 8192);
            OutputStream output = new FileOutputStream(path);
            byte data[] = new byte[1024];
            long total = 0;
            while ((count = input.read(data)) != -1) {
                total += count;
                output.write(data, 0, count);
            }
            output.flush();
            output.close();
            input.close();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}
