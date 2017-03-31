package com.bfmj.viewcore.view;

import java.util.ArrayList;
import java.util.List;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.util.GLFocusUtils;
import com.bfmj.viewcore.adapter.GLListAdapter;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;

import android.content.Context;
import android.util.Log;


public class GLListArcView extends GLAdapterView<GLListAdapter>{
	/**
	 * 排列顺序 按行排序
	 */
	public static final int VERTICAL=0x01;
	/**
	 * 排列顺序 按列排序
	 */
	public static final int HORIZONTAL=0x02;
	
	
	private ArrayList<GLView> mList;//数据集
	private int mStartIndex=0;//当前开始的索引
	private int mSelectedIndex=0;//当前选中的索引
	private int mPreSelectedIndex=-1;//上一次选中的索引
	private int mFocusIndex = -1;//获取焦点的索引，这个是内部索引，范围在一屏显示的数内
	private int mFocusPreIndex = -1;
	private float mItemSpacing=0.0f;//项间距离
	private GLListAdapter mGLListAdapter;
	private int mNumOneScreen=5;
	private int mTotalCount = 0;
	private int mOrderType=0x02;
	private OnItemSelectedListener mSelectedListener;
	private int mMoveStepWidth=0;
	private List<GLRectView> mGLRectViewList;
	private int mGLRectViewListNum;
	private GLRectView convertView;
	private int mMoveDuration=200;//动画时间
	private Context mContext;
	private float mDefaultDepth = 0.0f;
	private OnItemSelectedListener mOnItemSelectedListener;
	private OnItemClickListener mOnItemClickListener;
	private boolean isMoveRight = false;
	private GLOnKeyListener outViewKeyListener;
	private boolean isAnimationFinished = true;
	private int angle=0;
	
	public void setIsAnimationFinished(boolean isf){
		isAnimationFinished = isf;
	}
	
	/**
	 * 设置移动动画时间
	 * @param duration 动画时间（毫秒）
	 */
	public void setMoveDuration(int duration){
		this.mMoveDuration = duration;
	}
	
	/**
	 * 设置排列类型
	 * @param orderType 排列方向 GLListView.VERTICAL按行排列   GLListView.HORIZONTAL按列排列
	 */
	public void setOrderType(int orderType){
		this.mOrderType = orderType;
		
	}
	
	/**
	 * 设置水平间矩
	 * @param horizontalSpacing 水平间矩
	 */
	public void setItemSpacing(float itemSpacing){
		this.mItemSpacing = itemSpacing;
	}	
	
	/**
	 * 设置一屏显示几条数据
	 * @param oneScreen
	 */
	public void setNumOneScreen(int oneScreen){
		this.mNumOneScreen = oneScreen;
		this.mGLRectViewListNum = this.mNumOneScreen + 2;
		mGLRectViewList = new ArrayList<GLRectView>();
	}
	
	/**
	 * 设置当前第一个显示的索引
	 * @param index 开始的索引，从0开始算
	 */
	public void setStartIndex(int index){
		this.mStartIndex = index;
	}
	
	/**
	 * 获取当前的索引
	 * @return 当前索引号
	 */
	public int getStartIndex(){
		return this.mStartIndex;
	}

	/*
	 * 构造方法
	 * @param context 上下文
	 * @param orderType 排列方式
	 * */
	public GLListArcView(Context context, int orderType) {
		super(context);
		mContext = context;
		// TODO Auto-generated constructor stub
		this.mOrderType = orderType;
		initGLListView();
	}
	
	/**
	 * 初始化ListView
	 */
	private void initGLListView(){
		mGLRectViewList = new ArrayList<GLRectView>(); //new GLRectView[this.mNumOneScreen + 2];
		/**
		 * 设置焦点事件
		 */

		
		this.setFocusListener(new GLViewFocusListener() {
			@Override
			public void onFocusChange(GLRectView view, boolean focused) {
				
				if (focused){
					if(mOnListViewFocusChange!=null)
						mOnListViewFocusChange.focusChange(view, true);
				} else {
					mPreSelectedIndex = -1;
					if(mOnListViewFocusChange!=null)
						mOnListViewFocusChange.focusChange(view, false);
				}
			}
		});
		
//		this.setBackground(new GLColor(1.0f, 1.0f, 1.0f));
		
	}
	
