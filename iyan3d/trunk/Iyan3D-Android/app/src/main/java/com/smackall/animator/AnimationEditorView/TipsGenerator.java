package com.smackall.animator.AnimationEditorView;

import android.content.Context;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

/**
 * Created by Sabish.M on 9/10/15.
 * Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
 */
public class TipsGenerator {

    Context context;

    public ArrayList<String> getTips(Context context) throws IOException, JSONException {
        this.context = context;
        JSONArray jsonArray = null;

        try {
            InputStream is = context.getAssets().open("helpStatements.json");
            int size = is.available();
            byte[] data = new byte[size];
            is.read(data);
            is.close();
            String json = new String(data, "UTF-8");
            JSONObject result = new JSONObject(json);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (JSONException r) {
            r.printStackTrace();
        }
        return null;
    }
}
