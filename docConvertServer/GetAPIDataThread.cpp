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
	try
	{
		res = curl_easy_perform(curl);
	}
	catch (...)
	{
		return res;
		return -1;
	}
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
#if 0
// 			response = "{\"status\":\"003\",\"message\":\"\u6570\u636e\u8fd4\u56de\u6210\u529f\",\"result\":[{\"id\":\"62222375\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b2a20541f.pptx\",\"filetype\":\"pptx\",\"filesize\":\"3383643\",\"is_original\":\"0\"},{\"id\":\"62222394\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3135436\/3135436-5823b2a872eb0.doc\",\"filetype\":\"doc\",\"filesize\":\"162309\",\"is_original\":\"0\"},{\"id\":\"62222450\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b2c182d94.pptx\",\"filetype\":\"pptx\",\"filesize\":\"9363379\",\"is_original\":\"0\"},{\"id\":\"62222632\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b30f4cbfb.pptx\",\"filetype\":\"pptx\",\"filesize\":\"8839542\",\"is_original\":\"0\"},{\"id\":\"62222766\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b34abba48.docx\",\"filetype\":\"docx\",\"filesize\":\"411168\",\"is_original\":\"0\"},{\"id\":\"62222792\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b356c3e74.docx\",\"filetype\":\"docx\",\"filesize\":\"973810\",\"is_original\":\"0\"},{\"id\":\"62222833\",\"softlink\":\"http:\/\/up14.book118.com\/uploadfiles\/20161110\/3152850\/3152850-5823b3657128f.docx\",\"filetype\":\"docx\",\"filesize\":\"1304228\",\"is_original\":\"0\"}]}";
//			response = "{\"status\":\"003\",\"message\":\"\u6570\u636e\u8fd4\u56de\u6210\u529f\",\"result\":[{\"id\":\"62042834\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2947076\/2947076-582271cc37aa9.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62042835\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2338811\/2338811-582271ccbb73b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62042838\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2947076\/2947076-582271b735367.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62042839\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/3158079\/3158079-582271cde9374.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62042840\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/3196921\/3196921-582271ce3dba6.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62042841\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2947076\/2947076-582271cdeb2bf.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62042842\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/3253156\/3253156-582271cf191e6.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62042843\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2947076\/2947076-582271cfaa3cb.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62042844\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2947076\/2947076-582271cf85050.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62042845\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161109\/2576670\/2576670-582271cf8676d.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"}]}";
//			response = "{\"status\":\"003\",\"message\":\"\u6570\u636e\u8fd4\u56de\u6210\u529f\",\"result\":[{\"id\":\"62451129\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254846bc3d6.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451131\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-58254847713a5.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451132\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-582548471ed0c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451133\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-582548477f21e.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451134\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-582548471f3fd.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451135\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3282587\/3282587-58254847d9d48.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451136\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-58254842d73d8.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451137\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/2598667\/2598667-582548479cf74.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451138\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254848eb2b5.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451139\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825484908364.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451141\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-58254849ef4a0.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451142\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825484a1b197.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451145\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825484a763f4.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451146\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825484895cf1.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451147\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825484a9324a.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451148\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-5825484b3bc41.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451149\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825484b6b8b9.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451150\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825484ba570b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451151\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825484bc2157.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451152\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825484c895b3.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451153\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825484c340c8.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451154\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825484cc7447.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451155\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825484cdb055.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451156\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825484d51870.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451159\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825484deac5b.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451161\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3257411\/3257411-5825484e842d8.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62451162\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825484ebf196.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451163\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825484f22386.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451164\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825484f598d5.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451165\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254850d2dce.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451170\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-58254855a44b3.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451174\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/2598667\/2598667-5825485a386e0.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451178\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485be258b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451179\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485cb6f5a.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451180\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3183285\/3183285-58254848e3f42.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451181\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825485cbc304.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451182\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3183285\/3183285-5825485da33c2.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451183\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825485d51abf.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451184\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825485d43ef4.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451185\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825485e121bd.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451187\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485e9cc02.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451188\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485e44761.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451189\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825485ee903f.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451190\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-5825485eea3ce.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451192\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825485f6a046.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451193\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-5825485f80822.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451194\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825485f4685e.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451195\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485f45293.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451196\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548602675c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451197\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825485fd75db.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451198\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254860399d1.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451199\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-582548607ecf7.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451200\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-58254860e3fd5.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451201\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-582548612d550.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451202\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254861052b5.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451203\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548615b034.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451204\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-582548614c11d.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451206\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-58254862af575.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451207\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548628cda6.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451208\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-582548627a7e2.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451210\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/2598667\/2598667-58254862ba585.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451211\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825486278886.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451212\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548631de60.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451213\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254863a3b2a.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451214\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548644b955.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451215\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486408d8e.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451216\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-58254864b61d7.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451217\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254864e84d1.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451219\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254865eaa5d.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451220\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254864a2f06.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451221\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825486600123.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451222\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254866384f4.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451224\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548667a36a.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451225\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254852d1b46.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451226\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825486713ec3.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451227\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-58254866b7760.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451228\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-58254866a8004.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451229\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-582548672a9fa.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451230\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-58254867c0511.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451231\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3257411\/3257411-582548673c54f.pptx\",\"filetype\":\"pptx\",\"is_original\":\"0\"},{\"id\":\"62451232\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-58254867c68b7.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451233\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548684485f.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451234\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825486928d47.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451235\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-582548690cacc.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451237\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-582548697cb93.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451238\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486971d59.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451240\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825486a34f45.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451241\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825486b5a6ae.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451242\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825486b4d862.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451243\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486a47eae.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451244\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486c1cc91.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451247\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486d75b42.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451249\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3304978\/3304978-5825486dde2c3.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451250\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825486e821b2.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451253\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825486d798f2.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451254\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146756\/3146756-5825486f529c1.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451255\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825486fa9b9c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451256\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3286553\/3286553-5825486fefde0.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62451257\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3283265\/3283265-5825486fa5d85.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62451258\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161111\/3146774\/3146774-5825486fa1490.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"}]}";
			response = "{\"status\":\"003\",\"message\":\"\u6570\u636e\u8fd4\u56de\u6210\u529f\",\"result\":[{\"id\":\"62971712\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a543a0b0.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971714\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a549e0ab.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971715\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a5484017.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971716\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a55e8399.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971717\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3304870\/3304870-58290a5642d1a.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971718\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a5584fd9.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971719\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a571b307.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971720\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1075144\/1075144-58290a5734f81.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971721\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a5646636.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971722\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a5791435.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971723\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a58c3db8.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971724\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1696710\/1696710-58290a58170f4.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971725\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a5891de3.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971726\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3304870\/3304870-58290a5891110.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971728\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a5939853.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971729\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a5964f62.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971730\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a59c13b0.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971731\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146760\/3146760-58290a59b5798.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971732\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146777\/3146777-58290a59c1d5f.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971733\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a5a41ce5.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971734\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a599f826.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971735\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a5a99b27.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971736\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a5bd913c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971737\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a5c3de3c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971739\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1696710\/1696710-58290a5c871de.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971740\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a5ccc68b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971741\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a5d9ce52.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971743\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a5ddb368.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971744\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a5e7a385.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971745\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146777\/3146777-58290a5ebf6a4.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971746\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146760\/3146760-58290a5f9c36d.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971747\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6070f13.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971748\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a60970de.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971749\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1696710\/1696710-58290a60a991f.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971750\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a616db57.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971751\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a613fce8.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971752\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a61a6406.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971753\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a6293743.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971754\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576632\/2576632-58290a635fb2c.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971755\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a64255de.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971756\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6500b9d.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971757\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a65ad666.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971760\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1696710\/1696710-58290a65f396d.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971761\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a61477a3.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971762\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146760\/3146760-58290a65f3103.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971763\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a661921c.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971764\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6660c07.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971765\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a666e847.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971766\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a6739718.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971768\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a679c17d.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971769\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a682a346.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971772\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a6a03c5b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971773\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6a93110.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971775\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a6b1c3bf.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971776\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a6b83ce9.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971777\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6c04309.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971778\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a6c6836a.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971779\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576632\/2576632-58290a6bae3c7.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971780\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146760\/3146760-58290a6bda34b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971781\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a6c9960e.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971782\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a6c9b34d.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971783\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a6cc1788.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971784\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a6d7c6eb.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971785\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a6ea24c3.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971786\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6df1d2c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971787\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a6e1f185.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971788\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a6ef1d22.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971789\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a6eaed46.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971790\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a6ed4c7e.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971791\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a6da1065.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971792\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a6e84772.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971793\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a7034701.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971794\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a70321b6.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971795\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a70bfafd.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971796\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a7102188.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971798\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a719628e.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971799\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576632\/2576632-58290a71b359f.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971800\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a7280cff.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971801\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a732cb71.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971802\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a733738c.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971803\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a723639b.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971804\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a73e1359.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971805\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a740944d.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971806\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146760\/3146760-58290a744364b.docx\",\"filetype\":\"docx\",\"is_original\":\"0\"},{\"id\":\"62971808\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a742716f.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971809\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a7559d5b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971810\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a7577053.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971811\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a7580514.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971813\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a757a010.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971814\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a76751f2.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971815\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576551\/2576551-58290a76e3c92.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971816\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a770f7c9.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971817\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3146777\/3146777-58290a7745924.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971818\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/1208819\/1208819-58290a77a9223.ppt\",\"filetype\":\"ppt\",\"is_original\":\"0\"},{\"id\":\"62971819\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a77bd136.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971821\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3216956\/3216956-58290a785d6ec.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971822\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/3164627\/3164627-58290a78a2031.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971824\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a79311e0.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971825\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2576651\/2576651-58290a79acade.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"},{\"id\":\"62971826\",\"softlink\":\"http:\/\/up2226.book118.com\/uploadfiles\/20161114\/2574755\/2574755-58290a7ad6d4b.doc\",\"filetype\":\"doc\",\"is_original\":\"0\"}]}";
#endif
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