
// docConvertServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "docConvertServerDlg.h"
#include "afxdialogex.h"
#include "GetAPIDataThread.h"
#include "GetConvertDataThread.h"
#include "ConvertThread.h"
#include "PostResultThread.h"
#include "mystruct.h"
#include "Util.h"
#include "ConnectDB.h"

#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CdocConvertServerDlg 对话框



CdocConvertServerDlg::CdocConvertServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdocConvertServerDlg::IDD, pParent)
	, m_bIsDelSrc(FALSE)
// 	, m_bIsSaveLog(FALSE)
	, m_nConvertThreads(0)
	, m_nConvertTimeout(0)
	, m_strFailedApi(_T(""))
	, m_strGetApi(_T(""))
	, m_strImgHttp(_T(""))
	, m_strImgSavePath(_T(""))
	, m_strLogPath(_T(""))
	, m_nMinPages(0)
	, m_strSuccessApi(_T(""))
	, m_strTxtHttp(_T(""))
	, m_strTxtSavePath(_T(""))
	, m_nSpanTime(0)
	, m_nSpanTimeNull(0)
	, m_strDownPath(_T(""))
	, m_bIsDownfile(FALSE)
	, m_bIsToImg(FALSE)
	, m_nMaxFileNumInFloder(0)
	, m_strYZApi(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pGetApiThread = NULL;
	m_pGetConvertDataThread = NULL;
	m_pPostResultThread = NULL;
	for (int i = 0; i < 100; i++)
	{
		m_pConvertThread[i] = NULL;
	}
}

void CdocConvertServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CK_DELSRC, m_bIsDelSrc);
	//  DDX_Check(pDX, IDC_CK_SAVELOG, m_bIsSaveLog);
	DDX_Text(pDX, IDC_CONVERT_THREADS, m_nConvertThreads);
	DDX_Text(pDX, IDC_CONVERT_TIMEOUT, m_nConvertTimeout);
	DDX_Text(pDX, IDC_FAILED_API, m_strFailedApi);
	DDX_Text(pDX, IDC_GETAPI, m_strGetApi);
	DDX_Text(pDX, IDC_IMGHTTP, m_strImgHttp);
	DDX_Text(pDX, IDC_IMGSAVEPATH, m_strImgSavePath);
	DDX_Control(pDX, IDC_LIST1, m_wndMsgList);
	DDX_Text(pDX, IDC_LOGPATH, m_strLogPath);
	DDX_Text(pDX, IDC_MINPAGES, m_nMinPages);
	DDX_Text(pDX, IDC_SUCCESS_API, m_strSuccessApi);
	DDX_Control(pDX, IDC_TAB1, m_wndTabCtrl);
	DDX_Text(pDX, IDC_TXTHTTP, m_strTxtHttp);
	DDX_Text(pDX, IDC_TXTSAVEPATH, m_strTxtSavePath);
	DDX_Control(pDX, IDC_BT_SAVECONFIG, m_wndBtSaveConfig);
	//  DDX_Control(pDX, IDC_STATIC_CONVERTTHREADS, m_wndStaticConvertThreads);
	DDX_Text(pDX, IDC_SPANTIME, m_nSpanTime);
	DDX_Text(pDX, IDC_SPANTIME_NULL, m_nSpanTimeNull);
	DDX_Text(pDX, IDC_FILEPATH, m_strDownPath);
	DDX_Check(pDX, IDC_CK_ISDOWNFILE, m_bIsDownfile);
	DDX_Control(pDX, IDC_LIST2, m_wndListCtrl);
	DDX_Check(pDX, IDC_CK_ISTOIMG, m_bIsToImg);
	DDX_Control(pDX, IDC_LIST3, m_wndListCtrlDomain);
	DDX_Text(pDX, IDC_MAXFILENUM, m_nMaxFileNumInFloder);
	DDX_Text(pDX, IDC_YZ_API, m_strYZApi);
}

BEGIN_MESSAGE_MAP(CdocConvertServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CdocConvertServerDlg::OnSelchangeTab1)
	ON_BN_CLICKED(IDC_BT_SAVECONFIG, &CdocConvertServerDlg::OnBnClickedBtSaveconfig)
	ON_BN_CLICKED(IDC_START, &CdocConvertServerDlg::OnBnClickedStart)
	ON_MESSAGE(WM_MSGLIST_SHOW, &CdocConvertServerDlg::OnMsglistShow)
	ON_MESSAGE(MSG_LISTCTRL_ADD, &CdocConvertServerDlg::OnMsgListctrlAdd)
	ON_MESSAGE(MSG_LISTCTRL_UPDATE, &CdocConvertServerDlg::OnMsgListctrlUpdate)
	ON_BN_CLICKED(IDC_CK_ISDOWNFILE, &CdocConvertServerDlg::OnBnClickedCkIsdownfile)
	ON_BN_CLICKED(IDC_CK_DELSRC, &CdocConvertServerDlg::OnBnClickedCkDelsrc)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_CLEARSQLITE, &CdocConvertServerDlg::OnBnClickedBtClearsqlite)
	ON_MESSAGE(MSG_MSG_LOG, &CdocConvertServerDlg::OnMsgMsgLog)
END_MESSAGE_MAP()


// CdocConvertServerDlg 消息处理程序

BOOL CdocConvertServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
// 	StartGetConvertDataThread();
// 	StartConvertThread();

