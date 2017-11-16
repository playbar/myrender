package com.viewcore.test;

import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLAnimation.OnGLAnimationListener;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLLinearView;
import com.bfmj.viewcore.view.GLRootView;

import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

public class ZhangActivity extends BaseViewActivity {
	private GLRootView rootView =null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);	
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		super.onCreate(savedInstanceState);
		//GLRootView rootView = (GLRootView)findViewById(R.id.rootview);
		
		rootView = getRootView();
		//setContentView(rootView);
		
	//	rootView.onResume();
		
		final GLImageView imageView2 = new GLImageView(this);
		imageView2.setBackground(new GLColor(1.0f, 0.0f, 0.0f));
		imageView2.setImage(R.drawable.ic_launcher);
		imageView2.setLayoutParams(200, 200);

		GLImageView imageView = new GLImageView(this);
		imageView.setBackground(new GLColor(0.0f, 1.0f, 0.0f));
		imageView.setImage(R.drawable.ic_launcher);
		imageView.setLayoutParams(200, 200);
		imageView.setMargin(200, 0,0,0);
		
//		GLSenceView senceView = new GLSenceView(this);
//		senceView.setImage(R.drawable.ballbg);
//		senceView.setObjFile("qiu.obj");
//		senceView.scale(50);
//		senceView.rotate(180, 0, 0, 1);
//		senceView.rotate(180, 0, 1, 0);
		
		final GLLinearView gview = new GLLinearView(this);
		gview.setX(100);
		gview.setY(100);
		gview.setDepth(3);
		gview.addView(imageView2);
		gview.addView(imageView);
		rootView.addView(gview);
		
		//rootView.addView(senceView);
		
		Log.d("com/bfmj/viewcore/test", "transSTART:"+gview.getDepth());
		GLTranslateAnimation animation = new GLTranslateAnimation(0,0,150);
		animation.setDuration(500);
		animation.setAnimView(gview);
		//animation.setOnlyChids(true);
		gview.startAnimation(animation);
		
		animation.setOnGLAnimationListener(new OnGLAnimationListener() {
			
			@Override
			public void onAnimationStart(GLAnimation animation) {
			}
			@Override
			public void onAnimationEnd(GLAnimation animation) {
				
				Log.d("com/bfmj/viewcore/test", "transEND:"+gview.getDepth());
			}
		});
		
		
//		
//		GLTranslateAnimation animation2 = new GLTranslateAnimation(-300,0,0);
//		animation2.setDuration(2000);
//		imageView.startAnimation(animation2);
		
//		GLScaleAnimation animation2 = new GLScaleAnimation(1.5f,1.5f);
//		animation2.setDuration(2000);
//		animation2.setAnimView(imageView2);
//		imageView2.startAnimation(animation2);
//		animation2.setOnGLAnimationListener(new OnGLAnimationListener() {
//			
//			@Override
//			public void onAnimationStart(GLAnimation animation) {
//				// TODO Auto-generated method stub
//				
//			}
//			
//			@Override
//			public void onAnimationEnd(GLAnimation animation) {
//				GLScaleAnimation animation3 = new GLScaleAnimation(1f,1f);
//				animation3.setDuration(2000);
//				animation3.setAnimView(imageView2);
//				imageView2.startAnimation(animation3);
//				
//			}
//		});
		
		//Matrix.rotateM(rotateMatrix, 0, 90, 0, 0.0001f, 0);
		
		
	}


}
