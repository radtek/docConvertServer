// libWordConvert.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "libWordConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

#define WM_MSG_WORD2TXT_STATUS WM_USER + 0x11
#define WM_MSG_WORD2IMG_STATUS WM_USER + 0x12


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

	int WINAPI StartConvertWordToTxt(const char *file, const char *outpath, const int &fileid, const int &minPage, int &pageNum, const int &txtSize, const int &timeout)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->txtstatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToTxtStatusThread(&nthreadId, pst);

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

		sprintf_s(paramstr, 2000, "wordtotxt.exe %s %s %d %d %d %d", file, outpath, nthreadId, fileid, minPage, txtSize);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->txtstatus != -1) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//转换超时
			if (bConvertTimeOut)
			{
				pst->txtstatus = 0;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while (pst->txtstatus == -1)
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
				case WM_MSG_WORD2TXT_STATUS:
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



	HANDLE StartListenToImgStatusThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI ToImgStatusThreadFunc(LPVOID*lpParamter);


	int WINAPI StartConvertWordToImg(const char *file, const char *outpath, const int &fileid, const int &timeout)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->pngstatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToImgStatusThread(&nthreadId, pst);

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

		sprintf_s(paramstr, 2000, "wordtoimg.exe %s %s %d %d", file, outpath, nthreadId, fileid);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->pngstatus != -1) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//转换超时
			if (bConvertTimeOut)
			{
				pst->pngstatus = 0;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while (pst->pngstatus == -1)
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
		int status = pst->pngstatus;
		delete pst;
		return status == -1 ? 3 : status;
	}


	DWORD WINAPI ToImgStatusThreadFunc(LPVOID*lpParamter)
	{
		pST_C_S pst = (pST_C_S)lpParamter;
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		while (!pst->bstop)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_MSG_WORD2IMG_STATUS:
				{
											   pst->pngstatus = msg.wParam;
											   return 0;
				}break;
				}
			}
			Sleep(50);
		}
		return 0;
	}

	HANDLE StartListenToImgStatusThread(DWORD *nid, pST_C_S pst)
	{
		HANDLE hThread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ToImgStatusThreadFunc, (LPVOID)pst, 0, nid);
		return hThread_;
	}


	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//to txt png  
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	HANDLE StartListenToTxtPngStatusThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI ToTxtPngStatusThreadFunc(LPVOID*lpParamter);


	void WINAPI StartConvertWordToTxtPng(const char *file, const char *outtxtpath, const char *outimgpath, const int &fileid, const int &isoriginal, const int &minPage, int &pageNum, const int &txtSize, const int &timeout, const int &istoimg, int &txtstatus, int &pngstatus)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->txtstatus = -1;
		pst->pngstatus = -1;
		pst->bstop = false;
		DWORD nthreadId;
		HANDLE hthread = StartListenToTxtPngStatusThread(&nthreadId, pst);

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

		sprintf_s(paramstr, 2000, "wordtotxtpng.exe %s %s %s %d %d %d %d %d %d", file, outtxtpath, outimgpath, nthreadId, fileid, minPage, txtSize, istoimg, isoriginal);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if ((pst->txtstatus != -1 && !istoimg) || (pst->txtstatus != -1 && pst->pngstatus != -1) || (pst->txtstatus != -1 && pst->txtstatus != 0)) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//转换超时
			if (bConvertTimeOut)
			{
				pst->txtstatus = 3;
				pst->pngstatus = 3;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while ((pst->txtstatus == -1 && !istoimg) || (pst->txtstatus == -1 && pst->pngstatus == -1))
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
		pageNum = pst->pageNum;
		txtstatus = pst->txtstatus == -1 ? 3 : pst->txtstatus;
		pngstatus = pst->pngstatus == -1 ? 3 : pst->pngstatus;
		delete pst;
	}


	DWORD WINAPI ToTxtPngStatusThreadFunc(LPVOID*lpParamter)
	{
		pST_C_S pst = (pST_C_S)lpParamter;
		MSG msg = { 0 };
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		while (!pst->bstop)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_MSG_WORD2TXT_STATUS:
				{
											   pst->txtstatus = msg.wParam;
											   pst->pageNum = msg.lParam;
											   if (pst->txtstatus != 0)
											   {
												   return 0;
											   }
				}break;
				case WM_MSG_WORD2IMG_STATUS:
				{
											   pst->pngstatus = msg.wParam;
											   return 0;
				}break;
				}
			}
			Sleep(50);
		}
		return 0;
	}

	HANDLE StartListenToTxtPngStatusThread(DWORD *nid, pST_C_S pst)
	{
		HANDLE hThread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ToTxtPngStatusThreadFunc, (LPVOID)pst, 0, nid);
		return hThread_;
	}

#ifdef __cplusplus
}
#endif // __cpluspl	