package com.bfmj.viewcore.render;

/**
 * 
 * ClassName: GLRenderParams <br/>
 * @author lixianke    
 * @date: 2015-3-31 上午10:22:39 <br/>  
 * description:
 */
public class GLRenderParams {
	public static final int RENDER_TYPE_COLOR = 0;
	public static final int RENDER_TYPE_IMAGE = 1;
	public static final int RENDER_TYPE_VIDEO = 2;
	
	private int type = RENDER_TYPE_COLOR;
	private float scaleX = 1;
	private float scaleY = 1;
	private float alpha = 1;
	private float mask = 1;
	private GLColor color;
	private int textureId = -1;

	private GLVideoRect.TextureType textureType;

	public GLRenderParams(int type){
		this.type = type;
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}

	public float getScaleX() {
		return scaleX;
	}

	public void setScaleX(float scaleX) {
		this.scaleX = scaleX;
	}

	public float getScaleY() {
		return scaleY;
	}

	public void setScaleY(float scaleY) {
		this.scaleY = scaleY;
	}

	public int getTextureId() {
		return textureId;
	}

	public void setTextureId(int textureId) {
		this.textureId = textureId;
	}

	public float getAlpha() {
		return alpha;
	}

	public void setAlpha(float alpha) {
		this.alpha = alpha;
	}

	public GLColor getColor() {
		return color;
	}

	public void setColor(GLColor color) {
		this.color = color;
	}

	public GLVideoRect.TextureType getTextureType() {
		return textureType;
	}

	public void setTextureType(GLVideoRect.TextureType textureType) {
		this.textureType = textureType;
	}

	public float getMask() {
		return mask;
	}

	public void setMask(float mask) {
		this.mask = mask;
	}
}
