
// docConvertServer.cpp : ����Ӧ�ó��������Ϊ��
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


// CdocConvertServerApp ����

CdocConvertServerApp::CdocConvertServerApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CdocConvertServerApp ����

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
list<p_st_tconvert> g_ltConvert;//��ת���ĵ�
CMutex g_mtxConvert;//�������ĵ��б���
list<p_st_tconverted> g_ltConvertSuccess;//ת���ɹ��б�
CMutex g_mtxConvertSuccess;
list<p_st_tconverted> g_ltConvertFailed;
CMutex g_mtxConvertFailed;
int g_nMaxFileInFloder;
int g_nMultiProcessId;//����̱�־
HANDLE g_hMulti;
BOOL g_bAutoStart;//����������Զ���ʼת��

char* g_strInifile;
char* g_strNode;

// CdocConvertServerApp ��ʼ��

BOOL CdocConvertServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

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
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	ReleaseGlobalData();
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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