package com.bfmj.viewcore.util;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import com.bfmj.viewcore.render.GLMesh;
import com.bfmj.viewcore.render.GLVector2;
import com.bfmj.viewcore.render.GLVector3;

import android.content.res.Resources;
import android.util.Log;

public class GLObjFileUtils {
	public static String LoadShader(String pathName){
		StringBuilder shaderSource=new StringBuilder();
		BufferedReader shaderReader=null;
		try{
			shaderReader=new BufferedReader(new FileReader(pathName));
			String line;
			while((line=shaderReader.readLine())!=null){
				shaderSource.append(line).append("\n");
				
			}
			shaderReader.close();
		}catch(Exception e){
			e.printStackTrace();
		}
		return shaderSource.toString();
		
	}
	
	public static GLMesh LoadMesh(Resources res, String filePath){
		String[] spliteArr=filePath.split("\\.");
		String ext=spliteArr[spliteArr.length-1];
		if(!ext.equals("obj")){
			System.out.println("obj is not exist");
			System.exit(0);
		}
		
		ArrayList<GLVector3> verList = new ArrayList<GLVector3>();
		ArrayList<Integer> verIndexList=new ArrayList<Integer>();
		ArrayList<GLVector3> norList = new ArrayList<GLVector3>();
		ArrayList<Integer> norIndexList=new ArrayList<Integer>();
		ArrayList<GLVector2> texList=new ArrayList<GLVector2>();
		ArrayList<Integer> texIndexList=new ArrayList<Integer>();
		BufferedReader meshReader;
		
		try{
			InputStream in = res.getAssets().open(filePath);
			meshReader= new BufferedReader(new InputStreamReader(in));
			String line;
			while((line=meshReader.readLine())!=null){
//				Log.d("video", "line : " + line);
				String[] tokens=line.split(" ");
				tokens = removeEmptyStrings(tokens);
				if(tokens.length==0 || tokens[0].equals("#")){
					continue;
				}
				if(tokens[0].equals("v")){
					verList.add(new GLVector3(Float.parseFloat(tokens[1]),
										Float.parseFloat(tokens[2]),
										Float.parseFloat(tokens[3])
											));
				}else if(tokens[0].equals("f")){
					verIndexList.add(Integer.parseInt(tokens[1].split("/")[0])-1);
					verIndexList.add(Integer.parseInt(tokens[2].split("/")[0])-1);
					verIndexList.add(Integer.parseInt(tokens[3].split("/")[0])-1);
					if(tokens.length>4){
						
						verIndexList.add(Integer.parseInt(tokens[1].split("/")[0])-1);
						verIndexList.add(Integer.parseInt(tokens[3].split("/")[0])-1);
						verIndexList.add(Integer.parseInt(tokens[4].split("/")[0])-1);
					}
					
					// normalindex
					norIndexList.add(Integer.parseInt(tokens[1].split("/")[2])-1);
					norIndexList.add(Integer.parseInt(tokens[2].split("/")[2])-1);
					norIndexList.add(Integer.parseInt(tokens[3].split("/")[2])-1);
					if(tokens.length>4){
						
						norIndexList.add(Integer.parseInt(tokens[1].split("/")[2])-1);
						norIndexList.add(Integer.parseInt(tokens[3].split("/")[2])-1);
						norIndexList.add(Integer.parseInt(tokens[4].split("/")[2])-1);
					}
					
					//textureindex
					if (tokens[1].contains("//")){
						texIndexList.add(0);
					} else {
						texIndexList.add(Integer.parseInt(tokens[1].split("/")[1])-1);
					}
					
					if (tokens[2].contains("//")){
						texIndexList.add(0);
					} else {
						texIndexList.add(Integer.parseInt(tokens[2].split("/")[1])-1);
					}
					
					if (tokens[3].contains("//")){
						texIndexList.add(0);
					} else {
						texIndexList.add(Integer.parseInt(tokens[3].split("/")[1])-1);
					}
//					Log.d("video", "tokens[1] : " + tokens[1] + "; tokens[2] : " +tokens[2]
//		        			+ "; tokens[3] " + tokens[3]);
//					Log.d("video", "tokens[1] : " + (Integer.parseInt(tokens[1].split("/")[1])-1)
//							+ "; tokens[2] : " +(Integer.parseInt(tokens[2].split("/")[1])-1)
//		        			+ "; tokens[3] " + (Integer.parseInt(tokens[3].split("/")[1])-1));
					if(tokens.length>4){
						
						if (tokens[1].contains("//")){
							texIndexList.add(0);
						} else {
							texIndexList.add(Integer.parseInt(tokens[1].split("/")[1])-1);
						}
						
						if (tokens[3].contains("//")){
							texIndexList.add(0);
						} else {
							texIndexList.add(Integer.parseInt(tokens[3].split("/")[1])-1);
						}
						
						if (tokens[4].contains("//")){
							texIndexList.add(0);
						} else {
							texIndexList.add(Integer.parseInt(tokens[4].split("/")[1])-1);
						}
//						Log.d("video", "tokens[1] : " + tokens[1] + "; tokens[3] : " +tokens[4]
//			        			+ "; tokens[4] " + tokens[4]);
					}
				} else if(tokens[0].equals("vt")){
					texList.add(new GLVector2(Float.parseFloat(tokens[1]),
							Float.parseFloat(tokens[2])));
				} else if(tokens[0].equals("vn")){
					norList.add(new GLVector3(Float.parseFloat(tokens[1]),
							Float.parseFloat(tokens[2]),
							Float.parseFloat(tokens[3])
								));
				}
			}
			meshReader.close();
		} catch (FileNotFoundException e){
			e.printStackTrace();
		} catch(Exception e){
			e.printStackTrace();
		}
		
		GLMesh mesh = new GLMesh();
		
		GLVector3[] vers = new GLVector3[verList.size()];
		verList.toArray(vers);
		mesh.setVertices(vers);
		
		Integer[] verIndexs = new Integer[verIndexList.size()];
		verIndexList.toArray(verIndexs);
		mesh.setVertexIndexs(verIndexs);
		
		GLVector3[] nors = new GLVector3[norList.size()];
		norList.toArray(nors);
		mesh.setNormals(nors);
		
		Integer[] norIndexs = new Integer[norIndexList.size()];
		norIndexList.toArray(norIndexs);
		mesh.setNormalIndexs(norIndexs);
		
		GLVector2[] texs =new GLVector2[texList.size()];
		texList.toArray(texs);
		mesh.setTextures(texs);
		
		Integer[] texIndexs =new Integer[texIndexList.size()];
		texIndexList.toArray(texIndexs);
		mesh.setTextureIndexs(texIndexs);
		
		return mesh;
		
	}
	
	public static String[] removeEmptyStrings(String[] tokens) {
		ArrayList<String> result=new ArrayList<String>();
		
		for(String res : tokens){
			if(!res.equals("")){
				result.add(res);	
			}
		}
		String[] res=new String[result.size()];
		result.toArray(res);
		return res;
	}
}