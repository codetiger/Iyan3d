package com.smackall.animator;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.GridView;

import com.smackall.animator.Adapters.SceneSelectionAdapter;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.FullScreen;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SceneDB;
import com.smackall.animator.Helper.UIHelper;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

public class SceneSelection extends AppCompatActivity {

    DatabaseHelper db = new DatabaseHelper();
    InfoPopUp infoPopUp = new InfoPopUp();
    SceneSelectionAdapter sceneAdapter;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scene_selection);
        initGridView();
    }

    private void initGridView(){
        GridView gridview = (GridView) findViewById(R.id.gridView);
        sceneAdapter = new SceneSelectionAdapter(this,db,gridview);
        gridview.setAdapter(sceneAdapter);
        gridview.setHorizontalSpacing(Constants.width / 20);
        gridview.setVerticalSpacing(Constants.height / 15);
    }

    public void addNewScene(View view)
    {
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
        List<SceneDB> sceneDBs = db.getAllScenes();
        String sceneName = (sceneDBs.size() <= 0) ? "My Scene " + 1 : "My Scene " + Integer.toString(sceneDBs.get(db.getSceneCount() - 1).getID() + 1);
        db.addNewScene(new SceneDB(sceneName, FileHelper.md5(sceneName), date));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
        GridView gridview = (GridView) findViewById(R.id.gridView);
        gridview.scrollTo(0, gridview.getTop());
    }

    public void showRenameDialog(final int position){
        final AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setMessage("Enter Scene Name");
        final EditText input = new EditText(this);
        input.setHint("Scene Name");
        alert.setView(input);
        alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (input.getText().toString().trim().length() != 0) {
                    if (FileHelper.isItHaveSpecialChar(input.getText().toString().trim())) {
                        UIHelper.informDialog(SceneSelection.this, "You cannot use Special Characters in scene name.");
                        return;
                    }
                    List<SceneDB> sceneDBs = db.getAllSceneDetailsWithSearch(input.getText().toString());
                    if (sceneDBs.size() != 0) {
                        UIHelper.informDialog(SceneSelection.this, "Scene Already Exists.");
                    } else {
                        renameScene(input.getText().toString().trim(), position);
                    }
                } else {
                    dialog.dismiss();
                    UIHelper.informDialog(SceneSelection.this, "Scene name Cannot be empty.");
                }
            }
        });

        alert.setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                FullScreen.HideStatusBar(SceneSelection.this);
                dialog.dismiss();
            }
        });
        AlertDialog alertDialog = alert.create();
        alertDialog.show();
    }

    private void renameScene(String name,int position)
    {
        List<SceneDB> sceneDBs = db.getAllScenes();
        db.updateSceneDetails(new SceneDB(sceneDBs.get(position).getID(), name, sceneDBs.get(position).getImage(), sceneDBs.get(position).getTime()));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void deleteScene(int position)
    {
        List<SceneDB> sceneDBs = db.getAllScenes();
        File image = new File(PathManager.LocalScenesFolder+"/" + sceneDBs.get(position).getImage() + ".png");
        File projectFile = new File(PathManager.LocalProjectFolder+"/" + sceneDBs.get(position).getImage() + ".sgb");
        if(projectFile.exists())
            projectFile.delete();
        if(image.exists())
            image.delete();
        db.deleteScene(sceneDBs.get(position).getName());
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void cloneScene(int position)
    {
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(new Date());
       List<SceneDB> sceneDBs = db.getAllScenes();
        String sceneName = (sceneDBs.size() <= 0) ? "My Scene " + 1 : "My Scene " + Integer.toString(sceneDBs.get(db.getSceneCount() - 1).getID() + 1);
        File thumpnailFrom = new File(PathManager.LocalScenesFolder+"/" + sceneDBs.get(position).getImage() + ".png");
        File thumpnailTo = new File(PathManager.LocalScenesFolder+"/" + FileHelper.md5(sceneName) + ".png");

        File projectFrom = new File(PathManager.LocalProjectFolder+"/" + sceneDBs.get(position).getImage() + ".sgb");
        File projectTo = new File(PathManager.LocalProjectFolder+"/" + FileHelper.md5(sceneName) + ".sgb");
        if(thumpnailFrom.exists())
            FileHelper.copy(thumpnailFrom, thumpnailTo);
        if(projectFrom.exists())
            FileHelper.copy(projectFrom, projectTo);
        db.addNewScene(new SceneDB(sceneName, FileHelper.md5(sceneName), date));
        this.sceneAdapter.sceneDBs = db.getAllScenes();
        this.sceneAdapter.notifyDataSetChanged();
    }

    public void loadScene(int position){
        Intent editorScene = new Intent(SceneSelection.this,EditorView.class);
        editorScene.putExtra("scenePosition", position);
        editorScene.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        editorScene.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        editorScene.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        editorScene.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        startActivity(editorScene);
        dealloc();
    }

    public void openSceneProps(View v){
        infoPopUp.infoPopUpMenu(SceneSelection.this,v);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        dealloc();
    }

    private void dealloc()
    {
        db = null;
        sceneAdapter = null;
        infoPopUp = null;
        finish();
    }
}
