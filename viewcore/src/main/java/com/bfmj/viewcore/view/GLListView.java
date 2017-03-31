package com.bfmj.viewcore.view;

import java.util.ArrayList;
import java.util.List;

import com.baofeng.mojing.input.base.MojingKeyCode;
import com.bfmj.viewcore.animation.GLAnimation;
import com.bfmj.viewcore.animation.GLTranslateAnimation;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.render.GLScreenParams;
import com.bfmj.viewcore.util.GLFocusUtils;
import com.bfmj.viewcore.adapter.GLListAdapter;

import android.content.Context;
import android.util.Log;


public class GLListView extends GLAdapterView<GLListAdapter>{
	/**
	 * 排列顺序 按行排序
	 */
	public static final int VERTICAL=0x01;
	/**
	 * 排列顺序 按列排序
	 */
	public static final int HORIZONTAL=0x02;


	public static final int MOVELEFT=0x00;

	public static final int MOVERIGHT=0x01;


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
	private int mMoveStepWidth=0;
	private List<GLRectView> mGLRectViewList;
	private int mGLRectViewListNum;
	private GLRectView convertView;
	private int mMoveDuration=200;//动画时间
	private Context mContext;
	private float mDefaultDepth = 0.0f;
	private OnItemSelectedListener mOnItemSelectedListener;
	private OnListViewFocusChange mListViewFocusChange;
	private OnItemClickListener mOnItemClickListener;
	private boolean isMoveRight = false;
	private GLOnKeyListener outViewKeyListener;
	private boolean isAnimationFinished = true;
	private MoveScreenCallBack mMoveScreenCallBack;

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
	public GLListView(Context context, int orderType) {
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

		mListViewFocusChange = getOnListViewFocusChange();
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

	public void setOpenHeadControl(boolean isOpen){
		if(isOpen){
			GLFocusUtils.openHeadControl();
			resetFocusForHead();
		}
		else{
			GLFocusUtils.closeHeadControl();
			setNoHeadListen();
		}
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
	public void requestLayout(){
		if(this.mChildView.size() > 0){
			removeAllGLViews();
		}
		resetParams();

		this.mTotalCount = mGLListAdapter.getCount();
		float mX = GLListView.this.getX();
		for(int j=0;j<this.mNumOneScreen;j++){

			if(j >this.mTotalCount-1)
				break;
			GLRectView view = getAdapterView(j,j, null, GLListView.this, true);
			if(view == null)
				continue;


			mX += view.getMarginLeft();
			view.rotate( mAngle, 0, 1, 0);
			if(this.mOrderType == this.HORIZONTAL){
				view.setX(mX);
//				view.setY(GLListView.this.getY() + getMarginTop());
				view.setY(GLListView.this.getY() + getPaddingTop()+ view.getMarginTop());
				mX += view.getWidth() + this.mItemSpacing;
			} else{
				view.setX(GLListView.this.getX() + getMarginLeft());
				view.setY(getY() + getMarginTop() + (view.getHeight() + this.mItemSpacing)*j);
				//view.setY(view.getMarginTop() + (view.getHeight() + this.mItemSpacing + view.getMarginTop() + view.getMarginBottom())*j);
			}

			mDefaultDepth = view.getDepth();


//			float translateX = (this.getWidth() / 2 - view.getWidth() / 2) - (view.getX() - this.getX());
//			float depth = GLScreenParams.getScreenWidth() / 2400 * translateX * mSinV;
//			view.setDepth( this.getDepth() + depth );

			if( view.getDepth() > this.getDepth() ){
				view.setDepth( this.getDepth() );
			}

			//views[j] = view;
			this.addView( view );
		}

//		try{
//			if(getContext() != null && (BaseViewActivity) getContext() != null && ((BaseViewActivity) getContext()).getRootView() != null){
//				(((BaseViewActivity) getContext()).getRootView()).queueEvent(new Runnable() {
//					@Override
//					public void run() {
//						for (GLRectView v : views){
//							if (v != null) {
//								GLListView.this.addView(v);
//							}
//						}
//						if (GLListView.this.getParent() != null){
//							resetChildView(GLListView.this);
//						}
//					}
//				});
//			}
//		}
//		catch(Exception e){
//
//		}

//		for(int k=0;k<GLListView.this.getChildView().size();k++){
//			GLRectView view1 = GLListView.this.getView(k);
//			if(view1.getDepth() > GLListView.this.getDepth()){
//				view1.setDepth(GLListView.this.getDepth());
//			}
//		}
//
		if(!isOpenHeadControl()){
			setNoHeadListen();
		}
		else{
//			this.setFocusListener(new GLViewFocusListener() {
//
//				@Override
//				public void onFocusChange(GLRectView view, boolean focused) {
//					// TODO Auto-generated method stub
//					if( mOnItemSelectedListener == null ){
//						return;
//					}
//					if( focused ){
//						mOnItemSelectedListener.onItemSelected( null, view, mSelectedPosition, mSelectedPosition);
//					}else{
//						mOnItemSelectedListener.onNothingSelected( null, view, mSelectedPosition, mSelectedPosition);
//					}
//
//
//				}
//			});
		}
	}

	@Override
	public void setAdapter(GLListAdapter adapter) {
		if( adapter == null ){
			return;
		}

		if( mGLListAdapter != null && mDataSetObserver != null){
			mGLListAdapter.unregisterDataSetObserver( mDataSetObserver);
		}

		// TODO Auto-generated method stub
		mGLListAdapter = adapter;
		mOnItemSelectedListener = getOnItemSelectedListener();
		mOnItemClickListener = getOnItemClickListener();
		mDataSetObserver = new GLAdapterDataSetObserver();
		mGLListAdapter.registerDataSetObserver( mDataSetObserver );

		requestLayout();
		return;
	}

	private void setNoHeadListen(){
		GLListView.this.setOnKeyListener(new GLOnKeyListener() {

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
							if(mSelectedIndex >= GLListView.this.mTotalCount -1){
								mSelectedIndex = GLListView.this.mTotalCount -1;
								if(mOnItemSelectedListener!=null){
									mOnItemSelectedListener.onNothingSelected(null, GLListView.this.getView(mFocusIndex), mFocusIndex, mSelectedIndex);
									mOnItemSelectedListener.onNoItemData();
								}
							}
							return true;
						}

					}

//					if(mSelectedIndex >= mTotalCount-1) return true;
					if(!isAnimationFinished)
						return true;
					isMoveRight = true;
					moveLeft();
					return true;
				}
				else if(keycode == MojingKeyCode.KEYCODE_DPAD_LEFT){
					//回调控件使用者注册的事件监听对象

					if (outViewKeyListener!=null) {
						if(outViewKeyListener.onKeyDown(view, keycode)){
							if(mFocusIndex <= 0 && mStartIndex==0) {//已经到头了
								mFocusIndex = 0;
								//回调失去焦点事件
								if(mOnItemSelectedListener!=null){
									mOnItemSelectedListener.onNothingSelected(null, GLListView.this.getView(mFocusIndex), mFocusIndex, 0);
								}
							}
							return true;
						}

					}
//					if(mSelectedIndex <= 0) return true;
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
//
					}
					if(mOnItemClickListener!=null){
						mOnItemClickListener.onItemClick(GLListView.this, GLListView.this.getView(mFocusIndex), mFocusIndex, mSelectedIndex);
					}

					return true;
				}

				if (mOrderType == HORIZONTAL && (keycode == MojingKeyCode.KEYCODE_DPAD_DOWN || keycode == MojingKeyCode.KEYCODE_DPAD_UP)) {
					//回调控件使用者注册的事件监听对象
					if (outViewKeyListener!=null) {
						if(outViewKeyListener.onKeyDown(view, keycode)){
							if(mFocusIndex >= 0 && mFocusIndex < mTotalCount) {
								mSelectedIndex = mStartIndex + mFocusIndex;
								//回调失去焦点事件
								if(mOnItemSelectedListener!=null){
									mOnItemSelectedListener.onNothingSelected(null, GLListView.this.getView(mFocusIndex), mFocusIndex, mSelectedIndex);
								}
							}
							return true;
						}
					}
				}

				if (mOrderType == VERTICAL && (keycode == MojingKeyCode.KEYCODE_DPAD_DOWN || keycode == MojingKeyCode.KEYCODE_DPAD_UP)) {
					//回调控件使用者注册的事件监听对象
					if (outViewKeyListener!=null) {
						if(outViewKeyListener.onKeyDown(view, keycode)){
							if(mFocusIndex >= 0 && mFocusIndex < mTotalCount) {
								mSelectedIndex = mStartIndex + mFocusIndex;
								//回调失去焦点事件
								if(mOnItemSelectedListener!=null){
									mOnItemSelectedListener.onNothingSelected(null, GLListView.this.getView(mFocusIndex), mFocusIndex, mSelectedIndex);
								}
							}
							return true;
						}
					}
				}
				return false;
			}
		});
	}

	private GLRectView getAdapterView(final int location, final int position, GLRectView view, GLGroupView parentView, boolean isNew){

		GLRectView childview = view;
		if(isNew){
			childview = mGLListAdapter.getGLView(position, view, parentView);
		}
		final GLRectView _childview = childview;
		if(_childview!= null && isOpenHeadControl()){//
			_childview.setFocusListener(new GLViewFocusListener() {

				@Override
				public void onFocusChange(GLRectView view, boolean focused) {
					// TODO Auto-generated method stub
					if(focused){
						mFocusIndex = location;
						if( mOnItemSelectedListener != null ) {
							mOnItemSelectedListener.onItemSelected(GLListView.this, view, location, mStartIndex + location);
						}
					}
					else if( mOnItemSelectedListener != null ){
						mOnItemSelectedListener.onNothingSelected(GLListView.this, view, location, mStartIndex + location);
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
							mOnItemClickListener.onItemClick(GLListView.this, view, location, mStartIndex + location);
						}
						return true;
					}
					return false;
				}
			});

		}
		return _childview;
	}

	private void resetFocusForHead(){

		for(int i=0;i<GLListView.this.getChildView().size();i++){
			GLRectView _childview = GLListView.this.getView(i);

			final int _index = i;

			if(_childview!= null && isOpenHeadControl()){//
				_childview.setFocusListener(new GLViewFocusListener() {

					@Override
					public void onFocusChange(GLRectView view, boolean focused) {
						// TODO Auto-generated method stub
						if(focused){
							mFocusIndex = _index;
							mOnItemSelectedListener.onItemSelected(GLListView.this, view, _index, mStartIndex + _index);
						}
						else{
							mOnItemSelectedListener.onNothingSelected(GLListView.this, view, _index, mStartIndex + _index);
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
//							downloadHandle(mCurrClickIndex, mCurrClickView);
							if(mOnItemClickListener!=null){
								mOnItemClickListener.onItemClick(GLListView.this, view, _index, mStartIndex + _index);
							}
							return true;
						}
						return false;
					}
				});

			}
		}
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
			if(mOnItemSelectedListener!=null){
				mOnItemSelectedListener.onNoItemData();
			}
			return;
		}

		isAnimationFinished = false;
		((BaseViewActivity) getContext()).getRootView().queueEvent(new Runnable() {

			@Override
			public void run() {
				boolean isRepeat = false;
				int changeNum = 0;
				if ((mFocusIndex+1) >= mTotalCount) {
//					Log.d("ddddd","mFocusIndex:" + mFocusIndex + "---mTotalCount:" + mTotalCount);
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
					int showIndex = GLListView.this.mStartIndex + mNumOneScreen;
					float _tempFloat = 0;
					float _floatJuji = 0;
					GLRectView _tempView = null;
					for(int i=0;i<changeNum;i++){
						GLRectView fvnext = getAdapterView(mFocusIndex, showIndex + i, null, GLListView.this, true);
						if (fvnext == null) {
							continue;
						}
						_tempFloat = fvnext.getWidth() + fvnext.getMarginLeft() + fvnext.getMarginRight() + GLListView.this.mItemSpacing;
						_tempView = GLListView.this.getView(mNumOneScreen-1+i);
						if (_tempView != null) {
							fvnext.setX(_tempView.getX() + _tempView.getWidth() + _tempView.getMarginRight() + GLListView.this.mItemSpacing);
						} else {
							fvnext.setX(GLListView.this.getX() + _tempFloat * (mNumOneScreen + i));
						}

						fvnext.setVisible(true);
						fvnext.setY(GLListView.this.getY() + fvnext.getMarginTop());
						fvnext.setDepth(mDefaultDepth);
						GLListView.this.addView(mNumOneScreen + i, fvnext);
					}

					_tempView = GLListView.this.getView(changeNum);
					if (_tempView != null) {
						_floatJuji = _tempView.getX() - GLListView.this.getX();
					}

					mStartIndex = mStartIndex + changeNum;
					final float _tempFloatJuLi = -_floatJuji;
					final int _tempChangeNum = changeNum;

					GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
					listAnimation.setDuration(mMoveDuration);
					listAnimation.setAnimView(GLListView.this);
					listAnimation.setOnlyChids(true);
					GLListView.this.startAnimation(listAnimation);
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
								GLListView.this.removeGLViewAt(0);
							}

							GLRectView _tempbig = GLListView.this.getView(mFocusIndex);
							mSelectedIndex = mStartIndex + mFocusIndex;
							//回调选中事件
							if(mOnItemSelectedListener!=null){
								if(_tempbig == null) return;
								mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);

								if(_tempChangeNum < mNumOneScreen){
									GLRectView _templittle = GLListView.this.getView(mFocusIndex-1);
									if(_templittle == null) return;
									mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex - 1, mStartIndex+mFocusIndex);
								}
							}

							if(mMoveScreenCallBack != null){
								mMoveScreenCallBack.callback();
								mMoveScreenCallBack.callback(GLListView.MOVELEFT);
							}
//							mFocusPreIndex = mNumOneScreen-1;
//							isAnimationFinished = true;
						}
					});
				}
				if(!isRepeat){
					GLRectView _tempbig = GLListView.this.getView(mFocusIndex);
					mSelectedIndex = mStartIndex+mFocusIndex;
					if(_tempbig == null) return;
					//回调选中事件
					if(mOnItemSelectedListener!=null){
						mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
					}

					if(mFocusPreIndex != -1){
						//上一个缩小
						GLRectView _templittle = GLListView.this.getView(mFocusIndex - 1);
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
		if(this.mGLListAdapter == null) return;
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
						GLRectView fvpre = getAdapterView(mFocusIndex, mStartIndex - i - 1, null, GLListView.this, true);
						if (fvpre == null) {
							continue;
						}
						_tempFloat = (fvpre.getWidth() + fvpre.getMarginLeft() + fvpre.getMarginRight() + GLListView.this.mItemSpacing);

						_tempView = GLListView.this.getView(0);
						if (_tempView != null) {
							fvpre.setX(_tempView.getX() - _tempFloat);
						} else {
							fvpre.setX( GLListView.this.getX() -_tempFloat * (i+1));
						}

						fvpre.setY(GLListView.this.getY() + fvpre.getMarginTop());
						fvpre.setVisible(true);
						fvpre.setDepth(mDefaultDepth);
						GLListView.this.addView(0, fvpre);
					}

					_tempView = GLListView.this.getView(0);
					if (_tempView != null) {
						_floatJuji = GLListView.this.getX() - _tempView.getX();
					}

					mStartIndex = mStartIndex - changeNum;
//
					final float _tempFloatJuLi = _floatJuji;
					GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
					listAnimation.setDuration(mMoveDuration);
					listAnimation.setAnimView(GLListView.this);
					listAnimation.setOnlyChids(true);
					GLListView.this.startAnimation(listAnimation);


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
								GLListView.this.removeGLViewAt(GLListView.this.mChildView.size()-1);
							}

							GLRectView _tempbig = GLListView.this.getView(mFocusIndex);
							if(_tempbig == null) return;
							mSelectedIndex = mStartIndex+mFocusIndex;
							//回调选中事件
							if(mOnItemSelectedListener!=null){

								mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
								if(_tempChangeNum<mNumOneScreen){
									GLRectView _templittle = GLListView.this.getView(mFocusIndex+1);
									if(_templittle == null) return;
									mOnItemSelectedListener.onNothingSelected(null, _templittle, mFocusIndex + 1, mStartIndex+mFocusIndex+1);
								}
							}

							if(mMoveScreenCallBack != null){
								mMoveScreenCallBack.callback();
								mMoveScreenCallBack.callback(GLListView.MOVERIGHT);
							}

							mFocusPreIndex = _tempChangeNum-1;