// 	InitConsoleWindow();

	curl_global_init(CURL_GLOBAL_ALL);

	wchar_t wp[100] = {};
	wsprintf(wp, L"docConvertServer--%d", g_nMultiProcessId);
	this->SetWindowTextW(wp);

	InitListCtrl();
	InitListCtrlDomain();

	ReadIni();

	//启动pdf转换服务
	ExecuteProcess("pdfconverthttpserver.exe");

	m_wndMsgList.SetHorizontalExtent(1000);

	m_wndTabCtrl.InsertItem(0, L"运行");
	m_wndTabCtrl.InsertItem(1, L"设置");

	m_wndTabCtrl.SetCurSel(0);

	COleDateTime now = COleDateTime::GetCurrentTime();
	CString strnow = now.Format(L"%Y-%m-%d %H:%M:%S");
	GetDlgItem(IDC_STARTTIME)->SetWindowTextW(strnow);


	if (g_bAutoStart)
	{
		OnBnClickedStart();
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CdocConvertServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CdocConvertServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		DrawWindow();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CdocConvertServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CdocConvertServerDlg::InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

char* CdocConvertServerDlg::GetApiNode(const char* apiurl)
{
	if (!apiurl || strlen(apiurl) == 0){
		return NULL;
	}
	int len, len1 = 0;
	int haspath = 0;
	char *strnode = NULL;
	len = len1 = strlen(apiurl) - 1;
	for (; len > 0; len--){
		if (apiurl[len] == '='){
			strnode = new char[len1 - len + 1];
			strcpy(strnode, (char*)(&apiurl[len + 1]));
			strnode[len1 - len] = '\0';
			haspath = 1;
			break;
		}
	}
// 	if (strnode != NULL)
// 	{
// 		g_strNode = new char[strlen(strnode) + 1];
// 		strcpy(g_strNode, strnode);
// 		g_strNode[strlen(strnode)] = '\0';
// 		delete[] strnode;
// 	}
	return strnode;
}

void CdocConvertServerDlg::ReadIni()
{
	UpdateData(TRUE);
	::GetPrivateProfileStringA("CONFIG", "GETAPIURL", "", g_strApiUrl, 1024, g_strInifile);

	g_strNode = GetApiNode(g_strApiUrl);

	::GetPrivateProfileStringA("CONFIG", "SUCCESSURL", "", g_strSuccessUrl, 1024, g_strInifile);
	::GetPrivateProfileStringA("CONFIG", "FAILEDURL", "", g_strFailedUrl, 1024, g_strInifile);
// 	::GetPrivateProfileStringA("CONFIG", "TXTPATH", "", g_strTxtPath, 1024, g_strInifile);
// 	::GetPrivateProfileStringA("CONFIG", "TXTHTTPHEAD", "", g_strTxtHttpHead, 1024, g_strInifile);
// 	::GetPrivateProfileStringA("CONFIG", "IMGPATH", "", g_strImgPath, 1024, g_strInifile);
// 	::GetPrivateProfileStringA("CONFIG", "IMGHTTPHEAD", "", g_strImgHttpHead, 1024, g_strInifile);
	::GetPrivateProfileStringA("CONFIG", "LOGPATH", "", g_strLogPath, 1024, g_strInifile);
	::GetPrivateProfileStringA("CONFIG", "DOWNPATH", "", g_strDownPath, 1024, g_strInifile);
	::GetPrivateProfileStringA("CONFIG", "YZAPI", "", g_strYZApi, 1024, g_strInifile);

	g_nConvertTimeOut = ::GetPrivateProfileIntA("CONFIG", "CONVERTTIMEOUT", 1000, g_strInifile);
	g_nThreadNumber = ::GetPrivateProfileIntA("CONFIG", "THREADNUM", 1, g_strInifile);
	g_nTimeSpanApi = ::GetPrivateProfileIntA("CONFIG", "TIMESPANAPI", 1, g_strInifile);
	g_nTimeSpanApiNull = ::GetPrivateProfileIntA("CONFIG", "TIMESPANAPINULL", 120, g_strInifile);
	g_bIsDelSrc = ::GetPrivateProfileIntA("CONFIG", "ISDELSRC", 0, g_strInifile) != 0 ? TRUE : FALSE;
	g_nMinPages = ::GetPrivateProfileIntA("CONFIG", "MINPAGE", 4, g_strInifile);
	g_bIsDownFile = ::GetPrivateProfileIntA("CONFIG", "ISDOWNFILE", 0, g_strInifile) != 0 ? TRUE : FALSE;
	g_bIsToImg = ::GetPrivateProfileIntA("CONFIG", "ISTOIMG", 0, g_strInifile) != 0 ? TRUE : FALSE;
	g_nMaxFileInFloder = ::GetPrivateProfileIntA("CONFIG", "MAXFILENUM", 1000, g_strInifile);
	g_bAutoStart = ::GetPrivateProfileIntA("CONFIG", "AUTOSTART", 1, g_strInifile) != 0 ? TRUE : FALSE;

	m_strGetApi = CharToCString(g_strApiUrl);
	m_strSuccessApi = CharToCString(g_strSuccessUrl);
	m_strFailedApi = CharToCString(g_strFailedUrl);
	m_strYZApi = CharToCString(g_strYZApi);
	m_strTxtSavePath = CharToCString(g_strTxtPath);
	m_strTxtHttp = CharToCString(g_strTxtHttpHead);
	m_strImgSavePath = CharToCString(g_strImgPath);
	m_strImgHttp = CharToCString(g_strImgHttpHead);
	m_strLogPath = CharToCString(g_strLogPath);
	m_strDownPath = CharToCString(g_strDownPath);
	m_nConvertThreads = g_nThreadNumber;
	m_nConvertTimeout = g_nConvertTimeOut;
	m_nSpanTime = g_nTimeSpanApi;
	m_nSpanTimeNull = g_nTimeSpanApiNull;
	m_nMinPages = g_nMinPages;
	m_bIsDelSrc = g_bIsDelSrc;
	m_bIsDownfile = g_bIsDownFile;
	m_bIsToImg = g_bIsToImg;
	m_nMaxFileNumInFloder = g_nMaxFileInFloder;

	if (m_bIsDownfile)
	{
		GetDlgItem(IDC_CK_DELSRC)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CK_DELSRC)->EnableWindow(FALSE);
	}


	int nCount = ::GetPrivateProfileIntA("DOMAIN", "COUNT", 0, g_strInifile);
	for (int i = 0; i < nCount; i++)
	{
		p_st_domain2path pdomain = new st_domain2path;
		pdomain->domain = new char[128];
		pdomain->path = new char[256];
		pdomain->txtpath = new char[256];
		pdomain->txthttp = new char[128];
		pdomain->imgpath = new char[256];
		pdomain->imghttp = new char[128];
		char p1[10] = { 0 };
		sprintf_s(p1, 10, "DOMAIN%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->domain, 128, g_strInifile);
		memset(p1, 0, 10);
		sprintf_s(p1, 10, "PATH%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->path, 256, g_strInifile);
		memset(p1, 0, 10);
		sprintf_s(p1, 10, "TXTPATH%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->txtpath, 256, g_strInifile);
		memset(p1, 0, 10);
		sprintf_s(p1, 10, "TXTHTTP%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->txthttp, 128, g_strInifile);
		memset(p1, 0, 10);
		sprintf_s(p1, 10, "IMGPATH%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->imgpath, 256, g_strInifile);
		memset(p1, 0, 10);
		sprintf_s(p1, 10, "IMGHTTP%d", i);
		::GetPrivateProfileStringA("DOMAIN", p1, "", pdomain->imghttp, 128, g_strInifile);

		AddListCtrlDomain(pdomain);
		g_vtDomainPath.push_back(pdomain);
	}

	UpdateData(FALSE);
}

void CdocConvertServerDlg::WriteIni()
{
	UpdateData(TRUE);
	char *temp = CStringToChar(m_strGetApi);
	if (strcmp(temp, g_strApiUrl) != 0)
	{
		strcpy(g_strApiUrl, temp);
		g_strApiUrl[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "GETAPIURL", g_strApiUrl, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strSuccessApi);
	if (strcmp(temp, g_strSuccessUrl) != 0)
	{
		strcpy(g_strSuccessUrl, temp);
		g_strSuccessUrl[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "SUCCESSURL", g_strSuccessUrl, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strFailedApi);
	if (strcmp(temp, g_strFailedUrl) != 0)
	{
		strcpy(g_strFailedUrl, temp);
		g_strFailedUrl[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "FAILEDURL", g_strFailedUrl, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strYZApi);
	if (strcmp(temp, g_strYZApi) != 0)
	{
		strcpy(g_strYZApi, temp);
		g_strYZApi[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "YZAPI", g_strYZApi, g_strInifile);
	}
	delete[] temp;

	/*
	temp = CStringToChar(m_strTxtSavePath);
	if (strcmp(temp, g_strTxtPath) != 0)
	{
		strcpy(g_strTxtPath, temp);
		g_strTxtPath[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "TXTPATH", g_strTxtPath, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strTxtHttp);
	if (strcmp(temp, g_strTxtHttpHead) != 0)
	{
		strcpy(g_strTxtHttpHead, temp);
		g_strTxtHttpHead[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "TXTHTTPHEAD", g_strTxtHttpHead, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strImgSavePath);
	if (strcmp(temp, g_strImgPath) != 0)
	{
		strcpy(g_strImgPath, temp);
		g_strImgPath[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "IMGPATH", g_strImgPath, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strImgHttp);
	if (strcmp(temp, g_strImgHttpHead) != 0)
	{
		strcpy(g_strImgHttpHead, temp);
		g_strImgHttpHead[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "IMGHTTPHEAD", g_strImgHttpHead, g_strInifile);
	}
	delete[] temp;
	*/

	temp = CStringToChar(m_strLogPath);
	if (strcmp(temp, g_strLogPath) != 0)
	{
		strcpy(g_strLogPath, temp);
		g_strLogPath[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "LOGPATH", g_strLogPath, g_strInifile);
	}
	delete[] temp;

	temp = CStringToChar(m_strDownPath);
	if (strcmp(temp, g_strDownPath) != 0)
	{
		strcpy(g_strDownPath, temp);
		g_strDownPath[strlen(temp)] = '\0';
		::WritePrivateProfileStringA("CONFIG", "DOWNPATH", g_strDownPath, g_strInifile);
	}
	delete[] temp;

	if (g_nConvertTimeOut != m_nConvertTimeout)
	{
		g_nConvertTimeOut = m_nConvertTimeout;
		char p[10] = { 0 };
		sprintf_s(p, "%d", g_nConvertTimeOut);
		::WritePrivateProfileStringA("CONFIG", "CONVERTTIMEOUT", p, g_strInifile);
	}

	if (g_nThreadNumber != m_nConvertThreads)
	{
		g_nThreadNumber = m_nConvertThreads;
		char p[10] = { 0 };
		sprintf_s(p, "%d", g_nThreadNumber);
		::WritePrivateProfileStringA("CONFIG", "THREADNUM", p, g_strInifile);
	}

	if (g_nTimeSpanApi != m_nSpanTime)
	{
		g_nTimeSpanApi = m_nSpanTime;
		char p[10] = { 0 };
		sprintf_s(p, "%d", g_nTimeSpanApi);
		::WritePrivateProfileStringA("CONFIG", "TIMESPANAPI", p, g_strInifile);
	}

	if (g_nTimeSpanApiNull != m_nSpanTimeNull)
	{
		g_nTimeSpanApiNull = m_nSpanTimeNull;
		char p[10] = { 0 };
		sprintf_s(p, "%d", g_nTimeSpanApiNull);
		::WritePrivateProfileStringA("CONFIG", "TIMESPANAPINULL", p, g_strInifile);
	}

	if (g_bIsDelSrc != m_bIsDelSrc)
	{
		g_bIsDelSrc = m_bIsDelSrc;
		char p[2] = { 0 };
		sprintf_s(p, "%d", g_bIsDelSrc);
		::WritePrivateProfileStringA("CONFIG", "ISDELSRC", p, g_strInifile);
	}

	if (g_bIsDownFile != m_bIsDownfile)
	{
		g_bIsDownFile = m_bIsDownfile;
		char p[2] = { 0 };
		sprintf_s(p, "%d", g_bIsDownFile);
		::WritePrivateProfileStringA("CONFIG", "ISDOWNFILE", p, g_strInifile);
	}

	if (g_bIsToImg != m_bIsToImg)
	{
		g_bIsToImg = m_bIsToImg;
		char p[2] = { 0 };
		sprintf_s(p, "%d", g_bIsToImg);
		::WritePrivateProfileStringA("CONFIG", "ISTOIMG", p, g_strInifile);
	}

	if (g_nMinPages != m_nMinPages)
	{
		g_nMinPages = m_nMinPages;
		char p[10] = { 0 };
		sprintf_s(p, "%d", g_nMinPages);
		::WritePrivateProfileStringA("CONFIG", "MINPAGE", p, g_strInifile);
	}

	if (g_nMaxFileInFloder != m_nMaxFileNumInFloder)
	{
		g_nMaxFileInFloder = m_nMaxFileNumInFloder;
		char p[10] = {0};
		sprintf_s(p, 10, "%d", g_nMaxFileInFloder);
		::WritePrivateProfileStringA("CONFIG", "MAXFILENUM", p, g_strInifile);
	}



	UpdateData(FALSE);
}


void CdocConvertServerDlg::DrawWindow()
{
	if (m_wndTabCtrl.GetSafeHwnd() == NULL) return;
	CRect r,r1,r2,r3,r4,r5,r6,r7,r8;
	this->GetClientRect(&r);

	m_wndTabCtrl.GetClientRect(&r1);
	r1.left = r.left;
	r1.right = r.right;
	r1.top = r.top;
	r1.bottom = r.bottom;
	m_wndTabCtrl.MoveWindow(r1);

// 	GetDlgItem(IDC_STATIC_MSG)->GetClientRect(&r2);
	r2.left = r1.left +5;
	r2.right = r1.right -5;
	r2.top = r1.top + 20;
	r2.bottom = r1.bottom - 40;
// 	GetDlgItem(IDC_STATIC_MSG)->MoveWindow(r2);

	GetDlgItem(IDC_LIST2)->GetClientRect(&r1);
	r1.left = r2.left + 5;
	r1.right = r2.right - 5;
	r1.top = r2.top + 15;
	r1.bottom = r2.bottom - 100;
	GetDlgItem(IDC_LIST2)->MoveWindow(r1);


	GetDlgItem(IDC_LIST1)->GetClientRect(&r2);
	r2 = r1;
	r2.top = r1.bottom + 10;
	r2.bottom = r2.top + 85;
	GetDlgItem(IDC_LIST1)->MoveWindow(r2);

	GetDlgItem(IDC_START)->GetClientRect(&r1);
	r1.left = r2.right - 80;
	r1.right = r2.right - 10;
	r1.top = r2.bottom + 5;
	r1.bottom = r1.top + 25;
	GetDlgItem(IDC_START)->MoveWindow(r1);

	GetDlgItem(IDC_STATIC_STARTTIME)->GetClientRect(&r1);
	r1.left  = r2.left;
	r1.right = r1.left + 64;
	r1.top = r2.bottom + 10;
	r1.bottom = r1.top + 20;
	GetDlgItem(IDC_STATIC_STARTTIME)->MoveWindow(r1);

	GetDlgItem(IDC_STARTTIME)->GetClientRect(&r2);
	r2 = r1;
	r2.left = r1.right + 5;
	r2.right = r2.left + 150;
	GetDlgItem(IDC_STARTTIME)->MoveWindow(r2);

	//设置
	//getapi
	GetDlgItem(IDC_STATIC_GETAPI)->GetClientRect(&r3);
	r3.left = r.left + 10;
	r3.right = r3.left + 130;
	r3.top = r.top + 30;
	r3.bottom = r3.top + 22;
	GetDlgItem(IDC_STATIC_GETAPI)->MoveWindow(r3);

	GetDlgItem(IDC_GETAPI)->GetClientRect(&r4);
	r4.left = r3.right + 10;
	r4.right = r.right - 10;
	r4.top = r3.top;
	r4.bottom = r3.bottom;
	GetDlgItem(IDC_GETAPI)->MoveWindow(r4);
	
	//
	GetDlgItem(IDC_STATIC_SUCCESSAPI)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r3.right;
	r5.top = r3.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_SUCCESSAPI)->MoveWindow(r5);

	GetDlgItem(IDC_SUCCESS_API)->GetClientRect(&r6);
	r6.left = r4.left;
	r6.right = r4.right;
	r6.top = r4.bottom + 10;
	r6.bottom = r6.top + 22;
	GetDlgItem(IDC_SUCCESS_API)->MoveWindow(r6);

	//
	GetDlgItem(IDC_STATIC_FAILEDAPI)->GetClientRect(&r3);
	r3.left = r5.left;
	r3.right = r5.right;
	r3.top = r5.bottom + 10;
	r3.bottom = r3.top + 22;
	GetDlgItem(IDC_STATIC_FAILEDAPI)->MoveWindow(r3);

	GetDlgItem(IDC_FAILED_API)->GetClientRect(&r4);
	r4.left = r6.left;
	r4.right = r6.right;
	r4.top = r6.bottom + 10;
	r4.bottom = r4.top + 22;
	GetDlgItem(IDC_FAILED_API)->MoveWindow(r4);

	//
	GetDlgItem(IDC_STATIC_LOGPATH)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r3.right;
	r5.top = r3.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_LOGPATH)->MoveWindow(r5);

	GetDlgItem(IDC_LOGPATH)->GetClientRect(&r6);
	r6.left = r4.left;
	r6.right = r4.right;
	r6.top = r4.bottom + 10;
	r6.bottom = r6.top + 22;
	GetDlgItem(IDC_LOGPATH)->MoveWindow(r6);

	//
	GetDlgItem(IDC_STATIC_YZAPI)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r3.right;
	r5.top = r6.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_YZAPI)->MoveWindow(r5);

	GetDlgItem(IDC_YZ_API)->GetClientRect(&r6);
	r6.left = r4.left;
	r6.right = r4.right;
	r6.top = r5.top;
	r6.bottom = r6.top + 22;
	GetDlgItem(IDC_YZ_API)->MoveWindow(r6);


	/*
	//
	GetDlgItem(IDC_STATIC_TXTSAVEPATH)->GetClientRect(&r3);
	r3.left = r5.left;
	r3.right = r5.right;
	r3.top = r5.bottom + 10;
	r3.bottom = r3.top + 22;
	GetDlgItem(IDC_STATIC_TXTSAVEPATH)->MoveWindow(r3);

	GetDlgItem(IDC_TXTSAVEPATH)->GetClientRect(&r4);
	r4.left = r6.left;
	r4.right = r6.right;
	r4.top = r6.bottom + 10;
	r4.bottom = r4.top + 22;
	GetDlgItem(IDC_TXTSAVEPATH)->MoveWindow(r4);

	//
	GetDlgItem(IDC_STATIC_TXTHTTP)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r3.right;
	r5.top = r3.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_TXTHTTP)->MoveWindow(r5);

	GetDlgItem(IDC_TXTHTTP)->GetClientRect(&r6);
	r6.left = r4.left;
	r6.right = r4.right;
	r6.top = r4.bottom + 10;
	r6.bottom = r6.top + 22;
	GetDlgItem(IDC_TXTHTTP)->MoveWindow(r6);

	//
	GetDlgItem(IDC_STATIC_IMGSAVEPATH)->GetClientRect(&r3);
	r3.left = r5.left;
	r3.right = r5.right;
	r3.top = r5.bottom + 10;
	r3.bottom = r3.top + 22;
	GetDlgItem(IDC_STATIC_IMGSAVEPATH)->MoveWindow(r3);

	GetDlgItem(IDC_IMGSAVEPATH)->GetClientRect(&r4);
	r4.left = r6.left;
	r4.right = r6.right;
	r4.top = r6.bottom + 10;
	r4.bottom = r4.top + 22;
	GetDlgItem(IDC_IMGSAVEPATH)->MoveWindow(r4);

	//
	GetDlgItem(IDC_STATIC_IMGHTTP)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r3.right;
	r5.top = r3.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_IMGHTTP)->MoveWindow(r5);

	GetDlgItem(IDC_IMGHTTP)->GetClientRect(&r6);
	r6.left = r4.left;
	r6.right = r4.right;
	r6.top = r4.bottom + 10;
	r6.bottom = r6.top + 22;
	GetDlgItem(IDC_IMGHTTP)->MoveWindow(r6);
	*/

	//下载
	GetDlgItem(IDC_CK_ISDOWNFILE)->GetClientRect(&r3);
	r3.left = r5.left;
	r3.right = r3.left + 130;
	r3.top = r5.bottom + 10;
	r3.bottom = r3.top + 22;
	GetDlgItem(IDC_CK_ISDOWNFILE)->MoveWindow(r3);

	GetDlgItem(IDC_STATIC_FILEPATH)->GetClientRect(&r4);
	r4 = r3;
	r4.left = r3.right + 10;
	r4.right = r4.left + 100;
	GetDlgItem(IDC_STATIC_FILEPATH)->MoveWindow(r4);

	GetDlgItem(IDC_FILEPATH)->GetClientRect(&r5);
	r5 = r4;
	r5.left = r4.right + 10;
	r5.right = r6.right;
	GetDlgItem(IDC_FILEPATH)->MoveWindow(r5);

	//超时
	GetDlgItem(IDC_STATIC_CONVERTTIMEOUT)->GetClientRect(&r5);
	r5.left = r3.left;
	r5.right = r5.left + 130;
	r5.top = r3.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_STATIC_CONVERTTIMEOUT)->MoveWindow(r5);

	GetDlgItem(IDC_CONVERT_TIMEOUT)->GetClientRect(&r6);
	r6.left = r5.right + 10;
	r6.right = r6.left + 80;
	r6.top = r5.top;
	r6.bottom = r5.bottom;
	GetDlgItem(IDC_CONVERT_TIMEOUT)->MoveWindow(r6);

	GetDlgItem(IDC_STATIC_CONVERTTHREADS)->GetClientRect(&r7);
	r7.left = r6.right + 10;
	r7.right = r7.left + 130;
	r7.top = r6.top;
	r7.bottom = r6.bottom;
	GetDlgItem(IDC_STATIC_CONVERTTHREADS)->MoveWindow(r7);

	GetDlgItem(IDC_CONVERT_THREADS)->GetClientRect(&r8);
	r8.left = r7.right + 10;
	r8.right = r8.left + 80;
	r8.top = r7.top;
	r8.bottom = r7.bottom;
	GetDlgItem(IDC_CONVERT_THREADS)->MoveWindow(r8);

	//
	GetDlgItem(IDC_STATIC_SPANTIME)->GetClientRect(&r1);
	r1 = r5;
	r1.top = r5.bottom + 10;
	r1.bottom = r1.top + 22;
	GetDlgItem(IDC_STATIC_SPANTIME)->MoveWindow(r1);

	GetDlgItem(IDC_SPANTIME)->GetClientRect(&r2);
	r2 = r6;
	r2.top = r1.top;
	r2.bottom = r1.bottom;
	GetDlgItem(IDC_SPANTIME)->MoveWindow(r2);

	GetDlgItem(IDC_STATIC_SPANTIMENULL)->GetClientRect(&r3);
	r3 = r7;
	r3.top = r1.top;
	r3.bottom = r1.bottom;
	GetDlgItem(IDC_STATIC_SPANTIMENULL)->MoveWindow(r3);

	GetDlgItem(IDC_SPANTIME_NULL)->GetClientRect(&r4);
	r4 = r8;
	r4.top = r1.top;
	r4.bottom = r1.bottom;
	GetDlgItem(IDC_SPANTIME_NULL)->MoveWindow(r4);

	//
	GetDlgItem(IDC_CK_DELSRC)->GetClientRect(&r5);
	r5 = r1;
	r5.top = r1.bottom + 10;
	r5.bottom = r5.top + 22;
	GetDlgItem(IDC_CK_DELSRC)->MoveWindow(r5);

	GetDlgItem(IDC_STATIC_MINPAGE)->GetClientRect(&r6);
	r6 = r3;
	r6.top = r5.top;
	r6.bottom = r5.bottom;
	GetDlgItem(IDC_STATIC_MINPAGE)->MoveWindow(r6);

	GetDlgItem(IDC_MINPAGES)->GetClientRect(&r7);
	r7 = r4;
	r7.top = r5.top;
	r7.bottom = r5.bottom;
	GetDlgItem(IDC_MINPAGES)->MoveWindow(r7);

	GetDlgItem(IDC_STATIC_MAXFILENUM)->GetClientRect(&r3);
	r3 = r7;
	r3.left = r7.right + 10;
	r3.right = r3.left + 130;
	GetDlgItem(IDC_STATIC_MAXFILENUM)->MoveWindow(r3);

	GetDlgItem(IDC_MAXFILENUM)->GetClientRect(&r4);
	r4 = r3;
	r4.left = r3.right + 10;
	r4.right = r4.left + 80;
	GetDlgItem(IDC_MAXFILENUM)->MoveWindow(r4);


	//
	GetDlgItem(IDC_CK_ISTOIMG)->GetClientRect(&r1);
	r1 = r5;
	r1.top = r5.bottom + 10;
	r1.bottom = r1.top + 22;
	GetDlgItem(IDC_CK_ISTOIMG)->MoveWindow(r1);

	GetDlgItem(IDC_STATIC_TIPS)->GetClientRect(&r2);
	r2 = r1;
	r2.left = r1.right + 10;
	r2.right = r.right - 10;
	GetDlgItem(IDC_STATIC_TIPS)->MoveWindow(r2);

	GetDlgItem(IDC_LIST3)->GetClientRect(&r2);
	r2.left = r1.left;
	r2.right = r.right - 10;
	r2.top = r1.bottom + 10;
	r2.bottom = r.bottom - 50;
	GetDlgItem(IDC_LIST3)->MoveWindow(r2);


	//button clear
	GetDlgItem(IDC_BT_CLEARSQLITE)->GetClientRect(&r8);
	r8.right = r.right - 100;
	r8.left = r.right - 170;
	r8.top = r.bottom - 42;
	r8.bottom = r.bottom - 15;
	GetDlgItem(IDC_BT_CLEARSQLITE)->MoveWindow(r8);


	//button save
	GetDlgItem(IDC_BT_SAVECONFIG)->GetClientRect(&r8);
	r8.right = r.right - 20;
	r8.left = r.right - 90;
	r8.top = r.bottom - 42;
	r8.bottom = r.bottom - 15;
	GetDlgItem(IDC_BT_SAVECONFIG)->MoveWindow(r8);

	UpTableShow();
}

void CdocConvertServerDlg::UpTableShow()
{
	if (m_wndTabCtrl.GetSafeHwnd() == NULL) return;

	int nSel = m_wndTabCtrl.GetCurSel();
	switch (nSel)
	{
	case 0:
// 		m_wndTabCtrl.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MSG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_START)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_STARTTIME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STARTTIME)->ShowWindow(SW_SHOW);
		//
		GetDlgItem(IDC_STATIC_GETAPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GETAPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SUCCESSAPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SUCCESS_API)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FAILEDAPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FAILED_API)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FILEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FILEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_YZAPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YZ_API)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_TXTSAVEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TXTSAVEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TXTHTTP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXTHTTP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_IMGSAVEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMGSAVEPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_IMGHTTP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMGHTTP)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_LOGPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LOGPATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONVERTTIMEOUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CONVERT_TIMEOUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONVERTTHREADS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CONVERT_THREADS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SPANTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPANTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SPANTIMENULL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPANTIME_NULL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CK_DELSRC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MINPAGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MINPAGES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BT_SAVECONFIG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BT_CLEARSQLITE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CK_ISDOWNFILE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CK_ISTOIMG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TIPS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MAXFILENUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MAXFILENUM)->ShowWindow(SW_HIDE);
		break;
	case 1:
// 		m_wndTabCtrl.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MSG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STARTTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STARTTIME)->ShowWindow(SW_HIDE);

		//
		GetDlgItem(IDC_STATIC_GETAPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GETAPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SUCCESSAPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SUCCESS_API)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FAILEDAPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_FAILED_API)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FILEPATH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_FILEPATH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_YZAPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YZ_API)->ShowWindow(SW_SHOW);

