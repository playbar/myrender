package com.viewcore.test;

import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLProcessCircleView;
import com.bfmj.viewcore.view.GLRelativeView;
import com.bfmj.viewcore.view.GLRootView;

import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;

public class LinTestActivity extends BaseViewActivity {
	GLRelativeView view3;
	int process = 0;
	boolean isProcess = false;
	GLRootView rootView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		/*
		getWindow().setFlags(
			    WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
			    WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);*/
		
		rootView = getRootView();
		rootView.onResume();
		
//		GLProcessCircleView circleView = new GLProcessCircleView(getApplicationContext());
//		circleView.setDirection(GLDirection.CLOCKWISE);
//		circleView.setAttribute(R.drawable.gl_process_circle_process, R.drawable.gl_process_circle_bg, 300);
//		circleView.setTextAttribute(36, null, true);
//		
//		GLSenceView sence1 = new GLSenceView(this);
//		sence1.setImage(R.drawable.gl_sence_play);
//		sence1.setObjFile("qiu.obj");
////		sence1.scale(0.4f);
//		sence1.scale(1.1f);
//		sence1.rotate(180f, 0, 0, 1);
//		sence1.rotate(180f, 0, 1, 0);
////		sence1.translate(6, 0, 0);
//		sence1.setVisible(true);
//		rootView.addView(sence1);
		
//		GLProcessCircleCanvasView canvasView = new GLProcessCircleCanvasView(getApplicationContext());
//		canvasView.setLayoutParams(300, 300);
//		canvasView.createView(0);
//		rootView.addView(canvasView);
		
//		rootView.addView(circleView.createView());
//		//circleView.setProcess(60);
//		setProcess(circleView);

		//setContentView(rootView);
		
		//GLRootView rootView = (GLRootView)findViewById(R.id.rootview);
		
//		GLImageView imageView = new GLImageView(this);
//		imageView.setBackground(new float[]{1.0f, 0.0f, 1.0f, 0.0f});
//		imageView.setImage(R.drawable.ic_launcher);
//		imageView.setLayoutParams(0, 0, 300, 160);
//		
//		GLImageView imageView2 = new GLImageView(this);
//		imageView.setBackground(new float[]{1.0f, 0.0f, 0.0f, 0.0f});
//		imageView2.setImage(R.drawable.ic_launcher);
//		imageView2.setLayoutParams(300, 200, 600, 400);
		
//		GLGroupView view = new GLGroupView(this);
//		
//		GLImageView imageView = new GLImageView(this);
//		//imageView.setBackground(new float[]{1.0f, 0.0f, 1.0f, 0.0f});
//		imageView.setImage(R.drawable.ic_launcher);
//		imageView.setLayoutParams(0, 0, 300, 160);
//		
//		GLImageView imageView2 = new GLImageView(this);
//		//imageView.setBackground(new float[]{1.0f, 0.0f, 1.0f, 0.0f});
//		imageView2.setImage(R.drawable.ic_launcher);
//		imageView2.setLayoutParams(300, 200, 600, 400);
//		
//		view.addView(imageView);
//		view.addView(imageView2);
		
//		GLLinearView view11 = new GLLinearView(this);
//		view11.setBackground(new GLColor(1.0f, 0.0f, 1.0f));
//		view11.setLayoutParams(800, 800);
//		//view11.setMargin(100, 100, 0, 0);
//		
//		GLRelativeView view12 = new GLRelativeView(this);
//		view12.setBackground(new GLColor(0.2f, 1.0f, 0.2f));
//		view12.setLayoutParams(700, 700);
//		view12.setId("1");
//		//view12.setMargin(30, 30, 0, 0);
//		//view12.setPadding(10, 10, 10, 10);
//		
//		GLRelativeView view13 = new GLRelativeView(this);
//		view13.setBackground(new GLColor(1.0f, 1.0f, 0.2f));
//		view13.setLayoutParams(400, 400);
//		view13.setId("2");
//		
//		GLRelativeView view14 = new GLRelativeView(this);
//		view14.setBackground(new GLColor(1.0f, 0.2f, 0.2f));
//		view14.setLayoutParams(200, 200);
//		view14.setId("3");
//		//view13.setMargin(30, 30, 0, 0);
//		//view13.setPadding(10, 10, 10, 10);
//		
//		view13.addView(view14);
//		view12.addView(view13);
//		view11.addView(view12);
		
//		GLLinearView view = new GLLinearView(this);
//		view.setBackground(new GLColor(1.0f, 0.0f, 1.0f));
//		view.setLayoutParams(800, 800);
		//view.setMargin(100, 100, 0, 0);
		
		
		//view.setPadding(10, 10, 0, 0);
		//view.setOrientation(GLOrientation.VERTICAL);
		/*
		for (int i = 0; i < 3; i++) {
			GLImageView imageView2 = new GLImageView(this);
			imageView2.setImage(R.drawable.ic_launcher);
			imageView2.setLayoutParams(0, 0, 100, 100);
			imageView2.setMargin(10, 0, 0, 0);
			
			GLRelativeView view2 = new GLRelativeView(this);
			view2.setBackground(new GLColor(0.2f, 1.0f, 0.2f));
			view2.setLayoutParams(200, 200);
			view2.setMargin(30, 30, 0, 0);
			view2.setPadding(10, 10, 10, 10);
			
			view2.addView(imageView2);
			view.addView(view2);
		}
		
		rootView.addView(view);*/
		
//		GLAbsoluteView absoluteView = new GLAbsoluteView(this);
//		absoluteView.setBackground(new GLColor(0.3f, 1.0f, 0.2f));
//		absoluteView.setLayoutParams(200, 200);
		
//		view3 = new GLRelativeView(this);
//		view3.setBackground(new GLColor(0.2f, 1.0f, 0.2f));
//		view3.setLayoutParams(200, 200);
//		//view3.setMargin(30, 30, 0, 0);
//		//view3.setPadding(10, 10, 10, 10);
//		
//		GLRelativeView view4 = new GLRelativeView(this);
//		view4.setBackground(new GLColor(1.0f, 1.0f, 0.2f));
//		view4.setLayoutParams(180, 180);
//		//view4.setMargin(30, 30, 0, 0);
//		//view4.setPadding(10, 10, 10, 10);
//		
//		view3.addView(view4);
		//view.addView(view3);
		
//		GLImageView imageView = new GLImageView(this);
//		imageView.setImage(R.drawable.ic_launcher);
//		imageView.setLayoutParams(0, 0, 100, 100);
//		//imageView.setMargin(10, 10, 10, 10);
//		
//		GLImageView imageView2 = new GLImageView(this);
//		imageView2.setImage(R.drawable.ic_launcher);
//		imageView2.setLayoutParams(0, 0, 100, 100);
//		imageView2.setMargin(10, 0, 0, 0);
//		
//		GLImageView imageView3 = new GLImageView(this);
//		imageView3.setImage(R.drawable.ic_launcher);
//		imageView3.setLayoutParams(0, 0, 100, 100);
//		
//		GLImageView imageView4 = new GLImageView(this);
//		imageView4.setImage(R.drawable.ic_launcher);
//		imageView4.setLayoutParams(0, 0, 100, 100);
//		imageView4.setMargin(30, 30, 0, 0);
		
		
		
//		final GLProcessView processView = new GLProcessView(this, GLProcessMode.CIRCLE);
//		processView.setLayoutParams(500, 500);
//		//processView.setBackground(new GLColor(0.0f, 0.0f, 0.0f));
//		processView.setMargin(20, 200, 0, 0);
////		processView.setDepth(4);
//		processView.setProcessColor(R.drawable.loading_icon);
//		processView.setProcessColor(R.drawable.p_bg);
//		//processView.setProcessColor(new GLColor(0.2f, 1.0f, 0.2f));
//		
//		setProcess(processView);
//		//processView.setProcess(10);
		
		
		