//							isAnimationFinished = true;

						}
					});

				}

				if(!isRepeat){
					GLRectView _tempbig = GLListView.this.getView(mFocusIndex);
					mSelectedIndex = mStartIndex+mFocusIndex;
					if(_tempbig == null) return;
					//回调选中事件
					if(mOnItemSelectedListener!=null){
//						Log.d("aaa-onitemselect---5","mfocusIndex:" + mFocusIndex);
						mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
					}

					if(mFocusPreIndex > 0){
						GLRectView _templittle = GLListView.this.getView(mFocusIndex+1);
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


	/* 直接滑动一屏后回调 */
	public interface MoveScreenCallBack{
		void callback();
		void callback(int fx);
	}

	public void setMoveScreenCallBack(MoveScreenCallBack callback){
		mMoveScreenCallBack = callback;
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
				if (mGLListAdapter == null) {
					return;
				}
				GLListView.this.mTotalCount = GLListView.this.mGLListAdapter.getCount();
				int changeNum=0;
				if( GLListView.this.mTotalCount > GLListView.this.mStartIndex + GLListView.this.mNumOneScreen *2 ){//还大于一屏
					changeNum = mNumOneScreen;
				}
				else{//剩下的不足一屏
					changeNum = GLListView.this.mTotalCount - (GLListView.this.mStartIndex + GLListView.this.mNumOneScreen);
				}
				int showIndex = GLListView.this.mStartIndex + changeNum;
				float _tempFloat = 0;
				float _floatJuji = 0;
				GLRectView _tempView = null;

				for(int i=0;i<changeNum;i++){
					int _trueindex = (changeNum < mNumOneScreen ? mNumOneScreen-changeNum+i : i);

					GLRectView fvnext = GLListView.this.getAdapterView( _trueindex, showIndex + _trueindex, null, GLListView.this, true);
					_tempFloat = fvnext.getWidth() + fvnext.getMarginLeft() + fvnext.getMarginRight() + GLListView.this.mItemSpacing;

					_tempView = GLListView.this.getView(mNumOneScreen-1+i);
					if (_tempView != null) {
						fvnext.setX(_tempView.getX() + _tempView.getWidth() + _tempView.getMarginRight() + GLListView.this.mItemSpacing);
					} else {
						fvnext.setX(GLListView.this.getX() + _tempFloat * (mNumOneScreen + i));
					}

					fvnext.setVisible(true);
					fvnext.setY(GLListView.this.getY() + fvnext.getMarginTop());
					fvnext.setDepth(mDefaultDepth);
					GLListView.this.addView(mNumOneScreen + i, fvnext);
				}


				_tempView = GLListView.this.getView(changeNum);
				if (_tempView != null) {
					_floatJuji = _tempView.getX() - GLListView.this.getX();
				}

				GLListView.this.mStartIndex = GLListView.this.mStartIndex + changeNum;

				final float _tempFloatJuLi = -_floatJuji;
				final int _tempChangeNum = changeNum;
				GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
				listAnimation.setDuration(mMoveDuration);
				listAnimation.setAnimView(GLListView.this);
				listAnimation.setOnlyChids(true);
				GLListView.this.startAnimation(listAnimation);
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
							GLListView.this.removeGLViewAt(0);
						}
						if(_tempChangeNum < mNumOneScreen){
							for(int j=0;j<mNumOneScreen;j++){
								GLRectView _view = GLListView.this.getView(j);
								GLListView.this.getAdapterView( j, GLListView.this.mStartIndex + j, _view, GLListView.this, false);
							}
						}

						for(int k=0;k<GLListView.this.getChildView().size();k++){
							GLRectView _view1 = GLListView.this.getView(k);
							if(_view1.getDepth() > GLListView.this.getDepth()){
								_view1.setDepth(GLListView.this.getDepth());
							}
						}

						if(callback != null)
							callback.moveFinished(GLListView.this.mStartIndex);
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
				if (mGLListAdapter == null) {
					return;
				}
				GLListView.this.mTotalCount = GLListView.this.mGLListAdapter.getCount();
				int changeNum = 0;
				if(GLListView.this.mStartIndex <=0){
					GLListView.this.mStartIndex=0;
					return;
				}
				if(GLListView.this.mStartIndex >= GLListView.this.mNumOneScreen){//滚动一屏
					changeNum = GLListView.this.mNumOneScreen;
				}
				else{//滚动部分
					changeNum = GLListView.this.mStartIndex;
				}

				float _tempFloat = 0;
				float _floatJuji = 0;
				GLRectView _tempView = null;
				for(int i=0;i<changeNum;i++){//
					GLRectView fvpre = GLListView.this.getAdapterView(GLListView.this.mNumOneScreen-i-1, GLListView.this.mStartIndex - i - 1, null, GLListView.this, true);
					_tempFloat = (fvpre.getWidth() + fvpre.getMarginLeft() + fvpre.getMarginRight() + GLListView.this.mItemSpacing);

					//计算位置
					_tempView = GLListView.this.getView(0);
					if (_tempView != null) {
						fvpre.setX(_tempView.getX() - _tempFloat);
					} else {
						fvpre.setX( GLListView.this.getX() -_tempFloat * (i+1));
					}

					fvpre.setY(GLListView.this.getY() + fvpre.getMarginTop());
					fvpre.setVisible(true);
					fvpre.setDepth(mDefaultDepth);
					GLListView.this.addView(0, fvpre);
				}

				_tempView = GLListView.this.getView(0);
				if (_tempView != null) {
					_floatJuji = GLListView.this.getX() - _tempView.getX();
				}

				GLListView.this.mStartIndex = GLListView.this.mStartIndex - changeNum;

				final float _tempFloatJuLi = _floatJuji;
				GLTranslateAnimation listAnimation = new GLTranslateAnimation(_tempFloatJuLi, 0, 0);
				listAnimation.setDuration(mMoveDuration);
				listAnimation.setAnimView(GLListView.this);
				listAnimation.setOnlyChids(true);
				GLListView.this.startAnimation(listAnimation);

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
							GLListView.this.removeGLViewAt(GLListView.this.mChildView.size()-1);
						}

						if(_tempChangeNum < mNumOneScreen){
							for(int j=0;j<mNumOneScreen;j++){
								GLRectView _view = GLListView.this.getView(j);
								GLListView.this.getAdapterView( j, GLListView.this.mStartIndex + j, _view, GLListView.this, false);
							}
						}

						for(int k=0;k<GLListView.this.getChildView().size();k++){
							GLRectView _view1 = GLListView.this.getView(k);
							if(_view1.getDepth() > GLListView.this.getDepth()){
								_view1.setDepth(GLListView.this.getDepth());
							}
						}

						if(callback != null)
							callback.moveFinished(GLListView.this.mStartIndex);
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

	public void removeItemAt(RemoveListener listener){
		mRemoveListener = listener;
		removeItemAt();
	}

	/**
	 * 删除一个子项
	 * @param index
	 */
	public void removeItemAt(){

		(((BaseViewActivity) getContext()).getRootView()).queueEvent(new Runnable() {
			@Override
			public void run() {
//				int index = mFocusIndex;
				int trueIndex = mStartIndex + mFocusIndex;

				if(mStartIndex + mNumOneScreen < mTotalCount){//不是最后一页，后面还有数据

					GLRectView fvnext = getAdapterView(mFocusIndex, mStartIndex + mNumOneScreen, null, GLListView.this, true);
					final float _tempFloat = fvnext.getWidth() + fvnext.getMarginLeft() + fvnext.getMarginRight() + GLListView.this.mItemSpacing;
					GLRectView _tempView = GLListView.this.getView(mNumOneScreen-1);

					if (_tempView!=null && fvnext != null) {
						fvnext.setX(_tempView.getX() + _tempFloat);

						fvnext.setVisible(true);
						fvnext.setY(GLListView.this.getY() + fvnext.getMarginTop());
						fvnext.setDepth(mDefaultDepth);
						GLListView.this.addView(mNumOneScreen, fvnext);
						GLListView.this.removeView(mFocusIndex);

						for(int i=mFocusIndex;i<mNumOneScreen;i++){
							GLRectView nView = GLListView.this.getView(i);
							float _oldX = nView.getX();
							nView.setX(_oldX-_tempFloat);
						}
						mGLListAdapter.removeIndex(trueIndex);
						mTotalCount = mGLListAdapter.getCount();
					}

				}
				else{//删除的是最后一个
					GLRectView _tempView = GLListView.this.getView(0);
					float _tempFloat = (_tempView.getWidth() + _tempView.getMarginLeft() + _tempView.getMarginRight() + GLListView.this.mItemSpacing);

					if(mTotalCount < mNumOneScreen || mStartIndex == 0){//只有一页，不做处理
						GLListView.this.removeView(mFocusIndex);
						mGLListAdapter.removeIndex(mFocusIndex);
						mTotalCount = mGLListAdapter.getCount();
						for(int i=mFocusIndex;i<mTotalCount;i++){
							GLRectView nView = GLListView.this.getView(i);
							float _oldX = nView.getX();
							nView.setX(_oldX-_tempFloat);
						}
						if(mFocusIndex == mTotalCount){
							mFocusIndex = mTotalCount -1;
						}

					}
					else{//前面还有数据
						mStartIndex = mStartIndex - 1;
						GLRectView fvpre = getAdapterView(mFocusIndex, mStartIndex, null, GLListView.this, true);
						if (_tempView != null && fvpre!=null) {
							fvpre.setX(_tempView.getX() - _tempFloat);
							fvpre.setY(GLListView.this.getY() + fvpre.getMarginTop());
							fvpre.setVisible(true);
							fvpre.setDepth(mDefaultDepth);
							GLListView.this.addView(0, fvpre);
							GLListView.this.removeView(mFocusIndex+1);
							for(int i=mFocusIndex;i>=0;i--){
								GLRectView pView = GLListView.this.getView(i);
								float _oldX = pView.getX();
								pView.setX(_oldX+_tempFloat);
							}
							mGLListAdapter.removeIndex(trueIndex);
							mTotalCount = mGLListAdapter.getCount();
						}
					}
				}



				mSelectedIndex = mStartIndex+mFocusIndex;
				if(mRemoveListener != null){
					mRemoveListener.back(mFocusIndex, mSelectedIndex);
				}
				if( isOpenHeadControl()){
					resetFocusForHead();
				}
				else{
					//焦点重新设定
					GLRectView _tempbig = GLListView.this.getView(mFocusIndex);
					if(_tempbig == null) {
						return;
					}
					//回调选中事件
					if(mOnItemSelectedListener!=null){
						mOnItemSelectedListener.onItemSelected(null, _tempbig, mFocusIndex, mSelectedIndex);
					}
				}



			}
		});
	}

	/**
	 *
	 * 初始化时获取焦点
	 * @author wang.f.z
	 * @date: 2015-3-9 下午1:51:19 <br/>
	 */
	public void setFocus() {
		setFocus(mFocusIndex);
	}

	public void setFocus(int index){
		if(isOpenHeadControl()) return;
		if (index<0) {
			mFocusIndex = -1;
			mFocusPreIndex = -1;
			mSelectedIndex = -1;
			moveLeft();
			return;
		}
		if (index >= mTotalCount) {
			return;
		}

		mFocusIndex = index;

		//获取焦点
		if(mOnItemSelectedListener!=null){
			GLRectView _view = GLListView.this.getView(mFocusIndex);
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
			GLRectView _view = GLListView.this.getView(mFocusIndex);
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
			_tempList.add(GLListView.this.getView(i));
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

	/**
	 * 更新数据内容
	 * @author wang.f.z
	 */
	public void notifyData(){
		List<GLRectView> list = this.getView();
		if(mGLListAdapter == null) return;
		for(int i=0;i<list.size();i++){
			mGLListAdapter.getGLView(i, list.get(i), this);
		}
	}



}
