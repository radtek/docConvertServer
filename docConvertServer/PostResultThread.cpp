// PostResultThread.cpp : 实现文件
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "PostResultThread.h"
#include <algorithm> // sort


// CPostResultThread

IMPLEMENT_DYNCREATE(CPostResultThread, CWinThread)

CPostResultThread::CPostResultThread()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bAbort = FALSE;
	m_bAutoDelete = FALSE;
	m_hMainWnd = NULL;
}

CPostResultThread::~CPostResultThread()
{
	CloseHandle(m_hKillEvent);
}

BOOL CPostResultThread::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CPostResultThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPostResultThread, CWinThread)
END_MESSAGE_MAP()


// CPostResultThread 消息处理程序


int CPostResultThread::Run()
{
	char *showmsg = new char[1024];
	sprintf_s(showmsg, 1024, "上传成功失败信息线程 --- 启动成功");
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)showmsg);

	COleDateTime lastSuccessPostTime = COleDateTime::GetCurrentTime();
	COleDateTime lastFailedPostTime = COleDateTime::GetCurrentTime();
	COleDateTime nowtime;

	int times = 10;
	while (WaitForSingleObject(m_hKillEvent, 100) != WAIT_OBJECT_0)
	{
		if (m_bAbort) break;
		if (times-- > 0) continue;
		times = 10;

// 		string content;
// 		HttpPostMsg(g_strSuccessUrl, "con=12", content);
// 		continue;

#if _DEBUG
// 		continue;
#endif
		nowtime = COleDateTime::GetCurrentTime();
		g_mtxConvertSuccess.Lock();
		if (g_ltConvertSuccess.size() >= 20 || ((nowtime - lastSuccessPostTime) > COleDateTimeSpan(0, 0, 0, 10) && g_ltConvertSuccess.size() > 0))
		{
			string postmsg = BuildPostMsg(g_ltConvertSuccess, TRUE);
			g_ltConvertSuccess.clear();
			g_mtxConvertSuccess.Unlock();
			int ret = PostSuccessOrFail(postmsg, TRUE);
			if (ret)
			{
				char *errmsg = new char[200];
				sprintf_s(errmsg, 200, "上传转换成功信息失败---返回码 %d", ret);
				PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)errmsg);
			}
			lastSuccessPostTime = COleDateTime::GetCurrentTime();
		}
		else
		{
			g_mtxConvertSuccess.Unlock();
		}
		nowtime = COleDateTime::GetCurrentTime();
		g_mtxConvertFailed.Lock();
		if (g_ltConvertFailed.size() >= 20 || ((nowtime - lastFailedPostTime) > COleDateTimeSpan(0, 0, 0, 10) && g_ltConvertFailed.size() > 0))
		{
			string postmsg = BuildPostMsg(g_ltConvertFailed, FALSE);
			g_ltConvertFailed.clear();
			g_mtxConvertFailed.Unlock();
			int ret = PostSuccessOrFail(postmsg, FALSE);
			if (ret)
			{
				char *errmsg = new char[200];
				sprintf_s(errmsg, 200, "上传转换失败信息失败---返回码 %d", ret);
				PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)errmsg);
			}

			lastFailedPostTime = COleDateTime::GetCurrentTime();
		}
		else
		{
			g_mtxConvertFailed.Unlock();
		}
	}

	return 0;
}

string CPostResultThread::BuildPostMsg(list<p_st_tconverted> &lists, const BOOL bSuccess)
{
	list<p_st_tconverted>::iterator it = lists.begin();
	Json::Value root;
	Json::FastWriter writer;
	if (it == lists.end())
	{
		return "";
	}
	while (it != lists.end())
	{
		Json::Value person;
		person["id"] = (*it)->fileid;
		if ((*it)->node != NULL)
		{
			person["node"] = (*it)->node;
		}
		else
		{
			person["node"] = "";
		}
		person["txtstatus"] = (*it)->txtstatus + 1;
		if (bSuccess)
		{
			if ((*it)->imgurl != NULL)
			{
				person["imgurl"] = (*it)->imgurl;
			}
			else
			{
				person["imgurl"] = "";
			}
		}
		person["pagenumber"] = (*it)->pagenumber;
		if ((*it)->txturl != NULL)
		{
			person["txturl"] = (*it)->txturl;
		}
		else
			person["txturl"] = "";
		{
		}
		root.append(person);

		if ((*it)->node != NULL)
		{
			delete[](*it)->node;
		}
		if ((*it)->imgurl != NULL)
		{
			delete[](*it)->imgurl;
		}
		if ((*it)->txturl != NULL)
		{
			delete[](*it)->txturl;
		}
		delete *it;

		it++;
	}
	string data = "data=" + writer.write(root);
	return data;
}

