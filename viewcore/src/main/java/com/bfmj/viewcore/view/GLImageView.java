package com.bfmj.viewcore.view;

import android.content.Context;
import android.graphics.Bitmap;

/**
 *
 * ClassName: GLImageView <br/>
 * @author lixianke
 * @date: 2015-3-9 下午1:51:44 <br/>
 * description:
 */
public class GLImageView extends GLRectView {

	public GLImageView(Context context) {
		super(context);
	}

	/**
	 * 设置图片
	 * @author lixianke  @Date 2015-3-11 下午5:06:59
	 * @param resId 资源ID
	 * @return
	 */
	public void setImage(int resId){
		if (mFrontResId == resId){
			return;
		}

		mFrontResId = resId;
		mFrontBitmap = null;

		updateTexture();
	}

	/**
	 * 设置图片
	 * @author lixianke  @Date 2015-3-11 下午5:07:28
	 * @param bitmap Bitmap对象
	 * @return
	 */
	public void setImage(Bitmap bitmap){
		if (mFrontBitmap == bitmap){
			return;
		}

		mFrontBitmap = bitmap;
		mFrontResId = 0;

		updateTexture();
	}


	@Override
	public void initDraw() {
		super.initDraw();
	}

	/**
	 * 设置是否裁边
	 * @author linzanxian  @Date 2015-6-30 下午3:24:24
	 * @param cutting 是否裁边
	 * @return void
	 */
	public void setCutting(boolean cutting) {
		mIsCutting = cutting;
	}

	@Override
	public void release() {
		super.release();
	}
}