// 		GetDlgItem(IDC_TXTSAVEPATH)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_STATIC_TXTSAVEPATH)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_STATIC_TXTHTTP)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_TXTHTTP)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_STATIC_IMGSAVEPATH)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_IMGSAVEPATH)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_STATIC_IMGHTTP)->ShowWindow(SW_SHOW);
// 		GetDlgItem(IDC_IMGHTTP)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_LOGPATH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LOGPATH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CONVERTTIMEOUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CONVERT_TIMEOUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CONVERTTHREADS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CONVERT_THREADS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPANTIME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPANTIME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPANTIMENULL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPANTIME_NULL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CK_DELSRC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MINPAGE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MINPAGES)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BT_SAVECONFIG)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BT_CLEARSQLITE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CK_ISDOWNFILE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CK_ISTOIMG)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_TIPS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MAXFILENUM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MAXFILENUM)->ShowWindow(SW_SHOW);

		break;
	}
}

void CdocConvertServerDlg::StartGetApiThread()
{
	if (m_pGetApiThread == NULL)
	{
		m_pGetApiThread = new CGetAPIDataThread();
		m_pGetApiThread->CreateThread(CREATE_SUSPENDED, 0, NULL);
		m_pGetApiThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		m_pGetApiThread->SetMainHwnd(this->GetSafeHwnd());
		m_pGetApiThread->ResumeThread();
	}
}

