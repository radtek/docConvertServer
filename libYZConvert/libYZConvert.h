// libYZConvert.h : main header file for the libYZConvert DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols



#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


	int WINAPI StartYZConvertToTxt(const char *file, const char *outpath, int filetype, const int &fileid, const int &minPage, int &pageNum);

#ifdef __cplusplus
}
#endif // __cpluspl	