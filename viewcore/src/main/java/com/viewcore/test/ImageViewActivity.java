package com.viewcore.test;

import android.annotation.SuppressLint;
import android.opengl.Matrix;
import android.os.Bundle;
import android.util.Log;

import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.player.GLSystemPlayer;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;

import java.util.Random;

public class ImageViewActivity extends BaseViewActivity {

	private GLRootView rootView;
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


		GLImageView[] imageViews = new GLImageView[1];
		Log.d("aaaaaaaaaaaa", "");


		Random random=new Random();
		for (int i = 0; i < imageViews.length; i++) {
			imageViews[i] = new GLImageView(this);
//			imageViews[i].setX(random.nextInt(2400));
//			imageViews[i].setY(random.nextInt(2400));
			imageViews[i].setX(800);
			imageViews[i].setY(800);
			imageViews[i].setLayoutParams(300, 300);
			imageViews[i].setImage(R.drawable.a2);
			imageViews[i].setAlpha(0.5f);
//			imageViews[i].setDepth(4 - (i - 100)*0.005f);
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



}
