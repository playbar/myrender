package com.bfmj.viewcore.render;

public class GLVector2 {
	public float s;
	public float t;
	
	public float getS() {
		return s;
	}

	public void setS(float s) {
		this.s = s;
	}

	public float getT() {
		return t;
	}

	public void setT(float t) {
		this.t = t;
	}

	public GLVector2(){
		this.s=0;
		this.t=0;
		
	}
	
	public GLVector2(float s,float t){
		this.s=s;
		this.t=t;
	}
	
	
	public static GLVector2 Zero(){	
		return new GLVector2(0,0);
	}
}
