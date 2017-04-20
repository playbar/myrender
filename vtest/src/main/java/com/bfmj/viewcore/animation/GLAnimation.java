package com.bfmj.viewcore.animation;


import com.bfmj.viewcore.view.GLRectView;

/**
 * 动画基类，应用层不直接使用
 * @author zhangxin    
 */
public abstract class GLAnimation {

	public final static int ABSOLUTE = 0;
	public final static int RELATIVE_TO_SELF = 1;
	public final static int RELATIVE_TO_PARENT = 2;
	
	//绘制动画对象调用getTransformation取得变换矩阵前，把此变量传给mStartTime
	public static final int START_ON_FIRST_FRAME = -1;
	
	//绘制动画对象开始绘制前，把START_ON_FIRST_FRAME传给该变量；
	long mStartTime = -1;

	// 毫秒，动画的执行市场
	long mDuration = 1500;

	// 毫秒，动画延迟多少毫秒后开始执行
	//long mStartOffset = 0;

	// 动画事件的回调对象
	OnGLAnimationListener mListener;

	//动画是否开始
	boolean mStarted = false;
	
	//动画是否结束
	boolean mEnded = false;
	
	//动画执行时间
	float sumTime = 0;
	
	//上次时间
	long lastTime = -1; 
	
    private GLTransformation mGlTransformation;
    private boolean isStart=false;
    private boolean isEnd=false;
    private boolean isOnlyChids=false;
    protected GLRectView mAnimView;
    protected float startX;
    protected float startY;
    protected float startZ;
    protected float startScaleX;
    protected float startScaleY;
    
	public GLAnimation() {
	}
	
	public void getStartAnimViewLayout() {
		if (mAnimView != null) {
			startX = mAnimView.getX();
			startY = mAnimView.getY();
			startZ = mAnimView.getDepth();
			
			startScaleX = mAnimView.getScaleX();
			startScaleY = mAnimView.getScaleY();
		}
	}
	
    /**
     * 设置动画时长
     * @Date 2015-6-19 下午4:41:11
     * @param durationMillis:动画时长，毫秒。
     * @return
     */
	public void setDuration(long durationMillis) {
		if (durationMillis < 0) {
			throw new IllegalArgumentException(
					"Animation duration cannot be negative");
		}
		mDuration = durationMillis;
	}

	public long getmDuration() {
		return mDuration;
	}

	public void setmDuration(long mDuration) {
		this.mDuration = mDuration;
	}

    /**
     * 设置动画回调事件。
     * @Date 2015-6-19 下午4:41:11
     * @param listener:回调事件对象
     */
	public void setOnGLAnimationListener(OnGLAnimationListener listener) {
		mListener = listener;
	}

    /**
     * @author zhangxin  @Date 2015-6-19 下午4:41:11
     * description: 动画回调事件接口。
     */
	public  interface OnGLAnimationListener {
		/**
		 * 动画开始回调事件
		 * @param animation 动画对象
		 * */
		void onAnimationStart(GLAnimation animation);
		/**
		 * 动画结束回调事件
		 * @param animation 动画对象
		 * */
		void onAnimationEnd(GLAnimation animation);
	}


	public void fireAnimationStart() {
		if (mListener != null) {
			mListener.onAnimationStart(this);
		}
	}


	public void fireAnimationEnd() {
		if (mListener != null) {
			mListener.onAnimationEnd(this);
		}

	}

	/**
	 * @author zhangxin  @Date 2015-3-10 上午9:42:12
	 * description: 根据绝对尺寸，相对于自身，相对于父类 计算动画变换尺寸
	 * @param 
	 * @return
	 */
	protected float resolveSize(int type, float value, int size, int parentSize) {
		switch (type) {
		case ABSOLUTE:
			return value;
		case RELATIVE_TO_SELF:
			return size * value;
		case RELATIVE_TO_PARENT:
			return parentSize * value;
		default:
			return value;
		}
	}

	
	/**
	 * 动画绘制对象每帧调用该方法，取得变换矩阵或alpha值，应用层无需使用
	 * @Date 2015-3-10 上午9:36:36
	 * @param currentTime：动画绘制对象的绘制调用时间；
	 * @param outTransformation：返回变换矩阵或alpha值的封装
	 * @return true：动画过期或结束
	 */
	public boolean getTransformation(long currentTime,
			GLTransformation outTransformation) {
		
		if (mStartTime == -1) {
			mStartTime = currentTime;
			lastTime = currentTime;
		}

		final long duration = mDuration;
		float normalizedTime;
		long dTime=0;
		
		if (duration != 0) {
			dTime = currentTime - lastTime;
			normalizedTime = ((float) (currentTime - lastTime))
					/ (float) duration;
		} else {
			normalizedTime = currentTime < mStartTime ? 0.0f : 1.0f;
		}
		
		lastTime = currentTime;
		sumTime += dTime;	
		
		//Log.d("test","normalizedTime:"+normalizedTime+";sumTime:"+sumTime+";mDuration="+mDuration);		

		if (!mStarted) {
			fireAnimationStart();
			mStarted = true;

		}

		applyTransformation(normalizedTime, outTransformation);
		
		//到达动画时间，动画结束
		if (sumTime >= mDuration) {
			if (!mEnded) {
				mEnded = true;
				fireAnimationEnd();
			}
			return true;
		}
		
		return false;
	}

	public void setStartTime(long startTimeMillis) {
		mStartTime = startTimeMillis;
		mStarted = mEnded = false;
	}
    
	/**
	 * @Date 2015-3-10 上午9:27:05
	 * description: 动画view对象开始动画前调用
	 * @param 
	 * @return
	 */
	public void start() {
        setStartTime(START_ON_FIRST_FRAME);
    }
    
    
    /**
     * @author zhangxin  @Date 2015-3-10 上午9:43:59
     * description:子类实现
     * @param 
     * @return
     */
    abstract protected void applyTransformation(float interpolatedTime, GLTransformation t);
    
    public GLTransformation getGlTransformation() {
		return mGlTransformation;
	}

	public void setGlTransformation(GLTransformation mGlTransformation) {
		this.mGlTransformation = mGlTransformation;
	}

	public boolean isStart() {
		return isStart;
	}

	public void setStart(boolean isStart) {
		this.isStart = isStart;
		getStartAnimViewLayout();
	}

	public boolean isEnd() {
		return isEnd;
	}

	public void setEnd(boolean isEnd) {
		this.isEnd = isEnd;
	}

	public GLRectView getAnimView() {
		return mAnimView;
	}

    /**
     * 设置执行动画的view对象。
     * @Date 2015-6-19 下午4:41:11
     * @param mAnimView：执行动画的view对象
     */
	public void setAnimView(GLRectView mAnimView) {
		this.mAnimView = mAnimView;
	}

	public boolean isOnlyChids() {
		return isOnlyChids;
	}

    /**
     * 设置是否只有子view执行动画。
     * @Date 2015-6-19 下午4:41:11
     * @param isOnlyChids：true：只有子view执行动画；false：父子view都执行动画
     */
	public void setOnlyChids(boolean isOnlyChids) {
		this.isOnlyChids = isOnlyChids;
	}


}