	private boolean isOpenHeadControl(){
		return GLFocusUtils.isOpenHeadControl;
	}


	@Override
	public GLListAdapter getGLAdapter() {
		// TODO Auto-generated method stub
		if(this.mGLListAdapter != null) {
			return this.mGLListAdapter;
		}
		return null;
	}
	/*
	 * 重置参数
	 */
	public void resetParams(){
		this.mFocusIndex = -1;
		this.mFocusPreIndex = -1;
		this.mStartIndex = 0;
		this.mSelectedIndex=0;
	}

	@Override
	public void setAdapter(GLListAdapter adapter) {
		// TODO Auto-generated method stub
		mGLListAdapter = adapter;
		this.mTotalCount = adapter.getCount();
		mOnItemSelectedListener = getOnItemSelectedListener();
		mOnItemClickListener = getOnItemClickListener();

		if(this.mChildView.size() > 0){
			removeAllGLViews();			
		}
		resetParams();
		
		float mX = GLListArcView.this.getX();
		for(int j=0;j<this.mNumOneScreen;j++){
			
			if(j >this.mTotalCount-1) break;
			final GLRectView _v = getAdapterView(j,j, null, GLListArcView.this, true);
			if(_v == null) continue;

			mX += _v.getMarginLeft();
			if(this.mOrderType == this.HORIZONTAL){
				_v.setX(mX);
				_v.setY(GLListArcView.this.getY() + _v.getMarginTop());
				
				mX += _v.getWidth() + _v.getMarginRight() + this.mItemSpacing;
			}
			else{
				_v.setX(GLListArcView.this.getX() + _v.getMarginRight()); 
				_v.setY(_v.getMarginTop() + (_v.getHeight() + this.mItemSpacing + _v.getMarginTop() + _v.getMarginBottom())*j);
			}
			mDefaultDepth = _v.getDepth();
			
			if (angle!=0) {
				int t1 = mNumOneScreen%2;
				int t2 = mNumOneScreen/2; //奇数时，t2为中心view
				
				if (t1==0) { //偶数
					_v.setX(-_v.getWidth() / 2 + 480);
					_v.setLookAngle((j - (t2 - 0.5f)) * -angle);
					
				} else {
					_v.setX(-_v.getWidth() / 2 + 480);
					_v.setLookAngle((j - t2) * -angle);
				}
			}
			
			(((BaseViewActivity) getContext()).getRootView()).queueEvent(new Runnable() {
				@Override
				public void run() {
					GLListArcView.this.addView(_v);
				}
			});
		}
		
		for(int k=0;k<GLListArcView.this.getChildView().size();k++){
			GLRectView _view1 = GLListArcView.this.getView(k);
			if(_view1.getDepth() > GLListArcView.this.getDepth()){
				_view1.setDepth(GLListArcView.this.getDepth());
			}
		}
		
		if(!isOpenHeadControl()){
			GLListArcView.this.setOnKeyListener(new GLOnKeyListener() {
				
				@Override
				public boolean onKeyUp(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					return false;
				}
				
				@Override
				public boolean onKeyLongPress(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					return false;
				}
				
				@Override
				public boolean onKeyDown(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					if(keycode == MojingKeyCode.KEYCODE_DPAD_RIGHT){
						//回调控件使用者注册的事件监听对象
						if (outViewKeyListener!=null) {
							if(outViewKeyListener.onKeyDown(view, keycode)){
								return true;
							}
						}

//						if(mSelectedIndex >= mTotalCount-1) return true;
						if(!isAnimationFinished) return true;
						isMoveRight = true;
						moveLeft();	
						return true;
					}
					else if(keycode == MojingKeyCode.KEYCODE_DPAD_LEFT){
						//回调控件使用者注册的事件监听对象
						if (outViewKeyListener!=null) {
							if(outViewKeyListener.onKeyDown(view, keycode)){
								return true;
							}
						}
//						if(mSelectedIndex <= 0) return true;
						if(!isAnimationFinished) return true;
						isMoveRight = false;
						moveRight();
										
						return true;
					}
					else if(keycode == MojingKeyCode.KEYCODE_ENTER){
						//回调控件使用者注册的事件监听对象
						if (outViewKeyListener!=null) {
							if(outViewKeyListener.onKeyDown(view, keycode)){
								return true;
							}
						}
						if(mOnItemClickListener!=null){						
							mOnItemClickListener.onItemClick(GLListArcView.this, GLListArcView.this.getView(mFocusIndex), mFocusIndex, mSelectedIndex);
						}
						
						return true;
					}
					
					if (mOrderType == HORIZONTAL && (keycode == MojingKeyCode.KEYCODE_DPAD_DOWN || keycode == MojingKeyCode.KEYCODE_DPAD_UP)) {
						if(mFocusIndex >= 0 && mFocusIndex < mTotalCount) {
							mSelectedIndex = mStartIndex + mFocusIndex;

							//回调控件使用者注册的事件监听对象
							if (outViewKeyListener!=null) {
								if(outViewKeyListener.onKeyDown(view, keycode)){
									return true;
								}
							}

							//回调失去焦点事件
							if(mOnItemSelectedListener!=null){
								mOnItemSelectedListener.onNothingSelected(null, GLListArcView.this.getView(mFocusIndex), mFocusIndex, mStartIndex+mFocusIndex);
							}
						}
						
					}
					return false;
				}
			});	
		}
		else{
			this.setFocusListener(new GLViewFocusListener() {
				
				@Override
				public void onFocusChange(GLRectView view, boolean focused) {
					// TODO Auto-generated method stub
					
				}
			});
		}
	}
	
