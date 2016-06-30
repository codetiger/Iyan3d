package com.smackall.animator;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.MediaController;
import android.widget.VideoView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.Events;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;

import java.io.File;

public class Preview extends AppCompatActivity implements View.OnClickListener {

    MediaController mediaController;

    VideoView preview_VideoView;
    ImageView preview_ImageView;
    int type = Constants.IMAGE;
    String path = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.preview);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        HitScreens.EditorView(Preview.this);

        mediaController = new MediaController(this);
        showPreview(getIntent().getExtras().getString("path"));
    }

    public void showPreview(String fileNameWithoutExt){

        if(FileHelper.checkValidFilePath(PathManager.RenderPath+"/"+ fileNameWithoutExt+".png"))
            type = Constants.IMAGE;
        else if(FileHelper.checkValidFilePath(PathManager.RenderPath+"/"+fileNameWithoutExt+".mp4"))
            type = Constants.VIDEO;
        else
            return;

        path = PathManager.RenderPath+"/"+fileNameWithoutExt+((type == Constants.IMAGE) ? ".png" : ".mp4");

        init(type);
        showPreviewScene();
    }

    private void init(int type){
        preview_VideoView = ((VideoView)findViewById(R.id.preview_videoView));
        preview_ImageView = ((ImageView)findViewById(R.id.preview_imageView));

        findViewById(R.id.preview_close).setOnClickListener(this);
        findViewById(R.id.preview_share).setOnClickListener(this);

        preview_VideoView.setVisibility((type == Constants.VIDEO) ? View.VISIBLE : View.GONE);
        preview_ImageView.setVisibility((type == Constants.IMAGE) ? View.VISIBLE : View.GONE);
    }

    private void showPreviewScene(){
        if(!FileHelper.checkValidFilePath(path)){
            findViewById(R.id.preview_close).performClick();
            return;
        }

        HitScreens.Preview(this, type);
        if(type == Constants.IMAGE)
            preview_ImageView.setImageBitmap(BitmapFactory.decodeFile(path));
        else{
            preview_VideoView.setVideoPath(path);
            if(mediaController == null)
                mediaController = new MediaController(this);
            preview_VideoView.setMediaController(mediaController);
            mediaController.setAnchorView(preview_VideoView);
            mediaController.setMediaPlayer(preview_VideoView);
            preview_VideoView.requestFocus();
            preview_VideoView.start();
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if(mediaController != null && type == Constants.VIDEO)
            mediaController.show(0);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.preview_close:
                finish();
                break;
            case R.id.preview_share:
                try {
                    Events.shareEvent(this,type);
                    Intent shareIntent = new Intent();
                    shareIntent.setAction(Intent.ACTION_SEND);
                    shareIntent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(new File(path)));
                    shareIntent.setType((type == Constants.IMAGE) ? "image/png" : "video/mp4");
                    startActivity(Intent.createChooser(shareIntent, "Share to"));
                }
                catch (ActivityNotFoundException e){
                    UIHelper.informDialog(this,"No Activity found for share this.");
                }
                break;
        }
    }

    @Override
    public void onBackPressed() {
        finish();
        super.onBackPressed();
    }
}
