package com.smackall.animator.ImportAndRig;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.GridView;

import com.smackall.animator.common.Constant;
import com.smackall.animator.common.FileHelper;
import com.smackall.animator.common.Services;
import com.smackall.animator.sceneSelectionView.SceneSelectionView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

import smackall.animator.R;


public class ImportObjAndTexture extends Activity {

    public static GridView obj_import_grid, texture_import_grid;

    ArrayList<String> objTempArray;
    ArrayList<String> textureTempArray;
    String[] objList;
    String[] textureList;
    Bitmap bmp;

    public static String objPath = "";
    public static String texturePath = "";
    public static String objFileName = "";
    public static String textureFileName = "";
    Button importObjBtn,cancel_btn;
    public static boolean isImported = false;

    String userMesh;
    Context mContext;
    boolean isFileManagerFound = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_import_obj_and_texture);
        Constant.FullScreencall(ImportObjAndTexture.this, this);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mContext = this;
        Constant.importObjAndTexture = ImportObjAndTexture.this;

        obj_import_grid = (GridView) findViewById(R.id.obj_browse_grid);
        texture_import_grid = (GridView) findViewById(R.id.texture_browse_grid);
        cancel_btn = (Button) findViewById(R.id.import_obj_cancel_btn);

        final Button import_obj_btn = (Button) findViewById(R.id.import_obj_btn);
        final Button import_texture_btn = (Button) findViewById(R.id.import_texture_btn);
        importObjBtn = (Button) findViewById(R.id.import_obj_next_btn);

        import_obj_btn.setText("Refresh");
        import_texture_btn.setText("Refresh");
        Constant.informDialog(mContext, "Please copy Obj and Texture files to SD-Card/Iyan3D folder.");

        import_obj_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                intent.setType("file/obj");
//                if (intent.resolveActivity(getPackageManager()) != null) {
//                    startActivityForResult(intent, 1);
//                }
//                else {
                    FileHelper.getObjAndTextureFromCommonIyan3dPath(mContext);
//                    import_obj_btn.setVisibility(View.INVISIBLE);
//                    import_texture_btn.setVisibility(View.INVISIBLE);
                    //Constant.informDialog(mContext, "FileManager not found for pick the file so you need copy files to SD-Card/Iyan3D folder.");
//                }
            }
        });

        import_texture_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//                intent.setType("file/png");
//                if (intent.resolveActivity(getPackageManager()) != null) {
//                    startActivityForResult(intent, 1);
//                }
//                else {
                    FileHelper.getObjAndTextureFromCommonIyan3dPath(mContext);
//                    import_obj_btn.setVisibility(View.INVISIBLE);
//                    import_texture_btn.setVisibility(View.INVISIBLE);
                    //Constant.informDialog(mContext, "FileManager not found for pick the file so you need copy files to SD-Card/Iyan3D folder.");
