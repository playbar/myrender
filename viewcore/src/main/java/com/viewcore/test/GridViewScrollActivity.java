package com.viewcore.test;

import android.opengl.Matrix;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.androidquery.AQuery;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.render.GLConstant;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLAdapterView;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLGridViewScroll;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLModelView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;
import com.bfmj.viewcore.view.GLView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class GridViewScrollActivity extends BaseViewActivity {

	private GLRootView rootView;
	private GLGridViewScroll gridView;
//	private GLSeekBarView processView;
	private AQuery aq;
	private GridViewAdapter adapter;
	private int index;
	private List<Map<String, Object>> listData = new ArrayList<Map<String, Object>>();
	private GLModelView	modelView;

	// 图片封装为一个数组
	private int[] icon = {
		R.drawable.address_book,
		R.drawable.calendar,
		R.drawable.camera,
		R.drawable.clock,
		R.drawable.games_control,
		R.drawable.messenger,
		R.drawable.ringtone,
		R.drawable.settings,
		R.drawable.speech_balloon,
		R.drawable.weather,
		R.drawable.world,
		R.drawable.youtube
	};

	private String[] iconName = {
		"通讯录", "日历", "照相机", "时钟", "游戏", "短信", "铃声",
		"设置", "语音", "天气", "浏览器", "视频"
	};

	public List<Map<String, Object>> getData(){
		//cion和iconName的长度是相同的，这里任选其一都可以
		for(index=0; index < 7; ++index){
			Map<String, Object> map = new HashMap<String, Object>();
			map.put("image", icon[index]);
			map.put("text", iconName[index]);
			listData.add(map);
		}

		return listData;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		Log.d("test time", "time 1 = > " + System.currentTimeMillis());
//		setMojingType("42EGCV-WQXG87-WHQHS8-YYYTEF-4SWGST-CRY7SS");
		// 3288 key
//		setMojingType("WNE8QN-QVCCZN-XXF8W3-9Q2YDH-AQZRA2-XVDZ9Q");
		super.onCreate(savedInstanceState);
		Log.d("test time", "time 2 = > " + System.currentTimeMillis());
		rootView = getRootView();

		int jj = 0;
		int ff = 0;
		for( int i = 0; i < 100; i++)
		{
			ff = ++jj;
		}

		rootView.onResume();
		//rootView.setDoubleScreen(false);
		rootView.setDistortionEnable( true );
		GLAdapterView.OnItemSelectedListener listener = new GLAdapterView.OnItemSelectedListener(){
			public void onItemSelected(GLAdapterView<?> glparent, GLView glview, int position, long id){
				Log.e("com/bfmj/viewcore/test", "onItemSelected:" + glview.toString());
				glview.setAlpha(0.3f);
			}

			public void onNothingSelected(GLAdapterView<?> glparent, GLView glview, int position, long id){
				Log.e("com/bfmj/viewcore/test", "onNothingSelected:" + glview.toString() );
				glview.setAlpha( 1.0f);
			}
			public void onNoItemData(){
				Log.e("com/bfmj/viewcore/test", "onNoItemData");
			}
		};

		GLAdapterView.OnItemClickListener clickListener = new GLAdapterView.OnItemClickListener(){
			public void onItemClick(GLAdapterView<?> glparent, GLView glview, int position, long id){
				Log.e("com/bfmj/viewcore/test", "onItemClick");

			}
		};

		gridView = new GLGridViewScroll( this, 1, 3);
		gridView.setX(500);
		gridView.setY(500);
		gridView.setLayoutParams(40, 40);
		gridView.setBackground( new GLColor(1.0f, 0.50f, 0.50f ));
		gridView.setHorizontalSpacing( 20.0f);
		gridView.setVerticalSpacing( 20.0f);
		gridView.setBtnHorSpace( 0);
		gridView.setBottomSpaceing( 100);
//		gridView.setPadding( 10, 10, 10, 10);
//		gridView.setMargin( 10, 10, 10, 10 );
//		gridView.setNumDefaultColor( new GLColor(1.0f, 0.0f, 1.0f ));
		gridView.setNumOnFouseColor( new GLColor(1.0f, 0.0f, 1.0f ));
		gridView.setFlipLeftIcon(R.drawable.flip_leftarrow);
		gridView.setFlipRightIcon( R.drawable.flip_rightarrow );
		gridView.setProcessBackground(R.drawable.playbar_progressbar_bg);
		gridView.setBarImage(R.drawable.playbar_progressbar);
		gridView.setOffsetX( 100);
		gridView.setBtnImageWidth(60);
		gridView.setBtnImageHeight(60);
		gridView.setProcessViewWidth(400);
		gridView.setProcessViewHeight(40);
		gridView.setPrvBtnImgViewVisible(false);
		gridView.setNextBtnImgViewVisible(false);
//		gridView.setSeekBarVisible( false );

		gridView.setOrientation(GLConstant.GLOrientation.HORIZONTAL );
		gridView.setPrvPageChange(new GLGridViewScroll.PageChangeListener() {
			public void onPageChange() {
				gridView.previousPage();
			}
		});

		gridView.setNextPageChange(new GLGridViewScroll.PageChangeListener() {
			@Override
			public void onPageChange() {
				gridView.nextPage();
			}
		});

		getData();
		adapter = new GridViewAdapter(listData, this);
		gridView.setOnItemSelectedListener( listener );
		gridView.setOnItemClickListener( clickListener );
		gridView.setWidth(1000);
		gridView.setHeight(1000);
//		gridView.getView(3);
//		gridView.setTotalCount( 6 );
		gridView.setAdapter( adapter );
		//gridView.rotate(90.0f, 1.0f, 0.0f, 0.0f );
//		gridView.rotate(30, 0, 1, 0);
//		gridView.setDepth( 3.5f);
		rootView.addView(gridView);

		GLImageView lineH = new GLImageView(this);
		lineH.setX(0);
		lineH.setY(500);
		lineH.setLayoutParams( 960, 2 );
		lineH.setBackground( new GLColor( 0, 0, 1));
		rootView.addView(lineH);

		GLImageView line = new GLImageView(this);
		line.setX(500);
		line.setY( 0);
		line.setLayoutParams(2, 960);
		line.setBackground(new GLColor(0, 1, 0));
		rootView.addView(line);

		//rootView.setRotationX( 90 );

		//gridView.setItemSpacing(20);

		gridView.setOnKeyListener(new GLOnKeyListener() {

			@Override
			public boolean onKeyDown(GLRectView view, int keycode) {
//				if(keycode == MojingKeyCode.KEYCODE_DPAD_RIGHT){
//					listView.moveLeft();
//				}
//				if(keycode == MojingKeyCode.KEYCODE_DPAD_LEFT){
//					listView.moveRight();
//				}
				Log.e("onCreate", "onKeyDown");
				return false;
			}

			@Override
			public boolean onKeyUp(GLRectView view, int keycode) {
				Log.e("onCreate", "onKeyUp");
				return false;
			}

			@Override
			public boolean onKeyLongPress(GLRectView view, int keycode) {
				return false;
			}

		});


//		processView = new GLSeekBarView(this);
//		processView.setBackground(R.drawable.playbar_progressbar_bg);
////		processView.setProcessColor(R.drawable.playbar_progressbar);
//		processView.setBarImage(R.drawable.playbar_progressbar);
//		processView.setLayoutParams(500,20);
//		processView.setX(1000);
//		processView.setY(2000);
//		rootView.addView(processView);
//		processView.setProcess(0);
//		processView.setMargin(30,10,0,0);

//		GLProcessView processView = new GLProcessView(this);
//		processView.setX(1000);
//		processView.setY(2000);
//		processView.setLayoutParams( 1000, 20);
//		processView.setBackground( new GLColor( 1.0f, 0.0f, 1.0f));
//		processView.setProcess(100);
//		rootView.addView( processView );

		GLTextView textView = new GLTextView(this);
		textView.setX( 1000);
		textView.setY( 2000 );
		textView.setLayoutParams(1000, 200 );
		textView.setTextColor(new GLColor(0.0f, 1.0f, 1.0f));
		textView.setText("111的境况");
		textView.setTextSize(100);
		textView.setOnKeyListener(new GLOnKeyListener() {
			@Override
			public boolean onKeyDown(GLRectView view, int keycode) {
				view.setAlpha( 0.3f );
				return false;
			}

			@Override
			public boolean onKeyUp(GLRectView view, int keycode) {
				view.setAlpha( 1.0f );
				return false;
			}

			@Override
			public boolean onKeyLongPress(GLRectView view, int keycode) {
				return false;
			}
		});
		textView.setFocusListener(new GLViewFocusListener() {
			@Override
			public void onFocusChange(GLRectView view, boolean focused) {
				if( focused )
					view.setAlpha( 0.3f );
				else
					view.setAlpha( 1.0f );
			}
		});

		//gridView.addView( textView );
		rootView.addView(textView);
//
//		gridView.setDepth(-4);

//		GLCursorView imageView = new GLCursorView(this);
//		imageView.setWidth(10);
//		imageView.setHeight(10);
//		imageView.setBackground(new GLColor(1.0f, 1.0f, 1.0f));
//		imageView.setDepth(3);
//		imageView.setLayoutParams(1000, 1000, 100, 100);
//		rootView.addView(imageView);

		GLCursorView cursorView = new GLCursorView(this);
		cursorView.setX(1190);
		cursorView.setY(1190);
		cursorView.setLayoutParams( 20, 20);
		cursorView.setBackground(new GLColor(1.0f, 0, 0));
		cursorView.setDepth(3);
		rootView.addView(cursorView);
		//rootView.setBackgroundColor( 0xFFFFFF );

//		modelView = new GLModelView(this);
//		if(modelView != null) {
//			float[] prjMatrix = new float[16];
//			float[] mvpMatrix = new float[16];
//			float[] VMatrix = new float[16];
//			Matrix.setLookAtM(VMatrix, 0,
//					0, 0, 15,
//					0, 0, 0,
//					0, 1, 0);
//			Matrix.frustumM( prjMatrix, 0,
//                        -1, 1,
//                        -1, 1,
//                        3, 2000);
//
//			Matrix.multiplyMM(mvpMatrix, 0,
//					prjMatrix,0,
//					VMatrix, 0);
//			modelView.setMatrix(mvpMatrix);
//			modelView.loadModel("/sdcard/amenemhat/amenemhat.obj");
//			rootView.addView(modelView);
//		}
//		updateProgress();
//		rootView.addView(listView);		

	}


	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		Log.e("onKeyUp", "onKeyUpn code =" + keyCode);
		if( keyCode == 22 ){
			gridView.nextPage();
		}
		else if( keyCode == 21 )
		{
			gridView.previousPage();
		}
		else {
			gridView.onKeyUp(keyCode);
		}
		switch (keyCode) {
			case KeyEvent.KEYCODE_MENU:
				return true;
			case KeyEvent.KEYCODE_BACK:
				return true;
			default:
				break;
		}
		return false;
		//return super.onKeyUp(keyCode, event);
	}

	private void updateProgress() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				int i = 0;
				while (i++ <= 100) {
					try {

						Thread.sleep(100);
					} catch (Exception e) {

					}
//					processView.setProcess(i);
				}


			}
		}).start();
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		if( index >= icon.length )
			return super.onTouchEvent( event );
		if(event.getAction() == MotionEvent.ACTION_DOWN){
//			listview.changeItem(false);
//			listview.move();
//			listView.moveLeft();
//			getRootView().queueEvent(new Runnable() {
//				@Override
//				public void run() {
//					Map<String, Object> map = new HashMap<String, Object>();
//					map.put("image", icon[index]);
//					map.put("text", iconName[index]);
//					listData.add(map);
//					adapter.notifyDataSetChanged();
//					// todo
//					//gridView.showItem(0);
//					index++;
//					Log.e("OnTouchEvent", "add item");
//				}
//			});

		}
		if(event.getAction() == MotionEvent.ACTION_MOVE){
//			listview.changeItem(false);
//			listview.move();
//			listView.moveRight();
		}
		return super.onTouchEvent(event);
	}
	
}
