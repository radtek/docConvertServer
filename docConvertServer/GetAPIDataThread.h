#pragma once
#ifndef GETAPIDATATHREAD_H_
#define GETAPIDATATHREAD_H_

#include "ConnectDB.h"
#include "json/json.h"
#include <string>
#include <map>
using namespace std;

// CGetAPIDataThread

class CGetAPIDataThread : public CWinThread
{
	DECLARE_DYNCREATE(CGetAPIDataThread)

public:
	CGetAPIDataThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CGetAPIDataThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();


	int Get(const std::string & strUrl, std::string & strResponse);
	void printValueTree(map<int, p_st_tconvert> &hmap, Json::Value &value, const std::string &path = "", const int nid = 0);
	void SetMainHwnd(HWND hwnd);


public:
	BOOL m_bAbort;
	HANDLE m_hKillEvent;
	HWND m_hMainWnd;

public:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
};


#endif