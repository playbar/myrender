package com.bfmj.viewcore.view;

import java.util.Timer;
import java.util.TimerTask;

import com.bfmj.viewcore.interfaces.IGLPlayerListener;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.render.GLRenderParams;
import com.bfmj.viewcore.util.GLTextureUtils;
import android.content.Context;
import android.graphics.SurfaceTexture;

import com.bfmj.viewcore.render.GLVideoRect;

/**
 * GL播放器
 * ClassName: GLPlayerView <br/>
 * @author lixianke    
 * @date: 2015-3-9 下午1:51:19 <br/>  
 * description:
 */
public abstract class GLPlayerView extends GLRectView {
	public static int MODE_2D = 0x01;
	public static int MODE_3D_LEFT_RIGHT = 0x02;
	public static int MODE_3D_UP_DOWN = 0x03;
	
	private SurfaceTexture mSurfaceTexture;
	private GLRenderParams mRenderParams;
	private int mTextureId;
	private float mVideoWidth = 0;
	private float mVideoHeight = 0;
	private String mPath;
	private int mPlayMode = MODE_2D;
	private int mAngle = 0;
	private IGLPlayerListener mListener;
	private boolean isSurfaceCreated = false;
	private Timer mTimer;
	
	public GLPlayerView(Context context) {
		super(context);
		setBackground(new GLColor(0, 0, 0));
	}
	
	abstract protected boolean openVideo();
	
	/**
	 * 重置播放状态
	 * @author lixianke  @Date 2015-3-17 下午3:37:26
	 * @param
	 * @return
	 */
	abstract protected void reset();
	
	/**
	 * 开始播放
	 * @author lixianke  @Date 2015-3-17 下午3:38:06
	 * @param  
	 * @return
	 */
	 public void start(){
//		 if (mSurfaceTexture != null){
//			 mSurfaceTexture.setOnFrameAvailableListener(this);
//		 }
		 if (mRenderParams != null && mTextureId > 0){
			 mRenderParams.setTextureId(mTextureId);
		 }
		 startRender();
	 }
	 
	 private void startRender(){
		 stopRender();
		 
		 mTimer = new Timer();
		 mTimer.schedule(new TimerTask() {
			
			@Override
			public void run() {
				BaseViewActivity context = ((BaseViewActivity)getContext());
				if (context != null && context.getRootView() != null) {
					GLRootView rootView = context.getRootView();
					if (rootView.getRenderMode() == GLRootView.RENDERMODE_WHEN_DIRTY){
						rootView.requestRender();
					}
				}
			}
		}, 40, 40);
	 }
	 
	 private void stopRender(){
		 if (mTimer != null){
			 mTimer.cancel();
			 mTimer = null;
		 }
	 }
	
	/**
	 * 暂停播放
	 * @author lixianke  @Date 2015-3-17 下午3:38:30
	 * @param   
	 * @return 
	 */
	 public void pause(){
//		 if (mSurfaceTexture != null){
//			 mSurfaceTexture.setOnFrameAvailableListener(null);
//		 }
		 
		 stopRender();
	 }
	
	/**
	 * 停止播放
	 * @author lixianke  @Date 2015-3-17 下午3:38:59
	 * @param  
	 * @return 
	 */
	abstract public void stop();
	
	/**
	 * 释放资源
	 * @author lixianke  @Date 2015-3-17 下午3:39:21
	 * @param  
	 * @return 
	 */
	public void releasePlay(){
		stopRender();
	}
	
	/**
	 * 转到指定位置播放
	 * @author lixianke  @Date 2015-3-17 下午3:39:42
	 * @param pos 播放位置（毫秒） 
	 * @return 
	 */
	abstract public void seekTo(int pos);
	
	/**
	 * 获取当前的播放位置
	 * @author lixianke  @Date 2015-3-17 下午3:40:29
	 * @param 
	 * @return 播放位置（毫秒）
	 */
	abstract public int getCurrentPosition();
	
