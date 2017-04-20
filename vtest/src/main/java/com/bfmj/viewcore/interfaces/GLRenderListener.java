package com.bfmj.viewcore.interfaces;

/**
 * 
 * ClassName: GLRenderListener <br/>
 * @author lixianke    
 * @date: 2015-3-9 下午1:53:18 <br/>  
 * description:
 */
public interface GLRenderListener {
	void onBeforeDraw(boolean isLeft);
	void onAfterDraw(boolean isLeft);
	void onSurfaceCreated();
	void onSurfaceChanged(int width, int height);
}
