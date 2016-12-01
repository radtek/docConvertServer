// GetConvertDataThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "GetConvertDataThread.h"
#include "ConnectDB.h"

// CGetConvertDataThread

IMPLEMENT_DYNCREATE(CGetConvertDataThread, CWinThread)

CGetConvertDataThread::CGetConvertDataThread()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bAbort = FALSE;
	m_bAutoDelete = FALSE;
	m_hMainWnd = NULL;
}

CGetConvertDataThread::~CGetConvertDataThread()
{
	CloseHandle(m_hKillEvent);
}

BOOL CGetConvertDataThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CGetConvertDataThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGetConvertDataThread, CWinThread)
END_MESSAGE_MAP()


// CGetConvertDataThread ��Ϣ�������


int CGetConvertDataThread::Run()
{
	char *showmsg = new char[1024];
	sprintf_s(showmsg, 1024, "��ת�ļ���ȡ�߳� --- �����ɹ�");
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)showmsg);

	g_mtxConvert.Lock();
	CConnectDB::GetInstance()->query_convert_table(g_ltConvert);
	list<p_st_tconvert>::iterator it = g_ltConvert.begin();
	while (it != g_ltConvert.end())
	{
		int bok = CConnectDB::GetInstance()->update_convert_table((p_st_tconvert)*it);
		it++;
	}
	g_mtxConvert.Unlock();

	int times = 6;
	while (WaitForSingleObject(m_hKillEvent,100) != WAIT_OBJECT_0 )
	{
		if (m_bAbort) break;

		if (times-- > 0) continue;
		times = 6;

		g_mtxConvert.Lock();
		if (g_ltConvert.size() != 0)
		{
			g_mtxConvert.Unlock();
			continue;
		}
		int ncount = CConnectDB::GetInstance()->query_convert_table(g_ltConvert, 0);
		list<p_st_tconvert>::iterator it = g_ltConvert.begin();
		while (it != g_ltConvert.end())
		{
			int bok = CConnectDB::GetInstance()->update_convert_table((p_st_tconvert)*it);			
			it++;
		}
		g_mtxConvert.Unlock();
	}

	return 0;
}

void CGetConvertDataThread::SetMainHwnd(HWND hwnd)
{
	m_hMainWnd = hwnd;
}