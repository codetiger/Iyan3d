package com.smackall.animator.UserOnBoarding;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.View;
import android.widget.TextView;

import com.smackall.animator.Analytics.HitScreens;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.UIHelper;
import com.smackall.animator.R;
import com.smackall.animator.SceneSelection;

public class UserOnBoarding extends FragmentActivity implements View.OnClickListener{

	private CustomViewPager viewPager;
	private View indicator1;
	private View indicator2;
	private View indicator3;
	private View indicator4;
	private View indicator5;
	boolean firstTimeUser = false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.user_on_boarding);

		this.setFinishOnTouchOutside(false);
		this.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
		this.getWindow().setLayout((int)(Constants.width/1.15),(int)(Constants.height/1.15));

		Bundle bundle = getIntent().getExtras();
		if(bundle != null && bundle.getInt("firstTimeUser") == 1) {
			firstTimeUser = (bundle.getInt("firstTimeUser") == 1);
		}

		indicator1 = (View) findViewById(R.id.indicator1);
		indicator2 = (View) findViewById(R.id.indicator2);
		indicator3 = (View) findViewById(R.id.indicator3);
		indicator4 = (View) findViewById(R.id.indicator4);
		indicator5 = (View) findViewById(R.id.indicator5);

		findViewById(R.id.next_btn).setOnClickListener(this);
		findViewById(R.id.more_info).setOnClickListener(this);
		findViewById(R.id.close_btn).setOnClickListener(this);

		viewPager = (CustomViewPager) findViewById(R.id.viewPager);
		viewPager.setAdapter(new ViewPagerAdapter(getSupportFragmentManager()));
		viewPager.addOnPageChangeListener(new WizardPageChangeListener());
		updateUI(0);
		HitScreens.UserOnBoarding(this);
	}

	@Override
	public void onBackPressed() {
		if (viewPager.getCurrentItem() == 0) {
			super.onBackPressed();
		} else {
			viewPager.setCurrentItem(viewPager.getCurrentItem() - 1);
		}
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()){
			case R.id.next_btn:
				viewPager.arrowScroll(View.FOCUS_RIGHT);
				if(((TextView)findViewById(R.id.next_btn_text)).getText().toString().toLowerCase().equals("done"))
					finish();
				break;
			case R.id.more_info:
				String url = "https://www.iyan3dapp.com/tutorial-videos/";
				Intent i = new Intent(Intent.ACTION_VIEW);
				i.setData(Uri.parse(url));
				try {
					startActivity(i);
				}
				catch (ActivityNotFoundException e){
					UIHelper.informDialog(this,"Browser not found.");
				}
				finish();
				break;
			case R.id.close_btn:
				finish();
				if(firstTimeUser) {
					Intent intent = new Intent(Intent.ACTION_MAIN);
					intent.setClass(this,SceneSelection.class);
					intent.addCategory(Intent.ACTION_MAIN);
					intent.addCategory(Intent.CATEGORY_LAUNCHER);
					intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP |
							Intent.FLAG_ACTIVITY_SINGLE_TOP);
					intent.putExtra("userOnBoardClosed", 1);
					PendingIntent pi = PendingIntent.getActivity(this, 0, intent,
							PendingIntent.FLAG_UPDATE_CURRENT);
					try {
						pi.send();
					} catch (PendingIntent.CanceledException e) {
						e.printStackTrace();
					}
				}
				break;
		}
	}

	private class ViewPagerAdapter extends FragmentPagerAdapter {

		private int WIZARD_PAGES_COUNT = 5;

		public ViewPagerAdapter(FragmentManager fm) {
			super(fm);
		}

		@Override
		public Fragment getItem(int position) {
			return WizardFragment.newInstance(position);
		}

		@Override
		public int getCount() {
			return WIZARD_PAGES_COUNT;
		}

	}

	private class WizardPageChangeListener implements OnPageChangeListener {

		@Override
		public void onPageScrollStateChanged(int position) {
			// TODO Auto-generated method stub

		}

		@Override
		public void onPageScrolled(int position, float positionOffset,
								   int positionOffsetPixels) {
			// TODO Auto-generated method stub

		}

		@Override
		public void onPageSelected(int position) {
			updateUI(position);
		}

	}

	public void updateUI(int position) {
		DisplayMetrics metrics = getResources().getDisplayMetrics();
		int resizeValue = (int) TypedValue.applyDimension(
				TypedValue.COMPLEX_UNIT_DIP, 15, metrics);
		int defaultValue = (int) TypedValue.applyDimension(
				TypedValue.COMPLEX_UNIT_DIP, 10, metrics);

		findViewById(R.id.more_info).setVisibility(View.INVISIBLE);
		((TextView)findViewById(R.id.next_btn_text)).setText("Next");

		setIndicator1Value(defaultValue);
		setIndicator2Value(defaultValue);
		setIndicator3Value(defaultValue);
		setIndicator4Value(defaultValue);
		setIndicator5Value(defaultValue);
		switch (position) {
			case 0:
				((TextView)findViewById(R.id.next_btn_text)).setText("Let's get started!");
				setIndicator1Value(resizeValue);
				break;
			case 1:
				setIndicator2Value(resizeValue);
				break;
			case 2:
				setIndicator3Value(resizeValue);
				break;
			case 3:
				setIndicator4Value(resizeValue);
				break;
			case 4:
				setIndicator5Value(resizeValue);
				((TextView)findViewById(R.id.next_btn_text)).setText("Done");
				findViewById(R.id.more_info).setVisibility(View.VISIBLE);
				break;
			default:
				break;
		}
	}

	void setIndicator1Value(int value){
		indicator1.getLayoutParams().height = value;
		indicator1.getLayoutParams().width = value;
		indicator1.requestLayout();
	}
	void setIndicator2Value(int value){
		indicator2.getLayoutParams().height = value;
		indicator2.getLayoutParams().width = value;
		indicator2.requestLayout();
	}
	void setIndicator3Value(int value){
		indicator3.getLayoutParams().height = value;
		indicator3.getLayoutParams().width = value;
		indicator3.requestLayout();
	}
	void setIndicator4Value(int value){
		indicator4.getLayoutParams().height = value;
		indicator4.getLayoutParams().width = value;
		indicator4.requestLayout();
	}
	void setIndicator5Value(int value){
		indicator5.getLayoutParams().height = value;
		indicator5.getLayoutParams().width = value;
		indicator5.requestLayout();
	}
}
