package com.viewcore.test;


import java.util.ArrayList;

import com.bfmj.viewcore.render.GLColor;

import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLAdapterView;
import com.bfmj.viewcore.view.GLAdapterView.OnItemSelectedListener;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLGridView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLListView;
import com.bfmj.viewcore.view.GLRelativeView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLView;

import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;

public class WangFZDemoActivity extends BaseViewActivity {
	private GLGridView grid;
	private ArrayList<GLRelativeView> list;
	private ArrayList<Integer> listData;
	private WangDemoAdpater wdAdapter;
	private GLListView listview;

	private GLRootView rootView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		//requestWindowFeature(Window.FEATURE_NO_TITLE);	
		//getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		//getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_wangfz_demo);
		
		rootView = getRootView();
		rootView.onResume();

//		grid = new GLGridView(this, 3, 4);
//		grid.setHorizontalSpacing(150.0f);
//		grid.setVerticalSpacing(30.0f);
//		grid.setWidth(2560.0f);
//		grid.setHeight(1440.0f);
//		grid.setStartIndex(5);
		
		list = new ArrayList<GLRelativeView>();
		listData = new ArrayList<Integer>();
		
		
		
		for(int i=0;i<9;i++){
			GLImageView image = new GLImageView(this);
			image.setId("timage");
			image.setWidth(100.0f);
			image.setHeight(200.0f);
			
			
//			
//			image.setBackground(R.drawable.ic_launcher);
			//image.setImage(R.drawable.haibaodemo);
			switch(i){
				case 0:{
//					image.setImage(R.drawable.a1);
					listData.add(R.drawable.a1);
					break;
				}
				case 1:{
//					image.setImage(R.drawable.a2);
					listData.add(R.drawable.a2);
					break;
				}
				case 2:{
//					image.setImage(R.drawable.a3);
					listData.add(R.drawable.a3);
					break;
				}
				case 3:{
//					image.setImage(R.drawable.a4);
					listData.add(R.drawable.a4);
					break;
				}
				case 4:{
//					image.setImage(R.drawable.a5);
					listData.add(R.drawable.a5);
					break;
				}
				case 5:{
//					image.setImage(R.drawable.a6);
					listData.add(R.drawable.a6);
					break;
				}
				case 6:{
//					image.setImage(R.drawable.a7);
					listData.add(R.drawable.a7);
					break;
				}
				case 7:{
//					image.setImage(R.drawable.a8);
					listData.add(R.drawable.a8);
					break;
				}
				case 8:{
//					image.setImage(R.drawable.a9);
					listData.add(R.drawable.a9);
					break;
				}
			
			}
			String _id = "";
			for(int ii=0;ii<listData.size();ii++){
				_id += listData.get(ii) + "--";
			}
			Log.d("bbbb-all:", _id);
			
			GLRelativeView view4 = new GLRelativeView(WangFZDemoActivity.this);
//			view4.setBackground(new float[]{0.2f, 0.2f, 0.2f, 1.0f});
			view4.setLayoutParams(100, 200);
//			view4.setMargin(30+i*10, 30+i*10, 0, 0);
//			view4.setBackground(new GLColor(0.2f, 1.0f, 0.2f));
			view4.addView(image);
			list.add(view4);
		}
		
		WangDemoAdpater wdAdapter = new WangDemoAdpater(WangFZDemoActivity.this, listData);
		
//		grid.setBackground(R.drawable.ic_launcher);
//		grid.setAdapter(wdAdapter);
		
		
		listview = new GLListView(this, GLListView.HORIZONTAL);
		listview.setItemSpacing(10.0f);
		listview.setMargin(0, 500, 0, 0);
		listview.setNumOneScreen(5);
//		listview.setOrderType(GLListView.VERTICAL);
		
		
		listview.setBackground(new GLColor(1.0f, 0.0f, 0.0f));
		listview.setWidth(500);
		listview.setHeight(300);
		
		listview.setOnItemSelectedListener(new OnItemSelectedListener() {
			@Override
			public void onItemSelected(GLAdapterView<?> glparent,
					GLView glview, int position, long id) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onNothingSelected(GLAdapterView<?> glparent,
					GLView glview, int position, long id) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onNoItemData() {
				// TODO Auto-generated method stub
				
			}
		});
		
		listview.setAdapter(wdAdapter);
		
		GLCursorView imageView = new GLCursorView(this);
		imageView.setBackground(new GLColor(1.0f, 1.0f, 1.0f));
		imageView.setDepth(3);
//		imageView.setImage(R.drawable.ic_launcher);
//		textView.setText("北京欢迎你");
//		textView.setAlpha(0.3f);
		imageView.setX( 460 );
		imageView.setY( 460);
		imageView.setLayoutParams(40, 40);
		
//		rootView.addView(grid);
		rootView.addView(listview);
		rootView.addView(imageView);
	}
	


	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		if(event.getAction() == MotionEvent.ACTION_DOWN){
//			listview.changeItem(false);
//			listview.move();
//			listview.moveLeft();
		}
		if(event.getAction() == MotionEvent.ACTION_UP){
//			listview.changeItem(false);
//			listview.move();
//			listview.moveRight();
		}
		return super.onTouchEvent(event);
	}

	
}
