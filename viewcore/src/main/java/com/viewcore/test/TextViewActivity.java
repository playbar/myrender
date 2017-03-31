package com.viewcore.test;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.bfmj.distortion.Logger;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.render.GLVector2;
import com.bfmj.viewcore.render.GLVector3;
import com.bfmj.viewcore.util.GLGenTexTask;
import com.bfmj.viewcore.util.IntersectionTest;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;

import java.util.Random;

public class TextViewActivity extends BaseViewActivity {

	private GLRootView rootView;
	private GLGenTexTask mTask;
	private Bitmap mBitmap;

	AssetManager assetManager =null;

	public void testIntersection()
	{
		GLVector3 tl = new GLVector3(-2.4002545f, 0.8000848f, -3.7f);
		GLVector3 tr = new GLVector3(0.0f,        0.8000848f, -3.3f);
		GLVector3 bl = new GLVector3(-2.4002545f, -0.0f,      -3.7f);

		GLVector3 light = new GLVector3(-0.17050685f, -0.18699859f, -0.9674465f);
		IntersectionTest test = new IntersectionTest(tl, tr, bl);
		GLVector2 vec2 = new GLVector2();
		boolean b = test.Intersection( light, vec2);
		Logger.printTime("" + b);
		return;
	}

	public void testIntersection1()
	{
		GLVector3 tl = new GLVector3(-1.2001272f, 0.8000848f, -2.7998729f);
		GLVector3 tr = new GLVector3(1.2001272f,  0.8000848f, -5.200127f);
		GLVector3 bl = new GLVector3(-1.2001272f, -0.0f,      -2.7998729f);

		GLVector3 light = new GLVector3(-0.3503908f, 0.067306325f, -0.93419f);
		IntersectionTest test = new IntersectionTest(tl, tr, bl);
		GLVector2 vec2 = new GLVector2();
		boolean b = test.Intersection( light, vec2);
		Logger.printTime("" + b);
		return;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
//		setMojingType("42EGCV-WQXG87-WHQHS8-YYYTEF-4SWGST-CRY7SS");
//		setMojingType("WNE8QN-QVCCZN-XXF8W3-9Q2YDH-AQZRA2-XVDZ9Q");
		super.onCreate(savedInstanceState);

		testIntersection1();

//		BitmapOp bmtOp = new BitmapOp();
//		String str = bmtOp.stringFromJNI();
//		Log.e("TextViewActivity", "onCreate");

//		mTask = new GLGenTexTask(GLGenTexTask.class.hashCode());
//		mTask.setGenTexIdInterface( new GenTexIdInterface(){
//			public void ExportTextureId(int mTextureId, int mHashCode){
//				Log.e("TextViewActivity", "mTask");
//				mTask.uninit();
//				mBitmap.recycle();
//			}
//		});
//		mTask.init();
//		assetManager= getAssets();
//		try {
//			InputStream in=assetManager.open("textext.png");
//			mBitmap = BitmapFactory.decodeStream(in);
//			mTask.GenTexId(mBitmap, mBitmap.getWidth(), mBitmap.getHeight());
//		} catch (Exception e) {
//			// TODO: handle exception
//		}


		rootView = getRootView();
//		rootView.setMultiThread( true );
//		rootView.setTimeWarp( true );

		rootView.onResume();
		//rootView.setDoubleScreen(false);
		rootView.setDistortionEnable( true );


//		addShowFPS();


		GLTextView textView = new GLTextView(this);
		textView.setX( 1700);
		textView.setY( 1000 );
		textView.setLayoutParams(600, 200 );
		textView.setBackground( new GLColor(1.0f, 1.0f, 1.0f));
		textView.setTextColor(new GLColor(1.0f, 0.0f, 0.0f));
		textView.setText("88");
		textView.setPadding(10, 5, 0, 0);
		textView.setTextSize(40);
		textView.rotate(-45, 0, 1, 0);
		textView.setDepth(3.5f);
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
					view.setAlpha( 0.6f );
				else
					view.setAlpha( 1.0f );
			}
		});

		rootView.addView(textView);
//
		GLCursorView cursorView = new GLCursorView(this);
		cursorView.setX(1190);
		cursorView.setY(1190);
		cursorView.setLayoutParams( 20, 20);
		cursorView.setBackground(new GLColor(0xff0000));
//		cursorView.setBackground(new GLColor(1.0f, 1.0f, 1.0f));
		cursorView.setDepth(3);
		rootView.addView(cursorView);
//
//		GLRectView rectView = new GLRectView(this);
//		rectView.setBackground( new GLColor(1.0f, 0.0f, 0.0f));
//		rectView.setX( 1000);
//		rectView.setY( 1000);
//		rectView.setLayoutParams( 200, 200 );
//		rootView.addView( rectView );

		GLImageView imgView = new GLImageView(this);
		imgView.setX(800);
		imgView.setY(800);
		imgView.setLayoutParams(300, 300);
		imgView.setEdgeWH( 0.01f, 0.08f);
//		imgView.setLTColor( new float[]{0.0f, 1.0f, 0.0f, 1.0f});
		imgView.setImage(R.drawable.weather);
		rootView.addView(imgView);

		addImageViewTest();

	}

	public void addImageViewTest(){
		GLImageView[] imageViews = new GLImageView[100];
		Random random=new Random();
		for (int i = 0; i < imageViews.length; i++) {
			imageViews[i] = new GLImageView(this);
			imageViews[i].setX(random.nextInt(2400));
			imageViews[i].setY(random.nextInt(2400));
			imageViews[i].setLayoutParams(300, 300);
			imageViews[i].setImage(R.drawable.calendar);
			imageViews[i].setEdgeWH(0.1f, 0.1f);
//			imageViews[i].setDepth(4 - (i - 100)*0.005f);
			rootView.addView(imageViews[i]);
		}
	}


	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		Log.e("onKeyUp", "onKeyUpn code =" + keyCode);
//		switch (keyCode) {
//			case KeyEvent.KEYCODE_MENU:
//				return true;
//			case KeyEvent.KEYCODE_BACK:
//				addImageViewTest();
//				return true;
//			default:
//				break;
//		}
//		return false;
		return super.onKeyUp(keyCode, event);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub

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

	private void addShowFPS(){
		final GLTextView fps = new GLTextView(this);
		fps.setX(900);
		fps.setY(2200);
		fps.setLayoutParams(600, 100);
		fps.setFixed(true);
		fps.setBackground(new GLColor(0x000000, 0.5f));
		fps.setTextColor(new GLColor(0xffffff));
		fps.setTextSize(80);

		getRootView().addView(fps);

		new Thread(new Runnable() {
			long times = 0;
			int max = 0;
			int min = 60;

			@Override
			public void run() {
				getRootView().getFPS();
				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				while (true){
					final int f = getRootView().getFPS();
					if (f > 0 && f < 70){
						times++;
						if (times > 2) {
							max = Math.max(f, max);
							min = Math.min(f, min);
							getRootView().queueEvent(new Runnable() {
								@Override
								public void run() {
									String msg = "FPS : " + f;
									if (max > 0){
										msg +=  " [" + min + "~" + max + "]";
									}
									fps.setText(msg);
									Log.e("apptest", msg);
								}
							});
						}
					}
					try {
						Thread.sleep(500);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}

			}
		}).start();
	}

	
}
