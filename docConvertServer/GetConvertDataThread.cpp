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

	list<p_st_tconvert> templist;

	g_mtxConvert.Lock();
	CConnectDB::GetInstance()->query_convert_table(g_ltConvert, 500);
	list<p_st_tconvert>::iterator it = g_ltConvert.begin();
	while (it != g_ltConvert.end())
	{
		if ((*it)->status == 0)
		{
			int bok = CConnectDB::GetInstance()->update_convert_table((p_st_tconvert)*it);
		}
		it++;
	}
	g_mtxConvert.Unlock();

	int times = 10;
	while (WaitForSingleObject(m_hKillEvent,100) != WAIT_OBJECT_0 )
	{
		if (m_bAbort) break;

		if (times-- > 0) continue;
		times = 10;

		g_mtxConvert.Lock();
		int nltsize = g_ltConvert.size();
		printf("***GetConvertData ltConvert size = %d ***\n", nltsize);
		if (nltsize > 80)
		{
			g_mtxConvert.Unlock();
			continue;
		}
		g_mtxConvert.Unlock();

		int ncount = CConnectDB::GetInstance()->query_convert_table(templist, 300, 0);
		if (templist.size() > 0)
		{
			list<p_st_tconvert>::iterator it = templist.begin();
			while (it != templist.end())
			{
				g_mtxConvert.Lock();
				g_ltConvert.push_back(*it);
				g_mtxConvert.Unlock();
				it++;
			}
			templist.clear();
		}
		if (templist.size() > 0)
		{
			list<p_st_tconvert>::iterator it = templist.begin();
			while (it != templist.end())
			{
				int bok = CConnectDB::GetInstance()->update_convert_table((p_st_tconvert)*it);
				it++;
			}
			templist.clear();
		}

	}

	return 0;
}

void CGetConvertDataThread::SetMainHwnd(HWND hwnd)
{
	m_hMainWnd = hwnd;
}