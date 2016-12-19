// libDownfile.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "libDownfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_MSG_DOWNFILE_STATUS WM_USER + 0x40


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	

	typedef struct ST_CONVERT_STATUS
	{
		int downstatus;
		bool bstop;
	}ST_C_S, *pST_C_S;

	HANDLE StartListenToDownStatusThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI DownfileStatusThreadFunc(LPVOID*lpParamter);


	int WINAPI StartDownfile(const char *url, const char *filepath, const int &timeout)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->downstatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToDownStatusThread(&nthreadId, pst);

		char paramstr[2000] = { 0 };
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		si.cbReserved2 = NULL;
		si.lpReserved2 = NULL;

		sprintf_s(paramstr, 2000, "Downfile.exe %d %s %s", nthreadId, url, filepath);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->downstatus != -1) break;
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
				pst->downstatus = 1;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while (pst->downstatus == -1)
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
		int status = pst->downstatus;
		delete pst;
		return status;
	}


	DWORD WINAPI DownfileStatusThreadFunc(LPVOID*lpParamter)
	{
		pST_C_S pst = (pST_C_S)lpParamter;
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		while (!pst->bstop)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_MSG_DOWNFILE_STATUS:
				{
											  pst->downstatus = msg.wParam;
											  return 0;
				}break;
				}
			}
			Sleep(50);
		}
		return 0;
	}

	HANDLE StartListenToDownStatusThread(DWORD *nid, pST_C_S pst)
	{
		HANDLE hThread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownfileStatusThreadFunc, (LPVOID)pst, 0, nid);
		return hThread_;
	}




#ifdef __cplusplus
}
#endif // __cpluspl	