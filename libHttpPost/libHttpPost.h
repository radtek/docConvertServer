// libHttpPost.h : libHttpPost DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


// 	int WINAPI lib_HttpPostMsg(const char *url, const char *message, const int &timeout);
	int WINAPI lib_HttpPostMsg(const char *url, int type, int id, const char *node, int txtstatus, int pagenumber, const char *txturl, const char *imgurl, const int &timeout);

#ifdef __cplusplus
}
#endif // __cpluspl	