// ConvertThread.cpp : ʵ���ļ�
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
#include "libDownfile.h"
#include "libHttpPost.h"

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
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CConvertThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CConvertThread, CWinThread)
END_MESSAGE_MAP()


// CConvertThread ��Ϣ�������


int CConvertThread::Run()
{
	char *showmsg = new char[1024];
	sprintf_s(showmsg, 1024, "ת���߳� threadid = %d  --- �����ɹ�", this->m_nThreadID);
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 0, (LPARAM)showmsg);

	int span = 5;
	while (WaitForSingleObject(m_hKillEvent,10) != WAIT_OBJECT_0)
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

			if (NULL == pConvert)
			{
				continue;
			}

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
#if 0
				int ret = downloadfile(pConvert->softlink, g_strDownPath, 10, filepath);
#else
				char filename[100] = { 0 };
				strcpy(filepath, g_strDownPath);
				getFileNameFormUrl(filename, pConvert->softlink);
				strcat(filepath, filename);
				int ret = 1;
				while (ret && pConvert->trytimes++ < 3)//����ʧ�ܻ��������أ����3��
				{
					ret = StartDownfile(pConvert->softlink, filepath, 3);
					Sleep(10);
				}
#endif
			}
			else
			{
				filepath = BuildFilePath(pConvert->softlink, domain);
			}
			if (NULL == domain)
			{
				AfxMessageBox(L"�������ö�Ӧ��û�����ô������������ú���������");
			}
			//�ж��ļ��Ƿ����
			if (filepath != NULL && IsFileExist(filepath) && domain)
			{
				//�ļ�����
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
				while (nTotxtStatus && pConvert->trytimes++ < 3)//ת��ʧ�ܻ�����ת�������3��
				{
					switch (pConvert->filetype)
					{
					case MSWORD:
						StartConvertWordToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
						break;
					case MSEXCEL:
						StartConvertExcelToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
						break;
					case MSPPT:
						StartConvertPptToTxtJpg(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
						break;
					case MSPDF:
						StartConvertPdfToTxtPng(filepath, outtxtpath, outimgpath, pConvert->fileid, pConvert->isoriginal, g_nMinPages, pagenum, 1024, g_nConvertTimeOut, g_bIsToImg, nTotxtStatus, nToimgStatus);
						break;
					default:
						break;
					}
					Sleep(10);
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
					CConnectDB::GetInstance()->delete_convert_table(pConvert->nid);
#ifdef SYNC_POST_RESULT
					PostSuccessOrFail(pConverted, TRUE);
#else
					g_mtxConvertSuccess.Lock();
					g_ltConvertSuccess.push_back(pConverted);
					g_mtxConvertSuccess.Unlock();
#endif
				}
				else
				{

					CConnectDB::GetInstance()->delete_convert_table(pConvert->nid);
#ifdef SYNC_POST_RESULT
					PostSuccessOrFail(pConverted, FALSE);
#else
					g_mtxConvertFailed.Lock();
					g_ltConvertFailed.push_back(pConverted);
					g_mtxConvertFailed.Unlock();
#endif
				}
			}
			else				//�ļ�������
			{
				pConverted->txtstatus = 1;
				pConverted->pagenumber = 0;
				SendMsgShow(pConverted, MSG_LISTCTRL_UPDATE, FileLoss);
				CConnectDB::GetInstance()->delete_convert_table(pConvert->nid);
#ifdef SYNC_POST_RESULT
				PostSuccessOrFail(pConverted, FALSE);
#else
				g_mtxConvertFailed.Lock();
				g_ltConvertFailed.push_back(pConverted);
				g_mtxConvertFailed.Unlock();
#endif
			}
			delete pConvert;
			pConvert = NULL;
			delete domain;
			domain = NULL;
		}
		else
		{
			g_mtxConvert.Unlock();
		}
	}
	return 0;
}