//                }
            }
        });

        importObjBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isImported){
                    Intent i = new Intent();
                    i.setClass(ImportObjAndTexture.this, RigModel.class);
                    i.putExtra("objPath", objPath);
                    i.putExtra("objFileName", objFileName);
                    i.putExtra("texturePath", texturePath);
                    i.putExtra("textureFileName", textureFileName);
                    startActivity(i);
                    ImportObjAndTexture.this.finish();
                }
            }
        });

        cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(ImportObjAndTexture.this, SceneSelectionView.class);
                startActivity(intent);
                ImportObjAndTexture.this.finish();
            }
        });

        String mesh = Environment.getExternalStorageDirectory()+"/Android/data/" +getPackageName()+"/mesh/";
        Constant.mkDir(mesh);
        userMesh = Environment.getExternalStorageDirectory()+"/Android/data/" +getPackageName()+"/mesh/usermesh/";
        Constant.mkDir(userMesh);
        gridLoader(userMesh);
    }

    private void gridViewObj() {
        obj_import_grid.setAdapter(new CustomAdapterForObjImportGrid(this, objList));
        obj_import_grid.setNumColumns(3);
        obj_import_grid.setHorizontalSpacing(Constant.width / 60);
        obj_import_grid.setVerticalSpacing(Constant.width / 60);
    }

    private void gridViewTexture() {
        texture_import_grid.setAdapter(new CustomAdapterForTextureImportGrid(this, textureList));
        texture_import_grid.setNumColumns(3);
        texture_import_grid.setHorizontalSpacing(Constant.width / 60);
        texture_import_grid.setVerticalSpacing(Constant.width / 60);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        // TODO Auto-generated method stub
        switch(requestCode){
            case 1:
                if(resultCode==RESULT_OK){
                    Intent data = intent;
                    Uri selectedTexture = data.getData();
                    String filePath = Constant.getPathFromUri(selectedTexture, this);
                    String fileName = new File(filePath).getName();
                    String ext = Constant.getFileExt(new File(filePath).getName());

                    if(ext.equals("obj") || ext.equals("png")){
                        addToLocal(filePath, fileName);
                    }
                    else {
                        Constant.informDialog(this,"File Not Support!");
                    }
                }
                break;
        }
    }

    private void addToLocal(String filePath, String fileName){

        File from = new File(filePath);
        File to = new File(userMesh + fileName);

        if(bmp != null && !bmp.isRecycled())
        {
            bmp = null;
        }

        try {
            Constant.copy(from, to);
        } catch (IOException e) {
            e.printStackTrace();
        }
        finally {
            gridLoader(userMesh);
        }
    }

    public void gridLoader(String userMesh){
        File f = new File(userMesh);
        File file[] = f.listFiles();

        objTempArray = new ArrayList<>();
        for (int i = 0; i < file.length; i++) {
            if (file[i].getPath().endsWith(".obj")) {
                String objFileName = new File(file[i].getPath()).getName();
                objTempArray.add(objFileName);
            }
        }

        textureTempArray = new ArrayList<>();
        for (int i = 0; i < file.length; i++) {
            if (file[i].getPath().endsWith(".png")) {
                String userImageThump = Environment.getExternalStorageDirectory()+"/Android/data/"+getPackageName()+"/images/";
                Constant.mkDir(userImageThump);
                bmp = BitmapFactory.decodeFile(file[i].getPath());
                Bitmap scaledBitmap = Bitmap.createScaledBitmap(bmp, 128, 128, false);
                String textureFileName = new File(file[i].getPath()).getName();
                savePng(scaledBitmap, userImageThump, textureFileName);
                textureTempArray.add(textureFileName);
            }
        }

        objList = new String[objTempArray.size()];
        textureList = new String[textureTempArray.size()];
        for (int i=0; i < objTempArray.size(); i++)
        {
            objList[i]  = objTempArray.get(i);
        }
        for (int t=0; t < textureTempArray.size(); t++)
        {
            textureList[t]  = textureTempArray.get(t);
        }

        if(objList.length != 0)
            gridViewObj();
        if(textureList.length !=0) {
            gridViewTexture();
        }
    }

    public void savePng( Bitmap img,String filePath, String fileName) {

        try {
            File dumpFile = new File(filePath, fileName);
            FileOutputStream os = new FileOutputStream(dumpFile);
            img.compress(Bitmap.CompressFormat.PNG, 50, os);
            os.flush();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onBackPressed(){
        Intent intent = new Intent(ImportObjAndTexture.this,SceneSelectionView.class);
        startActivity(intent);
        ImportObjAndTexture.this.finish();

    }

    @Override
    public void onResume(){
        super.onResume();
        stopService(new Intent(this, Services.class));
//            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
//            intent.setType("file/obj");
//            if (intent.resolveActivity(getPackageManager()) == null) {
                FileHelper.getObjAndTextureFromCommonIyan3dPath(mContext);
//        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        startService(new Intent(this, Services.class));
    }
}