	private GLRectView getAdapterView(final int location, final int position, GLRectView view, GLGroupView parentView, boolean isNew){
		
		GLRectView childview = view;
		if(isNew){
			childview = mGLListAdapter.getGLView(position, view, parentView);
		}
		
		final GLRectView _childview = childview;
		
		if(_childview!= null && isOpenHeadControl()){//
//			Log.d("itemfocus","listview-position:" + position);
			_childview.setFocusListener(new GLViewFocusListener() {
				
				@Override
				public void onFocusChange(GLRectView view, boolean focused) {
					// TODO Auto-generated method stub
					if(focused){
						mOnItemSelectedListener.onItemSelected(GLListArcView.this, view, location, mStartIndex + location);
					}
					else{
						mOnItemSelectedListener.onNothingSelected(GLListArcView.this, view, location, mStartIndex + location);
					}
				}
			});
			
			_childview.setOnKeyListener(new GLOnKeyListener() {
				
				@Override
				public boolean onKeyUp(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					return false;
				}
				
				@Override
				public boolean onKeyLongPress(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					return false;
				}
				
				@Override
				public boolean onKeyDown(GLRectView view, int keycode) {
					// TODO Auto-generated method stub
					if(keycode == MojingKeyCode.KEYCODE_ENTER){
//						downloadHandle(mCurrClickIndex, mCurrClickView);
						if(mOnItemClickListener!=null){						
							mOnItemClickListener.onItemClick(GLListArcView.this, view, location, mStartIndex + location);
						}
						return true;
					}
					return false;
				}
			});

		}
		return _childview;
	}
	
	
	/**
	 * 焦点往右移动，列表往左移动
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/>  
	 */
	