	/**
	 * 获取影片长度
	 * @author lixianke  @Date 2015-4-9 上午9:49:26
	 * @param  
	 * @return 影片长度（毫秒）
	 */
	abstract public int getDuration();
	
	/**
	 * 获取是否在正在播放
	 * @author lixianke  @Date 2015-4-9 上午11:01:16
	 * @param 
	 * @return 是否正在播放
	 */
	abstract public boolean isPlaying();
	
	/**
	 * 获取SurfaceTexture
	 * @author lixianke  @Date 2015-3-17 下午2:52:16
	 * @param 
	 * @return
	 */
	public SurfaceTexture getSurfaceTexture(){
		return mSurfaceTexture;
	}
	
	/**
	 * 设置播放地址
	 * @author lixianke  @Date 2015-3-17 下午3:36:46
	 * @param path 播放地址 
	 * @return 
	 */
	public void setVideoPath(String path){
		mPath = path;
		openVideo();
	}
	
	protected String getPath(){
		return mPath;
	}
	
	/**
	 * 设置播放事件回调
	 * @author lixianke  @Date 2015-3-17 下午3:36:01
	 * @param listener ISystemPlayerListener回调对象
	 * @return
	 */
	public void setListener(IGLPlayerListener listener){
		mListener = listener;
	}
	
	/**
	 * 设置视频尺寸
	 * @author lixianke  @Date 2015-3-17 下午2:52:43
	 * @param width 视频宽
	 * @param height 视频高 
	 * @return
	 */
	public void setVideoSize(int width, int height) {
		mVideoWidth = width;
		mVideoHeight = height;
		
		updateSize();
	}
	
	/**
	 * 是否为3d
	 * @author lixianke  @Date 2015-3-17 下午2:54:11
	 * @param 
	 * @return 是否3d
	 */
	public int getPlayMode() {
		return mPlayMode;
	}
	
