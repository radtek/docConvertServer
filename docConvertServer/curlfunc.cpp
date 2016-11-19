#include "stdafx.h"
#include "curlfunc.h"
#include "Util.h"
#include "curl/curl.h"



double curDownloadLen;

void getFileNameFormUrl(char* fileName, const char* url)
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

/* 得到本地文件大小的函数, 若不是续传则返回0, 否则返回指定路径地址的文件大小 */
long getLocalFileLenth(const char* localPath)
{
	FILE* fp = fopen(localPath, "rb");
	if (fp != NULL)
	{
		int localfilelen = filelength(fileno(fp));
		fclose(fp);
		return localfilelen;
	}
	return 0;
}

static size_t save_header(void *ptr, size_t size, size_t nmemb, void *data)
{
	return (size_t)(size * nmemb);
}


double getTotalFileLenth(const char* url)
{
	char mUrl[512] = { 0 };
	strcpy(mUrl, url);
	double downloadFileLenth = -1;
	int reGetTimes = 5;
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_URL, mUrl);
	curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);
	curl_easy_setopt(pCurl, CURLOPT_NOBODY, 1L);
	string content;
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &content);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, save_header);

	while (reGetTimes > 0 && downloadFileLenth == -1)
	{
		reGetTimes--;
		if (curl_easy_perform(pCurl) == CURLE_OK)
		{
			curl_easy_getinfo(pCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
		}
		else
		{
			downloadFileLenth = -1;
			TRACE("get length error\n");
		}
	}
	curl_easy_cleanup(pCurl);
	return downloadFileLenth;
}


size_t writeFunc(char *str, size_t size, size_t nmemb, void *stream)
{
	return fwrite(str, size, nmemb, (FILE*)stream);
}

size_t progressFunc(
	double* pFileLen,
	double t,// 下载时总大小  
	double d, // 已经下载大小  
	double ultotal, // 上传是总大小  
	double ulnow)   // 已经上传大小  
{
	if (t == 0) return 0;
	*pFileLen = d;
	return 0;
}



int downloadfile(const char *url, char *down_path, int timeout, char *fileName)
{
	if (url == NULL || strlen(url) == 0)
	{
		return 5;
	}
	int nreturn = 0;

	CURL* pCurl = curl_easy_init();

	char down_file_name[128] = { 0 };
	//	GetFilePathFormUrl(down_path_, url);
	getFileNameFormUrl(down_file_name, url);


	do
	{
		//char fileName[1152] = { 0 };
		CreateMultipleDirectory(down_path);
		strcpy(fileName, down_path);
		if (down_path[strlen(down_path) - 1] != '\\' && down_path[strlen(down_path) - 1] != '/')
		{
			strcat(fileName, "\\");
		}
		strcat(fileName, down_file_name);

		long locallength = getLocalFileLenth(fileName);
		double totalFileLen = getTotalFileLenth(url);

		if (locallength == totalFileLen)
		{
			//已下载，跳过
			nreturn = 1;
			break;
		}

		if ((long)totalFileLen <= 0)
		{
			//下载失败
			nreturn = 2;
			break;
		}
		FILE* fp = fopen(fileName, "ab+");
		if (fp == NULL) //，进行下一个文件的下载
		{
			//文件创建失败
			nreturn = 3;
			break;
		}
		curl_easy_setopt(pCurl, CURLOPT_URL, url);
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);
		curl_easy_setopt(pCurl, CURLOPT_NOBODY, 0L);
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(pCurl, CURLOPT_RESUME_FROM, locallength);

		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, writeFunc);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fp);

		curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, &curDownloadLen);

		//开始下载
		if (curl_easy_perform(pCurl))
		{
			//下载失败
			nreturn = 4;
		}
		fclose(fp);
		Sleep(150);
	} while (0);

	curl_easy_cleanup(pCurl);
	return nreturn;
}
