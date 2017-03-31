package com.bfmj.viewcore.view;

import com.bfmj.viewcore.render.GLConstant.GLOrientation;
import com.bfmj.viewcore.render.GLConstant.GLAlign;

import android.content.Context;

/**
 * LinearLayout
 * ClassName: GLLinearView <br/>
 * @author linzanxian    
 * @date: 2015年3月9日 上午10:45:18 <br/>  
 * description:线性布局
 */
public class GLLinearView extends GLGroupView {
	private GLOrientation mOrientation = GLOrientation.HORIZONTAL; //默认模向
	
	public GLLinearView(Context context) {
		super(context);
	}

	/**
	 * 设置外边距
	 * @author linzanxian  @Date 2015年3月10日 下午5:25:39
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
	 * 设置内边距
	 * @author linzanxian  @Date 2015年3月10日 下午5:41:50
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
	 * 添加view, 对齐方式只接收 (CENTER， CENTER_HORIZONTAL， CENTER_VERTICAL)
	 * @author linzanxian  @Date 2015年3月12日 下午2:10:09
	 * @param view GLRectView 视图
	 * @param align GLAlign 对齐方式
	 * @return void
	 */
	public void addView(GLRectView view, GLAlign align) {
		if (align == GLAlign.CENTER) {
			addViewCenter(view);
		} else if (align == GLAlign.CENTER_HORIZONTAL) {
			addViewCenterHorizontal(view);
		} else if (align == GLAlign.CENTER_VERTICAL) {
			addViewCenterVertical(view);
		} else {
			addView(view);
		}
	}
	
