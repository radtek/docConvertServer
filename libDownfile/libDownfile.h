// libDownfile.h : libDownfile DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


	int WINAPI StartDownfile(const char *url, const char *filepath, const int &timeout);

#ifdef __cplusplus
}
#endif // __cpluspl	