void CdocConvertServerDlg::StopGetApiThread()
{
	if (m_pGetApiThread != NULL)
	{
		m_pGetApiThread->m_bAbort = TRUE;
		SetEvent(m_pGetApiThread->m_hKillEvent);
		if (WaitForSingleObject(m_pGetApiThread->m_hThread, 300) != WAIT_OBJECT_0)
		{
			return;
		}
		delete m_pGetApiThread;
		m_pGetApiThread = NULL;
	}
}

void CdocConvertServerDlg::StartGetConvertDataThread()
{
	if (m_pGetConvertDataThread == NULL)
	{
		m_pGetConvertDataThread = new CGetConvertDataThread();
		m_pGetConvertDataThread->CreateThread(CREATE_SUSPENDED, 0, NULL);
		m_pGetConvertDataThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		m_pGetConvertDataThread->SetMainHwnd(this->GetSafeHwnd());
		m_pGetConvertDataThread->ResumeThread();
	}
}

void CdocConvertServerDlg::StopGetConvertDataThread()
{
	if (m_pGetConvertDataThread != NULL)
	{
		m_pGetConvertDataThread->m_bAbort = TRUE;
		SetEvent(m_pGetConvertDataThread->m_hKillEvent);
		WaitForSingleObject(m_pGetConvertDataThread->m_hThread, INFINITE);
		delete m_pGetConvertDataThread;
		m_pGetConvertDataThread = NULL;
	}
}

