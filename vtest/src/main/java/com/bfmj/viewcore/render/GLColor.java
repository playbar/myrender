package com.bfmj.viewcore.render;

/**
 * 
 * ClassName: GLColor <br/>
 * @author lixianke    
 * @date: 2015-3-11 上午11:14:22 <br/>  
 * description:
 */
public class GLColor {
	private float r;
	private float g;
	private float b;
	private float a;
	
	public GLColor(){
		init(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	public GLColor(float[] c){
		if (c.length == 3){
			init(c[0], c[1], c[2], 1.0f);
		} else if (c.length >= 4){
			init(c[0], c[1], c[2], c[3]);
		} else {
			init(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	
	public GLColor(float r, float g, float b){
		init(r, g, b, 1.0f);
	}
	
	public GLColor(float r, float g, float b, float a){
		init(r, g, b, a);
	}

	public GLColor(int hex){
		 this(hex, 1.0f);
	}

	public GLColor(int hex, float a){
		int r = (hex & 0xFF0000) >> 16;
		int g = (hex & 0x00FF00) >> 8;
		int b = hex & 0x0000FF;
		init(r / 255.0f, g / 255.0f, b / 255.0f, a);
	}

	public GLColor(int r, int g, int b, float a) {
		init(r, g, b, a);
	}

	private void init(float r, float g, float b, float a){
		this.r = r;
		this.g = g;
		this.b = b;
		this.a = a;
	}
	
	public void setR(float r){
		this.r = r;
	}
	
	public float getR(){
		return this.r;
	}
	
	public void setG(float g){
		this.g = g;
	}
	
	public float getG(){
		return this.g;
	}
	
	public void setB(float b){
		this.b = b;
	}
	
	public float getB(){
		return this.b;
	}
	
	public void setA(float a){
		this.a = a;
	}
	
	public float getA(){
		return this.a;
	}
	
	public void setRGBA(float r, float g, float b, float a){
		init(r, g, b, a);
	}
}
