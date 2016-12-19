// libHttpPost.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "libHttpPost.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define WM_MSG_HTTPPOST WM_USER + 0x41


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	

	typedef struct ST_CONVERT_STATUS
	{
		int poststatus;
		bool bstop;
	}ST_C_S, *pST_C_S;

	HANDLE StartListenToHttpPostThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI HttpPostStatusThreadFunc(LPVOID*lpParamter);


	int WINAPI lib_HttpPostMsg(const char *url, int type, int id, const char *node, int txtstatus, int pagenumber, const char *txturl, const char *imgurl, const int &timeout)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->poststatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToHttpPostThread(&nthreadId, pst);

		char paramstr[5000] = { 0 };
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		si.cbReserved2 = NULL;
		si.lpReserved2 = NULL;

		sprintf_s(paramstr, 5000, "HttpPost.exe %d %s %d %d %s %d %d %s %s", nthreadId, url, type, id, node, txtstatus, pagenumber, txturl, imgurl);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->poststatus != -1) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//下载超时
			if (bConvertTimeOut)
			{
				pst->poststatus = 1;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while (pst->poststatus == -1)
				{
					COleDateTime currt = COleDateTime::GetCurrentTime();
					COleDateTimeSpan spant = COleDateTimeSpan(0, 0, 0, 10);
					if (currt - bTime > spant)
					{
						break;
					}

					Sleep(500);
				}
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
		}
		if (hthread != NULL)
		{
			try
			{
				pst->bstop = true;
				CloseHandle(hthread);
			}
			catch (...)
			{
			}
		}
		int status = pst->poststatus;
		delete pst;
		return status;
	}


	DWORD WINAPI HttpPostStatusThreadFunc(LPVOID*lpParamter)
	{
		pST_C_S pst = (pST_C_S)lpParamter;
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		while (!pst->bstop)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_MSG_HTTPPOST:
				{
											   pst->poststatus = msg.wParam;
											   return 0;
				}break;
				}
			}
			Sleep(50);
		}
		return 0;
	}

	HANDLE StartListenToHttpPostThread(DWORD *nid, pST_C_S pst)
	{
		HANDLE hThread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HttpPostStatusThreadFunc, (LPVOID)pst, 0, nid);
		return hThread_;
	}




#ifdef __cplusplus
}
#endif // __cpluspl	