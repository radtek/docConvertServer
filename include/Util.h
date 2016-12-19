#ifndef UTIL_H_
#define UTIL_H_

#include <afxwin.h>
#include <io.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "tlhelp32.h"

using namespace std;

static char* CStringToChar(CString str)
{
	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, str, nLength, NULL, 0, NULL, NULL);
	char *p = new char[nBytes + 1];
	memset(p, 0, nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, p, nBytes, NULL, NULL);
	p[nBytes] = 0;
	return p;
}

static char* CStringToUTF8(CString str, int &nlen)
{
	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)str, -1, NULL, 0, NULL, NULL);
	char *p = new char[nBytes + 1];
	// 	memset(p, 0, nBytes * 2 + 2);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)str, -1, p, nBytes, NULL, NULL);
	p[nBytes] = 0;
	nlen = nBytes;
	return p;
}


static char* WCharToChar(wchar_t* wstr,int nlen)
{
	// 	int nLength = wcslen(wstr);
	int nBytes = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *p = new char[nBytes + 1];
	memset(p, 0, nBytes + 1);
	WideCharToMultiByte(CP_OEMCP, 0, wstr, nlen, p, nBytes, NULL, NULL);
	p[nBytes] = 0;
	return p;
}

static CString CharToCString(const char *ch)
{
	int num = MultiByteToWideChar(0, 0, ch, -1, NULL, 0);
	if (num > 20000) return L"";
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(0, 0, ch, -1, wide, num);
	CString str(wide);
	delete[] wide;
	return str;
}

static CString CharToUTF8CString(const char *ch)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, ch, -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, ch, -1, wide, num);
	CString str(wide);
	delete[] wide;
	return str;
}



static void ReadStringCharToUnicode(CString &str)
{
	char *szBuf = new char[str.GetLength() + 1];//注意“+1”，char字符要求结束符，而CString没有
	memset(szBuf, '\0', str.GetLength());

	int i;
	for (i = 0; i < str.GetLength(); i++)
	{
		szBuf[i] = (char)str.GetAt(i);
	}
	szBuf[i] = '\0';//结束符。否则会在末尾产生乱码。

	int nLen;
	WCHAR *ptch;
	CString strOut;
	if (szBuf == NULL)
	{
		return;
	}
	nLen = MultiByteToWideChar(CP_ACP, 0, szBuf, -1, NULL, 0);//获得需要的宽字符字节数
	ptch = new WCHAR[nLen];
	memset(ptch, '\0', nLen);
	MultiByteToWideChar(CP_ACP, 0, szBuf, -1, ptch, nLen);
	str.Format(_T("%s"), ptch);

	if (NULL != ptch)
		delete[] ptch;
	ptch = NULL;

	if (NULL != szBuf)
		delete[]szBuf;
	szBuf = NULL;
	return;
}


static void GetNowTime(char *c_time)
{
	time_t t = time(0); 
// 	tm _tm;
// 	localtime_s(&_tm,&t);
	char tmpbuf[64];
	strftime( tmpbuf, 64, "%Y-%m-%d %H:%M:%S",localtime(&t) );
	strcpy(c_time,tmpbuf);
}

static long GetNowTimeLong()
{
	time_t t = time(0);
	return (long)t;
}

static void GetNowTimeSpan(char *c_time,int spant)
{
	time_t t = time(0); 
	long seconds = spant*3600;
	t-=seconds;
	char tmpbuf[64];
	tm *_tm = localtime(&t);
	strftime( tmpbuf, 64, "%Y-%m-%d %H:%M:%S",_tm );
	strcpy(c_time,tmpbuf);
}

static int GetYear()
{
	time_t t = time(0); 
	char tmpbuf[64];
	strftime( tmpbuf, 64, "%Y",localtime(&t) );
	int nyear = atoi(tmpbuf);
	return nyear;
}

static int GetMonth()
{
	time_t t = time(0); 
	char tmpbuf[64];
	strftime( tmpbuf, 64, "%m",localtime(&t) );
	int nmonth = atoi(tmpbuf);
	return nmonth;
}

static int GetDay()
{
	time_t t = time(0); 
	char tmpbuf[64];
	strftime( tmpbuf, 64, "%d",localtime(&t) );
	int nday = atoi(tmpbuf);
	return nday;
}

static int GetHour()
{
	time_t t = time(0); 
	char tmpbuf[64];
	strftime( tmpbuf, 64, "%H",localtime(&t) );
	int nday = atoi(tmpbuf);
	return nday;
}