	public void moveLeft(){
		if(this.mGLListAdapter == null) return;
		this.mTotalCount = this.mGLListAdapter.getCount();
		if(mSelectedIndex >= this.mTotalCount -1){
			mSelectedIndex = this.mTotalCount -1;
			if(mOnItemSelectedListener!=null)
				mOnItemSelectedListener.onNoItemData();
			return;
		}

		isAnimationFinished = false;
		((BaseViewActivity) getContext()).getRootView().queueEvent(new Runnable() {
			
			@Override
			public void run() {
				boolean isRepeat = false;
				int changeNum = 0;
				if ((mFocusIndex+1) >= mTotalCount) {
					if(mOnItemSelectedListener!=null)
						mOnItemSelectedListener.onNoItemData();
					return;
				}
//				Log.d("aaa-onitemselect", "mFocusIndex:" + mFocusIndex);
				if(mNumOneScreen >= mTotalCount){//如果显示一屏
					mFocusIndex++;
				}
				else{//多屏
					mFocusIndex++;
					if(mStartIndex + mFocusIndex >= mTotalCount){//到结尾						
						isRepeat = false;
						mFocusIndex--;
						return;
					} else if(mFocusIndex%mNumOneScreen == 0 && mFocusIndex != 0 ){//换下一页
						mFocusIndex = 0;
						if( mTotalCount > mStartIndex + mNumOneScreen *2 ){//还大于一屏
							changeNum = mNumOneScreen;
						}
						else{//剩下的不足一屏
							changeNum = mTotalCount - (mStartIndex + mNumOneScreen);
							mFocusIndex = mNumOneScreen - changeNum;
						}
						
						isRepeat = true;					
					}				
				}
				if(isRepeat){

					isAnimationFinished = false;
					int showIndex = GLListArcView.this.mStartIndex + mNumOneScreen;
					float _tempFloat = 0; 
					float _floatJuji = 0;
					GLRectView _tempView = null;
					for(int i=0;i<changeNum;i++){						
						GLRectView fvnext = getAdapterView(mFocusIndex, showIndex + i, null, GLListArcView.this, true);
						_tempFloat = fvnext.getWidth() + fvnext.getMarginLeft() + fvnext.getMarginRight() + GLListArcView.this.mItemSpacing;
						_tempView = GLListArcView.this.getView(mNumOneScreen-1+i);
						if (_tempView != null) {
							fvnext.setX(_tempView.getX() + _tempView.getWidth() + _tempView.getMarginRight() + GLListArcView.this.mItemSpacing);
						} else {
							fvnext.setX(GLListArcView.this.getX() + _tempFloat * (mNumOneScreen + i));
						}
						
						fvnext.setVisible(true);						
						fvnext.setY(GLListArcView.this.getY() + fvnext.getMarginTop());	
						fvnext.setDepth(mDefaultDepth);
						GLListArcView.this.addView(mNumOneScreen + i, fvnext);
					}
					
					_tempView = GLListArcView.this.getView(changeNum);
					if (_tempView != null) {
						_floatJuji = _tempView.getX() - GLListArcView.this.getX();
					}
					
					mStartIndex = mStartIndex + changeNum;					
					final float _tempFloatJuLi = -_floatJuji;
					final int _tempChangeNum = changeNum;
					
					GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
					listAnimation.setDuration(mMoveDuration);
					listAnimation.setAnimView(GLListArcView.this);
					listAnimation.setOnlyChids(true);
					GLListArcView.this.startAnimation(listAnimation);
					listAnimation.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
						@Override
						public void onAnimationStart(GLAnimation animation) {
							// TODO Auto-generated method stub							
						}
						
						@Override
						public void onAnimationEnd(GLAnimation animation) {
							// TODO Auto-generated method stub
//							
							for(int i=0;i<_tempChangeNum;i++){								
								GLListArcView.this.removeGLViewAt(0);
							}							
							
							GLRectView _tempbig = GLListArcView.this.getView(mFocusIndex);
							mSelectedIndex = mStartIndex + mFocusIndex;
							//回调选中事件				
							if(mOnItemSelectedListener!=null){
								if(_tempbig == null) return;
								mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
								
								if(_tempChangeNum < mNumOneScreen){
									GLRectView _templittle = GLListArcView.this.getView(mFocusIndex-1);
									if(_templittle == null) return;
									mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex - 1, mStartIndex+mFocusIndex);
								}
							}
//							mFocusPreIndex = mNumOneScreen-1;
//							isAnimationFinished = true;
						}
					});
				}
				if(!isRepeat){
					GLRectView _tempbig = GLListArcView.this.getView(mFocusIndex);
					mSelectedIndex = mStartIndex+mFocusIndex;
					if(_tempbig == null) return;
					//回调选中事件				
					if(mOnItemSelectedListener!=null){
						mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
					}
					
					if(mFocusPreIndex != -1){
						//上一个缩小
						GLRectView _templittle = GLListArcView.this.getView(mFocusIndex - 1);					
						//回调失去焦点事件
						if(mOnItemSelectedListener!=null){
							if(_templittle == null) return;
							mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex - 1, mSelectedIndex-1);
						}
					}
					mFocusPreIndex = mFocusIndex;
				}
			}
		});
	}
	
	/**
	 * 
	 * 焦点往左移动，列表往右移动
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/>  
	 */
	public void moveRight(){
		this.mTotalCount = this.mGLListAdapter.getCount();
		((BaseViewActivity) getContext()).getRootView().queueEvent(new Runnable() {			
			@Override
			public void run() {
				
				boolean isRepeat = false;
				int changeNum = 0;
				mFocusIndex--;
				if(mFocusIndex < 0 && mStartIndex==0) {//已经到头了
					mFocusIndex = 0;
					return;
				}
				isAnimationFinished = false;
				if(mFocusIndex < 0){//翻页					
					if(mStartIndex >= mNumOneScreen){//滚动一屏
						changeNum = mNumOneScreen;
					}
					else{//滚动部分
						changeNum = mStartIndex;
					}					
					isRepeat = true;
				}
				
				if(isRepeat){

					isAnimationFinished = false;
					if(changeNum == mNumOneScreen) {
						mFocusIndex = mNumOneScreen - 1;
					}
					else{
						mFocusIndex = changeNum - 1;
					}					
					float _tempFloat = 0;		
					float _floatJuji = 0;
					GLRectView _tempView = null;
					for(int i=0;i<changeNum;i++){//						
						GLRectView fvpre = getAdapterView(mFocusIndex, mStartIndex - i - 1, null, GLListArcView.this, true);						
						_tempFloat = (fvpre.getWidth() + fvpre.getMarginLeft() + fvpre.getMarginRight() + GLListArcView.this.mItemSpacing);
						
						_tempView = GLListArcView.this.getView(0);
						if (_tempView != null) {
							fvpre.setX(_tempView.getX() - _tempFloat);
						} else {
							fvpre.setX( GLListArcView.this.getX() -_tempFloat * (i+1));
						}

						fvpre.setY(GLListArcView.this.getY() + fvpre.getMarginTop());	
						fvpre.setVisible(true);
						fvpre.setDepth(mDefaultDepth);
						GLListArcView.this.addView(0, fvpre);
					}
					
					_tempView = GLListArcView.this.getView(0);
					if (_tempView != null) {
						_floatJuji = GLListArcView.this.getX() - _tempView.getX();
					}
					
					mStartIndex = mStartIndex - changeNum;
//					
					final float _tempFloatJuLi = _floatJuji;
					GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
					listAnimation.setDuration(mMoveDuration);
					listAnimation.setAnimView(GLListArcView.this);
					listAnimation.setOnlyChids(true);
					GLListArcView.this.startAnimation(listAnimation);
					
					
					final int _tempChangeNum = changeNum;
					
					listAnimation.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
						
						@Override
						public void onAnimationStart(GLAnimation animation) {
							// TODO Auto-generated method stub
							
						}
						
						@Override
						public void onAnimationEnd(GLAnimation animation) {
							// TODO Auto-generated method stub
//							
							for(int i=0;i<_tempChangeNum;i++){
								GLListArcView.this.removeGLViewAt(GLListArcView.this.mChildView.size()-1);
							}
							
							GLRectView _tempbig = GLListArcView.this.getView(mFocusIndex);
							if(_tempbig == null) return;
							mSelectedIndex = mStartIndex+mFocusIndex;							
							//回调选中事件							
							if(mOnItemSelectedListener!=null){
								
								mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
								if(_tempChangeNum<mNumOneScreen){
//									Log.d("ListView","mFocusIndex:" + mFocusIndex);
									GLRectView _templittle = GLListArcView.this.getView(mFocusIndex+1);
									if(_templittle == null) return;
									mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex + 1, mStartIndex+mFocusIndex+1);
								}
							}
							
							mFocusPreIndex = _tempChangeNum-1;
//							isAnimationFinished = true;
							
						}
					});
					
				}
				
				if(!isRepeat){
					GLRectView _tempbig = GLListArcView.this.getView(mFocusIndex);
					mSelectedIndex = mStartIndex+mFocusIndex;
					if(_tempbig == null) return;
					//回调选中事件				
					if(mOnItemSelectedListener!=null){
//						Log.d("aaa-onitemselect---5","mfocusIndex:" + mFocusIndex);
						mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
					}
					
					if(mFocusPreIndex > 0){					
						GLRectView _templittle = GLListArcView.this.getView(mFocusIndex+1);
						if(mOnItemSelectedListener!=null){
							if(_templittle == null) return;
							mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex + 1, mStartIndex+mFocusIndex+1);
						}
					}
					mFocusPreIndex = mFocusIndex;
				}				
			}
		});
		
	}
	
	
	/* 滑动一屏后回调 */
	public interface MoveOneScreenCallBack{
		void moveFinished(int startIndex);
	}
	
	/**
	 * 
	 * 一屏整体滑动 往左滑
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/>  
	 */
	public void moveOneScreenLeft(final MoveOneScreenCallBack callback){
		(((BaseViewActivity)mContext).getRootView()).queueEvent(new Runnable() {
			@Override
			public void run() {
				GLListArcView.this.mTotalCount = GLListArcView.this.mGLListAdapter.getCount();
				int changeNum=0;
				if( GLListArcView.this.mTotalCount > GLListArcView.this.mStartIndex + GLListArcView.this.mNumOneScreen *2 ){//还大于一屏
					changeNum = mNumOneScreen;
				}
				else{//剩下的不足一屏
					changeNum = GLListArcView.this.mTotalCount - (GLListArcView.this.mStartIndex + GLListArcView.this.mNumOneScreen);			
				}
				int showIndex = GLListArcView.this.mStartIndex + changeNum;
				float _tempFloat = 0; 
				float _floatJuji = 0;
				GLRectView _tempView = null;
				
				for(int i=0;i<changeNum;i++){
					int _trueindex = (changeNum < mNumOneScreen ? mNumOneScreen-changeNum+i : i);
					
					GLRectView fvnext = GLListArcView.this.getAdapterView( _trueindex, showIndex + _trueindex, null, GLListArcView.this, true);
					_tempFloat = fvnext.getWidth() + fvnext.getMarginLeft() + fvnext.getMarginRight() + GLListArcView.this.mItemSpacing;
					
					_tempView = GLListArcView.this.getView(mNumOneScreen-1+i);
					if (_tempView != null) {
						fvnext.setX(_tempView.getX() + _tempView.getWidth() + _tempView.getMarginRight() + GLListArcView.this.mItemSpacing);
					} else {
						fvnext.setX(GLListArcView.this.getX() + _tempFloat * (mNumOneScreen + i));
					}
					
					fvnext.setVisible(true);						
					fvnext.setY(GLListArcView.this.getY() + fvnext.getMarginTop());	
					fvnext.setDepth(mDefaultDepth);
					GLListArcView.this.addView(mNumOneScreen + i, fvnext);
				}
				
				
				_tempView = GLListArcView.this.getView(changeNum);
				if (_tempView != null) {
					_floatJuji = _tempView.getX() - GLListArcView.this.getX();
				}
				
				GLListArcView.this.mStartIndex = GLListArcView.this.mStartIndex + changeNum;	
				
				final float _tempFloatJuLi = -_floatJuji;
				final int _tempChangeNum = changeNum;
				GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
				listAnimation.setDuration(mMoveDuration);
				listAnimation.setAnimView(GLListArcView.this);
				listAnimation.setOnlyChids(true);
				GLListArcView.this.startAnimation(listAnimation);
				listAnimation.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
					@Override
					public void onAnimationStart(GLAnimation animation) {
						// TODO Auto-generated method stub							
					}
					
					@Override
					public void onAnimationEnd(GLAnimation animation) {
						// TODO Auto-generated method stub
//						
						for(int i=0;i<_tempChangeNum;i++){								
							GLListArcView.this.removeGLViewAt(0);
						}
						if(_tempChangeNum < mNumOneScreen){
							for(int j=0;j<mNumOneScreen;j++){
								GLRectView _view = GLListArcView.this.getView(j);
								GLListArcView.this.getAdapterView( j, GLListArcView.this.mStartIndex + j, _view, GLListArcView.this, false);
							}
						}
						
						for(int k=0;k<GLListArcView.this.getChildView().size();k++){
							GLRectView _view1 = GLListArcView.this.getView(k);
							if(_view1.getDepth() > GLListArcView.this.getDepth()){
								_view1.setDepth(GLListArcView.this.getDepth());
							}
						}
						
						if(callback != null)
							callback.moveFinished(GLListArcView.this.mStartIndex);
					}
				});
			}
		});
	}
	
	
	/**
	 * 
	 * 一屏整体滑动 往右滑
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/> 
	 */
	public void moveOneScreenRight(final MoveOneScreenCallBack callback){		
		(((BaseViewActivity)mContext).getRootView()).queueEvent(new Runnable() {
			@Override
			public void run() {
				GLListArcView.this.mTotalCount = GLListArcView.this.mGLListAdapter.getCount();
				int changeNum = 0;
				if(GLListArcView.this.mStartIndex <=0){
					GLListArcView.this.mStartIndex=0;
					return;
				}
				if(GLListArcView.this.mStartIndex >= GLListArcView.this.mNumOneScreen){//滚动一屏
					changeNum = GLListArcView.this.mNumOneScreen;
				}
				else{//滚动部分
					changeNum = GLListArcView.this.mStartIndex;
				}	
				
				float _tempFloat = 0;
				float _floatJuji = 0;
				GLRectView _tempView = null;
				for(int i=0;i<changeNum;i++){//						
					GLRectView fvpre = GLListArcView.this.getAdapterView(GLListArcView.this.mNumOneScreen-i-1, GLListArcView.this.mStartIndex - i - 1, null, GLListArcView.this, true);						
					_tempFloat = (fvpre.getWidth() + fvpre.getMarginLeft() + fvpre.getMarginRight() + GLListArcView.this.mItemSpacing);
					
					//计算位置
					_tempView = GLListArcView.this.getView(0);
					if (_tempView != null) {
						fvpre.setX(_tempView.getX() - _tempFloat);
					} else {
						fvpre.setX( GLListArcView.this.getX() -_tempFloat * (i+1));
					}
					
					fvpre.setY(GLListArcView.this.getY() + fvpre.getMarginTop());	
					fvpre.setVisible(true);
					fvpre.setDepth(mDefaultDepth);
					GLListArcView.this.addView(0, fvpre);
				}
				
				_tempView = GLListArcView.this.getView(0);
				if (_tempView != null) {
					_floatJuji = GLListArcView.this.getX() - _tempView.getX();
				}
				
				GLListArcView.this.mStartIndex = GLListArcView.this.mStartIndex - changeNum;
				
				final float _tempFloatJuLi = _floatJuji;
				GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
				listAnimation.setDuration(mMoveDuration);
				listAnimation.setAnimView(GLListArcView.this);
				listAnimation.setOnlyChids(true);
				GLListArcView.this.startAnimation(listAnimation);
				
				final int _tempChangeNum = changeNum;
				
				listAnimation.setOnGLAnimationListener(new GLAnimation.OnGLAnimationListener() {
					
					@Override
					public void onAnimationStart(GLAnimation animation) {
						// TODO Auto-generated method stub
						
					}
					
					@Override
					public void onAnimationEnd(GLAnimation animation) {
						// TODO Auto-generated method stub
//						
						for(int i=0;i<_tempChangeNum;i++){
							GLListArcView.this.removeGLViewAt(GLListArcView.this.mChildView.size()-1);
						}
						
						if(_tempChangeNum < mNumOneScreen){
							for(int j=0;j<mNumOneScreen;j++){
								GLRectView _view = GLListArcView.this.getView(j);								
								GLListArcView.this.getAdapterView( j, GLListArcView.this.mStartIndex + j, _view, GLListArcView.this, false);
							}
						}
						
						for(int k=0;k<GLListArcView.this.getChildView().size();k++){
							GLRectView _view1 = GLListArcView.this.getView(k);
							if(_view1.getDepth() > GLListArcView.this.getDepth()){
								_view1.setDepth(GLListArcView.this.getDepth());
							}
						}
						
						if(callback != null)
							callback.moveFinished(GLListArcView.this.mStartIndex);
					}
				});
			}
		});
	}
	
	


	@Override
	public GLView getSelectedGLView() {
		// TODO Auto-generated method stub
		return null;
		
	}
	
	/**
	 * 
	 * 移除所有的子view
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/>
	 */
	@Override
	public void removeAllGLViews() {
		if (this.mGLListAdapter == null) return;

		for(int i=0;i<this.mTotalCount;i++){
			//this.mGLListAdapter.removeIndex(i);
		}
		
		mGLRectViewList.clear();
		super.removeAllGLViews();
	}

	/**
	 * 
	 * 初始化时获取焦点
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/>  
	 */
	public void setFocus() {
		if(isOpenHeadControl()) return;

		if (mFocusIndex<0) {
			mFocusIndex = -1;
			mFocusPreIndex = -1;
			mSelectedIndex = -1;
			moveLeft();
			return;
		}
		if (mFocusIndex >= mTotalCount) {
			return;
		}

		//获取焦点
		if(mOnItemSelectedListener!=null){
			GLRectView _view = GLListArcView.this.getView(mFocusIndex);
			if(_view == null) return;
			mOnItemSelectedListener.onItemSelected(null, _view, mFocusIndex, mStartIndex+mFocusIndex);
		}
	}
	/**
	 * 
	 * 失去焦点
	 * @author wang.f.z 
	 * @date: 2015-3-9 下午1:51:19 <br/> 
	 */
	public void lostFocus() {
		if(isOpenHeadControl()) return;
		if (mFocusIndex<0) {
			return;
		}
		if (mFocusIndex >= mTotalCount) {
			return;
		}

		//获取焦点
		if(mOnItemSelectedListener!=null){
			GLRectView _view = GLListArcView.this.getView(mFocusIndex);
			if(_view == null) return;
			mOnItemSelectedListener.onNothingSelected(null, _view, mFocusIndex, mStartIndex+mFocusIndex);
		}
	}
	

	/**
	 * 重置焦点
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public void resetFocusIndex() {
		mFocusIndex = 0;
		mStartIndex = 0;
	}
	
	/**
	 * 初始化焦点数据
	 * @return boolean true向右移，false向左移
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public void initFocus() {
		mStartIndex=0;//当前开始的索引
		mSelectedIndex=0;//当前选中的索引
		mPreSelectedIndex=-1;//上一次选中的索引
		mFocusIndex = -1;//获取焦点的索引，这个是内部索引，范围在一屏显示的数内
		mFocusPreIndex = -1;
	}
	
	/**
	 * 是否向右移 。加载更多
	 * @return boolean true向右移，false向左移
	 * @author linzanxian  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public boolean getIsMovieRight() {
		return isMoveRight;
	}
	
	/**
	 * 得到焦点的索引
	 * @return boolean true向右移，false向左移
	 * @author wang.f.z  @Date 2015年4月1日 下午2:04:17
	 */
	public int getFocusIndex(){
		return mFocusIndex;
	}
	/**
	 *得到当前第一个索引在总集合里的索引
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public int getCurrStartIndex() {
		return mStartIndex;
	}
/**
 * 得到当前Item控件集
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public List<GLRectView> getList() {
		List<GLRectView> _tempList = new ArrayList<GLRectView>();
		for(int i=0;i<this.mNumOneScreen;i++){
			if(i>mTotalCount) break;
			_tempList.add(GLListArcView.this.getView(i));
		}
		return _tempList;
	}

	/**
	 * 得到总数量
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public int getTotalCount() {
		return mTotalCount;
	}
	
	/**
	 * 得到每屏显示的item数
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public int getNumOneScreen() {
		return mNumOneScreen;
	}

	/**
	 * 自定义KeyDown事件，不使用ListView默认事件
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public GLOnKeyListener getOutViewKeyListener() {
		return outViewKeyListener;
	}
/**
 * 设置自定义KeyDown事件
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public void setOutViewKeyListener(GLOnKeyListener outViewKeyListener) {
		this.outViewKeyListener = outViewKeyListener;
	}

	 /**
	  * 得到当前选中的索引
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public int getCurrSelectIndex(){
		return this.mSelectedIndex;
	}

	 /**
	  * 是否已经是选中最后一个了
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public boolean isToLast() {
		return this.mSelectedIndex == this.mTotalCount-1 ? true : false;
	}	
	/**
	 * 设置初始景深
	 * @author wang.f.z  
	 * @Date 2015年4月1日 下午2:04:17
	 */
	public void setDefaultDepth(float depth) {
		mDefaultDepth = depth;
	}

	public int getAngle() {
		return angle;
	}

	public void setAngle(int angle) {
		this.angle = angle;
	}

	
	
}