void CdocConvertServerDlg::StartConvertThread()
{
	for (int i = 0; i < g_nThreadNumber;i++)
	{
		if (m_pConvertThread[i] == NULL)
		{
			m_pConvertThread[i] = new CConvertThread();
			m_pConvertThread[i]->CreateThread(CREATE_SUSPENDED, 0, NULL);
			m_pConvertThread[i]->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			m_pConvertThread[i]->SetMainHwnd(this->GetSafeHwnd());
			m_pConvertThread[i]->ResumeThread();
		}
	}
}

void CdocConvertServerDlg::StopConvertThread()
{
	for (int i = 0; i < g_nThreadNumber; i++)
	{
		if (m_pConvertThread[i] != NULL)
		{
			m_pConvertThread[i]->m_bAbort = TRUE;
		}
	}
	for (int i = 0; i < g_nThreadNumber; i++)
	{
		if (m_pConvertThread[i] != NULL)
		{
			SetEvent(m_pConvertThread[i]->m_hKillEvent);
			if (WaitForSingleObject(m_pConvertThread[i]->m_hThread, 100) != WAIT_OBJECT_0)
			{
				continue;
			}
			delete m_pConvertThread[i];
			m_pConvertThread[i] = NULL;
		}
	}
}


void CdocConvertServerDlg::StartPostResultThread()
{
	if (m_pPostResultThread == NULL)
	{
		m_pPostResultThread = new CPostResultThread();
		m_pPostResultThread->CreateThread(CREATE_SUSPENDED, 0, NULL);
		m_pPostResultThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		m_pPostResultThread->SetMainHwnd(this->GetSafeHwnd());
		m_pPostResultThread->ResumeThread();
	}

}

