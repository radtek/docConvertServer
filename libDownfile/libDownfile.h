// libDownfile.h : libDownfile DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#ifdef __cplusplus
extern "C" {
#endif // __cpluspl	


	int WINAPI StartDownfile(const char *url, const char *filepath, const int &timeout);

#ifdef __cplusplus
}
#endif // __cpluspl	