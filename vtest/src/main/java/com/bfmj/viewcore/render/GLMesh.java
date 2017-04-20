package com.bfmj.viewcore.render;

/**
 * 
 * ClassName: GLMesh <br/>
 * @author lixianke    
 * @date: 2015-3-12 上午11:02:12 <br/>  
 * description:
 */
public class GLMesh {
	private GLVector3[] vertices;
	private Integer[] vertexIndexs;
	private GLVector3[] normals;
	private Integer[] normalIndexs;
	private GLVector2[] textures;
	private Integer[] textureIndexs;

	public GLVector2[] getTextures() {
		return textures;
	}
	
	public void setTextures(GLVector2[] textures) {
		this.textures = textures;
	}
	
	public Integer[] getVertexIndexs() {
		return vertexIndexs;
	}
	
	public void setVertexIndexs(Integer[] vertexIndexs) {
		this.vertexIndexs = vertexIndexs;
	}
	
	public GLVector3[] getVertices() {
		return vertices;
	}
	
	public void setVertices(GLVector3[] vertices) {
		this.vertices = vertices;
	}
	
	public Integer[] getTextureIndexs() {
		return textureIndexs;
	}
	
	public void setTextureIndexs(Integer[] textureIndexs) {
		this.textureIndexs = textureIndexs;
	}

	public GLVector3[] getNormals() {
		return normals;
	}

	public void setNormals(GLVector3[] normals) {
		this.normals = normals;
	}

	public Integer[] getNormalIndexs() {
		return normalIndexs;
	}

	public void setNormalIndexs(Integer[] normalIndexs) {
		this.normalIndexs = normalIndexs;
	}
}
