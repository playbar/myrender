package com.bfmj.viewcore.util;

import java.security.acl.Group;
import java.util.ArrayList;

import android.annotation.SuppressLint;
import android.util.Log;

import com.baofeng.mojing.MojingSDK;
import com.bfmj.distortion.Logger;
import com.bfmj.viewcore.render.GLScreenParams;
import com.bfmj.viewcore.render.GLVector2;
import com.bfmj.viewcore.render.GLVector3;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLGroupView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLView;

/**
 * 
 * ClassName: GLFocusUtils <br/>
 * @author lixianke    
 * @date: 2015-3-9 下午1:52:08 <br/>  
 * description:
 */
public class GLFocusUtils {
	private static final String TAG = "GLFocusUtils";
	public static final int TO_LEFT = 0;
	public static final int TO_RIGHT = 1;
	public static final int TO_UP = 2;
	public static final int TO_DOWN = 3;
	public static final int TO_UNKNOWN = 4;
	public static float[] headView;
	
	public static boolean isOpenHeadControl = true;
	
	private static OnCursorDepthChangeListener mCursorDepthChangeListener;
	
	private static GLRectView mFocusedView;
	private static int mComputeTimes = 0;
	
	/**
	 * 开启头控
	 * @author lixianke  @Date 2015-3-20 上午11:43:48
	 * @param 
	 * @return
	 */
	public static void openHeadControl(){
		isOpenHeadControl = true;		
	}
	
	/**
	 * 关闭头控
	 * @author lixianke  @Date 2015-3-20 上午11:43:48
	 * @param 
	 * @return
	 */
	public static void closeHeadControl(){
		isOpenHeadControl = false;
	}
	
	/**
	 * 获取拥有焦点的view索引
	 * @author lixianke  @Date 2015-3-20 上午11:45:07
	 * @param 
	 * @return 索引
	 */
	public static GLRectView getFocusedView(){
		return mFocusedView;
	}

	public static void setFousedView(GLRectView view){
//		mFocusedView = view;
	}

	public static void getEulerAngles(float[] eulerAngles, int offset) {
		getEulerAngles(headView, eulerAngles, offset);
	}
	
	@SuppressLint("FloatMath")
	public static void getEulerAngles(float[] headView, float[] eulerAngles, int offset) {
		if (offset + 3 > eulerAngles.length) {
			throw new IllegalArgumentException(
					"Not enough space to write the result");
		}
		float pitch = (float) Math.asin(headView[6]);
		float roll;
		float yaw;
		if (Math.sqrt(1.0F - headView[6] * headView[6]) >= 0.01F) {
			yaw = (float) Math.atan2(-headView[2], headView[10]);
			roll = (float) Math.atan2(-headView[4], headView[5]);
		} else {
			yaw = 0.0F;
			roll = (float) Math.atan2(headView[1], headView[0]);
		}
		
//		Log.d("video", "yaw = " + Math.toDegrees(-yaw) + "; pitch = " + Math.toDegrees(-pitch) + "; roll = " + Math.toDegrees(-roll));
		
		eulerAngles[(offset + 0)] = (-yaw);
		eulerAngles[(offset + 1)] = (-pitch);
		eulerAngles[(offset + 2)] = (-roll);
	}

	/**
	 * 获取实际坐标x值
	 * @param x
	 * @return
     */
	public static float getX(float x){
		return (x - GLScreenParams.getXDpi() / 2) / GLScreenParams.getXDpi() * GLScreenParams.getScreenWidth();
	}

	/**
	 * 获取实际坐标y值
	 * @param y
	 * @return
     */
	public static float getY(float y){
		return (GLScreenParams.getYDpi() / 2 - y) / GLScreenParams.getYDpi() * GLScreenParams.getScreenHeight();
	}

	private static int[] mCurosrPosition = new int[]{-1, -1};

	/**
	 * 获取当前的焦点位置
	 * @return 焦点位置
     */
	public static int[] getCursorPosition(){
		return mCurosrPosition;
	}

	public static float[] getInverser(float[] vec)
	{
		float []dst = new float[4];
		float n = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3];
		if (n == 1.0f)
		{
			dst[0] = vec[0];
			dst[1] = -vec[1];
			dst[2] = -vec[2];
			dst[3] = -vec[3];
			//w = w;

			return dst;
		}

		// Too close to zero.
		if (n < 0.000001f)
			return dst;

