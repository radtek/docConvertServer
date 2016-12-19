#include "stdafx.h"
#include "MyHttp.h"

#include <afxinet.h>


CMyHttp::CMyHttp()
{
}


CMyHttp::~CMyHttp()
{
}

CString CMyHttp::getFileNameFormUrl(const CString &strurl)
{
	if (strurl.Find(L"/") < 0) return L"";
	CString filename = strurl;
	int i = strurl.Find(L"/");
	while (i >= 0)
	{
		filename = filename.Mid(i+1);
		i = filename.Find(L"/");
	}
	if (filename != strurl)
		return filename; 
	else
		return L"";
}

BOOL CMyHttp::Download(const  CString &  strFileURLInServer,  // 待下载文件的URL 
	const  CString  &  strFileLocalFullPath, CString &strfilename) // 存放到本地的路径 
{
	ASSERT(strFileURLInServer != "");
	ASSERT(strFileLocalFullPath != "");
	strfilename = strFileLocalFullPath + getFileNameFormUrl(strFileURLInServer);
	CInternetSession session;
	CHttpConnection *  pHttpConnection = NULL;
	CHttpFile *  pHttpFile = NULL;
	CString strServer, strObject;
	INTERNET_PORT wPort;

	DWORD dwType;
	const   int  nTimeOut = 2000;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut);  // 重试之间的等待延时 
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);    // 重试次数 
	char *  pszBuffer = NULL;

	try
	{
		AfxParseURL(strFileURLInServer, dwType, strServer, strObject, wPort);
		pHttpConnection = session.GetHttpConnection(strServer, wPort);
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
		if (pHttpFile->SendRequest() == FALSE)
			return   FALSE;
		DWORD dwStateCode;

		pHttpFile->QueryInfoStatusCode(dwStateCode);
		if (dwStateCode == HTTP_STATUS_OK)
		{
			HANDLE hFile = CreateFile(strfilename, GENERIC_WRITE,
				FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
				NULL);   // 创建本地文件 
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pHttpFile->Close();
				pHttpConnection->Close();
				session.Close();
				return   FALSE;
			}

			char  szInfoBuffer[1000];   // 返回消息 
			DWORD dwFileSize = 0;    // 文件长度 
			DWORD dwInfoBufferSize = sizeof (szInfoBuffer);
			BOOL bResult = FALSE;
			bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,
				(void *)szInfoBuffer, &dwInfoBufferSize, NULL);

			dwFileSize = atoi(szInfoBuffer);
			const   int  BUFFER_LENGTH = 1024 * 10;
			pszBuffer = new   char[BUFFER_LENGTH];   // 读取文件的缓冲 
			DWORD dwWrite, dwTotalWrite;
			dwWrite = dwTotalWrite = 0;
			UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);  // 读取服务器上数据 

			while (nRead > 0)
			{
				WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL);   // 写到本地文件 
				dwTotalWrite += dwWrite;
				nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);
			}

			delete[]pszBuffer;
			pszBuffer = NULL;
			CloseHandle(hFile);
		}
		else
		{
			delete[]pszBuffer;
			pszBuffer = NULL;
			if (pHttpFile != NULL)
			{
				pHttpFile->Close();
				delete pHttpFile;
				pHttpFile = NULL;
			}
			if (pHttpConnection != NULL)
			{
				pHttpConnection->Close();
				delete pHttpConnection;
				pHttpConnection = NULL;
			}
			session.Close();
			return   FALSE;
		}
	}
	catch (...)
	{
		delete[]pszBuffer;
		pszBuffer = NULL;
		if (pHttpFile != NULL)
		{
			pHttpFile->Close();
			delete pHttpFile;
			pHttpFile = NULL;
		}
		if (pHttpConnection != NULL)
		{
			pHttpConnection->Close();
			delete pHttpConnection;
			pHttpConnection = NULL;
		}
		session.Close();
		return   FALSE;
	}

	if (pHttpFile != NULL)
		pHttpFile->Close();
	if (pHttpConnection != NULL)
		pHttpConnection->Close();
	session.Close();
	return   TRUE;
}