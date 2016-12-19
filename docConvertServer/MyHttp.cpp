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

BOOL CMyHttp::Download(const  CString &  strFileURLInServer,  // �������ļ���URL 
	const  CString  &  strFileLocalFullPath, CString &strfilename) // ��ŵ����ص�·�� 
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
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut);  // ����֮��ĵȴ���ʱ 
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);    // ���Դ��� 
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
				NULL);   // ���������ļ� 
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pHttpFile->Close();
				pHttpConnection->Close();
				session.Close();
				return   FALSE;
			}

			char  szInfoBuffer[1000];   // ������Ϣ 
			DWORD dwFileSize = 0;    // �ļ����� 
			DWORD dwInfoBufferSize = sizeof (szInfoBuffer);
			BOOL bResult = FALSE;
			bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,
				(void *)szInfoBuffer, &dwInfoBufferSize, NULL);

			dwFileSize = atoi(szInfoBuffer);
			const   int  BUFFER_LENGTH = 1024 * 10;
			pszBuffer = new   char[BUFFER_LENGTH];   // ��ȡ�ļ��Ļ��� 
			DWORD dwWrite, dwTotalWrite;
			dwWrite = dwTotalWrite = 0;
			UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);  // ��ȡ������������ 

			while (nRead > 0)
			{
				WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL);   // д�������ļ� 
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