void CdocConvertServerDlg::StopPostResultThread()
{
	if (m_pPostResultThread != NULL)
	{
		m_pPostResultThread->m_bAbort = TRUE;
		SetEvent(m_pPostResultThread->m_hKillEvent);
		WaitForSingleObject(m_pPostResultThread->m_hThread, INFINITE);
		delete m_pPostResultThread;
		m_pPostResultThread = NULL;
	}
}


void CdocConvertServerDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpTableShow();
	*pResult = 0;
}


void CdocConvertServerDlg::OnBnClickedBtSaveconfig()
{
	WriteIni();
}


void CdocConvertServerDlg::OnBnClickedStart()
{
	StartGetApiThread();
	StartGetConvertDataThread();
#ifndef SYNC_POST_RESULT
	StartPostResultThread();
#endif
	StartConvertThread();

	GetDlgItem(IDC_START)->EnableWindow(FALSE);
}

//*
void CdocConvertServerDlg::ShowMsgList(char *msg, BOOL bsavelog)
{
	if (m_wndMsgList.GetCount()> 100)
	{
		m_wndMsgList.DeleteString(100);
	}
	CString strnow = COleDateTime::GetCurrentTime().Format(L"%Y-%m-%d %H:%M:%S");
	m_wndMsgList.InsertString(0, strnow + L" -- " + CharToCString(msg));
// 	m_wndMsgList.AddString(strnow + L" -- " +CharToCString(msg));
// 	m_wndMsgList.SetCurSel(m_wndMsgList.GetCount()-1);
	if (bsavelog)
		WriteLog(g_strLogPath, msg);
}

