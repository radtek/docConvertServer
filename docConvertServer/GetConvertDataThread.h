#pragma once



// CGetConvertDataThread

class CGetConvertDataThread : public CWinThread
{
	DECLARE_DYNCREATE(CGetConvertDataThread)

public:
	CGetConvertDataThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CGetConvertDataThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void SetMainHwnd(HWND hwnd);

public:
	BOOL m_bAbort;
	HANDLE m_hKillEvent;
	HWND m_hMainWnd;

public:
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};


