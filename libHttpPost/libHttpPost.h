// libHttpPost.h : libHttpPost DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


// 	int WINAPI lib_HttpPostMsg(const char *url, const char *message, const int &timeout);
	int WINAPI lib_HttpPostMsg(const char *url, int type, int id, const char *node, int txtstatus, int pagenumber, const char *txturl, const char *imgurl, const int &timeout);

#ifdef __cplusplus
}
#endif // __cpluspl	