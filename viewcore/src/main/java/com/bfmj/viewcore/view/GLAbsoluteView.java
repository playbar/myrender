package com.bfmj.viewcore.view;

import android.content.Context;

/**
 * AbsoluteLayout
 * ClassName: GLAbsoluteView <br/>
 * @author linzanxian    
 * @date: 2015年3月9日 上午10:45:54 <br/>  
 * description:
 */
public class GLAbsoluteView extends GLGroupView {

	public GLAbsoluteView(Context context) {
		super(context);
	}

	/**
	 * 设置内边距
	 * @author linzanxian  @Date 2015年3月13日 下午2:56:29
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下  
	 * @return void
	 */
	@Override
	public void setPadding(float left, float top, float right, float bottom) {
		if (mChildView != null) {
			for (GLRectView childView : mChildView) {
				setChildViewPadding(childView, left, top, right, bottom);
			}
		}
		
		super.setPadding(left, top, right, bottom);
	}

	/**
	 * 设置外边距
	 * @author linzanxian  @Date 2015年3月13日 下午2:56:33
	 * @param left 左  
	 * @param top 上
	 * @param right 右  
	 * @param bottom 下  
	 * @return void
	 */
	@Override
	public void setMargin(float left, float top, float right, float bottom) {
		setViewMargin(left, top, right, bottom);
		
		super.setMargin(left, top, right, bottom);
	}
	
	/**
	 * 添加view, 坐标相对于父坐标，左上角为原点(0,0)
	 * @author linzanxian  @Date 2015年3月13日 下午3:00:30
	 * @param view GLRectView 
	 * @param x x坐标
	 * @param y y坐标
	 * @return void
	 */
	public void addView(GLRectView view, float x, float y) {
		view.setMargin(x, y, 0, 0);
		view.setX(getX() + getPaddingLeft() + x);
		view.setY(getY() + getPaddingTop() + y);
		
		super.addView(view);
	}

	/**
	 * 添加view, 坐标相对于父坐标，左上角为原点(0,0)
	 * @author linzanxian  @Date 2015年3月13日 下午3:00:30
	 * @param view GLRectView 
	 * @param x x坐标
	 * @param y y坐标
	 * @return void
	 */
	@Override
	public void addView(GLRectView view) {
		view.setX(getX() + getPaddingLeft() + view.getX());
		view.setY(getY() + getPaddingTop() + view.getY());
		
		super.addView(view);
	}
}