		//absoluteView.addView(imageView3, 20, 20);
		//absoluteView.addView(imageView4, 50, 50);
		
		//view.addView(imageView);
		
		//view.addView(imageView2);
		//view.addView(absoluteView);
		//view4.addView(imageView, GLAlign.BOTTOM);
		//view3.addView(view4);
		
		//view3.addViewOf(imageView2, null, null, null, imageView);
		
		//view.addView(view3);
//		view.addView(processView);
		
		
		//view3.addView(imageView, GLAlign.CENTER_VERTICAL);
		
//		GLSenceView senceView = new GLSenceView(this);
//		senceView.setImage(R.drawable.ballbg);
//		senceView.setObjFile("qiu.obj");
//		senceView.scale(50);
//		senceView.rotate(180, 0, 0, 1);
//		senceView.rotate(180, 0, 1, 0);

		//rootView.addView(senceView);
		//rootView.addView(view11);
		
		//view11.rotate(45, 0, 1, 0);
		//rootView.addView(view);
		//view.rotate2(-30, 0, 1.0f, 0);
		//view.rotate(-30, 0, 1.0f, 0);
		//view.scale(1.4f, 1.4f);
//		GLScaleAnimation animation = new GLScaleAnimation(1.4f,1.4f);
//		animation.setDuration(500);
//		animation.setAnimView(view);
//		view.startAnimation(animation);

		//view.removeView(imageView);
		//view.setMargin(120, 100, 10, 10);
		//view.setPadding(100, 100, 0, 0);
		//view3.setPadding(20, 20, 20, 20);
		//view3.removeView(imageView3);
		//imageView3.setMargin(50, 50, 0, 0);
		//absoluteView.removeView(imageView3);
		//absoluteView.addView(imageView3, 60, 0);
		//view3.setPadding(50, 50, 0, 0);
		//view3.setDepth(4.5f);
		//processView.setMargin(50, 50, 0, 0);
//		Log.d("test2", view4.getX() +"----"+ view4.getY());
	}
	
	int i = 0;
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		/*
		GLRelativeView view4 = new GLRelativeView(LinTestActivity.this);
		view4.setBackground(new float[]{0.2f, 0.2f, 0.2f, 1.0f});
		view4.setLayoutParams(100, 100);
		view4.setMargin(30+i*10, 30+i*10, 0, 0);
		
		view3.addView(view4);
		*/
		++i;
		return super.onTouchEvent(event);
	}
	
	private void setProcess(final GLProcessCircleView processView) {
		new Handler().postDelayed(new Runnable(){    
  		    public void run() {
  		    	
  		    	getRootView().queueEvent(new Runnable() {
					
					@Override
					public void run() {
						processView.setProcess(process);
					}
				});
  		    	
  		    	if (process == 100) {
  		    		return;
  		    	}
  		    	++process;
  		    	
  		    	if (isProcess) {
  		    		setProcess(processView);
  		    	}
  		    }    
  		}, 500);
	}

	@Override
	protected void onResume() {
		super.onResume();
		
		isProcess = true;
	}

	@Override
	protected void onPause() {
		rootView.onPause();
		super.onPause();
		
		isProcess = false;
	}
	
	
}