void CConvertThread::ShowSuccessMsg(const int &fileid, BOOL bSuccess)
{
	int errlen = 100;
	char *errlog = new char[errlen];
	memset(errlog, 0, errlen);
	if (!bSuccess)
	{
		sprintf_s(errlog, errlen, "�ϴ�ת��ʧ����Ϣ�ɹ�--�ļ�ID:%d", fileid);
	}
	else
	{
		sprintf_s(errlog, errlen, "�ϴ�ת���ɹ���Ϣ�ɹ�--�ļ�ID:%d", fileid);
	}
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 1, (LPARAM)errlog);
}

void CConvertThread::ShowErrorMsg(const int &ret, const string &msg, const int &fileid, BOOL bSuccess)
{
	int errlen = msg.length() + 200;
	char *errlog = new char[errlen];
	memset(errlog, 0, errlen);
	if (!bSuccess)
	{
		sprintf_s(errlog, errlen, "�ϴ�ת��ʧ����Ϣʧ��--������:%d,�ļ�ID(%d)", ret, fileid);
	}
	else
	{
		sprintf_s(errlog, errlen, "�ϴ�ת���ɹ���Ϣʧ��--������:%d,�ļ�ID(%d)", ret, fileid);
	}
	PostMessageA(m_hMainWnd, WM_MSGLIST_SHOW, 1, (LPARAM)errlog);
}

int CConvertThread::PostSuccessOrFail(p_st_tconverted converted, BOOL bSuccess)
{
	int fileid = converted->fileid;
// 	string msg = BuildPostMsg(converted, bSuccess);

// 	int ret = PostSuccessOrFail(msg, bSuccess);

	int err = 1;
	int ntimes = 0;

	while (err != 3 && ntimes++ < 3)
	{
		std::string txturl = "null";
		std::string imgurl = "null";
		if (converted->txturl != NULL && converted->txturl != "")
		{
			txturl = converted->txturl;
		}
		if (converted->imgurl != NULL && converted->imgurl != "")
		{
			imgurl = converted->imgurl;
		}
		if (bSuccess)
		{
			err = lib_HttpPostMsg(g_strSuccessUrl, bSuccess, converted->fileid, converted->node, converted->txtstatus, converted->pagenumber, txturl.c_str(), imgurl.c_str(), 1);
		}
		else
		{
			err = lib_HttpPostMsg(g_strFailedUrl, bSuccess, converted->fileid, converted->node, converted->txtstatus, converted->pagenumber, txturl.c_str(), imgurl.c_str(), 1);
		}
	}



	if (err != 3)
	{
		ShowErrorMsg(err, "", fileid, bSuccess);
	}
	else
	{
		ShowSuccessMsg(fileid, bSuccess);
	}
	return err;
}
int CConvertThread::PostSuccessOrFail(const string &msg, BOOL bSuccess)
{
	if (msg.empty() || msg == "") return 0;
	string content;
	int err = 1;
	int ntimes = 0;
	if (bSuccess)
	{
		while (err!=3&&ntimes++<3)
		{
// 			err = lib_HttpPostMsg(g_strSuccessUrl, msg.c_str(), 1);
		}
	}
	else
	{
		while (err != 3 && ntimes++ < 3)
		{
//			err = lib_HttpPostMsg(g_strFailedUrl, msg.c_str(), 1);
		}
	}
	return err;
}


string CConvertThread::BuildPostMsg(p_st_tconverted converted, BOOL bSuccess)
{
	Json::Value root;
	Json::FastWriter writer;
	if (NULL == converted)
	{
		return "";
	}

	Json::Value person;
	person["id"] = converted->fileid;
	if (converted->node != NULL)
	{
		person["node"] = converted->node;
	}
	else
	{
		person["node"] = "";
	}
	person["txtstatus"] = converted->txtstatus + 1;
	if (bSuccess)
	{
		if (converted->imgurl != NULL)
		{
			person["imgurl"] = converted->imgurl;
		}
		else
		{
			person["imgurl"] = "";
		}
	}
	person["pagenumber"] = converted->pagenumber;
	if (converted->txturl != NULL)
	{
		person["txturl"] = converted->txturl;
	}
	else
		person["txturl"] = "";
	{
	}
	root.append(person);
	delete converted;

	string data = "data=" + writer.write(root);
	return data;
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
		//����û�ж�Ӧ�ı����ļ���
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
			//�������ص�json
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

