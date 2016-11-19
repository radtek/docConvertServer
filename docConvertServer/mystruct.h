#ifndef _UTIL_H_
#define _UTIL_H_

#include "stdafx.h"
#include "Util.h"
#include <string>
#include <vector>
using namespace std;

enum OutStatus{
	ConvertSuccess,//0:ת���ɹ�
	FileLoss,//1:����ʧ��/�ļ�������
	NoText,//2:ɨ�����ߴ�ͼƬ
	TotxtFailed,//3:totxtת��ʧ��
	PageLess,//4:ҳ������ָ��ֵ
	WaitConvert,
	Converting
};

const char strConvertStatus[][100] = { "ת���ɹ�", "����ʧ��", "ɨ�����ߴ�ͼƬ", "totxtת��ʧ��", "ҳ������ָ��ֵ", "�ȴ�ת��", "����ת��" };

enum FILETYPE
{
	MSWORD,
	MSEXCEL,
	MSPPT,
	MSPDF
};

enum OUTFILETYPE
{
	OUTTXT,
	OUTPNG,
	OUTJPG
};

const char strFileType[][100] = { "doc", "xls", "ppt", "pdf" };

const char strOutFileType[][100] = { "txt", "png", "jpg" };


typedef struct ST_DOMAIN2PATH
{
	ST_DOMAIN2PATH(){ domain = NULL; path = NULL; txtpath = NULL; txthttp = NULL; imgpath = NULL; imghttp = NULL; }
	char *domain;
	char *path;
	char *txtpath;
	char *txthttp;
	char *imgpath;
	char *imghttp;
}st_domain2path, *p_st_domain2path;



typedef struct ST_T_CONVERT
{
	ST_T_CONVERT(){ softlink = NULL; status = 0; }
	int nid;
	int fileid;
	char *softlink;
	int filetype;
	int isoriginal;
	int status;
}st_tconvert, *p_st_tconvert;

typedef struct ST_T_CONVERTED
{
	ST_T_CONVERTED(){ node = NULL; txturl = NULL; imgurl = NULL; pagenumber = 0; }
	int nid;
	int fileid;
	int filetype;
	char *node;
	int txtstatus;
	char *txturl;
	int imgstatus;
	char *imgurl;
	int pagenumber;
}st_tconverted, *p_st_tconverted;

typedef struct ST_MSG{
	int threadid;
	p_st_tconverted sc;
}st_msg, *p_st_msg;

static char *ReSolveSoftlink(const char *softlink, char *domain, int domainlen, char *filepath, int filepathlen)
{
	if (!softlink || strlen(softlink) == 0){
		return NULL;
	}
	int len = 0;
	int haspath = 0;
	len = strlen(softlink) - 1;
	int len1=0;
	for (int i=0; i<len; i++){
		if (softlink[i] == '/' && softlink[i + 1] == '/'){
			//			filename = (char*)(&strbuf[len + 1]);
			strcpy_s(domain, domainlen, (char*)(&softlink[i + 2]));
			len1 = i+2;
			i++;
			continue;
		}
		if (softlink[i] == '/')
		{
			strncpy(domain, domain, i - len1);
			domain[i - len1] = '\0';
			strcpy_s(filepath, filepathlen, (char*)(&softlink[i + 1]));
			haspath = 1;
			break;
		}
	}
	if (!haspath)
	{
		strcpy_s(filepath, filepathlen, softlink);
	}

	return filepath;
}


static int ReplaceChar(char* source, char* oldchar, char *newchar)
{
	string strtemp(source);
	string strold(oldchar);
	string strnew(newchar);
	strtemp = replace_all_distinct(strtemp, strold, strnew);
	strcpy(source, strtemp.c_str());
	source[strtemp.length()] = '\0';
	return 0;
}

#endif