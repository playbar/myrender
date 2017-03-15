#include "InternetProfile.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingTimer.h"
#if defined(MJ_OS_WIN32)
#include "../3rdPart/Curl/include/windows/curl.h"
#elif defined(MJ_OS_MAC)
//#include "../3rdPart/Curl/include/ios/curl.h"
#include "../Interface/ios/HttpRequestIOS.h"
#else
#include "../3rdPart/Curl/include/android/curl.h"
#endif

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


namespace Baofeng
{
	namespace Mojing
	{
		static double g_dLastPeformFailedTime = 0;
		struct InternetReturnBuffer
		{
			unsigned int m_uiBufferSize;
			unsigned char m_pBuffer[1];
		};
		InternetProfile::InternetProfile()
		{
			SetCallBack(NULL);
			SetCallBackParam(NULL);
			Priority = BelowNormalPriority;
		}

		InternetProfile::~InternetProfile()
        
		{
			MOJING_TRACE(g_APIlogger, ">>>>>>>>>>~InternetProfile<<<<<<<<<<<");
		}

		size_t InternetProfile::WriteCallbackString(void *ptr, size_t size, size_t numemb, void *strJason)
		{
			unsigned int ** pBufferBase = (unsigned int **)strJason;
			unsigned int* pCheckPointer = *pBufferBase;
			
			unsigned int iBufferSize = size * numemb + sizeof (InternetReturnBuffer);
			InternetReturnBuffer *pRetBuffer = NULL;
			InternetReturnBuffer *pOldBuffer = NULL;
			
			if (pCheckPointer)
			{// 需要与之前的Buffer合并
				pOldBuffer = (InternetReturnBuffer*)pCheckPointer;
				*pBufferBase = (unsigned int *)new unsigned char[iBufferSize + pOldBuffer->m_uiBufferSize];
				pRetBuffer = (InternetReturnBuffer *)(*pBufferBase);

				memset(pRetBuffer, 0, iBufferSize + pOldBuffer->m_uiBufferSize);
				pRetBuffer->m_uiBufferSize = pOldBuffer->m_uiBufferSize + size * numemb;
				memcpy(pRetBuffer->m_pBuffer, pOldBuffer->m_pBuffer, pOldBuffer->m_uiBufferSize);
				memcpy(pRetBuffer->m_pBuffer + pOldBuffer->m_uiBufferSize , ptr, size * numemb);
				delete pOldBuffer;
			}
			else
			{// 第一次得到的Buffer
				*pBufferBase = (unsigned int *)new unsigned  char[iBufferSize];
				pRetBuffer = (InternetReturnBuffer *)(*pBufferBase);
				memset(pRetBuffer, 0, iBufferSize);
				memcpy(pRetBuffer->m_pBuffer, ptr, size * numemb);
				pRetBuffer->m_uiBufferSize = size * numemb;
			}
			return size * numemb;
		}

#ifdef MJ_OS_MAC
        int InternetProfile::HttpRequest()
        {
            try
            {
                char *pRespString = NULL;
                char *pID = NULL;
                int respcode = -1;

                if (GetFunction() == IPF_GET_UP)
                {
                    pID = (char *)GetTransportFiles();
                }
#ifdef _DEBUG
                String sUrl = GetURL();
                sUrl += "?";
                sUrl += GetParametData();
                MOJING_TRACE(g_APIlogger, "Get! url = " << sUrl.ToCStr());
#endif
                HttpRequestForIOS(GetURL(), GetParametData(), GetFunction(), &respcode, &pRespString);
                if (respcode == 200)
                {// 成功发送了该报告
                    MOJING_TRACE(g_APIlogger, "Http request perform succeed.");
                }
                else
                {
                    MOJING_WARN(g_APIlogger, "Http request perform failed.");
                }
            
                if (GetCallBack())
                {
                    if (pID != NULL)
                    {
                        m_fpCallBack((unsigned char *)pID, 0 ,respcode, GetCallBackParam());
                    }
                    else
                    {
                        InternetReturnBuffer *pBuffer = (InternetReturnBuffer *)pRespString;
                        if (pBuffer)
                            m_fpCallBack((unsigned char *)pBuffer->m_pBuffer , pBuffer->m_uiBufferSize , respcode, GetCallBackParam());
                        else
                            m_fpCallBack(NULL, 0, respcode, GetCallBackParam());
                    }
                }
                
                if (pRespString)
                    delete pRespString;
            }
            catch (...)
            {
                MOJING_ERROR(g_APIlogger, "Send/Get profile failed for exception!");
                return -1;
            }
            //MOJING_TRACE(g_APIlogger, "InternetProfile::Run exit");
            return 1;
        }
     
#else
		bool FormAdd(struct curl_httppost **formpost, struct curl_httppost **lastptr, char* buffer)
		{
			if (buffer == NULL)
			{
				return false;
			}
			
			char *p = strchr(buffer, '=');
			if (p == NULL)
			{
				return false;
			}

			int len = p - buffer;
			char *fieldname = new char[len + 1];
			memcpy(fieldname, buffer, len);
			fieldname[len] = 0;
			char *fieldvalue = p + 1;
	
			curl_formadd(formpost,
				lastptr,
				CURLFORM_COPYNAME, fieldname,
				CURLFORM_COPYCONTENTS, fieldvalue,
				CURLFORM_END);

			return true;
		}