	/**
	 * 设置是否为3d
	 * @author lixianke  @Date 2015-3-17 下午2:55:17
	 * @param mode 是否为3d
	 * @return 
	 */
	public void setPlayMode(int mode){
		mPlayMode = mode;
		
		if (mode == MODE_2D && mRenderParams != null){
			mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_ALL);
		}
	}
	
	/**
	 * 设置旋转角度
	 * @author lixianke  @Date 2015-3-17 下午3:33:39
	 * @param angle 角度 0~360
	 * @return 
	 */
	public void rotate(int angle){
		rotate(angle - mAngle, 0, 0, 1);
		
		mAngle = angle;
		
		if (mAngle % 360 == 90 || mAngle % 360 == 270){
			scale(getHeight() / getWidth());
		} else {
			scale(1);
		}
	}
	
	protected int getAngle() {
		return mAngle;
	}
	
	protected void playPrepared(){
		if (mListener != null){
			mListener.onPrepared(this);
		}
		startRender();
	}
	
	protected boolean playInfo(int what, Object extra){
		if (mListener != null){
			return mListener.onInfo(this, what, extra);
		}
		return false;
	}
	
	protected void playBufferingUpdate(int percent){
		if (mListener != null){
			mListener.onBufferingUpdate(this, percent);
		}
	}
	
	protected void playCompletion(){
		if (mListener != null){
			mListener.onCompletion(this);
		}
	}
	
	protected void playSeekComplete(){
		if (mListener != null){
			mListener.onSeekComplete(this);
		}
		startRender();
	}
	
	protected boolean playError(int what, int extra){
		if (mListener != null){
			return mListener.onError(this, what, extra);
		}
		return false;
	}
	protected void playVideoSizeChanged(GLPlayerView player, int width, int height){
		if (mListener != null){
			mListener.onVideoSizeChanged(this, width, height);
		}
	}
	
	protected void playTimedText(GLPlayerView player, String text){
		if (mListener != null && text != null){
			mListener.onTimedText(this, text.toString());
		}
	}
	
	private void updateSize() {
		if (mVideoWidth == 0){
			return;
		}
		
		float ratio = mVideoHeight / mVideoWidth;
		
		float width = getWidth();
		float height = getHeight();
		float left = 0;
		float top = 0;
		
		if (ratio > height / width){
			width = height / ratio;
			left = (getWidth() - width) / 2;
		} else {
			height = width * ratio;
			top = (getHeight() - height) / 2;
		}
		
		setPadding(left, top, left, top);
	}
	
	@Override
	public void setWidth(float width) {
		super.setWidth(width);
		
		updateSize();
	}
	
	@Override
	public void setHeight(float height) {
		super.setHeight(height);
		
		updateSize();
	}
	
	@Override
	public void setLayoutParams(float width, float height) {
		super.setLayoutParams(width, height);
		updateSize();
	}
	
	@Override
	protected void updateRenderSize(GLRenderParams render, float width,
			float height) {
		super.updateRenderSize(render, width, height);
		
		if (render == mRenderParams && (mVideoWidth == 0 || mVideoHeight == 0)){
			render.setScaleX(1.0f);
			render.setScaleY(1.0f);
		}
	}
	
	@Override
	public void initDraw() {
		super.initDraw();
		
		if (isSurfaceCreated == false){
			onSurfaceCreated();
		}
		
		isSurfaceCreated = true;
	}
	
	@Override
	public void onResume(){
		super.onResume();
	}
	
	@Override
	public void onPause(){
		super.onPause();
		
		isSurfaceCreated = false;
//		if (mRenderParams != null){
////			mSurfaceTexture.setOnFrameAvailableListener(null);
//			removeRender(mRenderParams);
//			mRenderParams = null;
//			return;
//		}
	}
	
	@Override
	public void onSurfaceCreated() {
		
		if (mRenderParams != null){
			removeRender(mRenderParams);
		}

		mTextureId = GLTextureUtils.createVideoTextureID();
		
		mRenderParams = new GLRenderParams(GLRenderParams.RENDER_TYPE_VIDEO);
		mRenderParams.setTextureId(mTextureId);
		updateRenderSize(mRenderParams, getInnerWidth(), getInnerHeight());
		
		mSurfaceTexture = new SurfaceTexture(mTextureId);
//		mSurfaceTexture.setOnFrameAvailableListener(this);
		
		addRender(mRenderParams);
	}

	@Override
	public void onBeforeDraw(boolean isLeft) {
		if (mRenderParams == null){
			return;
		}

		if (mSurfaceTexture != null && isLeft){
			try {
				mSurfaceTexture.updateTexImage();
				float[] mtx = new float[16];
				mSurfaceTexture.getTransformMatrix(mtx);
			} catch (RuntimeException e) {
				return;
			}

		}

		if (mRenderParams == null){
			return;
		}

		if (isReverseScreen()) {
			isLeft = !isLeft;
		}


		if (mPlayMode == MODE_3D_LEFT_RIGHT){
			if (isLeft){
				mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_LEFT);
			} else {
				mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_RIGHT);
			}
		} else if(mPlayMode == MODE_3D_UP_DOWN){
			if (isLeft){
				mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_TOP);
			} else {
				mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_BOTTOM);
			}
		} else {
			mRenderParams.setTextureType(GLVideoRect.TextureType.TEXTURE_TYPE_ALL);
		}

		super.onBeforeDraw(isLeft);
	}
	
	private boolean isReverseScreen(){
		BaseViewActivity activity = ((BaseViewActivity)getContext());
		if (activity != null) {
			GLRootView rootView = activity.getRootView();
			if (rootView != null && rootView.isReverseScreen()) {
				return true;
			}
		}
		
		return false;
	}

	@Override
	public void release() {
		if (mRenderParams != null){
			mRenderParams.setTextureId(0);
		}
	}
}