void CdocConvertServerDlg::InitListCtrlDomain()
{
	DWORD dwStyle = m_wndListCtrlDomain.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	// 	dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_wndListCtrlDomain.SetExtendedStyle(dwStyle); //设置扩展风格

	m_wndListCtrlDomain.InsertColumn(0, L"域名", LVCFMT_LEFT, 130);
	m_wndListCtrlDomain.InsertColumn(1, L"域名对应磁盘路径", LVCFMT_LEFT, 140);
	m_wndListCtrlDomain.InsertColumn(2, L"txt路径", LVCFMT_LEFT, 140);
	m_wndListCtrlDomain.InsertColumn(3, L"txt域名", LVCFMT_LEFT, 140);
	m_wndListCtrlDomain.InsertColumn(4, L"图片路径", LVCFMT_LEFT, 140);
	m_wndListCtrlDomain.InsertColumn(5, L"图片域名", LVCFMT_LEFT, 140);
}

void CdocConvertServerDlg::AddListCtrlDomain(p_st_domain2path sm)
{
	// 	if (m_wndListMsg.GetItemCount() > 1000) m_wndListMsg.DeleteItem(0);
	int nid = m_wndListCtrlDomain.GetItemCount();

	m_wndListCtrlDomain.InsertItem(nid, CharToCString(sm->domain));
	m_wndListCtrlDomain.SetItemText(nid, 1, CharToCString(sm->path));
	m_wndListCtrlDomain.SetItemText(nid, 2, CharToCString(sm->txtpath));
	m_wndListCtrlDomain.SetItemText(nid, 3, CharToCString(sm->txthttp));
	m_wndListCtrlDomain.SetItemText(nid, 4, CharToCString(sm->imgpath));
	m_wndListCtrlDomain.SetItemText(nid, 5, CharToCString(sm->imghttp));


	// 	m_wndListMsg.EnsureVisible(sm->sc->id, FALSE);
}


