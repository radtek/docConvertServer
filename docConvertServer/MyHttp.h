#pragma once
class CMyHttp
{
public:
	CMyHttp();
	~CMyHttp();

	static CString getFileNameFormUrl(const CString &strurl);
	static BOOL Download(const  CString &  strFileURLInServer,  // 待下载文件的URL 
		const  CString  &  strFileLocalFullPath); // 存放到本地的路径 

};

