package com.bfmj.viewcore.render;

public class GLConstant {
	//ORIENTATION
	public static enum GLOrientation{HORIZONTAL, VERTICAL, VERTICAL_RIGHT }
	
	//ALIGN
	public static enum GLAlign{LEFT, RIGHT, TOP, BOTTOM, LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM, CENTER, CENTER_VERTICAL, CENTER_HORIZONTAL, DEFAULT}
	
	//PROCESS MODE
	public static enum GLProcessMode{RECTANGLE, CIRCLE} //长方形、圆形
	
	//PROCESS CIRCLE Direction
	public static enum GLDirection{CLOCKWISE, ANTICLOCKWISE} //顺时针、逆时针
}
