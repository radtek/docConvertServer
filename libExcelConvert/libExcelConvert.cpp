// libExcelConvert.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "libExcelConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

#define WM_MSG_EXCEL2TXT_STATUS WM_USER + 0x13
#define WM_MSG_EXCEL2IMG_STATUS WM_USER + 0x14


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	

	typedef struct ST_CONVERT_STATUS
	{
		int txtstatus;
		int pageNum;
		int imgstatus;
		bool bstop;
	}ST_C_S, *pST_C_S;

	HANDLE StartListenToTxtStatusThread(DWORD *nid, pST_C_S pst);
	DWORD WINAPI ToTxtStatusThreadFunc(LPVOID*lpParamter);

	int WINAPI StartConvertExcelToTxt(const char *file, const char *outpath, const int &fileid, const int &minPage, int &pageNum, const int &txtSize, const int &timeout)
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

		sprintf_s(paramstr, 2000, "exceltotxt.exe %s %s %d %d %d %d", file, outpath, nthreadId, fileid, minPage, txtSize);

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
			//ת����ʱ
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
				case WM_MSG_EXCEL2TXT_STATUS:
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


	int WINAPI StartConvertExcelToImg(const char *file, const char *outpath, const int &fileid, const int &timeout)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->imgstatus = -1;
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

		sprintf_s(paramstr, 2000, "exceltoimg.exe %s %s %d %d", file, outpath, nthreadId, fileid);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if (pst->imgstatus != -1) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//ת����ʱ
			if (bConvertTimeOut)
			{
				pst->imgstatus = 0;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while (pst->imgstatus == -1)
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
		int status = pst->imgstatus;
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
				case WM_MSG_EXCEL2IMG_STATUS:
				{
												pst->imgstatus = msg.wParam;
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


	void WINAPI StartConvertExcelToTxtPng(const char *file, const char *outtxtpath, const char *outimgpath, const int &fileid, const int &isoriginal, const int &minPage, int &pageNum, const int &txtSize, const int &timeout, const int &istoimg, int &txtstatus, int &pngstatus)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		pST_C_S pst = new ST_C_S;
		pst->txtstatus = -1;
		pst->imgstatus = -1;
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

		sprintf_s(paramstr, 2000, "exceltotxtpng.exe %s %s %s %d %d %d %d %d %d", file, outtxtpath, outimgpath, nthreadId, fileid, minPage, txtSize, istoimg, isoriginal);

		COleDateTime bTime = COleDateTime::GetCurrentTime();
		BOOL bConvertTimeOut = FALSE;

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			while (WaitForSingleObject(pi.hThread, 100) != WAIT_OBJECT_0)
			{
				if ((pst->txtstatus != -1 && !istoimg) || (pst->txtstatus != -1 && pst->imgstatus != -1) || (pst->txtstatus != -1 && pst->txtstatus != 0)) break;
				COleDateTime currt = COleDateTime::GetCurrentTime();
				COleDateTimeSpan spant = COleDateTimeSpan(0, 0, timeout, 0);
				if (currt - bTime > spant&&timeout > 0)
				{
					bConvertTimeOut = TRUE;
					break;
				}
			}
			//ת����ʱ
			if (bConvertTimeOut)
			{
				pst->txtstatus = 3;
				pst->imgstatus = 3;
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
			}
			else
			{
				while ((pst->txtstatus == -1 && !istoimg) || (pst->txtstatus == -1 && pst->imgstatus == -1))
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
		pngstatus = pst->imgstatus == -1 ? 3 : pst->imgstatus;
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
				case WM_MSG_EXCEL2TXT_STATUS:
				{
											   pst->txtstatus = msg.wParam;
											   pst->pageNum = msg.lParam;
											   if (pst->txtstatus != 0)
											   {
												   return 0;
											   }
				}break;
				case WM_MSG_EXCEL2IMG_STATUS:
				{
											   pst->imgstatus = msg.wParam;
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