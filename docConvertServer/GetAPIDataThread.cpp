// GetAPIDataThread.cpp : 实现文件
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "GetAPIDataThread.h"
#include "curl/curl.h"
#include <algorithm> // sort

// CGetAPIDataThread

IMPLEMENT_DYNCREATE(CGetAPIDataThread, CWinThread)

CGetAPIDataThread::CGetAPIDataThread()
{
	m_bAbort = FALSE;
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bAutoDelete = FALSE;
	m_hMainWnd = NULL;
}

CGetAPIDataThread::~CGetAPIDataThread()
{
	CloseHandle(m_hKillEvent);
}

BOOL CGetAPIDataThread::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CGetAPIDataThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGetAPIDataThread, CWinThread)
END_MESSAGE_MAP()


// CGetAPIDataThread 消息处理程序


static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int CGetAPIDataThread::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CGetAPIDataThread::Run()
{
	char *showmsg = new char[1024];
	sprintf_s(showmsg, 1024, "api获取线程 --- 启动成功");
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)showmsg);

	int nspan = 50;
	while (WaitForSingleObject(m_hKillEvent,10)!= WAIT_OBJECT_0)
	{
		if (m_bAbort) break;

		if (nspan-- > 0) continue;
		
		int ncount = CConnectDB::GetInstance()->count_convert_table(0);
		if (ncount > 0)
		{
			nspan = 50;
			continue;
		}

		string response;
		if (Get(g_strApiUrl, response) == CURLE_OK)
		{
			//解析response
			Json::Features features;
			Json::Reader reader(features);
			Json::Value root;
			bool parsingSuccessful = reader.parse(response, root);
			if (parsingSuccessful)
			{
				map<int, p_st_tconvert> hmap;
				printValueTree(hmap, root);
				if (hmap.size() > 0)
				{
					map<int, p_st_tconvert>::iterator it = hmap.begin();
					while (it != hmap.end())
					{
						CConnectDB::GetInstance()->insert_convert_table(it->second);
						delete[] it->second->softlink;
						delete it->second;
						it++;
					}
					hmap.clear();
					nspan = g_nTimeSpanApi / 10;
				}
				else
				{
					nspan = g_nTimeSpanApiNull / 10;
				}
			}
		}
		else
		{
			nspan = 600;
		}
	}

	return 0;
}


void CGetAPIDataThread::printValueTree(map<int, p_st_tconvert> &hmap, Json::Value &value, const std::string &path, const int nid)
{
	switch (value.type())
	{
	case Json::nullValue:
		// 		fprintf(fout, "%s=null\n", path.c_str());
		break;
	case Json::intValue:
		// 		fprintf(fout, "%s=%d\n", path.c_str(), value.asInt());
		break;
	case Json::uintValue:
		// 		fprintf(fout, "%s=%u\n", path.c_str(), value.asUInt());
		break;
	case Json::realValue:
		// 		fprintf(fout, "%s=%.16g\n", path.c_str(), value.asDouble());
		break;
	case Json::stringValue:
	{
							  if (strcmp(path.c_str(), "id") == 0)
							  {
								  map<int, p_st_tconvert>::iterator it = hmap.find(nid);
								  p_st_tconvert sd = NULL;
								  if (it != hmap.end())
								  {
									  sd = (p_st_tconvert)it->second;
								  }
								  else
								  {
									  sd = new st_tconvert;
									  sd->fileid = nid;
									  sd->status = 0;
								  }
								  sd->fileid = atoi(value.asString().c_str());
								  hmap[nid] = sd;
							  }
							  else if (strcmp(path.c_str(), "softlink") == 0)
							  {
								  map<int, p_st_tconvert>::iterator it = hmap.find(nid);
								  p_st_tconvert sd = NULL;
								  if (it != hmap.end())
								  {
									  sd = (p_st_tconvert)it->second;
								  }
								  else
								  {
									  sd = new st_tconvert;
									  sd->fileid = nid;
									  sd->status = 0;
// 									  sd->datetime = new char[64];
// 									  GetNowTime(sd->datetime);
								  }
// 								  sd->softlink = value.asString();
								  sd->softlink = new char[1024];
								  memset(sd->softlink, 0, 1024);
								  strcpy_s(sd->softlink, 1024, value.asString().c_str());
								  hmap[nid] = sd;
							  }
							  else if (strcmp(path.c_str(), "filetype") == 0)
							  {
								  map<int, p_st_tconvert>::iterator it = hmap.find(nid);
								  p_st_tconvert sd = NULL;
								  if (it != hmap.end())
								  {
									  sd = (p_st_tconvert)it->second;
								  }
								  else
								  {
									  sd = new st_tconvert;
									  sd->fileid = nid;
									  sd->status = 0;
// 									  sd->datetime = new char[64];
// 									  GetNowTime(sd->datetime);
								  }

								  if (strcmp(value.asString().c_str(), "pdf") == 0)
								  {
									  sd->filetype = MSPDF;
								  }
								  else if (strcmp(value.asString().c_str(), "ppt") == 0 || strcmp(value.asString().c_str(), "pptx") == 0)
								  {
									  sd->filetype = MSPPT;
								  }
								  else if (strcmp(value.asString().c_str(), "xls") == 0 || strcmp(value.asString().c_str(), "xlsx") == 0)
								  {
									  sd->filetype = MSEXCEL;
								  }
								  else if (strcmp(value.asString().c_str(), "doc") == 0 || strcmp(value.asString().c_str(), "docx") == 0)
								  {
									  sd->filetype = MSWORD;
								  }
								  hmap[nid] = sd;
							  }
							  else if (strcmp(path.c_str(), "is_original") == 0)
							  {
								  map<int, p_st_tconvert>::iterator it = hmap.find(nid);
								  p_st_tconvert sd = NULL;
								  if (it != hmap.end())
								  {
									  sd = (p_st_tconvert)it->second;
								  }
								  else
								  {
									  sd = new st_tconvert;
									  sd->fileid = nid;
									  sd->status = 0;
// 									  sd->datetime = new char[64];
// 									  GetNowTime(sd->datetime);
								  }
								  sd->isoriginal = atoi(value.asString().c_str());
								  hmap[nid] = sd;
							  }
	}
		break;
	case Json::booleanValue:
		// 		fprintf(fout, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false");
		break;
	case Json::arrayValue:
	{
							 int size = value.size();
							 for (int index = 0; index < size; ++index)
							 {
								 printValueTree(hmap, value[index], path, index);
							 }
	}
		break;
	case Json::objectValue:
	{
							  Json::Value::Members members(value.getMemberNames());
							  std::sort(members.begin(), members.end());
							  for (Json::Value::Members::iterator it = members.begin();
								  it != members.end();
								  ++it)
							  {
								  const std::string &name = *it;
								  printValueTree(hmap, value[name], name, nid);
							  }
	}
		break;
	default:
		break;
	}
}

void CGetAPIDataThread::SetMainHwnd(HWND hwnd)
{
	m_hMainWnd = hwnd;
}