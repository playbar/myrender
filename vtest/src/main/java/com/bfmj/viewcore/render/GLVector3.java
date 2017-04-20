package com.bfmj.viewcore.render;

public class GLVector3 {
	public float x;
	public float y;
	public float z;
	
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

	public GLVector3(){
		this.x=0;
		this.y=0;
		this.z=0;
		
	}
	
	public GLVector3(float x,float y,float z){
		this.x=x;
		this.y=y;
		this.z=z;
	}
	
	public static float Dot(GLVector3 lhs,GLVector3 rhs){
		 
		return lhs.x*rhs.x+lhs.y*rhs.y+lhs.z*rhs.z;
	}
	
	public static GLVector3 Zero(){
		
		return new GLVector3(0,0,0);
		
	}
	
	//������;
	public static GLVector3 NegativeVec(GLVector3 vec){
		vec.x=vec.x*(-1);
		vec.y=vec.y*(-1);
		vec.z=vec.z*(-1);
		return vec;
	}
	//��ȡģ��;
	public static float Length(GLVector3 vec){
		
		return (float) Math.sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
	}
	
	public static GLVector3 Mul(float k,GLVector3 vec){
		vec.x=vec.x*k;
		vec.y=vec.y*k;
		vec.z=vec.z*k;
		return vec;
	}
	
	public static GLVector3 Add(GLVector3 lhs,GLVector3 rhs){
		lhs.x=lhs.x+rhs.x;
		lhs.y=lhs.y+rhs.y;
		lhs.z=lhs.z+rhs.z;
		return lhs;
	}
	
	public static GLVector3 Sub(GLVector3 lhs,GLVector3 rhs){
		lhs.x-=rhs.x;
		lhs.y-=rhs.y;
		lhs.z-=rhs.z;
		return lhs;
	}

	public static GLVector3 Sub(GLVector3 lhs,GLVector3 rhs, GLVector3 re)
	{
		re.x = lhs.x - rhs.x;
		re.y = lhs.y - rhs.y;
		re.z = lhs.z - rhs.z;
		return re;
	}
	
	public static float Angle(GLVector3 lhs,GLVector3 rhs){
		
		float dot=Dot(lhs,rhs);
		float lhsLength=Length(lhs);
		float rhsLength=Length(rhs);
		return (float)Math.acos(dot/(lhsLength*rhsLength));
		
	}
	
	public static GLVector3 Normalize(GLVector3 vec){
		float length=Length(vec);
		vec.x/=length;
		vec.y/=length;
		vec.z/=length;
		return vec;
		
	}
	
	public static GLVector3 Cross(GLVector3 lhs,GLVector3 rhs){
		GLVector3 result=new GLVector3();
		result.x=lhs.y*rhs.z-lhs.z*rhs.y;
		result.y=lhs.z*rhs.x-lhs.x*rhs.z;
		result.z=lhs.x*rhs.y-lhs.y*rhs.x;
		return result;
	}
	
	//��ָ��λ���ϵ�������ͶӰ
	public static GLVector3 ShadowVec(GLVector3 sVec,GLVector3 tVec){
	     
		 GLVector3 nor=Normalize(tVec);
		 float dot=Dot(sVec,nor);
		 float length=Length(nor)*Length(nor);
		 float multiple=dot/length;
		 return Mul(multiple,nor);
	}
}
