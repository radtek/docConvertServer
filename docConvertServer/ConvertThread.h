#pragma once

#include "json/json.h"
#include "mystruct.h"

// CConvertThread

class CConvertThread : public CWinThread
{
	DECLARE_DYNCREATE(CConvertThread)

public:
	CConvertThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CConvertThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void SendMsgShow(p_st_tconverted sc, UINT msg, OutStatus status);

	void SetMainHwnd(HWND hwnd);
	p_st_domain2path GetDomain(const char *softlink);

	char* BuildFilePath(const char *softlink, p_st_domain2path domain);
	char* BuildOutPath(const char* path, const int &fileid);
	char* BuildOutUrl(const char* http, const int &fileid, const int &type);

	int PostSuccessOrFail(list<p_st_tconverted> lists, const BOOL bSuccess);
	void ResolvePostResult(Json::Value &value, int &status, const std::string &path = "");
	int HttpPostMsg(const char *url, const string & postdata, string & content);

public:
	BOOL m_bAbort;
	HANDLE m_hKillEvent;
	HWND m_hMainWnd;

public:
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};


