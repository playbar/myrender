package com.viewcore.test;


import android.annotation.SuppressLint;
import android.media.MediaPlayer;
import android.opengl.Matrix;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;

import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.interfaces.IGLPlayerListener;
import com.bfmj.viewcore.player.GLSystemPlayer;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLPanoView;
import com.bfmj.viewcore.view.GLPlayerView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;

import java.io.IOException;

public class LxkTestActivity extends BaseViewActivity {
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
		textView.setTextColor(new GLColor(0.0f, 1.0f, 1.0f));
		textView.setText("北京欢迎你");
		textView.setTextSize(100);
		rootView.addView(textView);

		initLog();

		long time =  System.currentTimeMillis();

		for (int i = 0; i < 2000; i++){
			float[] mMVPMatrix = new float[16];
			float[] mVMatrix = new float[16];
			float[] mProjMatrix = new float[16];
			float[] currMatrix = new float[16];

//			Matrix.setIdentityM(mProjMatrix, 0);
//			Matrix.setIdentityM(mProjMatrix, 0);
//			Matrix.setIdentityM(mProjMatrix, 0);

//			Matrix.multiplyMM(mMVPMatrix, 0, mVMatrix, 0, currMatrix, 0);
//			Matrix.multiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mMVPMatrix, 0);
		}

		Log.d("testmmtime", "m1 total = " + (System.currentTimeMillis() - time));
		time =  System.currentTimeMillis();

		for (int i = 0; i < 2000; i++){
			float[] mVMatrix = new float[16];
			float[] mProjMatrix = new float[16];
			float[] currMatrix = new float[16];

//			Matrix.setIdentityM(mProjMatrix, 0);
//			Matrix.setIdentityM(mProjMatrix, 0);
//			Matrix.setIdentityM(mProjMatrix, 0);

			multiplay(multiplay(currMatrix, mVMatrix ), mProjMatrix);
		}

		Log.d("testmmtime", "m2 total = " + (System.currentTimeMillis() - time));

// 		GLImageView imageView = new GLImageView(this);
//		imageView.setImage(R.drawable.ic_launcher);
//		imageView.setLayoutParams(460, 460, 40, 40);
//
//		GLSenceView senceView = new GLSenceView(this);
//		senceView.setImage(R.drawable.k4_left3);
//		senceView.setObjFile("qiu.obj");
//		senceView.scale(4);
//		senceView.rotate(180, 0, 0, 1);
//		senceView.rotate(180, 0, 1, 0);
//		rootView.addView(senceView);
//
//		GLSenceView sence1 = new GLSenceView(this);
//		//sence1.setImage(R.drawable.gl_sence_half);
//		sence1.setObjFile("left.obj");
////		sence1.scale(0.4f);
//		sence1.rotate(-90, 0, 0, 1);
////		sence1.translate(6, 0, 0);
//		sence1.setVisible(true);
//		rootView.addView(sence1);
//		// 地面
//		GLSenceView sence2 = new GLSenceView(this);
//		//sence2.setImage(R.drawable.gl_purplehome_ground);
//		sence2.setObjFile("gl_purplehome_ground.obj");
////		sence2.rotate(180, 0, 1, 0);
//		sence2.setVisible(true);
//		rootView.addView(sence2);

		//////////

//		new Thread(new Runnable() {
//			@Override
//			public void run() {
//				try {
//					Thread.sleep(5000);
//				} catch (InterruptedException e) {
//					e.printStackTrace();
//				}
//
//				getRootView().queueEvent(new Runnable() {
//					@Override
//					public void run() {
//						GLPanoView videoView = GLPanoView.getSharedPanoView(LxkTestActivity.this);
//						videoView.reset();
//						videoView.setRenderType(GLPanoView.RENDER_TYPE_VIDEO);
//						videoView.setSceneType(GLPanoView.SCENE_TYPE_SPHERE);
//						videoView.setPlayType(GLPanoView.PLAY_TYPE_3D_LR);
//						player = new MediaPlayer();
//						try {
//							player.setDataSource("/mnt/sdcard/1.mp4");
//						} catch (IOException e) {
//							e.printStackTrace();
//						}
//						player.setSurface(new Surface(videoView.getSurfaceTexture()));
//						player.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
//							@Override
//							public void onPrepared(MediaPlayer mp) {
//								mp.start();
//							}
//						});
//						player.prepareAsync();
//					}
//				});
//			}
//		}).start();


//		player = new GLSystemPlayer(this);
//		player.setVideoPath("/mnt/sdcard/1.mp4");
//		player.setLayoutParams(960, 960);
//		player.setDepth(4);
////		player.set3D(true);
//		//player.rotate(90);
//		player.setListener(new IGLPlayerListener() {
//
//			@Override
//			public void onVideoSizeChanged(GLPlayerView player, int width, int height) {
//				// TODO Auto-generated method stub
//
//			}
//
//			@Override
//			public void onTimedText(GLPlayerView player, String text) {
//				// TODO Auto-generated method stub
//
//			}
//
//			@Override
//			public void onSeekComplete(GLPlayerView player) {
//				// TODO Auto-generated method stub
//
//			}
//
//			@Override
//			public void onPrepared(GLPlayerView player) {
//				player.start();
//			}
//
//			@Override
//			public boolean onInfo(GLPlayerView player, int what, Object extra) {
//				// TODO Auto-generated method stub
//				return false;
//			}
//
//			@Override
//			public boolean onError(GLPlayerView player, int what, int extra) {
//				// TODO Auto-generated method stub
//				return false;
//			}
//
//			@Override
//			public void onCompletion(GLPlayerView player) {
//				// TODO Auto-generated method stub
//
//			}
//
//			@Override
//			public void onBufferingUpdate(GLPlayerView player, int percent) {
//				// TODO Auto-generated method stub
//
//			}
//		});
//
//		rootView.addView(player);

//		GLSenceView senceView = new GLSenceView(this);
//		senceView.setImage(R.drawable.sence);
//		senceView.setObjFile("qiu.obj");
//		senceView.scale(4.0f);
//		senceView.rotate(180, 0, 0, 1);
//		senceView.rotate(180, 0, 1, 0);
//		rootView.addView(senceView);
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