	/**
	 * 添加View到Center
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenter(GLRectView view) {
		view.setAlign(GLAlign.CENTER);
		view.setX(getX() + (getWidth() - view.getWidth() - getPaddingLeft() - getPaddingRight())/2 + getPaddingLeft());
		view.setY(getY() + (getHeight() - view.getHeight() - getPaddingTop() - getPaddingBottom())/2 + getPaddingTop());

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到CenterHorizontal
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenterHorizontal(GLRectView view) {
		view.setAlign(GLAlign.CENTER_HORIZONTAL);
		view.setX(getX() + (getWidth() - view.getWidth() - getPaddingLeft() - getPaddingRight())/2 + getPaddingLeft());
		
		if (mOrientation.equals(GLOrientation.HORIZONTAL)) { //横排
			view.setY(getY() + getPaddingTop() + view.getMarginTop());
		} else {
			float maxY = 0;
			int size = mChildView.size();
			if (size == 0) {
				view.setY(getY() + getPaddingTop() + view.getMarginTop());
			} else {
				for (int i = 0; i < size; i++) {
					GLRectView childView = mChildView.get(i);
					float childY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
					if (maxY < childY) {
						maxY = childY;
					}
				}
				view.setY(maxY + view.getMarginTop());
			}
		}

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加View到CenterVertical
	 * @author linzanxian  @Date 2015年3月11日 下午5:01:09
	 * @param view GLRectView
	 * @return void
	 */
	public void addViewCenterVertical(GLRectView view) {
		view.setAlign(GLAlign.CENTER_VERTICAL);
		view.setY(getY() + (getHeight() - view.getHeight() - getPaddingTop() - getPaddingBottom())/2 + getPaddingTop());
		
		if (mOrientation.equals(GLOrientation.HORIZONTAL)) { //横排
			float maxX = 0;
			int size = mChildView.size();
			if (size == 0) {
				view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
			} else {
				for (int i = 0; i < size; i++) {
					GLRectView childView = mChildView.get(i);
					float childX = childView.getX() + childView.getWidth() + childView.getMarginRight();
					if (maxX < childX) {
						maxX = childX;
					}
				}
				view.setX(maxX + view.getMarginLeft());
			}
		} else {
			view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
		}

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 添加view
	 * @author linzanxian  @Date 2015年3月10日 下午5:05:27
	 * @param view GLRectView
	 * @return void
	 */
	@Override
	public void addView(GLRectView view) {
		//设置父级
		view.setAlign(GLAlign.DEFAULT);
		
		int size = mChildView.size();
		if (size == 0) {
			view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
			view.setY(getY() + getPaddingTop() + view.getMarginTop());
		} else {
			GLRectView childView = mChildView.get(0);
			float maxX = childView.getX() + childView.getWidth() + childView.getMarginRight();
			float maxY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
			
			for (int i = 0; i < size; i++) {
				childView = mChildView.get(i);
				float childX = childView.getX() + childView.getWidth() + childView.getMarginRight();
				float childY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
				if (maxX < childX) {
					maxX = childX;
				}
				if (maxY < childY) {
					maxY = childY;
				}
			}
			
			if (mOrientation.equals(GLOrientation.HORIZONTAL)) { //横排
				view.setX(maxX + view.getMarginLeft());
				view.setY(getY() + getPaddingTop() + view.getMarginTop());
			} else {
				view.setX(getX() + getPaddingLeft() + view.getMarginLeft());
				view.setY(maxY + view.getMarginTop());
			}
		}

		//resetChildView(view);
		super.addView(view);
	}
	
	/**
	 * 删除view
	 * @author linzanxian  @Date 2015年3月10日 下午5:26:55
	 * @param view GLRectView
	 * @return void
	 */
	@Override
	public void removeView(GLRectView view) {
		int size = mChildView.size();
		if (size > 0) {
			boolean isChange = false;
			float maxX = 0;
			float maxY = 0;
			GLRectView prevView = null; //上一个view
			for (int i = 0; i < size; i++) {
				GLRectView childView = mChildView.get(i); //当前view
				if (view.equals(childView)) {
					if (i > 0) {
						prevView = mChildView.get(i - 1);
						maxX = prevView.getX() + prevView.getWidth() + prevView.getMarginRight();
						maxY = prevView.getY() + prevView.getHeight() + prevView.getMarginBottom();
					}
					isChange = true;
					continue;
				}
				if (!isChange) {
					continue;
				}
				
				//第一个
				if (prevView == null) {
					if (childView.getAlign() == null
							|| childView.getAlign() == GLAlign.DEFAULT) {
						childView.setX(getX() + getPaddingLeft() + childView.getMarginLeft());
						childView.setY(getY() + getPaddingTop() + childView.getMarginTop());
					} else if(childView.getAlign() == GLAlign.CENTER_HORIZONTAL) {
						childView.setY(getY() + getPaddingTop() + childView.getMarginTop());
					} else if(childView.getAlign() == GLAlign.CENTER_VERTICAL) {
						childView.setX(getX() + getPaddingLeft() + childView.getMarginLeft());
					}

					maxX = childView.getX() + childView.getWidth() + childView.getMarginRight();
					maxY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
				} else {
					if (mOrientation.equals(GLOrientation.HORIZONTAL)) { //横排
						if (childView.getAlign() == null
								|| childView.getAlign() == GLAlign.DEFAULT) {
							childView.setX(maxX + childView.getMarginLeft());
							childView.setY(getY() + getPaddingTop() + childView.getMarginTop());
						} else if(childView.getAlign() == GLAlign.CENTER_HORIZONTAL) {
							childView.setY(getY() + getPaddingTop() + childView.getMarginTop());
						} else if(childView.getAlign() == GLAlign.CENTER_VERTICAL) {
							childView.setX(maxX + childView.getMarginLeft());
						}
						
						if (maxX < (childView.getX() + childView.getWidth() + childView.getMarginRight())) {
							maxX = childView.getX() + childView.getWidth() + childView.getMarginRight();
						}
					} else {
						if (childView.getAlign() == null
								|| childView.getAlign() == GLAlign.DEFAULT) {
							childView.setX(getX() + getPaddingLeft() + childView.getMarginLeft());
							childView.setY(maxY + childView.getMarginTop());
						} else if(childView.getAlign() == GLAlign.CENTER_HORIZONTAL) {
							childView.setY(maxY + childView.getMarginTop());
						} else if(childView.getAlign() == GLAlign.CENTER_VERTICAL) {
							childView.setX(getX() + getPaddingLeft() + childView.getMarginLeft());
						}
						
						if (maxY < (childView.getY() + childView.getHeight() + childView.getMarginBottom())) {
							maxY = childView.getY() + childView.getHeight() + childView.getMarginBottom();
						}
					}
				}
				
				//resetChildView(childView);
				
				prevView = childView;
			}
		}
		
		super.removeView(view);
	}
	
	/**
	 * 设置布局方向
	 * @author linzanxian  @Date 2015年3月10日 下午6:01:02
	 * @param orientation 方向
	 * @return void
	 */
	public void setOrientation(GLOrientation orientation) {
		mOrientation = orientation;
	}
	
	/**
	 * 获取布局方向
	 * @author linzanxian  @Date 2015年3月10日 下午5:36:47
	 * @return String
	 */
	public GLOrientation getOrientation() {
		return mOrientation;
	}
}
