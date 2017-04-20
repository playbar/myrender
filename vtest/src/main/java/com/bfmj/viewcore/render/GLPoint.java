package com.bfmj.viewcore.render;

/**
 * 
 * ClassName: GLPoint <br/>
 * @author lixianke    
 * @date: 2015-3-16 下午3:40:30 <br/>  
 * description:
 */
public class GLPoint {
	private float x;
	private float y;
	private float z;
	private float size;
	private GLColor color;
	
	public GLPoint(float x, float y, float z, float size, GLColor color){
		this.x = x;
		this.y = y;
		this.z = z;
		this.size = size;
		this.color = color;
	}

	public float getX() {
		return x;
	}

	public void setX(float x) {
		this.x = x;
	}

	public float getY() {
		return y;
	}

	public void setY(float y) {
		this.y = y;
	}

	public float getZ() {
		return z;
	}

	public void setZ(float z) {
		this.z = z;
	}

	public float getSize() {
		return size;
	}

	public void setSize(float size) {
		this.size = size;
	}

	public GLColor getColor() {
		return color;
	}

	public void setColor(GLColor color) {
		this.color = color;
	}
}
