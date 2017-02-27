using UnityEngine;
using UnityEditor;
using System.IO;

public class UpdateSVR : MonoBehaviour
{
	[MenuItem( "SVR/Update Libraries" )]
	static void CopySomething( )
	{
		try
		{
            Debug.Log(Directory.GetCurrentDirectory());

			string libDestPath = "./Assets/Plugins/Android/libs/armeabi-v7a/";

			System.IO.File.Copy( "../../../svrApi/libs/armeabi-v7a/libsvrapi.so", libDestPath + "libsvrapi.so", true );
			System.IO.File.Copy( "../build/android-ndk/libs/armeabi-v7a/libsvrplugin.so", libDestPath + "libsvrplugin.so", true );
				
			string jarDestPath = "./Assets/Plugins/Android/";
			if(System.IO.File.Exists(jarDestPath + "svrApi.jar"))
			{
				System.IO.File.SetAttributes(jarDestPath + "svrApi.jar", System.IO.File.GetAttributes(jarDestPath + "svrApi.jar") & ~FileAttributes.ReadOnly);
			}
			System.IO.File.Copy( "../../../svrApi/libs/svrApi.jar", jarDestPath + "svrApi.jar", true );
			
			Debug.Log("Updated SVR Libraries");
		}
		catch (IOException e)
		{
			Debug.LogError( e.Message );
		}
	}
}
