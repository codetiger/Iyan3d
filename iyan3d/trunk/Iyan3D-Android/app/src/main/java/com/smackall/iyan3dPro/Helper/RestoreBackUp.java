package com.smackall.iyan3dPro.Helper;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;

import com.smackall.iyan3dPro.R;
import com.smackall.iyan3dPro.SceneSelection;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Created by Sabish.M on 28/5/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class RestoreBackUp {

    private Context mContext;

    public RestoreBackUp(Context mContext) {
        this.mContext = mContext;
    }

    public void restoreI3D(String path) {
        if (!FileHelper.checkValidFilePath(path)) return;
        FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/");
        FileHelper.mkDir(PathManager.LocalCacheFolder + "/");
        PathManager.initPaths(((Activity) mContext));
        if (unZipI3DToCacheDir(path)) {
            manageTheExtractedFiles();
        }
    }

    private boolean unZipI3DToCacheDir(String path) {
        ZipInputStream zis = null;
        try {
            zis = new ZipInputStream(
                    new BufferedInputStream(new FileInputStream(path)));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            ZipEntry ze;
            int count;
            byte[] buffer = new byte[8192];
            if (zis != null) {
                while ((ze = zis.getNextEntry()) != null) {
                    File file = new File(PathManager.LocalCacheFolder + "/", ze.getName());
                    File dir = ze.isDirectory() ? file : file.getParentFile();
                    if (!dir.isDirectory() && !dir.mkdirs())
                        throw new FileNotFoundException("Failed to ensure directory: " +
                                dir.getAbsolutePath());
                    if (ze.isDirectory())
                        continue;
                    FileOutputStream fout = new FileOutputStream(file);
                    try {
                        while ((count = zis.read(buffer)) != -1)
                            fout.write(buffer, 0, count);
                    } finally {
                        fout.close();
                    }
                }
            }
        } finally {
            try {
                if (zis != null)
                    zis.close();
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
            return true;
        }
    }

    private void manageTheExtractedFiles() {
        String date = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault()).format(new Date());
        List<SceneDB> sceneDBs = ((SceneSelection) mContext).db.getAllScenes();
        String prefix = mContext.getResources().getString(R.string.myscene);
        String newSceneName = (sceneDBs.size() <= 0) ? prefix + " " + 1 : prefix + " " + Integer.toString(sceneDBs.get(((SceneSelection) mContext).db.getSceneCount() - 1).getID() + 1);
        String newSceneHash = FileHelper.md5(newSceneName);
        if (getFileListFromCacheDir() != null && getFileListFromCacheDir().length > 0) {
            File files[] = getFileListFromCacheDir();
            manageSGBAndThumpnail(files, newSceneHash);
            manageTextures(files);
            manageMesh(files);
            manageFonts(files);
            manageParticles(files);
            ((SceneSelection) mContext).db.addNewScene(new SceneDB(newSceneName, newSceneHash, date));
            ((Activity) mContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ((SceneSelection) mContext).sceneAdapter.sceneDBs = ((SceneSelection) mContext).db.getAllScenes();
                    ((SceneSelection) mContext).sceneAdapter.notifyDataSetChanged();
                    RecyclerView gridview = (RecyclerView) ((Activity) mContext).findViewById(R.id.gridView);
                    gridview.scrollToPosition(((SceneSelection) mContext).sceneAdapter.sceneDBs.size());
                }
            });
            FileHelper.deleteFilesAndFolder(PathManager.LocalCacheFolder + "/");
        }
    }

    private void manageSGBAndThumpnail(File[] files, String newName) {
        for (File file : files) {
            if (FileHelper.getFileExt(file).toLowerCase().equals("sgb")) {
                String name = FileHelper.getFileWithoutExt(file);
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".sgb", PathManager.LocalProjectFolder + "/" + newName + ".sgb");
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".png", PathManager.LocalScenesFolder + "/" + newName + ".png");
                file.delete();
                new File(PathManager.LocalCacheFolder + "/" + name + ".png").delete();
                break;
            }
        }
    }

    private void manageTextures(File[] files) {
        for (File file : files) {
            String name = FileHelper.getFileWithoutExt(file);
            if (FileHelper.getFileExt(file).toLowerCase().equals("png")) {
                if (file.getAbsolutePath().toLowerCase().endsWith("-cm.png")) {
                    if (!FileHelper.checkValidFilePath(PathManager.LocalTextureFolder + "/" + name + ".png"))
                        FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".png", PathManager.LocalTextureFolder + "/" + name + ".png");
                    file.delete();
                } else {
                    if (!FileHelper.checkValidFilePath(PathManager.LocalImportedImageFolder + "/" + name + ".png"))
                        FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".png", PathManager.LocalImportedImageFolder + "/" + name + ".png");
                }
            }
        }
    }

    private void manageMesh(File[] files) {
        for (File file : files) {
            String name = FileHelper.getFileWithoutExt(file);
            String ext = FileHelper.getFileExt(file).toLowerCase();
            if (ext.equals("sgm") || ext.equals("sgr")) {
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + "." + ext, PathManager.LocalMeshFolder + "/" + name + "." + ext);
                file.delete();
            }
        }
    }

    private void manageFonts(File[] files) {
        for (File file : files) {
            String name = FileHelper.getFileWithoutExt(file);
            String ext = FileHelper.getFileExt(file).toLowerCase();
            if (ext.equals("ttf") || ext.equals("otf")) {
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + "." + ext, PathManager.LocalFontsFolder + "/" + name + "." + ext);
                file.delete();
            }
        }
    }

    private void manageParticles(File[] files) {
        for (File file : files) {
            String name = FileHelper.getFileWithoutExt(file);
            String ext = FileHelper.getFileExt(file).toLowerCase();
            if (ext.equals("json")) {
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + "." + ext, PathManager.LocalMeshFolder + "/" + name + "." + ext);
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".sgm", PathManager.LocalMeshFolder + "/" + name + ".sgm");
                FileHelper.copy(PathManager.LocalCacheFolder + "/" + name + ".png", PathManager.LocalTextureFolder + "/" + name + "-cm.png");
                file.delete();
            }
        }
    }

    private File[] getFileListFromCacheDir() {
        final String commonFolder = PathManager.LocalCacheFolder + "/";
        final File f = new File(commonFolder);
        return f.listFiles();
    }
}
