package com.smackall.animator.TextAssetsView;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.AsyncTask;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.smackall.animator.common.Constant;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

import smackall.animator.R;


/**
 * Created by android-developer on 15/9/15.
 */
public class DownloadFonts {

    public void downloadFontsFromUrl(Context context, int position, String url, String filename, String downloadPath, String toPath) {

        new Download(context, position, url, filename, downloadPath, toPath).execute();
    }


    private class Download extends AsyncTask<String, String, String> {

        Context context;
        String url;
        String filename;
        int position;
        String from;
        String to;

        public Download(Context context, int position, String url, String filename, String downloadPath, String toPath) {
            this.context = context;
            this.url = url;
            this.filename = filename;
            this.position = position;
            this.from = downloadPath;
            this.to = toPath;

        }

        protected void onPreExecute() {

        }

        protected String doInBackground(String... params) {
            int count;
            try {
                URL url = new URL(this.url);
                URLConnection conection = url.openConnection();
                conection.connect();
                // getting file length
                int lenghtOfFile = conection.getContentLength();

                // input stream to read file - with 8k buffer
                InputStream input = new BufferedInputStream(url.openStream(), 8192);

                // Output stream to write file
                Constant.mkDir(this.from);
                Constant.mkFile(this.from+this.filename);
                OutputStream output = new FileOutputStream(this.from+this.filename,false);

                byte data[] = new byte[1024];

                long total = 0;

                while ((count = input.read(data)) != -1) {
                    total += count;
                    // publishing the progress....
                    // After this onProgressUpdate will be called
                    publishProgress(""+(int)((total*100)/lenghtOfFile));

                    // writing data to file
                    output.write(data, 0, count);
                }

                // flushing output
                output.flush();

                // closing streams
                output.close();
                input.close();

            } catch (IOException e) {
                e.printStackTrace();
                return "";
            } finally {
            }

            return "done";
        }

        protected void onPostExecute(String result) {

            if (result.equals("done")) {
                System.out.println("Downloaded Fonts");
                File from = new File(this.from+filename);
                File to = new File(this.to+filename);
                from.renameTo(to);
                from.delete();

                View viewItem = TextAssetsSelectionView.gridView.getChildAt(position);

                if (viewItem != null) {
                    TextView textInside = (TextView) viewItem.findViewById(position);
                    textInside.setVisibility(View.VISIBLE);
                    ProgressBar progressBarTextAssets = (ProgressBar) viewItem.findViewById(R.id.progressBarTextAssets);
                    progressBarTextAssets.setVisibility(View.GONE);
                    try {
                        Typeface fontFace = Typeface.createFromFile(new File(this.to+filename));
                        textInside.setTypeface(fontFace);
                        textInside.setText("Text");
                        textInside.setTextSize(TextAssetsSelectionView.width / 45);
                        textInside.setTextColor(Color.WHITE);
                        if(position == 0){
                            TextAssetsSelectionView.hashMap.put("fileName", TextAssetsSelectionView.fontName[0]);
                            TextAssetsSelectionView.textUpdater();
                        }
                    }
                    catch (Exception e){
                        progressBarTextAssets.setVisibility(View.VISIBLE);
                        textInside.setVisibility(View.GONE);
                        File errorFile = new File(to+filename);
                        errorFile.delete();
                        System.out.println("Font set Failed");
                    }
                }
            }
        }
    }
}
