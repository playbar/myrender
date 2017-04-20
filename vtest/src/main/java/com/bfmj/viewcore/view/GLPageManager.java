package com.bfmj.viewcore.view;

import java.util.Stack;

import com.bfmj.viewcore.util.GLExtraData;
import com.bfmj.viewcore.util.GLFocusUtils;

/**
 * 
 * ClassName: GLPageManager <br/>
 * @author lixianke    
 * @date: 2015-3-26 下午4:25:26 <br/>  
 * description:
 */
public class GLPageManager {
	private Stack<GLViewPage> mPageStack = new Stack<GLViewPage>();
	private GLRectView mCurrentView;
	private GLRootView mRootView;
	
	/**
	 * 设置当前的GLRootView，由系统自动调用
	 * @author lixianke  @Date 2015-6-19 下午5:16:18
	 * @param rootView 当前的GLRootView对象
	 * @return
	 */
	public void setRootView(GLRootView rootView){
		mRootView = rootView;
	}
	
	/**
	 * 打开指定的页面
	 * @author lixianke  @Date 2015-6-19 下午5:26:11
	 * @param page GLViewPage页面
	 * @param data 打开页面携带的数据
	 * @return
	 */
	public void push(GLViewPage page, GLExtraData data) {
		if (page == null){
			return;
		}

		onPause();

		page.setPageManager(this);
		page.getView(data);
		mPageStack.push(page);
		
		updateCrurrentView(false);
	}
	
	/**
	 * 删除指定的页面
	 * @author lixianke  @Date 2015-6-19 下午5:25:20
	 * @param  viewPage GLViewPage页面
	 * @return
	 */
	public void pop(GLViewPage viewPage) {
		if (viewPage == null){
			return;
		}
		
		if (mPageStack.contains(viewPage)){
			viewPage.onPause();
			mPageStack.remove(viewPage);
			GLRectView view = viewPage.getView(null);
			view.release();
		}
		
		updateCrurrentView(true);
	}
	
	/**
	 * 弹出栈顶的页面，返回上一页
	 * @author lixianke  @Date 2015-6-19 下午5:24:11
	 * @param 
	 * @return
	 */
	public void pop() {
		if (mPageStack.size() > 1){
			onPause();
			GLViewPage page = mPageStack.lastElement();
			page.onFinish();
			mPageStack.pop();
			GLRectView view = page.getView(null);
			view.release();
			updateCrurrentView(true);
		}
	}

	/**
	 * 释放所有页面
	 */
	public void finish(){
		for (GLViewPage page : mPageStack){
			page.onFinish();
			GLRectView view = page.getView(null);
			view.release();
		}
		mPageStack.clear();
	}

	/**
	 * 获取页面栈里的页面数量是否大于1
	 * @author lixianke  @Date 2015-6-19 下午5:22:34
	 * @param
	 * @return 页面数量是否大于1
	 */
	public boolean hasMorePage(){
		return mPageStack.size() > 1;
	}
	
	/**
	 * 当前页面切换到前台
	 * @author lixianke  @Date 2015-6-19 下午5:22:04
	 * @param 
	 * @return
	 */
	public void onResume(){
		if (mPageStack.size() == 0){
			return;
		}

		GLViewPage page = mPageStack.lastElement();
		if (page != null){
			page.onResume();
		}
	}
	
	/**
	 * 当前页面即将切换到后台
	 * @author lixianke  @Date 2015-6-19 下午5:21:04
	 * @param
	 * @return
	 */
	public void onPause(){
		if (mPageStack.size() == 0){
			return;
		}
		
		GLViewPage page = mPageStack.lastElement();
		if (page != null){
			page.onPause();
		}
	}
	
	private void updateCrurrentView(boolean isPop){
		if (mPageStack.size() == 0){
			return;
		}
		
		GLViewPage page = mPageStack.lastElement();
		
		if (page != null){
			GLRectView view = page.getView(null);
			
			if (view != null && mCurrentView != view){
				if (mCurrentView != null){
					mRootView.removeView(mCurrentView);
					GLFocusUtils.lostAllViewFocus();
				}
				mRootView.addView(view, true);
//				if (!view.isFocused()){
//					view.doRequestFocus();
//				}
				mCurrentView = view;

				if (mPageStack.size() > 1 || isPop){
					onResume();
				}
			}
		}
	}
	
	/**
	 * 返回栈顶view, 判断当前在哪个view
	 * @author linzanxian  @Date 2015-5-28 下午8:19:57
	 * @return 栈顶的GLViewPage
	 */
	public GLViewPage getIndexView() {
		if (mPageStack.size() == 0){
			return null;
		}

		GLViewPage page = mPageStack.lastElement();
		return page;
	}
}
