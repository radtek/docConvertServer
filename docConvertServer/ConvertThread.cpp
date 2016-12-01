// ConvertThread.cpp : 实现文件
//

#include "stdafx.h"
#include "docConvertServer.h"
#include "ConvertThread.h"
#include "mystruct.h"
#include "libWordConvert.h"
#include "libExcelConvert.h"
#include "libPptConvert.h"
#include "libPdfConvert.h"
#include "curl/curl.h"
#include "json/json.h"
#include <algorithm> // sort
#include "curlfunc.h"
#include "ConnectDB.h"

// CConvertThread

IMPLEMENT_DYNCREATE(CConvertThread, CWinThread)

CConvertThread::CConvertThread()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bAbort = FALSE;
	m_bAutoDelete = FALSE;
	m_hMainWnd = NULL;
}

CConvertThread::~CConvertThread()
{
	CloseHandle(m_hKillEvent);
}

BOOL CConvertThread::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CConvertThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CConvertThread, CWinThread)
END_MESSAGE_MAP()


// CConvertThread 消息处理程序


int CConvertThread::Run()
{
	char *showmsg = new char[1024];
	sprintf_s(showmsg, 1024, "转换线程 threadid = %d  --- 启动成功", this->m_nThreadID);
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)showmsg);

	int span = 5;
	while (WaitForSingleObject(m_hKillEvent,100) != WAIT_OBJECT_0)
	{
		if (m_bAbort) break;

		if (span-- > 0) continue;
		span = 5;

		g_mtxConvert.Lock();
		if (g_ltConvert.size() > 0)
		{
			p_st_tconvert pConvert = g_ltConvert.front();
			g_ltConvert.pop_front();
			g_mtxConvert.Unlock();


			p_st_tconverted pConverted = new st_tconverted;
			pConverted->fileid = pConvert->fileid;
			pConverted->filetype = pConvert->filetype;
			pConverted->node = new char[strlen(g_strNode) + 1];
			strcpy(pConverted->node, g_strNode);
			pConverted->node[strlen(g_strNode)] = '\0';

			SendMsgShow(pConverted, MSG_LISTCTRL_ADD, WaitConvert);

			p_st_domain2path domain = new st_domain2path;
			char *filepath = NULL;
			if (g_bIsDownFile)
			{
				filepath = new char[1024];
				domain = GetDomain(pConvert->softlink);
				int ret = downloadfile(pConvert->softlink, g_strDownPath, 10, filepath);
			}
			else
			{
				filepath = BuildFilePath(pConvert->softlink, domain);
			}
			if (NULL == domain)
			{
				AfxMessageBox(L"域名配置对应表没有配置此域名，请配置后重启程序！");
			}
			//判断文件是否存在
			if (filepath != NULL && IsFileExist(filepath) && domain)
			{
				//文件存在
				char *outtxtpath = BuildOutPath(domain->txtpath, pConvert->fileid);
				char* outimgpath = BuildOutPath(domain->imgpath, pConvert->fileid);
				pConverted->txturl = BuildOutUrl(domain->txthttp, pConvert->fileid, OUTTXT);
				if (pConvert->filetype == MSPPT)
				{
					pConverted->imgurl = BuildOutUrl(domain->imghttp, pConvert->fileid, OUTJPG);
				}
				else
				{
					pConverted->imgurl = BuildOutUrl(domain->imghttp, pConvert->fileid, OUTPNG);
				}

				SendMsgShow(pConverted, MSG_LISTCTRL_UPDATE, Converting);

				int pagenum = 0;
				int nTotxtStatus = 3;
				int nToimgStatus = 3;
				switch (pConvert->filetype)
				{
				case MSWORD:
// 					nTotxtStatus = StartConvertWordToTxt(filepath, outtxtpath, pConvert->fileid, g_nMinPages, pagenum, 1024, g_nConvertTimeOut);
// 					if (!nTotxtStatus)
// 					{
// 						nToimgStatus = StartConvertWordToImg(filepath, outimgpath, pConvert->fileid, g_nConvertTimeOut);
// 					}
					StartConvertWordToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
					break;
				case MSEXCEL:
// 					nTotxtStatus = StartConvertExcelToTxt(filepath, outtxtpath, pConvert->fileid, g_nMinPages, pagenum, 1024, g_nConvertTimeOut);
// 					if (!nTotxtStatus)
// 					{
// 						nToimgStatus = StartConvertExcelToImg(filepath, outimgpath, pConvert->fileid, g_nConvertTimeOut);
// 					}
					StartConvertExcelToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
					break;
				case MSPPT:
// 					nTotxtStatus = StartConvertPptToTxt(filepath, outtxtpath, pConvert->fileid, g_nMinPages, pagenum, 1024, g_nConvertTimeOut);
// 					if (!nTotxtStatus)
// 					{
// 						nToimgStatus = StartConvertPptToImg(filepath, outimgpath, pConvert->fileid, g_nConvertTimeOut);
// 					}
					StartConvertPptToTxtJpg(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
					break;
				case MSPDF:
					StartConvertPdfToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
					break;
				default:
					break;
				}
				if (g_bIsDownFile&&g_bIsDelSrc)
				{
					remove(filepath);
				}
				delete[] outtxtpath;
				delete[] outimgpath;
				delete[] filepath;

				SendMsgShow(pConverted, MSG_LISTCTRL_UPDATE, (OutStatus)nTotxtStatus);

				pConverted->txtstatus = nTotxtStatus;
				pConverted->imgstatus = nToimgStatus;
				pConverted->pagenumber = pagenum;

				if (!nTotxtStatus)
				{
					g_mtxConvertSuccess.Lock();
					g_ltConvertSuccess.push_back(pConverted);
					g_mtxConvertSuccess.Unlock();
				}
				else
				{
					g_mtxConvertFailed.Lock();
					g_ltConvertFailed.push_back(pConverted);
					g_mtxConvertFailed.Unlock();
				}
			}
			else
			{
				//文件不存在
				pConverted->txtstatus = 1;
				pConverted->pagenumber = 0;
				SendMsgShow(pConverted, MSG_LISTCTRL_UPDATE, FileLoss);
				g_mtxConvertFailed.Lock();
				g_ltConvertFailed.push_back(pConverted);
				g_mtxConvertFailed.Unlock();

			}
			CConnectDB::GetInstance()->delete_convert_table(pConvert->nid);
			delete[] pConvert->softlink;
			delete pConvert;
			delete domain;
		}
		else
		{
			g_mtxConvert.Unlock();
		}
	}
	return 0;
}

