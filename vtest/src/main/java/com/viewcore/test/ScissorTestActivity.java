package com.viewcore.test;


import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;

import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.player.GLSystemPlayer;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;

public class ScissorTestActivity extends BaseViewActivity {
	private GLRootView rootView;
	private DownloadIconView iconView;
//	MediaPlayer player;

	GLSystemPlayer player;


	@SuppressLint("SdCardPath")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.setDistortionEnable(false);
		super.onCreate(savedInstanceState);

		rootView = getRootView();
		//rootView.setDoubleScreen(false);
		rootView.onResume();

		//////////////
		GLViewFocusListener listener = new GLViewFocusListener() {

			@Override
			public void onFocusChange(GLRectView view, boolean focused) {
				if (focused){
					view.setBackground(R.drawable.a1);
				} else {
					view.setBackground(R.drawable.a2);
				}
			}
		};

		iconView = new DownloadIconView(this);
		iconView.setX(1000);
		iconView.setY(1000);
		iconView.setVisible(true);
		iconView.start();
		rootView.addView(iconView);


		GLImageView[] imageViews = new GLImageView[2];
		Log.d("aaaaaaaaaaaa", "");

//		GLImageView line = new GLImageView(this);
//		line.setX( 1198);
//		line.setY( 0);
//		line.setLayoutParams(4, 2400);
//		line.setBackground(new GLColor(0, 1, 0));
//		rootView.addView(line);



		for (int i = 0; i < imageViews.length; i++) {
			imageViews[i] = new GLImageView(this);
			imageViews[i].setX(100);
			imageViews[i].setY(100);
			imageViews[i].setLayoutParams(300, 300);
			imageViews[i].setImage(R.drawable.a2);
			imageViews[i].setDepth(4 - (i - 100)*0.005f);
			imageViews[i].setFocusListener(listener);
			rootView.addView(imageViews[i]);
		}

//		imageViews[0].setBackground(new GLColor(1, 0, 0));

		GLCursorView cursorView = new GLCursorView(this);
		cursorView.setX( 1190);
		cursorView.setY( 1190);
		cursorView.setLayoutParams(20, 20);
		cursorView.setBackground(new GLColor(1.0f, 0, 0));
		cursorView.setDepth(3);
		rootView.addView(cursorView);

		GLTextView textView = new GLTextView(this);
		textView.setX( 1000 );
		textView.setY( 1200);
		textView.setLayoutParams( 1000, 200 );
//		textView.setScissor( 1000, 1200, 1000, 200);
		textView.setTextColor(new GLColor(0.0f, 1.0f, 1.0f));
		textView.setText("北京欢迎你");
		textView.setTextSize(100);

		GLAnimation animation = new GLTranslateAnimation(500, 0, 0);
		animation.setAnimView(textView);
		animation.setDuration(2000);
		textView.startAnimation(animation);

		rootView.addView(textView);
		rootView.setDistortionEnable(true);
		textView.stopAnimation(animation);

//		initLog();

		long time =  System.currentTimeMillis();

//		for (int i = 0; i < 2000; i++){
//			float[] mMVPMatrix = new float[16];
//			float[] mVMatrix = new float[16];
//			float[] mProjMatrix = new float[16];
//			float[] currMatrix = new float[16];
//
////			Matrix.setIdentityM(mProjMatrix, 0);
////			Matrix.setIdentityM(mProjMatrix, 0);
////			Matrix.setIdentityM(mProjMatrix, 0);
//
////			Matrix.multiplyMM(mMVPMatrix, 0, mVMatrix, 0, currMatrix, 0);
////			Matrix.multiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mMVPMatrix, 0);
//		}

		Log.d("testmmtime", "m1 total = " + (System.currentTimeMillis() - time));
		time =  System.currentTimeMillis();


	}

	@Override
	protected void onPause(){
		super.onPause();
		iconView.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		iconView.onResume();
	}

	private void initLog(){
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

	private static float scaleX = 6.8f * 1.75f;
	private static float scaleY = 6.8f * 1.75f;

	private int getX(int x){
		return x;
	}

	private int getY(int y){
		return y;
	}

	private int getWidth(int w){
		return w;
	}

	private int getHeight(int h){
		return h;
	}

	public float[] multiplay(float[] a, float[] b){
		float[] d = new float[16];
		int i = 0;
		do {
			d[i * 4] = a[i * 4] * b[0] + a[i * 4 + 1] * b[4] + a[i * 4 + 2] * b[8] + a[i * 4 + 3] * b[12];
			d[i * 4 + 1] = a[i * 4] * b[1] + a[i * 4 + 1] * b[5] + a[i * 4 + 2] * b[9] + a[i * 4 + 3] * b[13];
			d[i * 4 + 2] = a[i * 4] * b[2] + a[i * 4 + 1] * b[6] + a[i * 4 + 2] * b[10] + a[i * 4 + 3] * b[14];
			d[i * 4 + 3] = a[i * 4] * b[3] + a[i * 4 + 1] * b[7] + a[i * 4 + 2] * b[11] + a[i * 4 + 3] * b[15];
		} while ((++i) < 4);

		return d;
	}

}
