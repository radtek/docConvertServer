#pragma once

#include "json/json.h"
#include "curl/curl.h"


// CPostResultThread

class CPostResultThread : public CWinThread
{
	DECLARE_DYNCREATE(CPostResultThread)

public:
	CPostResultThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CPostResultThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void ShowSuccessMsg(const int &count, const int &ntype);
	void ShowErrorMsg(const int &ret, const string &msg, const int &ntype);

	string BuildPostMsg(list<p_st_tconverted> &lists, list<p_st_tconverted> &outlist, const BOOL bSuccess);
	int PostSuccessOrFail(const string &msg, const BOOL bSuccess);
	int PostSuccessOrFail(list<p_st_tconverted> lists, const BOOL bSuccess);
	void ResolvePostResult(Json::Value &value, int &status, const std::string &path = "");
	int HttpPostMsg(const char *url, const string & postdata, string & content);

	void SetMainHwnd(HWND hwnd);

public:
	BOOL m_bAbort;
	HANDLE m_hKillEvent;
	HWND m_hMainWnd;


public:
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};


