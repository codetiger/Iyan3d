package com.smackall.animator.Adapters;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.support.v7.widget.CardView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.animator.Helper.FileHelper;
import com.smackall.animator.Helper.NewsFeedItem;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.NewsFeed;
import com.smackall.animator.R;

import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.List;

/**
 * Created by Sabish.M on 14/7/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */
public class NewsFeedItemAdapter extends BaseAdapter {
    public List<NewsFeedItem> itemDetailsrrayList;
    private HashMap<Integer, String> queueList = new HashMap<>();
    private Context mContext;
    private LayoutInflater l_Inflater;
    private NewsFeed newsFeed;
    public NewsFeedItemAdapter(Context context, List<NewsFeedItem> results, NewsFeed newsFeed) {
        this.mContext = context;
        itemDetailsrrayList = results;
        l_Inflater = LayoutInflater.from(context);
        this.newsFeed = newsFeed;
    }

    public int getCount() {
        return itemDetailsrrayList.size();
    }

    public Object getItem(int position) {
        return itemDetailsrrayList.get(position);
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(final int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = l_Inflater.inflate(R.layout.newsfeed_cell, parent, false);
            holder = new ViewHolder();
            holder.title = (TextView) convertView.findViewById(R.id.title);
            holder.description = (TextView) convertView.findViewById(R.id.description);
            holder.itemImage = (ImageView) convertView.findViewById(R.id.newsfeed_img);
            holder.cardView = (CardView)convertView.findViewById(R.id.cardView);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }
        holder.itemImage.setImageBitmap(null);
        holder.title.setText(itemDetailsrrayList.get(position).getTitle());
        holder.description.setText(itemDetailsrrayList.get(position).getMessage());
        if (FileHelper.checkValidFilePath(PathManager.LocalNewsFeedThumbnail + "/" + itemDetailsrrayList.get(position).getThumbnailUrl())) {
            convertView.findViewById(R.id.newsfeed_progress).setVisibility(View.INVISIBLE);
            Bitmap bitmap = BitmapFactory.decodeFile(PathManager.LocalNewsFeedThumbnail + "/" + itemDetailsrrayList.get(position).getThumbnailUrl());
            if (bitmap != null)
                holder.itemImage.setImageBitmap(Bitmap.createScaledBitmap(bitmap, 128, 128, false));
        } else if (!queueList.containsKey(position)) {
            queueList.put(position, "");
            convertView.findViewById(R.id.newsfeed_progress).setVisibility(View.VISIBLE);
            new GetThumbnail(itemDetailsrrayList.get(position).getThumbnailUrl()).execute();
        }
        holder.cardView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                newsFeed.newsFeedItemSelection(itemDetailsrrayList,position);
            }
        });
        return convertView;
    }

    static class ViewHolder {
        TextView title;
        TextView description;
        ImageView itemImage;
        CardView cardView;
    }

    public class GetThumbnail extends AsyncTask<Integer, Integer, String> {
        String name;

        public GetThumbnail(String name) {
            this.name = name;
        }

        private void downloadImage(String name) {
            int count;
            try {
                URL url = new URL("https://www.iyan3dapp.com/appapi/newsfeed/" + name);
                URLConnection conection = url.openConnection();
                conection.connect();
                int lenghtOfFile = conection.getContentLength();
                InputStream input = new BufferedInputStream(url.openStream(), 8192);
                OutputStream output = new FileOutputStream(PathManager.LocalNewsFeedThumbnail + "/" + name);
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
                    notifyDataSetChanged();
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