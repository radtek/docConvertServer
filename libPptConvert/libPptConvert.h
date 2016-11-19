// libPptConvert.h : libPptConvert DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


	int WINAPI StartConvertPptToTxt(const char *file, const char *outpath, const int &fileid, const int &minPage, int &pageNum, const int &txtSize, const int &timeout);

	int WINAPI StartConvertPptToImg(const char *file, const char *outpath, const int &fileid, const int &timeout);

	void WINAPI StartConvertPptToTxtJpg(const char *file, const char *outtxtpath, const char *outimgpath, const int &fileid, const int &isoriginal, const int &minPage, int &pageNum, const int &txtSize, const int &timeout, const int &istoimg, int &txtstatus, int &pngstatus);

#ifdef __cplusplus
}
#endif // __cpluspl	