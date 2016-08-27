package com.smackall.animator;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.os.AsyncTask;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ListView;

import com.smackall.animator.Adapters.NewsFeedItemAdapter;
import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.DatabaseHelper;
import com.smackall.animator.Helper.JSONParser;
import com.smackall.animator.Helper.NewsFeedItem;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;
import com.smackall.animator.Helper.UIHelper;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by Sabish.M on 2/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NewsFeed {

    Context mContext;
    Dialog dialog;
    SharedPreferenceManager sp;
    DatabaseHelper db;
    ProgressDialog progressDialog;

    public NewsFeed(Context context, SharedPreferenceManager sp, DatabaseHelper db) {
        this.mContext = context;
        this.sp = sp;
        this.db = db;
    }

    public void showNewsFeed(MotionEvent event, View v) {
        Dialog newsFeed = new Dialog(mContext);
        newsFeed.requestWindowFeature(Window.FEATURE_NO_TITLE);
        newsFeed.setContentView(R.layout.news_feed);
        newsFeed.setCancelable(false);
        newsFeed.setCanceledOnTouchOutside(true);
        switch (UIHelper.ScreenType) {
            case Constants.SCREEN_NORMAL:
                newsFeed.getWindow().setLayout((int) (Constants.width / 1.5), Constants.height);
                break;
            default:
                newsFeed.getWindow().setLayout(Constants.width / 3, (int) (Constants.height / 1.25));
                break;
        }
        Window window = newsFeed.getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.gravity = Gravity.TOP | Gravity.START;
        wlp.dimAmount = 0.0f;
        if (event != null) {
            wlp.x = (int) event.getX();
            wlp.y = (int) event.getY();
        } else {
            int[] location = new int[2];
            v.getLocationOnScreen(location);
            wlp.x = location[0];
            wlp.y = location[1];
        }
        window.setAttributes(wlp);

        dialog = newsFeed;
        newsFeed.show();
        HitScreens.NewsFeed(mContext);
        syncNewsFeed(false);
    }

    private void initView(final Dialog dialog, final List<NewsFeedItem> newsFeedItems) {
        final ListView listView = (ListView) dialog.findViewById(R.id.newsFeed_listview);
        listView.setAdapter(new NewsFeedItemAdapter(mContext, newsFeedItems,this));

        dialog.findViewById(R.id.clear).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (newsFeedItems != null) {
                    if (newsFeedItems.size() > 0)
                        sp.setData(mContext, "newsFeedLastId", newsFeedItems.get(0).getId());
                    ((NewsFeedItemAdapter) listView.getAdapter()).itemDetailsrrayList.clear();
                    ((NewsFeedItemAdapter) listView.getAdapter()).notifyDataSetChanged();
                }
            }
        });
    }

    public void syncNewsFeed(boolean isSync) {
        new GetNewsFeeds(isSync).execute();
    }

    public class GetNewsFeeds extends AsyncTask<Integer, Integer, String> {
        String response;
        int lastid;
        boolean isSync;

        public GetNewsFeeds(boolean isSync) {
            lastid = sp.getInt(mContext, "newsFeedLastId");
            this.isSync = isSync;
        }

        @Override
        protected String doInBackground(Integer... params) {
            String charset = "UTF-8";
            JSONParser jParser = new JSONParser();
            response = jParser.getJSONFromUrl("https://www.iyan3dapp.com/appapi/json/newsfeed.json");
            return response;
        }

        protected void onPostExecute(String st) {
            if (response != null && response.length() > 0) {
                try {
                    List<NewsFeedItem> newsFeedItems = new ArrayList<>();
                    JSONArray jArrayForFeeds = new JSONArray(response);
                    for (int n = 0; n < jArrayForFeeds.length(); n++) {
                        JSONObject jsonObject = jArrayForFeeds.getJSONObject(n);
                        newsFeedItems.add(n, new NewsFeedItem(
                                jsonObject.getInt("id"), jsonObject.getInt("type"), jsonObject.getString("title"),
                                jsonObject.getString("message"), jsonObject.getString("url"),
                                jsonObject.getString("thumbnailurl")));
                    }
                    SQLiteDatabase feedDb = SQLiteDatabase.openDatabase(PathManager.Iyan3DDatabse, null, SQLiteDatabase.CREATE_IF_NECESSARY);
                    feedDb.beginTransaction();
                    try {
                        for (int i = 0; i < newsFeedItems.size(); i++) {
                            ContentValues values = new ContentValues();
                            values.put(DatabaseHelper.NEWS_FEED_FEED_ID, newsFeedItems.get(i).getId());
                            values.put(DatabaseHelper.NEWS_FEED_MESSAGE, newsFeedItems.get(i).getMessage());
                            values.put(DatabaseHelper.NEWS_FEED_THUMBNAILURL, newsFeedItems.get(i).getThumbnailUrl());
                            values.put(DatabaseHelper.NEWS_FEED_TITLE, newsFeedItems.get(i).getTitle());
                            values.put(DatabaseHelper.NEWS_FEED_TYPE, newsFeedItems.get(i).getType());
                            values.put(DatabaseHelper.NEWS_FEED_URL, newsFeedItems.get(i).getUrl());
                            int value = feedDb.update(DatabaseHelper.NEWS_FEED_TABLE, values, DatabaseHelper.NEWS_FEED_FEED_ID + " = ?", new String[]{String.valueOf(newsFeedItems.get(i).getId())});
                            if (value == 0)
                                feedDb.insert(DatabaseHelper.NEWS_FEED_TABLE, null, values);
                        }
                        feedDb.setTransactionSuccessful();
                    } finally {
                        feedDb.endTransaction();
                        if (feedDb.isOpen())
                            feedDb.close();
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                ((Activity) mContext).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        List<NewsFeedItem> newsFeedItems = db.getAllFeeds(String.valueOf(lastid));
                        if (isSync && mContext != null) {
                            int lastIdForCount = sp.getInt(mContext, "newsFeedCountLastId");
                            if (db.getAllFeeds(String.valueOf(lastIdForCount)) != null && db.getAllFeeds(String.valueOf(lastIdForCount)).size() > 0) {
                                ((SceneSelection) mContext).showNewsFeedBadge(String.valueOf(db.getAllFeeds(String.valueOf(lastIdForCount)).size()));
                            }
                        } else
                            initView(dialog, newsFeedItems);
                    }
                });
            }
        }
    }

    public void newsFeedItemSelection(final List<NewsFeedItem> newsFeedItems,int position)
    {
        if (newsFeedItems == null || newsFeedItems.size() == 0) return;
        int type = newsFeedItems.get(position).getType();
        switch (type) {
            case Constants.ALERT_TYPE:
                UIHelper.informDialog(mContext, newsFeedItems.get(position).getMessage());
                if (dialog.isShowing())
                    dialog.dismiss();
                break;
            case Constants.IMAGE_TYPE_FEED:
                progressDialog = new ProgressDialog(mContext);
                progressDialog.setTitle(mContext.getString(R.string.please_wait));
                progressDialog.setCanceledOnTouchOutside(false);
                progressDialog.show();
                String url = newsFeedItems.get(position).getUrl();
                new DownloadImage("feedImage.png", PathManager.LocalCacheFolder, url).execute();
                break;
            case Constants.URL_TYPE:
                String link = newsFeedItems.get(position).getUrl();
                Intent shareIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(link));
                mContext.startActivity(shareIntent);
                if (dialog.isShowing())
                    dialog.dismiss();
                break;
        }
    }

    public class DownloadImage extends AsyncTask<Integer, Integer, String> {
        String name;
        String path;
        String link;

        public DownloadImage(String name, String outputPath, String url) {
            this.name = name;
            this.path = outputPath;
            this.link = url;
        }

        private void downloadImage(String name) {
            int count;
            try {
                URL url = new URL(link);
                URLConnection conection = url.openConnection();
                conection.connect();
                int lenghtOfFile = conection.getContentLength();
                InputStream input = new BufferedInputStream(url.openStream(), 8192);
                OutputStream output = new FileOutputStream(path + "/" + name);
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
                UIHelper.informDialog(mContext, mContext.getString(R.string.check_network_msg));
                return;
            }
            ((Activity) mContext).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Intent i = new Intent(mContext, Preview.class);
                    i.putExtra("path", "feedImage");
                    mContext.startActivity(i);
                    if (progressDialog != null && progressDialog.isShowing())
                        progressDialog.dismiss();
                    if (dialog != null && dialog.isShowing())
                        dialog.dismiss();
                }
            });
        }

        @Override
        protected String doInBackground(Integer... params) {
            downloadImage(name);
            return "";
        }

        protected void onPostExecute(String st) {
        }
    }
}
