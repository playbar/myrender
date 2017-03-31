package com.bfmj.viewcore.animation;

/**
 * 动画转换类，应用层不直接使用。
 * @author zhangxin    
 * @Date: 2015-3-10 下午2:04:11 
 */
public class GLTransformation {
	public static final int TYPE_IDENTITY = 0x0;

	public static final int TYPE_ALPHA = 0x1;
	public static final int TYPE_MATRIX = 0x2;
	public static final int TYPE_BOTH = TYPE_ALPHA | TYPE_MATRIX;

	protected float[] mValues;
	protected float mAlpha = 1.0f;
	protected float mDegree;
	protected int mTransformationType;
	
	protected float mX;
	protected float mY;
	protected float mZ;
	
	public GLTransformation() {
    }
	
	public GLTransformation(float[] values) {
		mValues = values;
		mAlpha = 1.0f;
		mTransformationType = TYPE_BOTH;
    }
	
	public void init() {
		if (mValues == null) {
			mValues = new float[16];
		} else {
			for(int i=0; i<16; i++) {
				mValues[i]=1; 
			}
		}
		mAlpha = 1.0f;
		mTransformationType = TYPE_BOTH;
	}

	public float[] getValues() {
		return mValues;
	}

	public void setValues(float[] values) {
		for(int i=0; i<16; i++) {
			this.mValues[i] = values[i];
		}
	}

	public void setValuesPoint(float[] values) {
		this.mValues = values;
	}
	
	public float getAlpha() {
		return mAlpha;
	}

	public void setAlpha(float alpha) {
		this.mAlpha = alpha;
	}

	public int getTransformationType() {
		return mTransformationType;
	}

	public void setTransformationType(int transformationType) {
		this.mTransformationType = transformationType;
	}

	public float getX() {
		return mX;
	}

	public void setXYZ(float mX,float mY,float mZ) {
		this.mX = mX;
		this.mY = mY;
		this.mZ = mZ;
	}

	public float getY() {
		return mY;
	}

	public float getZ() {
		return mZ;
	}

	public float getDegree() {
		return mDegree;
	}

	public void setDegree(float mDegree) {
		this.mDegree = mDegree;
	}
	
	public void setZ(float z) {
		mZ = z;
	}
}
