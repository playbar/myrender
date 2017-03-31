package com.bfmj.viewcore.view;

import java.lang.reflect.Method;

import com.baofeng.mojing.MojingSDK;
import com.baofeng.mojing.input.base.MojingKeyCode;

import android.app.Activity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.View;
import android.view.Window;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

public class BaseViewActivity extends Activity {
	private static BaseViewActivity instance;
	private RelativeLayout rootLayout;
	private GLRootView rootView;
	private GLPageManager mPageManager;
	private boolean isGroyEnable = true;
	private boolean isDistortionEnable = true;
	private boolean isLockViewAngle = false;
	private float mLockedAngle = 0f;
	private String mMojingType = "F79F2H-Q8ZNXN-2HQN2F-2ZA9YV-QG4H9H-QGAYAE";

	static {
		System.loadLibrary("viewcore");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);
		instance = this;

		if (!MojingSDK.GetInitSDK()) {
			MojingSDK.Init(this.getApplicationContext());
		}
		rootView = new GLRootView(this);
		rootView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
		
		rootView.setGlassesKey(mMojingType);
		
		if (isVirtualKey()) {
			rootView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION); 
		}
		
		mPageManager = new GLPageManager();
		mPageManager.setRootView(rootView);
		
		rootLayout = new RelativeLayout(this);
		rootLayout.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

		rootLayout.addView(rootView);
		setContentView(rootLayout);
	}

	public static void log(String msg){
		if (instance != null){
			instance.doLog(msg);
		}
	}

	private void doLog(String msg){
//		Log.d("BaseViewActivity", msg);
	}

	/**
	 * 
	 * @author linzanxian  @Date 2015年3月16日 下午2:41:34
	 * description:
	 * @return GLRootView
	 */
	public GLRootView getRootView() {
		return rootView;
	}
	
	public RelativeLayout getRootLayout(){
		return rootLayout;
	}
	
	public GLPageManager getPageManager() {
		return mPageManager;
	}
	
	/**
	 * 隐藏光标，具体由子类实
	 * @author lixianke  @Date 2015-5-14 下午2:14:23
	 */
	public void hideCursorView(){}
	
	/**
	 * 显示光标，具体由子类实
	 * @author lixianke  @Date 2015-5-14 下午2:14:23
	 */
	public void showCursorView(){}

	
	@Override
	protected void onResume() {
		if (rootView != null){
			rootView.startTracker();
		}
		
		if (mPageManager != null){
			mPageManager.onResume();
		}
		super.onResume();
	}

	@Override
	protected void onPause() {
		if (rootView != null){
			rootView.stopTracker();
		}
		
		if (mPageManager != null){
			mPageManager.onPause();
		}
		super.onPause();
	}
	
	@Override
	protected void onDestroy() {
		if (mPageManager != null){
			mPageManager.finish();
		}

		if (rootView != null){
			rootView.onDestroy();
		}
		super.onDestroy();
	}
	
	/**
	 * 判断是否有虚拟键
	 * @author linzanxian  @Date 2015年3月19日 上午10:37:23
	 * description:判断是否有虚拟键
	 * @return void
	 */
	private boolean isVirtualKey() {
		if (getScreen().equals(getDefaultScreen())) {
			return false;
		}
		
		return true;
	}
	
	/**
	 * 获取默认屏幕格式
	 * @author linzanxian  @Date 2015年3月19日 上午10:34:39
	 * description:获取默认屏幕格式
	 * @return String
	 */
	private String getDefaultScreen() {
		DisplayMetrics dm=new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm); 
        
        return dm.widthPixels+"*"+dm.heightPixels;
	}
	
	/**
	 * 获取可用屏幕格式
	 * @author linzanxian  @Date 2015年3月19日 上午10:35:59
	 * description:获取可用屏幕格式 
	 * @return String
	 */
	private String getScreen() {   
		String dpi=null;
        Display display = getWindowManager().getDefaultDisplay();
        DisplayMetrics dm = new DisplayMetrics();
        
        @SuppressWarnings("rawtypes")
        Class c;
        try {
            c = Class.forName("android.view.Display");
            @SuppressWarnings("unchecked")
            Method method = c.getMethod("getRealMetrics", DisplayMetrics.class);
            method.invoke(display, dm);
            dpi=dm.widthPixels+"*"+dm.heightPixels;
        }catch(Exception e){
            e.printStackTrace();
        }  
        
        return dpi;
    }

	public boolean isGroyEnable() {
		return isGroyEnable;
	}

	public void setGroyEnable(boolean isGroyEnable) {
		this.isGroyEnable = isGroyEnable;
		
		if (rootView != null){
			rootView.setGroyEnable(isGroyEnable);
		}
	}

	public boolean isDistortionEnable() {
		return isDistortionEnable;
	}

	public void setDistortionEnable(boolean isDistortionEnable) {
		this.isDistortionEnable = isDistortionEnable;
		
		if (rootView != null){
			rootView.setDistortionEnable(isDistortionEnable);
		}
	}

	public boolean isLockViewAngle() {
		return isLockViewAngle;
	}

	public void setLockViewAngle(boolean isLockViewAngle) {
		this.isLockViewAngle = isLockViewAngle;
		
		if (rootView != null){
			rootView.setLockViewAngle(isLockViewAngle);
		}
	}

	public float getLockedAngle() {
		return mLockedAngle;
	}

	public void setLockedAngle(float mLockedAngle) {
		this.mLockedAngle = mLockedAngle;
		
		if (rootView != null){
			rootView.setLockedAngle(mLockedAngle);
		}
	}

	public String getMojingType() {
		return mMojingType;
	}

	public void setMojingType(String mojingType) {
		this.mMojingType = mojingType;
	}
	
	public boolean onZKeyDown(final int keyCode) {		
		rootView.queueEvent(new Runnable() {
			
			@Override
			public void run() {
				boolean flag = rootView.onKeyDown(keyCode);
				if (!flag && mPageManager != null){
					GLViewPage page = mPageManager.getIndexView();
					if (page != null){
						flag = page.onKeyDown(keyCode);
					}
					if (!flag && keyCode == MojingKeyCode.KEYCODE_BACK
					 		&& mPageManager.hasMorePage()){
						mPageManager.pop();
						flag = true;
					}
				}
				if (!flag && keyCode == MojingKeyCode.KEYCODE_BACK) {
					finish();
				}
			}
		});
		return false;
	}

	public boolean onZKeyUp(final int keyCode) {
		rootView.queueEvent(new Runnable() {
			
			@Override
			public void run() {
				if (!rootView.onKeyUp(keyCode) && mPageManager != null){
					GLViewPage page = mPageManager.getIndexView();
					if (page != null){
						page.onKeyUp(keyCode);
					}
				};
			}
		});
		return false;
	}

	public boolean onZKeyLongPress(final int keyCode) {
		rootView.queueEvent(new Runnable() {
			
			@Override
			public void run() {
				rootView.onKeyLongPress(keyCode);
			}
		});
		return false;
	}
}