static BOOL FirstInstance(CString title)
{
	CWnd*pwndFirst = CWnd::FindWindow(NULL, title);
	if (pwndFirst)
	{
		// another instance is already running - activate it
		CWnd*pwndPopup = pwndFirst->GetLastActivePopup();
		pwndFirst->SetForegroundWindow();
		if (pwndFirst->IsIconic())
			pwndFirst->ShowWindow(SW_SHOWNORMAL);
		if (pwndFirst != pwndPopup)
			pwndPopup->SetForegroundWindow();
		return FALSE;
	}
	return TRUE;
}

static int killpro(CString a)//CString a里面就是进程名字
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hpro = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hpro == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	BOOL nowrun = Process32First(hpro, &pe32);
	while (nowrun)
	{
		if (pe32.szExeFile == a)
		{
			DWORD proid = pe32.th32ProcessID;
			HANDLE hprocess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, proid);
			if (hprocess != NULL)
			{
				::TerminateProcess(hprocess, 0);
				::CloseHandle(hprocess);
// 				break;
			}
		}
		nowrun = ::Process32Next(hpro, &pe32);
	}
	return 0;
}

static int findnum(const char *str){
	int num=0;
	for(int i=0;str[i]!='\0';i++)
	{
		if(str[i]>='0'&&str[i]<='9')
			num=(num*10)+str[i]-'0';
	}
	return num;
}

static BOOL DeleteDirectory(CString strDirName)
{
	if (strDirName.IsEmpty() || strDirName == "" || strDirName.GetLength() < 5)
	{
		return FALSE;
	}
    CFileFind tempFind;
	CString strTempFileFind = strDirName;
	strTempFileFind.Append(L"\\*.*");
    BOOL IsFinded = tempFind.FindFile(strTempFileFind);
    while (IsFinded)
    {
        IsFinded = tempFind.FindNextFile();
        if (!tempFind.IsDots())
        {
			CString strFoundFileName = tempFind.GetFileName();
            if (tempFind.IsDirectory())
            {
				CString strTempDir = strDirName + L"\\" + strFoundFileName;
                DeleteDirectory(strTempDir);
            }
            else
            {
				CString strTempFileName = strDirName + L"\\" + strFoundFileName;
                DeleteFile(strTempFileName);
            }
        }
    }
    tempFind.Close();
    if(!RemoveDirectory(strDirName))
    {
        return FALSE;
    }
    return TRUE;
}