		n = 1.0f / n;
		dst[0] = vec[0] * n;
		dst[1] = -vec[1] * n;
		dst[2] = -vec[2] * n;
		dst[3] = -vec[3] * n;
		return dst;
	}

	public static float[] getVector(){
		float []dst = new float[4];
		float []val = new float[4];
		float[] q = new float[4];
		MojingSDK.getLastHeadQuarternion(q);
		float [] vec = new float[]{0, 0, -1, 0};

		dst[0] = q[0] * vec[0] + q[1]*vec[3] + q[2]*vec[2] - q[3]*vec[1];
		dst[1] = q[0] * vec[1] - q[1]*vec[2] + q[2]*vec[3] + q[3]*vec[0];
		dst[2] = q[0] * vec[2] + q[1]*vec[1] - q[2]*vec[0] + q[3]*vec[3];
		dst[3] = q[0] * vec[3] - q[1]*vec[0] - q[2]*vec[1] - q[3]*vec[2];

		/////

		float [] invec = getInverser( q );
		val[0] = dst[0] * invec[0] + dst[1]*invec[3] + dst[2]*invec[2] - dst[3]*invec[1];
		val[1] = dst[0] * invec[1] - dst[1]*invec[2] + dst[2]*invec[3] + dst[3]*invec[0];
		val[2] = dst[0] * invec[2] + dst[1]*invec[1] - dst[2]*invec[0] + dst[3]*invec[3];
		val[3] = dst[0] * invec[3] - dst[1]*invec[0] - dst[2]*invec[1] - dst[3]*invec[2];

		return val;
	}

    //[in/out] dir:direction to translate AND translated direction output
    //[in] qua: quaternion for translate
    private static void transDirectionByQuaternion(float[] dir, float[] qua)
    {
        float x = qua[0];
        float y = qua[1];
        float z = qua[2];
        float w = qua[3];

        //translate quaternion to matrix
        float matrix[][] = new float[3][3];
        matrix[0][0] = 1-2*y*y-2*z*z;
        matrix[0][1] = 2*x*y+2*w*z;
        matrix[0][2] = 2*x*z-2*w*y;

        matrix[1][0] = 2*x*y-2*w*z;
        matrix[1][1] = 1-2*x*x-2*z*z;
        matrix[1][2] = 2*y*z+2*w*x;

        matrix[2][0] = 2*x*z+2*w*y;
        matrix[2][1] = 2*y*z-2*w*x;
        matrix[2][2] = 1-2*x*x-2*y*y;

        float x0 = dir[0];
        float y0 = dir[1];
        float z0 = dir[2];
        //translate direction by matrix
        float x1 = x0*matrix[0][0]+y0*matrix[1][0]+z0*matrix[2][0];
        float y1 = x0*matrix[0][1]+y0*matrix[1][1]+z0*matrix[2][1];
        float z1 = x0*matrix[0][2]+y0*matrix[1][2]+z0*matrix[2][2];

        dir[0] = x1;
        dir[1] = y1;
        dir[2] = z1;
    }

    private static float[] mRemoteQuaternion = new float[]{0,0,0,1};

    private static float[] mRemoteDirection = new float[]{0,0,-1};
    private static void getRemoteDirection(float [] direction)
    {
        direction[0] = mRemoteDirection[0];
        direction[1] = mRemoteDirection[1];
        direction[2] = mRemoteDirection[2];
        direction[3] = 0;

        transDirectionByQuaternion(direction, mRemoteQuaternion);
    }

    public static void setRemoteDirection(float[] direction)
    {
        mRemoteDirection[0] = direction[0];
        mRemoteDirection[1] = direction[1];
        mRemoteDirection[2] = direction[2];
    }

    private static float[] mRemotePos = new float[]{0,0,0};
    public static void setRemotePos(float[] pos)
    {
        mRemotePos[0] = pos[0];
        mRemotePos[1] = pos[1];
        mRemotePos[2] = pos[2];
    }

    private static void getRemotePos(float[] pos)
    {
        pos[0] = mRemotePos[0];
        pos[1] = mRemotePos[1];
        pos[2] = mRemotePos[2];
    }
    private static float[] mRemoteIntersectionPos = new float[]{0,0,-1};
    private static float[] mRemoteIntersectionAirpos = new float[]{0,0,-1};
    private static boolean mRemoteIntersection = false;
    private static void setRemoteIntersectionPos(boolean intersection, float[] pos)
    {
        mRemoteIntersection = intersection;
        mRemoteIntersectionPos[0] = pos[0];
        mRemoteIntersectionPos[1] = pos[1];
        mRemoteIntersectionPos[2] = pos[2];

        if(intersection)
        {
            GLVector3 airpos = new GLVector3(pos[0], pos[1], pos[2]);
            GLVector3.Normalize(airpos);
            mRemoteIntersectionAirpos[0] = airpos.x;
            mRemoteIntersectionAirpos[1] = airpos.y;
            mRemoteIntersectionAirpos[2] = airpos.z;
        }

        //Log.d(TAG, String.format("intersection="+intersection+" inter pos:(%.03f,%.03f,%.03f) airpos:(%.03f,%.03f,%.03f)", mRemoteIntersectionPos[0], mRemoteIntersectionPos[1], mRemoteIntersectionPos[2], mRemoteIntersectionAirpos[0],mRemoteIntersectionAirpos[1],mRemoteIntersectionAirpos[2]));
    }

    public static boolean getRemoteIntersectionPos(float[] pos, float[] airpos)
    {
        for(int i=0;i<3;i++)
        {
            pos[i] = mRemoteIntersectionPos[i];
            airpos[i] = mRemoteIntersectionAirpos[i];
        }

        return mRemoteIntersection;
    }

    //cal no focus intersection pos
    //[in] pos:remote pos
    //[in] intersectionPos: remote direction
    //[out] airpos: return intersection pos for no foused situation
    private static void calcAirpos(float[] pos, float[] vec, float[] airpos)
    {
        GLVector3 t = new GLVector3(vec[0], vec[1], vec[2]);
        GLVector3.Normalize(t);
        airpos[0] = t.x+pos[0];
        airpos[1] = t.y+pos[1];
        airpos[2] = t.z+pos[2];
    }

	/**
	 * 处理焦点
	 * @author lixianke  @Date 2015-3-16 上午10:52:11
	 * @param headView 陀螺仪矩阵
	 * @param views View列表
	 * @return 
	 */
	public static void handleFocused(float[] headView, ArrayList<GLView> views){
		GLFocusUtils.headView = headView;
		
		if (!isOpenHeadControl){
			return;
		}
		
		if (mComputeTimes < 10){
			mComputeTimes++;
			return;
		} else {
			mComputeTimes = 0;
		}

		float defualtDepth = GLScreenParams.getDefualtDepth();
		boolean hasFocused = false;

		float x1 = getX(0);
		float y1 = getY(0);

		float x2 = getX(GLScreenParams.getXDpi());
		float y2 = getY(GLScreenParams.getYDpi());

		float[] outPos = new float[]{0, 0};

		boolean isFoused = MojingSDK.DirectionalRadiaInRect(headView, new float[]{x1, y1}, new float[]{x2, y2}, -defualtDepth, outPos);

		float rate = GLScreenParams.getXDpi() / GLScreenParams.getScreenWidth();

		mCurosrPosition[0] = (int)(outPos[0] * rate);
		mCurosrPosition[1] = (int)(outPos[1] * rate);

		float [] vec = new float[]{0, 0, -1, 0};
		float [] pos = new float[] {0,0,0};
		float [] intersectionPos = new float[] {0,0,0};
		boolean intersection = false;
		getRemoteDirection(vec);
		getRemotePos(pos);

		for (int i = views.size() - 1; i >= 0; i--) {
			if (!(views.get(i) instanceof GLRectView)) {
				continue;
			}

			GLRectView v = (GLRectView) views.get(i);
			if (!v.isVisible() || v instanceof GLCursorView) {
				continue;
			}

			float vx1 = getX(v.getLeft() + v.getX());
			float vy1 = getY(v.getTop() + v.getY());
			float vx2 = getX(v.getLeft() + v.getX() + v.getWidth());
			float vy2 = getY(v.getTop() + v.getY() + v.getHeight());
			float z = -v.getDepth();

//			if( v.getAngelY() > 0.01) {
//				float centerx = v.getCenterX();
//				float len = vx2 - centerx;
//				float aa = len / 2;
//				float vx22 = 3.5f*vx2 / (aa + 3.5f);
//				vx2 = vx22;
//				vx1 = vx1*8/7;
//			}
//			else if( v.getAngelY() < -0.01){
//				float centerx = v.getCenterX();
//				float len = centerx - vx1;
//				float aa = len /2;
//				float vx11 = 3.5f*vx1 / (aa + 3.5f);
//				vx1 = vx11;
//				vx2 = vx2 * 8 / 7;
//			}


//			float[] q = new float[4];
//			MojingSDK.getLastHeadQuarternion(q);
//			Log.e("FocusUtil", "x=" + q[0] + ",y=" + q[1] + ",z=" + q[2] + ",w=" + q[3]);

			/////////
			float centerx = v.getCenterX();
			float len = vx2 - centerx;
			float radian = (float) (Math.PI / 180 * (v.getAngelY()));
			float fsin = (float) Math.sin(radian);
			float depth = len * fsin;

			//3 points of control view
			GLVector3 tl = new GLVector3(vx1-pos[0], vy1-pos[1], z + depth-pos[2]);
			GLVector3 tr = new GLVector3(vx2-pos[0], vy1-pos[1], z - depth-pos[2]);
			GLVector3 bl = new GLVector3(vx1-pos[0], vy2-pos[1], z + depth-pos[2]);

			//init intersction class with control view
			IntersectionTest test = new IntersectionTest(tl, tr, bl);

			//translate direction to headview or custom view
			float []ret = IntersectionTest.vecMulMatrxi(vec, v.isCostomHeadView() ? v.getMatrixState().getVMatrix() : headView );

			//translated direction
			GLVector3 transDirection = new GLVector3(ret[0], ret[1], ret[2]);
			intersectionPos[0] = transDirection.x;
			intersectionPos[1] = transDirection.y;
			intersectionPos[2] = transDirection.z;

			//[output var] pos on the control view
			GLVector2 vec2 = new GLVector2();

			//do intersection detect
			intersection = test.Intersection( transDirection, vec2);

//			if (MojingSDK.DirectionalRadiaInRect(v.isCostomHeadView() ? v.getMatrixState().getVMatrix() : headView, new float[]{vx1, vy1}, new float[]{vx2, vy2}, z, new float[2])){
			if(intersection){
				intersectionPos[0] = tl.x+vec2.s*(tr.x-tl.x);
				intersectionPos[1] = tl.y+vec2.t*(bl.y-tl.y);
				intersectionPos[2] = tl.z;

				if (v.hasListeter()){
					hasFocused = true;
				} else if (getHasListenerParent(v) != null){
					v = getHasListenerParent(v);
					hasFocused = true;
				} else {
					hasFocused = false;
				}
				if (hasFocused && v != mFocusedView && v.isEnable()) {
					if (mFocusedView != null) {
						if (mFocusedView.isGrandParent(v)){ //  if v is mFocusedView's parent, first let v' child lost focus
							GLRectView child = ((GLGroupView)v).getFocusedChild();
							if (child != null){
								child.onFocusChange(TO_UNKNOWN, false);
							}
						} else if (!v.isGrandParent(mFocusedView)){ //  if mFocusedView is not v's parent
							mFocusedView.onFocusChange(TO_UNKNOWN, false);
						}
					}

					v.doRequestFocus();
					mFocusedView = v;
				}
				break;
			}
		}

		if (!hasFocused) {
			lostAllViewFocus();
			float[] airpos = new float[] {0,0,0};
			calcAirpos(pos, intersectionPos, airpos);
			setRemoteIntersectionPos(intersection, airpos);
		}
		else
		{
			setRemoteIntersectionPos(intersection, intersectionPos);
		}
	}

	public static void lostAllViewFocus(){
		if (mFocusedView != null){
			mFocusedView.onFocusChange(TO_UNKNOWN, false);
			GLRectView parent = mFocusedView.getParent();
			if (parent != null && parent instanceof GLGroupView){
				((GLGroupView)parent).lostParentFocus();
			}
			mFocusedView = null;
		}
	}

	private static GLGroupView getHasListenerParent(GLRectView view){
		GLGroupView parent = view.getParent();
		while (parent != null){
			if (parent.hasListeter()){
				return  parent;
			}
			parent = parent.getParent();
		}
		return null;
	}

	public static float[] getPosition(float[] headView, float depth){
		float x1 = getX(0);
		float y1 = getY(0);

		float x2 = getX(GLScreenParams.getXDpi());
		float y2 = getY(GLScreenParams.getYDpi());

		float[] outPos = new float[]{0, 0};

		boolean isFoused = MojingSDK.DirectionalRadiaInRect(headView, new float[]{x1, y1}, new float[]{x2, y2}, -depth, outPos);

		float rate = GLScreenParams.getXDpi() / GLScreenParams.getScreenWidth();
		outPos[0] = (int)(outPos[0] * rate);
		outPos[1] = (int)(outPos[1] * rate);
		return outPos;
	}
	
	/**
	 * 处理焦点
	 * @author lixianke  @Date 2015-3-16 上午10:52:11
	 * @param views View列表
	 * @return 
	 */
	public void handleFocused(ArrayList<GLView> views){
		if (!isOpenHeadControl){
			return;
		}
		
		handleFocused(GLFocusUtils.headView, views);
	}
	
	/**
	 * 处理焦点
	 * @author lixianke  @Date 2015-3-16 上午10:52:11
	 * @param views View列表
	 * @return 
	 */
	public static boolean handleFocused(int direction, GLRectView view, ArrayList<GLRectView> views){
		if (isOpenHeadControl){
			return false;
		}
		
		mFocusedView = view;
		
		GLRectView focusedView = null;
		float x = -10000;
		float y = -10000;
		float vx11 = 0;
		float vx12 = 0;
		float vy11 = 0;
		float vy12 = 0;
		float vx21 = 0;
		float vx22 = 0;
		float vy21 = 0;
		float vy22 = 0;
		
		for (int i = 0; i < views.size(); i++) {
			GLRectView v = views.get(i);
			
			if (!v.isFocusable() || !v.isVisible() || !v.isEnable()){
				continue;
			}
			
			vx11 = v.getLeft() + v.getX();
			vx12 = v.getLeft() + v.getX() + v.getWidth();
			vy11 = v.getTop() + v.getY();
			vy12 = v.getTop() + v.getY() + v.getHeight();
			
			switch (direction) {
				case TO_LEFT:
					if (view != null){
						vx21 = view.getLeft() + view.getX();
						vy21 = view.getTop() + view.getY();
						vy22 = view.getTop() + view.getY() + view.getHeight();
						if (vx12 <= vx21 && (vx12 > x || (vx12 == x && Math.abs(vy21 + vy22 - vy11 - vy12) < Math.abs(vy21 + vy22 - y)))){
							x = vx12;
							y = vy11 + vy12;
							focusedView = v;
						}
					} else {
						if (vx12 > x){
							x = vx12;
							focusedView = v;
						}
					}
					break;
				case TO_RIGHT:
					if (view != null){
						vx22 = view.getLeft() + view.getX() + view.getWidth();
						vy21 = view.getTop() + view.getY();
						vy22 = view.getTop() + view.getY() + view.getHeight();
						if (vx11 >= vx22 && ((vx11 < x || x == -10000) || (vx11 == x && Math.abs(vy21 + vy22 - vy11 - vy12) < Math.abs(vy21 + vy22 - y)))){
							x = vx11;
							y = vy11 + vy12;
							focusedView = v;
						}
					} else {
						if (vx11 < x || x == -10000){
							x = vx11;
							focusedView = v;
						}
					}
					break;
				case TO_UP:
					if (view != null){
						vx21 = view.getLeft() + view.getX();
						vx22 = view.getLeft() + view.getX() + view.getWidth();
						vy21 = view.getTop() + view.getY();
						if (vy12 <= vy21 && (vy12 > y || (vy12 == y && Math.abs(vx21 + vx22 - vx11 - vx12) < Math.abs(vx21 + vx22 - x)))){
							x = vx11 + vx12;
							y = vy12;
							focusedView = v;
						}
					} else {
						if (vy12 > y){
							y = vy12;
							focusedView = v;
						}
					}
					break;
				case TO_DOWN:
					if (view != null){
						vx21 = view.getLeft() + view.getX();
						vx22 = view.getLeft() + view.getX() + view.getWidth();
						vy22 = view.getTop() + view.getY() + view.getHeight();
						if (vy11 >= vy22 && ((vy11 < y || y == -10000) || (vy11 == y && Math.abs(vx21 + vx22 - vx11 - vx12) < Math.abs(vx21 + vx22 - x)))){
							x = vx11 + vx12;
							y = vy11;
							focusedView = v;
						}
					} else {
						if (vy11 > y || y == -10000){
							y = vy11;
							focusedView = v;
						}
					}
					break;
				default:
					break;
			}
		}
		
		if (focusedView != null && mFocusedView != focusedView){
			if (mFocusedView != null){
				mFocusedView.onFocusChange(direction, false);
			}
			focusedView.onFocusChange(direction, true);
			mFocusedView = focusedView;
			
			return true;
		}
		
		return false;
	}
	
	public static void setOnCursorDepthChangeListener(OnCursorDepthChangeListener listener){
		mCursorDepthChangeListener = listener;
	}
	
	public interface OnCursorDepthChangeListener {
		void onCursorDepthChange(float depth);
	}
}