int CPostResultThread::PostSuccessOrFail(string msg, const BOOL bSuccess)
{
	if (msg.empty() || msg == "") return 0;
	string content;
	int err = 1;
	if (bSuccess)
	{
		err = HttpPostMsg(g_strSuccessUrl, msg, content);
	}
	else
	{
		err = HttpPostMsg(g_strFailedUrl, msg, content);
	}
	if (!err)
	{
		Json::Features features;
		Json::Reader reader(features);
		Json::Value root;
		bool parsingSuccessful = reader.parse(content, root);
		if (parsingSuccessful)
		{
			//解析返回的json
			int status = -1;
			try{
				ResolvePostResult(root, status);
			}
			catch (...)
			{
				return status;
			}
			if (status == 3)
			{
				return 0;
			}
			return status;
		}
	}
	return -1;
}

int CPostResultThread::PostSuccessOrFail(list<p_st_tconverted> lists, const BOOL bSuccess)
{
	list<p_st_tconverted>::iterator it = lists.begin();
	Json::Value root;
	Json::FastWriter writer;
	while (it != lists.end())
	{
		Json::Value person;
		person["id"] = (*it)->fileid;
		person["node"] = (*it)->node;
		person["txtstatus"] = (*it)->txtstatus + 1;
		if (bSuccess)
		{
			person["imgurl"] = (*it)->imgurl;
		}
		person["pagenumber"] = (*it)->pagenumber;
		person["txturl"] = (*it)->txturl;
		root.append(person);

		it++;
	}
	string data = writer.write(root);

	string content;
	int err = 1;
	if (bSuccess)
	{
		err = HttpPostMsg(g_strSuccessUrl, data, content);
	}
	else
	{
		err = HttpPostMsg(g_strFailedUrl, data, content);
	}
	if (!err)
	{
		Json::Features features;
		Json::Reader reader(features);
		Json::Value root;
		bool parsingSuccessful = reader.parse(content, root);
		if (parsingSuccessful)
		{
			//解析返回的json
			int status = -1;
			try{
				ResolvePostResult(root, status);
			}
			catch (...)
			{
				return status;
			}
			if (status == 3)
			{
				return 0;
			}
			return status;
		}
	}
	return -1;
}

void CPostResultThread::ResolvePostResult(Json::Value &value, int &status, const std::string &path)
{
	switch (value.type())
	{
	case Json::nullValue:
		break;
	case Json::intValue:
	{
						   if (strcmp(path.c_str(), "status") == 0)
						   {
							   status = value.asInt();
						   }
	}
		break;
	case Json::uintValue:
		break;
	case Json::realValue:
		break;
	case Json::stringValue:
	{
							  if (strcmp(path.c_str(), "status") == 0)
							  {
								  status = atoi(value.asString().c_str());
							  }
	}
		break;
	case Json::booleanValue:
		break;
	case Json::arrayValue:
	{
							 int size = value.size();
							 for (int index = 0; index < size; ++index)
							 {
								 ResolvePostResult(value[index], status, path);
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
								  ResolvePostResult(value[name], status, name);
							  }
	}
		break;
	default:
		break;
	}
}

static size_t process_data(void *data, size_t size, size_t nmemb, string &content)
{
	long sizes = size * nmemb;
	string temp;
	temp = string((char*)data, sizes);
	content += temp;
	return sizes;
}

int CPostResultThread::HttpPostMsg(const char *url, const string & postdata, string & content)
{
	try
	{
		CURL *curl = NULL;
		CURLcode res;
		curl = curl_easy_init();
		if (curl == NULL)
		{
			fprintf(stderr, "curl_easy_init() error.\n");
			return -1;
		}

		// 	char data[2000] = { 0 };
		// 	sprintf_s(data, "onlineUrl=%s&convertType=2&id=%d&isDelSrc=1&isGetPageNum=0&Pages=1-4", downurl, id);

		// 	g_mtx_post.Lock();
		content.clear();
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, process_data);

		curl_easy_setopt(curl, CURLOPT_URL, url);
// 		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		// 设置http发送的内容类型为JSON  
		curl_slist *headers = NULL;// = curl_slist_append(NULL, "Content-Type:application/json");

// 		headers = curl_slist_append(headers, "Accept: application/json");
// 		headers = curl_slist_append(headers, "Content-Type: application/json");
// 		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
// 		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.length());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600);
		// 		SetShareHandle(curl);
		// 	g_mtx_post.Unlock();

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			curl_easy_cleanup(curl);
			return res;
		}

		long retcode = 0;
		CURLcode code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if ((code == CURLE_OK) && retcode == 200){
			// 		printf("result: %s \n", content.c_str());
		}

		curl_easy_cleanup(curl);
		return 0;
	}
	catch (...)
	{
		return -2;
	}
}



void CPostResultThread::SetMainHwnd(HWND hwnd)
{
	m_hMainWnd = hwnd;
}