void CConvertThread::SendMsgShow(p_st_tconverted sc, UINT msg, OutStatus status)
{
	p_st_msg sm = new st_msg;
	sm->threadid = this->m_nThreadID;
	sm->sc = new st_tconverted;
	sm->sc->fileid = sc->fileid;
	sm->sc->filetype = sc->filetype;
	PostMessage(m_hMainWnd, msg, status, (LPARAM)sm);

}

void CConvertThread::SetMainHwnd(HWND hwnd)
{
	m_hMainWnd = hwnd;
}

p_st_domain2path CConvertThread::GetDomain(const char *softlink)
{
	p_st_domain2path domain = NULL;
	int temppathlen = strlen(softlink) + 1;
	char *temppath = new char[temppathlen];
	memset(temppath, 0, temppathlen);
	char *tempdomain = new char[temppathlen];
	memset(tempdomain, 0, temppathlen);
	ReSolveSoftlink(softlink, tempdomain, temppathlen, temppath, temppathlen);
	vector<p_st_domain2path>::iterator it = g_vtDomainPath.begin();
	while (it != g_vtDomainPath.end())
	{
		if (strcmp(((p_st_domain2path)*it)->domain, tempdomain) == 0)
		{
			domain = new st_domain2path;
			memcpy(domain, (p_st_domain2path)*it, sizeof(st_domain2path));
			break;
		}
		it++;
	}
	delete[] temppath;
	delete[] tempdomain;

	return domain;
}

char* CConvertThread::BuildFilePath(const char *softlink, p_st_domain2path domain)
{
	int temppathlen = strlen(softlink) + 1;
	char *filepath = NULL;
	char *temppath = new char[temppathlen];
	memset(temppath, 0, temppathlen);
	char *tempdomain = new char[temppathlen];
	memset(tempdomain, 0, temppathlen);
	ReSolveSoftlink(softlink, tempdomain, temppathlen, temppath, temppathlen);
	vector<p_st_domain2path>::iterator it = g_vtDomainPath.begin();
	BOOL bHavePath = FALSE;
	while (it != g_vtDomainPath.end())
	{
		if (strcmp(((p_st_domain2path)*it)->domain, tempdomain) == 0)
		{
			memcpy(domain, (p_st_domain2path)*it, sizeof(st_domain2path));
// 			domain = (p_st_domain2path)*it;
			int filepathlen = temppathlen + strlen(((p_st_domain2path)*it)->path);
			filepath = new char[filepathlen];
			strcpy_s(filepath, filepathlen, ((p_st_domain2path)*it)->path);
			bHavePath = TRUE;
			break;
		}
		it++;
	}
	if (!bHavePath)
	{
		//域名没有对应的本地文件夹
		delete[] temppath;
		delete[] tempdomain;
		return NULL;
	}
	strcat(filepath, temppath);
	delete[] temppath;
	delete[] tempdomain;
	return filepath;
}

char* CConvertThread::BuildOutPath(const char* path, const int &fileid)
{
	int len = strlen(path) + 50;
	char* outtxtpath = new char[len];
	memset(outtxtpath, 0, len);
	strcpy_s(outtxtpath, len, path);
	if (outtxtpath[strlen(outtxtpath) - 1] == '\\' || outtxtpath[strlen(outtxtpath) - 1] == '/')
	{
		sprintf_s(outtxtpath, len, "%s%d\\%02d%02d\\book%d", outtxtpath, GetYear(), GetMonth(), GetDay(), fileid / g_nMaxFileInFloder + 1);
	}
	else
	{
		sprintf_s(outtxtpath, len, "%s\\%d\\%02d%02d\\book%d", outtxtpath, GetYear(), GetMonth(), GetDay(), fileid / g_nMaxFileInFloder + 1);
	}
	CreateMultipleDirectory(outtxtpath);
	return outtxtpath;
}

char* CConvertThread::BuildOutUrl(const char* http, const int &fileid, const int &type)
{
	int len = strlen(http) + 50;
	char* outurl = new char[len];
	memset(outurl, 0, len);

	sprintf_s(outurl, len, "http://%s/%d/%02d%02d/book%d/%d.%s", http, GetYear(), GetMonth(), GetDay(), fileid / g_nMaxFileInFloder + 1, fileid, strOutFileType[type]);

	return outurl;
}


int CConvertThread::PostSuccessOrFail(list<p_st_tconverted> lists, const BOOL bSuccess)
{
	list<p_st_tconverted>::iterator it = lists.begin();
	Json::Value root;
	Json::FastWriter writer;
	while (it != lists.end())
	{
		Json::Value person;
		person["id"] = (*it)->fileid;
		person["node"] = (*it)->node;
		person["txtstatus"] = (*it)->txtstatus;
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

void CConvertThread::ResolvePostResult(Json::Value &value, int &status, const std::string &path)
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

int CConvertThread::HttpPostMsg(const char *url, const string & postdata, string & content)
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
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.length());
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