        int InternetProfile::CurlPerform()
        {
            try
            {
                CURL *curl = NULL;
                char *pRespString = NULL;
                char *pID = NULL;
                long respcode = -1;
                String sUrl;
                
                curl = curl_easy_init();
                if (curl)
                {
					if (Timer::GetSeconds() - g_dLastPeformFailedTime > 10) //距离上一次提交失败超过5s， 尝试提交
					{
						bool bSetOptSucc = true;
						curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
						if (strstr(GetURL(), "https"))
						{
							MOJING_TRACE(g_APIlogger, "Curl use openssl. url :" << GetURL());
							/*
							 char pCaPath[260] = { "/sdcard/mojing.cn.crt" };
							 if (pCaPath)
							 {
							 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
							 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
							 curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
							 }
							 else
							 {
							 */
							curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
							curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
							//}
						}
						if (GetFunction() == IPF_POST)
						{
							curl_easy_setopt(curl, CURLOPT_URL, GetURL());

							const char* pFilePath = GetTransportFiles();
							if ((pFilePath == NULL) || (strlen(pFilePath) == 0))
							{
								curl_easy_setopt(curl, CURLOPT_POST, 1L);
								curl_easy_setopt(curl, CURLOPT_POSTFIELDS, GetParametData());
								curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(GetParametData()));
								//MOJING_TRACE(g_APIlogger, "Post! url = " << GetParametData());
							}
							else
							{
								//MOJING_TRACE(g_APIlogger, "Curl post crash file...");
								const char* data = GetParametData();
								if (data == NULL || strlen(data) == 0)
								{
									MOJING_WARN(g_APIlogger, "Param data is empty, cancel to post crash file...");
									bSetOptSucc = false;
								}
								else
								{
									struct curl_httppost *formpost = NULL;
									struct curl_httppost *lastptr = NULL;
									char *p = strchr(data, '&');
									char *q = (char *)data;
									int nFieldCount = 0;
									while (p)
									{
										int len = p - q;
										char *buffer = new char[len + 1];
										memcpy(buffer, q, len);
										buffer[len] = 0;
										if (FormAdd(&formpost, &lastptr, buffer))
											nFieldCount++;
										delete[] buffer;
										q = p + 1;
										p = strchr(q, '&');
									}
									if (FormAdd(&formpost, &lastptr, q))
										nFieldCount++;
									if (nFieldCount != 11)
									{
										MOJING_WARN(g_APIlogger, "Form is incomplete, cancel to post crash file...");
										bSetOptSucc = false;
									}

									/* Fill in the file upload field */
									curl_formadd(&formpost,
										&lastptr,
										CURLFORM_COPYNAME, "userfile",
										CURLFORM_FILE, pFilePath,
										CURLFORM_END);

									/* Fill in the submit field too, even if this is rarely needed*/
									curl_formadd(&formpost,
										&lastptr,
										CURLFORM_COPYNAME, "submit",
										CURLFORM_COPYCONTENTS, "Submit",
										CURLFORM_END);

									//struct curl_slist * headerlist = curl_slist_append(headerlist, buf);
									curl_easy_setopt(curl, CURLOPT_HTTPHEADER, 0);
									curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
								}
							}
						}
						else if (GetFunction() == IPF_GET_UP)
						{
							sUrl = GetURL();
							sUrl += "?";
							sUrl += GetParametData();
#ifdef _DEBUG
							MOJING_TRACE(g_APIlogger, "Get! url = " << sUrl.ToCStr());
#endif
							curl_easy_setopt(curl, CURLOPT_URL, sUrl.ToCStr());
							pID = (char *)GetTransportFiles();
						}
						else if (GetFunction() == IPF_GET_QUERY)
						{
							sUrl = GetURL();
							if (strchr(sUrl.ToCStr(), '?') == NULL && GetParametData() && *GetParametData())
							{
								sUrl += "?";
								sUrl += GetParametData();
							}
							curl_easy_setopt(curl, CURLOPT_URL, sUrl.ToCStr());
							curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackString);
							curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pRespString);
							//MOJING_TRACE(g_APIlogger, "Get! url = " << sUrl.ToCStr()/*GetParametData()*/);
						}
						//MOJING_TRACE(g_APIlogger, "Curl perform start...");
						if (bSetOptSucc)
						{
							CURLcode res = curl_easy_perform(curl);
							if (0 == res) {
								//char * url;  //url to verify the connection worked
								// long respcode; //response code of the http transaction
								//long redirect; //number of redirects after we connect
								//curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url); //grabbing it from curl
								curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respcode);// grabbing it from curl
								//curl_easy_getinfo(curl, CURLINFO_REDIRECT_COUNT, &redirect); //grabbing it from curl

								if (respcode == 200)
								{// 成功发送了该报告
									MOJING_TRACE(g_APIlogger, "Curl perform succeed. mode:" << GetFunction());
								}
								else
								{
									MOJING_WARN(g_APIlogger, "Curl perform failed." << respcode);
								}
							}
							else
							{
								MOJING_ERROR(g_APIlogger, "Send/Get Profile Failed! res = " << res);
								g_dLastPeformFailedTime = Timer::GetSeconds();
							}
						}
					}
					else  //距离上次提交失败少于5s， 放弃此次提交， 直接返回错误码-1
					{
						respcode = -1;
#ifdef _DEBUG
						MOJING_WARN(g_APIlogger, "Network is abnormal, abandon this request." );
#endif
					}

                    curl_easy_cleanup(curl);
                }
                
                if (GetCallBack())
                {
                    if (pID != NULL)
                    {
                        m_fpCallBack((unsigned char *)pID, 0 ,respcode, GetCallBackParam());
                    }
                    else
                    {
                        InternetReturnBuffer *pBuffer = (InternetReturnBuffer *)pRespString;
                        if (pBuffer)
                            m_fpCallBack((unsigned char *)pBuffer->m_pBuffer , pBuffer->m_uiBufferSize , respcode, GetCallBackParam());
                        else
                            m_fpCallBack(NULL, 0, respcode, GetCallBackParam());
                    }
                }
                
                if (pRespString)
                    delete pRespString;
            }
            catch (...)
            {
                MOJING_ERROR(g_APIlogger, "Send/Get profile failed for exception!");
                return -1;
            }
            
            //MOJING_TRACE(g_APIlogger, "InternetProfile::Run exit");     
            return 1;
        }
#endif
        
        int InternetProfile::Run()
        {
#ifdef MJ_OS_MAC
            return HttpRequest();
#else
            return CurlPerform();
#endif
        }// end while
        
    }
}
