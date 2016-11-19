
// docConvertServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "mystruct.h"
#include <list>
#include <string>
#include <vector>
using namespace std;

// CdocConvertServerApp: 
// �йش����ʵ�֣������ docConvertServer.cpp
//

class CdocConvertServerApp : public CWinApp
{
public:
	CdocConvertServerApp();

// ��д
public:
	virtual BOOL InitInstance();

	void InitGlobalData();
	void ReleaseGlobalData();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CdocConvertServerApp theApp;
extern char* g_strApiUrl;
extern char* g_strSuccessUrl;
extern char* g_strFailedUrl;
extern int g_nMinPages;
extern BOOL g_bIsDownFile;
extern char* g_strDownPath;
extern char* g_strTxtPath;
extern char* g_strTxtHttpHead;
extern char* g_strImgPath;
extern char* g_strImgHttpHead;
extern char* g_strLogPath;
extern BOOL g_bIsDelSrc;
extern BOOL g_bIsToImg;
extern int g_nConvertTimeOut;
extern int g_nThreadNumber;
extern int g_nTimeSpanApiNull;
extern int g_nTimeSpanApi;
extern vector<p_st_domain2path> g_vtDomainPath;
extern list<p_st_tconvert> g_ltConvert;
extern CMutex g_mtxConvert;//�������ĵ��б���
extern list<p_st_tconverted> g_ltConvertSuccess;
extern CMutex g_mtxConvertSuccess;//
extern list<p_st_tconverted> g_ltConvertFailed;
extern CMutex g_mtxConvertFailed;
extern int g_nMaxFileInFloder;//�ļ�������ļ���


extern char* g_strInifile;
extern char* g_strNode;