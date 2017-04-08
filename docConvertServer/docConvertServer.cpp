
// docConvertServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "docConvertServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdocConvertServerApp

BEGIN_MESSAGE_MAP(CdocConvertServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CdocConvertServerApp 构造

CdocConvertServerApp::CdocConvertServerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CdocConvertServerApp 对象

CdocConvertServerApp theApp;
char* g_strApiUrl;
char* g_strSuccessUrl;
char* g_strFailedUrl;
int g_nMinPages;
BOOL g_bIsDownFile;
char* g_strDownPath;
char* g_strTxtPath;
char* g_strTxtHttpHead;
char* g_strImgPath;
char* g_strImgHttpHead;
char* g_strLogPath;
char* g_strYZApi;
BOOL g_bIsDelSrc;
BOOL g_bIsToImg;
int g_nConvertTimeOut;
int g_nThreadNumber;
int g_nTimeSpanApiNull;
int g_nTimeSpanApi;
vector<p_st_domain2path> g_vtDomainPath;
list<p_st_tconvert> g_ltConvert;//待转换文档
CMutex g_mtxConvert;//待处理文档列表锁
list<p_st_tconverted> g_ltConvertSuccess;//转换成功列表
CMutex g_mtxConvertSuccess;
list<p_st_tconverted> g_ltConvertFailed;
CMutex g_mtxConvertFailed;
int g_nMaxFileInFloder;
int g_nMultiProcessId;//多进程标志
HANDLE g_hMulti;
BOOL g_bAutoStart;//启动程序后自动开始转换

char* g_strInifile;
char* g_strNode;

// CdocConvertServerApp 初始化

BOOL CdocConvertServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	InitGlobalData();

	char p[512]; GetCurrentDirectoryA(512, p);
	sprintf_s(g_strInifile, 1024, "%s\\docConvertServer.ini", p);

	g_nMultiProcessId = ::GetPrivateProfileIntA("CONFIG", "MULTIPROCESSID", 1, g_strInifile);

	wchar_t wp[100] = {};
	wsprintf(wp, L"docConvertServer......%05d", g_nMultiProcessId);
	g_hMulti = CreateMutex(NULL, FALSE, wp);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ReleaseGlobalData();
		return FALSE;
	}
	

	CString s; s.LoadString(AFX_IDS_APP_TITLE);
// 	if (!FirstInstance(s)) return FALSE;







	CdocConvertServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	ReleaseGlobalData();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


void CdocConvertServerApp::InitGlobalData()
{
	g_strApiUrl = new char[1024];
	g_strSuccessUrl = new char[1024];
	g_strFailedUrl = new char[1024];
	g_strYZApi = new char[1024];
	g_nMinPages = 0;
	g_strDownPath = new char[1024];
	g_strTxtPath = new char[1024];
	g_strTxtHttpHead = new char[1024];
	g_strImgPath = new char[1024];
	g_strImgHttpHead = new char[1024];
	g_strLogPath = new char[1024];
	g_nThreadNumber = 0;
	g_nTimeSpanApiNull = 0;
	g_nTimeSpanApi = 0;
	g_strInifile = new char[1024];
	g_strNode = NULL;
	g_hMulti = NULL;
}

void CdocConvertServerApp::ReleaseGlobalData()
{
	delete[] g_strApiUrl;
	delete[] g_strSuccessUrl;
	delete[] g_strFailedUrl;
	delete[] g_strYZApi;
	delete[] g_strDownPath;
	delete[] g_strTxtPath;
	delete[] g_strTxtHttpHead;
	delete[] g_strImgPath;
	delete[] g_strImgHttpHead;
	delete[] g_strLogPath;
	delete[] g_strInifile;
	if (g_strNode != NULL)
	{
		delete[] g_strNode;
	}
	if (g_vtDomainPath.size() > 0)
	{
		vector<p_st_domain2path>::iterator it = g_vtDomainPath.begin();
		while (it != g_vtDomainPath.end())
		{
// 			if (((p_st_domain2path)*it)->domain != NULL)
// 				delete[]((p_st_domain2path)*it)->domain;
// 			if (((p_st_domain2path)*it)->path != NULL)
// 				delete[]((p_st_domain2path)*it)->path;
// 			if (((p_st_domain2path)*it)->txtpath != NULL)
// 				delete[]((p_st_domain2path)*it)->txtpath;
// 			if (((p_st_domain2path)*it)->txthttp != NULL)
// 				delete[]((p_st_domain2path)*it)->txthttp;
// 			if (((p_st_domain2path)*it)->imgpath != NULL)
// 				delete[]((p_st_domain2path)*it)->imgpath;
// 			if (((p_st_domain2path)*it)->imghttp != NULL)
// 				delete[]((p_st_domain2path)*it)->imghttp;
			(*it)->UnInit();
			delete (p_st_domain2path)*it;
			it++;
		}
		g_vtDomainPath.clear();
	}

	if (g_ltConvert.size() > 0)
	{
		list<p_st_tconvert>::iterator it = g_ltConvert.begin();
		while (it != g_ltConvert.end())
		{
// 			if (((p_st_tconvert)*it)->softlink != NULL)
// 				delete[]((p_st_tconvert)*it)->softlink;

			delete (p_st_tconvert)*it;
			it++;
		}
		g_ltConvert.clear();
	}

	if (g_ltConvertSuccess.size() > 0)
	{
		list<p_st_tconverted>::iterator it = g_ltConvertSuccess.begin();
		while (it != g_ltConvertSuccess.end())
		{
// 			if (((p_st_tconverted)*it)->txturl != NULL)
// 				delete[]((p_st_tconverted)*it)->txturl;
// 			if (((p_st_tconverted)*it)->imgurl != NULL)
// 				delete[]((p_st_tconverted)*it)->imgurl;

			delete (p_st_tconverted)*it;
			it++;
		}
		g_ltConvertSuccess.clear();
	}

	if (g_ltConvertFailed.size() > 0)
	{
		list<p_st_tconverted>::iterator it = g_ltConvertFailed.begin();
		while (it != g_ltConvertFailed.end())
		{
// 			if (((p_st_tconverted)*it)->txturl != NULL)
// 				delete[]((p_st_tconverted)*it)->txturl;
// 			if (((p_st_tconverted)*it)->imgurl != NULL)
// 				delete[]((p_st_tconverted)*it)->imgurl;

			delete (p_st_tconverted)*it;
			it++;
		}
		g_ltConvertFailed.clear();
	}
	if (g_hMulti != NULL)
	{
		CloseHandle(g_hMulti);
	}
}