void CdocConvertServerDlg::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	// 	dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_wndListCtrl.SetExtendedStyle(dwStyle); //设置扩展风格

	m_wndListCtrl.InsertColumn(0, L"线程ID", LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(1, L"文档ID", LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(2, L"类型", LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(3, L"开始时间", LVCFMT_LEFT, 150);
	m_wndListCtrl.InsertColumn(4, L"当前状态", LVCFMT_LEFT, 140);
}

void CdocConvertServerDlg::AddListCtrl(p_st_msg sm)
{
	// 	if (m_wndListMsg.GetItemCount() > 1000) m_wndListMsg.DeleteItem(0);
	int nid = m_wndListCtrl.GetItemCount();

	if (nid > g_nThreadNumber+6)
	{
		for (int i = 0; i < nid; i++)
		{
			CString strtemp = m_wndListCtrl.GetItemText(i, 4);
			if (strtemp.Compare(strConvertStatus[WaitConvert]) == 0)
			{
				CString strtime = m_wndListCtrl.GetItemText(i, 3);
				COleDateTime oleDate;
				oleDate.ParseDateTime(strtime);
				COleDateTime now = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant(0, 0, 3, 0);
				if (now - oleDate>spant)
				{
					m_wndListCtrl.DeleteItem(i);
					nid -= 1;
					break;
				}
			}
			else if (strtemp.Compare(strConvertStatus[Converting]) != 0)
			{
				// 				printf("delete item id = %d\n", i);
				m_wndListCtrl.DeleteItem(i);
				nid -= 1;
				break;
			}
		}
	}

	CString strid;
	strid.Format(L"%d", sm->threadid);
	m_wndListCtrl.InsertItem(nid, strid);
	COleDateTime now = COleDateTime::GetCurrentTime();
	strid.Format(L"%d", sm->sc->fileid);
	m_wndListCtrl.SetItemText(nid, 1, strid);
	m_wndListCtrl.SetItemText(nid, 2, CharToCString(strFileType[sm->sc->filetype]));
	m_wndListCtrl.SetItemText(nid, 3, now.Format(L"%Y-%m-%d %H:%M:%S"));
	m_wndListCtrl.SetItemText(nid, 4, strConvertStatus[WaitConvert]);


	// 	m_wndListMsg.EnsureVisible(sm->sc->id, FALSE);
}

void CdocConvertServerDlg::UpdateListCtrl(p_st_msg sm, OutStatus status)
{
	int ncount = m_wndListCtrl.GetItemCount();
	for (int i = 0; i < ncount; i++)
	{
		CString strtempid = m_wndListCtrl.GetItemText(i, 1);
		CString strtempthreadid = m_wndListCtrl.GetItemText(i, 0);
		int fileid = _ttoi(strtempid);
		int threadid = _ttoi(strtempthreadid);
		// 		printf("id = %d\n", fileid);
		if (fileid == sm->sc->fileid && threadid == sm->threadid)
		{
			m_wndListCtrl.SetItemText(i, 4, strConvertStatus[status]);

			break;
		}
	}


}//*/

afx_msg LRESULT CdocConvertServerDlg::OnMsglistShow(WPARAM wParam, LPARAM lParam)
{
	ShowMsgList((char*)lParam, (BOOL)wParam);
	delete[] (char*)lParam;
	return 0;
}

afx_msg LRESULT CdocConvertServerDlg::OnMsgMsgLog(WPARAM wParam, LPARAM lParam)
{
	char *msg = (char*)lParam;
	WriteLog(g_strLogPath, msg);
	delete[] msg;
	return 0;
}


afx_msg LRESULT CdocConvertServerDlg::OnMsgListctrlAdd(WPARAM wParam, LPARAM lParam)
{
	p_st_msg sm = (p_st_msg)lParam;
	if (sm != NULL)
	{
		if (sm->sc != NULL)
		{
			// 			printf("add msg : id = %d,  fileid = %d , filetype = %d \n", sm->sc->id, sm->sc->fileid, sm->sc->type);
			if (sm->sc->fileid < 0)
			{
// 				delete sm->sc;
				delete sm;
				return 0;
			}
			AddListCtrl(sm);
// 			delete sm->sc;
			delete sm;
		}
	}
	return 0;
}


afx_msg LRESULT CdocConvertServerDlg::OnMsgListctrlUpdate(WPARAM wParam, LPARAM lParam)
{
	p_st_msg sm = (p_st_msg)lParam;
	if (sm != NULL)
	{
		if (sm->sc != NULL)
		{
			if (sm->sc->fileid < 0)
			{
				char logmsg[2000] = { 0 };
				sprintf_s(logmsg, 2000, "%s line %d Error! fileid = %d, filetype = %s",__FILE__, __LINE__, sm->sc->fileid, strFileType[sm->sc->filetype]);
// 				WriteLog(g_strLogPath, logmsg);
				ShowMsgList(logmsg);

// 				delete sm->sc;
				delete sm;
				return 0;
			}
			UpdateListCtrl(sm, (OutStatus)wParam);
// 			delete sm->sc;
			delete sm;
		}
	}
	return 0;
}


void CdocConvertServerDlg::OnBnClickedCkIsdownfile()
{
	UpdateData(TRUE);
	if (m_bIsDownfile)
	{
		GetDlgItem(IDC_CK_DELSRC)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CK_DELSRC)->EnableWindow(FALSE);
		m_bIsDelSrc = FALSE;
	}
	UpdateData(FALSE);
// 	UpdateData(TRUE);
}


void CdocConvertServerDlg::OnBnClickedCkDelsrc()
{
	UpdateData(TRUE);
	if (m_bIsDelSrc)
	{
		AfxMessageBox(L"确定删除源文件！");
	}
}


void CdocConvertServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	StopGetConvertDataThread();
	StopGetApiThread();
	StopPostResultThread();
	StopConvertThread();
	curl_global_cleanup();
	exit(0);
}


void CdocConvertServerDlg::OnBnClickedBtClearsqlite()
{
	CConnectDB::GetInstance()->delete_convert_table_all();
}


