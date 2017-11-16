package com.bfmj.viewcore.view;

import android.content.Context;
import android.opengl.Matrix;

import com.baofeng.mojing.MojingSDK;
import com.bfmj.viewcore.util.GLFocusUtils;

/**
 * 用于头控状态下的光标
 * ClassName: GLCursorView <br/>
 * @author lixianke    
 * @date: 2015-6-19 下午5:14:35 <br/>  
 * description:
 */
public class GLCursorView extends GLImageView {

	public GLCursorView(Context context) {
		super(context);
		setFixed(true);
	}

	@Override
	public void onBeforeDraw(boolean isLeft) {
		if (!isFixed()){
			float[] o = new float[3];
			MojingSDK.getLastHeadEulerAngles(o);
			GLFocusUtils.getEulerAngles(getMatrixState().getVMatrix(), o, 0);
			double zAngle = Math.toDegrees(o[2]);
			float[] mtx = new float[16];
			Matrix.setIdentityM(mtx, 0);
			if (Math.abs(zAngle) > 90.0) {
				Matrix.rotateM(mtx, 0, -(float) Math.toDegrees(o[0]), 0, 1, 0);
				Matrix.rotateM(mtx, 0, -(float)Math.toDegrees(o[1]), 1, 0, 0);
				Matrix.rotateM(mtx, 0, -(float)Math.toDegrees(o[2]), 0, 0, 1);
			} else {
				Matrix.rotateM(mtx, 0, (float) Math.toDegrees(o[0]), 0, 1, 0);
				Matrix.rotateM(mtx, 0, (float)Math.toDegrees(o[1]), 1, 0, 0);
				Matrix.rotateM(mtx, 0, (float)Math.toDegrees(o[2]), 0, 0, 1);
			}

			getMatrixState().setVMatrix(mtx);
		}
		super.onBeforeDraw(isLeft);
	}
}
