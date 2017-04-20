package com.bfmj.viewcore.view;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.viewcore.util.GLExtraData;

import android.content.Context;

public abstract class GLViewPage {
	private Context mContext;
	private GLPageManager mPageManager;
	private GLRectView mView;
	
	public GLViewPage(Context context){
		mContext = context;
		onCreate();
	}
	
	protected Context getContext() {
		return mContext;
	}
	
	/**
	 * 设置当前的页面管理器
	 * @author lixianke  @Date 2015-6-19 下午7:17:47
	 * @param manager 当前的页面管理器 
	 * @return
	 */
	public void setPageManager(GLPageManager manager){
		mPageManager = manager;	
	}
	
	/**
	 * 生成当前页面的，返回根View
	 * @author lixianke  @Date 2015-6-19 下午7:18:41
	 * @param data 上一个页面传递过来的数据
	 * @return
	 */
	protected abstract GLRectView createView(GLExtraData data);
	
	/**
	 * 获取当前页面的根View
	 * @author lixianke  @Date 2015-6-19 下午7:22:11
	 * @param data 上一个页面传递过来的数据
	 * @return
	 */
	public GLRectView getView(GLExtraData data){
		if (mView == null){
			mView = createView(data);
		}
		return mView;
	}
	
	/**
	 * 页面创建的回调
	 * @author lixianke  @Date 2015-6-19 下午7:23:22
	 * @param
	 * @return
	 */
	public void onCreate(){
		
	}
	
	/**
	 * 页面切到前台的回调
	 * @author lixianke  @Date 2015-6-19 下午7:24:06
	 * description:{这里用一句话描述这个方法的作用}
	 * @param {引入参数名} {引入参数说明}  
	 * @return {返回值说明}
	 */
	public void onResume(){
		
	}
	
	/**
	 * 页面切到后台的回调
	 * @author lixianke  @Date 2015-6-19 下午7:24:34
	 * @param 
	 * @return
	 */
	public void onPause(){}
	
	/**
	 * 页面关闭的回调
	 * @author lixianke  @Date 2015-6-19 下午7:25:09
	 * @param 
	 * @return
	 */
	public void onFinish(){

	}
	
	/**
	 * 启动新页面
	 * @author lixianke  @Date 2015-6-19 下午7:25:42
	 * @param page 新页面
	 * @param data 携带的数据
	 * @return
	 */
	protected void startPage(GLViewPage page, GLExtraData data) {
		mPageManager.push(page, data);
	}
	
	/**
	 * 启动新页面
	 * @author lixianke  @Date 2015-6-19 下午7:26:04
	 * @param page 新页面 
	 * @return
	 */
	protected void startPage(GLViewPage page) {
		mPageManager.push(page, null);
	}
	
	/**
	 * 关闭当前页面
	 * @author lixianke  @Date 2015-6-19 下午7:26:28
	 * @param 
	 * @return
	 */
	public void finish(){
		mPageManager.pop(this);
		onFinish();
	}

	public boolean onKeyDown(int keyCode){return false;}

	public boolean onKeyUp(int keycode){return false;}
}
