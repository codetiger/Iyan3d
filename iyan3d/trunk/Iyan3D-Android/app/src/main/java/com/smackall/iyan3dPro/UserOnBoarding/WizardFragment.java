package com.smackall.iyan3dPro.UserOnBoarding;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.smackall.iyan3dPro.R;

import java.util.Locale;

public class WizardFragment extends Fragment {

    int wizard_page_position;
    Activity mActivity;

    public static WizardFragment newInstance(int position) {
        WizardFragment fragment = new WizardFragment();
        final Bundle args = new Bundle(1);
        args.putInt("position", position);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        wizard_page_position = getArguments().getInt("position");
        mActivity = getActivity();
    }

    @Override
    public View onCreateView(LayoutInflater inflater,
                             @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = null;
        switch (wizard_page_position) {
            case 0:
                view = inflater.inflate(R.layout.page1, container, false);
                break;
            case 1:
                view = inflater.inflate(R.layout.page2, container, false);
                ((ImageView) view.findViewById(R.id.onBoard_img)).setImageDrawable(ContextCompat.getDrawable(mActivity, R.drawable.step_1));
                ((TextView) view.findViewById(R.id.onBoard_title)).setText(String.format(Locale.getDefault(), "%s", "Create a Scene"));
                ((TextView) view.findViewById(R.id.onBoard_description)).setText(String.format(Locale.getDefault(), "%s", "Add characters, environment, props from our library of over 300 assets or import an OBJ file from an external source."));
                break;
            case 2:
                view = inflater.inflate(R.layout.page2, container, false);
                ((ImageView) view.findViewById(R.id.onBoard_img)).setImageDrawable(ContextCompat.getDrawable(mActivity, R.drawable.step_2));
                ((TextView) view.findViewById(R.id.onBoard_title)).setText(String.format(Locale.getDefault(), "%s", "Animate Character"));
                ((TextView) view.findViewById(R.id.onBoard_description)).setText(String.format(Locale.getDefault(), "%s", "Bring your character to life with our pre-defined motions, apply physics simulation or make one of your own actions."));
                break;
            case 3:
                view = inflater.inflate(R.layout.page2, container, false);
                ((ImageView) view.findViewById(R.id.onBoard_img)).setImageDrawable(ContextCompat.getDrawable(mActivity, R.drawable.step_3));
                ((TextView) view.findViewById(R.id.onBoard_title)).setText(String.format(Locale.getDefault(), "%s", "Export Video"));
                ((TextView) view.findViewById(R.id.onBoard_description)).setText(String.format(Locale.getDefault(), "%s", "Render the final video with the desired quality and style."));
                break;
            case 4:
                view = inflater.inflate(R.layout.page2, container, false);
                ((ImageView) view.findViewById(R.id.onBoard_img)).setImageDrawable(ContextCompat.getDrawable(mActivity, R.drawable.step_4));
                ((TextView) view.findViewById(R.id.onBoard_title)).setText(String.format(Locale.getDefault(), "%s", "Share your video"));
                ((TextView) view.findViewById(R.id.onBoard_description)).setText(String.format(Locale.getDefault(), "%s", "That's it. Send your 3D movie and showoff your skills to your friends."));
                break;
        }
        return view;
    }

}
