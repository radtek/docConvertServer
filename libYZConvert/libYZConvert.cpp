// libYZConvert.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "libYZConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_MSG_YZ2TXT_STATUS WM_USER + 0x21
#define WM_MSG_YZ2IMG_STATUS WM_USER + 0x22


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	

	typedef struct ST_CONVERT_STATUS
	{
		int txtstatus;
		int pageNum;
		int pngstatus;
		bool bstop;
	}ST_C_S, *pST_C_S;

	HANDLE StartListenToTxtStatusThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI ToTxtStatusThreadFunc(LPVOID*lpParamter);

	int WINAPI StartYZConvertToTxt(const char *file, const char *outpath, int filetype, const int &fileid, const int &minPage, int &pageNum)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->txtstatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToTxtStatusThread(&nthreadId, pst);

		char paramstr[3000] = { 0 };
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

		sprintf_s(paramstr, 3000, "YZConvertToTxt.exe %s %s %d %d %d %d", file, outpath, filetype, nthreadId, fileid, minPage);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->txtstatus != -1) break;
			}
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		while (pst->txtstatus == -1)
		{
			COleDateTime currt = COleDateTime::GetCurrentTime();
			COleDateTimeSpan spant = COleDateTimeSpan(0, 0, 0, 3);
			if (currt - bTime > spant)
			{
				break;
			}

			Sleep(50);
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
		int status = pst->txtstatus;
		pageNum = pst->pageNum;
		delete pst;
		return status == -1 ? 3 : status;
	}


	DWORD WINAPI ToTxtStatusThreadFunc(LPVOID*lpParamter)
	{
		pST_C_S pst = (pST_C_S)lpParamter;
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		while (!pst->bstop)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_MSG_YZ2TXT_STATUS:
				{
					pst->txtstatus = msg.wParam;
					pst->pageNum = (int)msg.lParam;
					return 0;
				}break;
				}
			}
			Sleep(50);
		}
		return 0;
	}

	HANDLE StartListenToTxtStatusThread(DWORD *nid, pST_C_S pst)
	{
		HANDLE hThread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ToTxtStatusThreadFunc, (LPVOID)pst, 0, nid);
		return hThread_;
	}

#ifdef __cplusplus
}
#endif // __cpluspl	