static BOOL FolderExist(CString& strPath)
{
	CString sCheckPath = strPath;

	if (sCheckPath.Right(1) != L"\\")
		sCheckPath += L"\\";

	sCheckPath += L"*.*";

	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;

	HANDLE hFind = FindFirstFile(sCheckPath, &wfd);

	if ((hFind != INVALID_HANDLE_VALUE) &&
		(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || (wfd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE))
	{
		//如果存在并类型是文件夹 
		rValue = TRUE;
	}

	FindClose(hFind);
	return rValue;
}


static bool CreateMultipleDirectory(CString strDir)
{
	strDir.Replace('/', '\\');
	//确保以'\'结尾以创建最后一个目录
	if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
	{
		strDir.AppendChar(_T('\\'));
	}
	std::vector<CString> vPath;//存放每一层目录字符串
	CString strTemp;//一个临时变量,存放目录字符串
	bool bSuccess = false;//成功标志
	//遍历要创建的字符串
	for (int i = 0; i < strDir.GetLength(); ++i)
	{
		if (strDir.GetAt(i) != _T('\\'))
		{//如果当前字符不是'\\'
			strTemp.AppendChar(strDir.GetAt(i));
		}
		else
		{//如果当前字符是'\\'
			vPath.push_back(strTemp);//将当前层的字符串添加到数组中
			strTemp.AppendChar(_T('\\'));
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<CString>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
		bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
	}

	return bSuccess;
}


static bool CreateMultipleDirectory(const char* szPath)
{
// 	CString strDir=CharToCString(szPath);//存放要创建的目录字符串
// 	//确保以'\'结尾以创建最后一个目录
// 	if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\') && strDir.GetAt(strDir.GetLength() - 1) != _T('/'))
// 	{
// 		strDir.AppendChar(_T('\\'));
// 	}
	string cpath(szPath);
	int len = cpath.length();
	if (cpath[len-1] != '\\' &&cpath[len-1] != '/')
	{
		cpath = cpath + "\\";
	}
	vector<string> vpath; 
	string strtemp;
// 	std::vector<CString> vPath;//存放每一层目录字符串
// 	CString strTemp;//一个临时变量,存放目录字符串
	bool bSuccess = false;//成功标志
	//遍历要创建的字符串
	for (unsigned int i=0;i<cpath.length();++i)
	{
		if (cpath[i] != '\\' && cpath[i] != '/')
		{//如果当前字符不是'\\'
			//strtemp = cpath.substr(0, i);
		}
		else 
		{//如果当前字符是'\\'
			strtemp = cpath.substr(0, i);
			vpath.push_back(strtemp);//将当前层的字符串添加到数组中
// 			strTemp.AppendChar(_T('\\'));
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<string>::const_iterator vIter;
	for (vIter = vpath.begin(); vIter != vpath.end(); vIter++)
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
		bSuccess = CreateDirectoryA((*vIter).c_str(), NULL) ? true : false;  
	}

	return bSuccess;
}


static char *get_file_type(const char *strbuf, char *filetype)
{
	if (!strbuf || strlen(strbuf) == 0){
		return NULL;
	}
	int len = 0;
	int haspath = 0;
	len = strlen(strbuf) - 1;
	for (; len > 0; len--){
		if (strbuf[len] == '.'){
			//			filename = (char*)(&strbuf[len + 1]);
			strcpy(filetype, (char*)(&strbuf[len + 1]));
			haspath = 1;
			break;
		}
	}

	return filetype;
}

static void GetFiles(string path, string type, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	string searchtype = "\\*.*";
	if ((hFile = _findfirst(p.assign(path).append(searchtype.c_str()).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFiles(p.assign(path).append("\\").append(fileinfo.name), type, files);
			}
			else
			{
				//fileinfo.size;
				char filetype[10] = { 0 };
				get_file_type(fileinfo.name, filetype);
				if (strcmp(filetype, type.c_str()) == 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

//文件夹复制
static void FolderCopy(const char* src,const char* dst)
{
	CreateDirectoryA(dst,NULL);
	char srcPath[250];
	char dstPath[250];
	HANDLE hFind;
	WIN32_FIND_DATAA FindFileData;
	//    char line[1024];
	char path[260];
	sprintf(path,"%s\\*",src);
	hFind = FindFirstFileA(path, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		return;
	do{
// 		printf("%s\n",FindFileData.cFileName);
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("%s\n",FindFileData.cFileName);
			if(FindFileData.cFileName[0] != '.')
			{
				sprintf(srcPath,"%s\\%s",src,FindFileData.cFileName);
				sprintf(dstPath,"%s\\%s",dst,FindFileData.cFileName);
				CreateDirectoryA(dstPath,NULL);
				FolderCopy(srcPath,dstPath);
			}
		}else {
			sprintf(srcPath,"%s\\%s",src,FindFileData.cFileName);
			sprintf(dstPath,"%s\\%s",dst,FindFileData.cFileName);
			CopyFileA(srcPath,dstPath,FALSE);
		}
	}while(FindNextFileA(hFind, &FindFileData));
	::FindClose(hFind);
}

//string 字符替换
static string&   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
{
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length())   {
		if ((pos = str.find(old_value, pos)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}

static void GetFileName(const char *filepath, char *filename)
{
	CString strfilename;
	CString strFile = CharToCString(filepath);
	int k = strFile.ReverseFind('.');
	strfilename = strFile.Left(k);
	k = strFile.ReverseFind('\\');
	strfilename = strfilename.Mid(k+1);
	char *c_filename = CStringToChar(strfilename);
	strcpy(filename,c_filename);
	delete[] c_filename;
	c_filename = NULL;
}

static char *get_file_name(const char *strbuf, char *filename)
{
	if (!strbuf || strlen(strbuf) == 0){
		return NULL;
	}
	int len = 0;
	int haspath = 0;
	len = strlen(strbuf) - 1;
	for (; len>0; len--){
		if (strbuf[len] == '/' || strbuf[len] == '\\'){
			//			filename = (char*)(&strbuf[len + 1]);
			strcpy(filename, (char*)(&strbuf[len + 1]));
			haspath = 1;
			break;
		}
	}
	if (!haspath)
	{
		strcpy(filename, strbuf);
	}

	return filename;
}

static void getFileNameFormUrl(char* fileName, const char* url)
{
	string strurl(url);
	if (strurl.find("://") < 0) return;
	int i = strurl.find_last_of("/");
	if (i >= 0)
	{
		strurl = strurl.substr(i + 1);
		strcpy(fileName, strurl.c_str());
	}
	return;
}

static void ParseOutFilePath(const char *filepath, const char *outpath, char *outfilepath, char *filename)
{
	CString strfilename;
	CString strFile = CharToCString(filepath);
	int k = strFile.ReverseFind('.');
	strfilename = strFile.Left(k);
	strfilename.Replace('/','\\');
	k = strfilename.ReverseFind('\\');
	strfilename = strfilename.Mid(k+1);
	char *c_filename = CStringToChar(strfilename);
	if (outfilepath != NULL)
	{
		strcpy(outfilepath, outpath);
		int len = strlen(outpath);
		if (outpath[len - 1] == '\\' || outpath[len - 1] == '/')
		{
			strcat(outfilepath, c_filename);
		}
		else
		{
			strcat(outfilepath, "\\");
			strcat(outfilepath, c_filename);
		}
		strcat(outfilepath, "\\");
		CreateMultipleDirectory(outfilepath);
	}
	strcpy(filename,c_filename);
	delete[] c_filename;
	c_filename = NULL;
}

static int IsFileExist(const char * file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(CharToCString(file), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf ("Invalid File Handle. Get Last Error reports %d ", GetLastError ());
		return 0;
	} else {
// 		printf ("The first file found is %s ", FindFileData.cFileName);
		FindClose(hFind);
		return 1;
	}
}

static int CheckCodeIsUnrecognizable(char *data)
{
	int i = 0;
	int count = 0;
	while (data[i])
	{
		if (count >= 5) return 1;
		unsigned char ch = data[i]; //注意要定义为无符号
		//先检查ch是否是字母：
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			count = 0;
			i++;
			continue;
		}
		else if (ch == '?')
		{
			count++;
			continue;
		}
		else 
		{
			count = 0;
			//再检查ch是否是数字：
			if (ch >= '0' && ch <= '9')
			{
				i++;
				continue;
			}
			else
			{
				//最后检查是否是汉字：（按GB 2312 汉字区检查，不考虑特殊汉字 B0A1-F7FE）
				if (ch >= 0xB0 && ch <= 0xF7)
				{
					//继续检查下一个字符是否能组织成汉字
					unsigned char ch1 = data[i + 1]; //注意要定义为无符号
					if (ch1 >= 0xA1 && ch1 <= 0xFE)
					{
						i += 2;
						return 0;
					}
				}
			}
		}
		i++;
		continue;
	}
	return 0;
}

static void WriteLog(char *path, char *msg)
{
#if USE_CFILE
	static CFile file;
#else
	static FILE *fp = NULL;
#endif
	static int nday = 0, nfirst = 1, nNum = 0;
	if (nday != GetDay() || nfirst)
	{
		try
		{
			nNum = 0;
#if USE_CFILE
			if (!nfirst) file.Close();
#else
			if (!nfirst && fp != NULL) fclose(fp);
#endif
			nfirst = 0;
			nday = GetDay();
			char logname[1024] = { 0 };
			if (path[strlen(path) - 1] != '\\' && path[strlen(path) - 1] != '/')
			{
				strcat(path, "\\");
			}
			CreateMultipleDirectory(path);
			sprintf_s(logname, "%slogdocConvertServer%d%02d%02d.log", path, GetYear(), GetMonth(), nday);
#if USE_CFILE
			BOOL bok = file.Open(CharToCString(logname), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareDenyNone);
			if (!bok)
			{
				nfirst = 1;
			}
			else
			{
				file.Seek(0, CFile::end);
			}
#else
			fp = fopen(logname, "a+");
			if (NULL == fp)
			{
				nfirst = 1;
			}
#endif
		}
		catch (CException* e)
		{
			nfirst = 1;
		}
	}
	if (!nfirst)
	{
		try
		{
			nNum++;
			char now[64] = { 0 };
			GetNowTime(now);
			char cnum[10] = {};
			sprintf_s(cnum, 10, "%d", nNum);
			int msglen = strlen(msg) + strlen(now) + 9 + strlen(cnum);
			char *wmsg = new char[msglen + 1];
			sprintf_s(wmsg, msglen + 1, "%s --%s--  %s\r\n", cnum, now, msg);
			wmsg[msglen] = '\0';
#if USE_CFILE
			file.Write(wmsg, strlen(wmsg));
#else
			if (fwrite(wmsg, strlen(wmsg), 1, fp) != 1)
			{
				TRACE("write msg error\n");
			}
			delete[] wmsg;
#endif
		}
		catch (CException* e)
		{
			nfirst = 1;
		}
	}
}

static void ExecuteProcess(const char *exefile)
{
	char paramstr[1024] = { 0 };
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;

	sprintf_s(paramstr, 1024, "%s", exefile);

	BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

}

#endif