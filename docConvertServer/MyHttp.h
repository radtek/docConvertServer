#pragma once
class CMyHttp
{
public:
	CMyHttp();
	~CMyHttp();

	static CString getFileNameFormUrl(const CString &strurl);
	static BOOL Download(const  CString &  strFileURLInServer,  // �������ļ���URL 
		const  CString  &  strFileLocalFullPath); // ��ŵ����ص�·�� 

};

