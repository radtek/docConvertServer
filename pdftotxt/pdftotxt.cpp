// pdftotxt.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"

// #include <windows.h>

#include "Util.h"

#include <atlimage.h>
#include<fstream>
#include<string>
#include<iostream>
#include <regex>

using namespace std;

#define WM_MSG_PDF2TXT_STATUS WM_USER + 0x17

enum OutStatus
{
	ConvertSuccess,//0:转换成功
	FileLoss,//1:下载失败/文件不存在
	NoText,//2:扫描版或者纯图片
	TotxtFailed,//3:totxt转换失败
	PageLess//4:页数低于指定值
};


// Data types
typedef void*	FPDF_MODULEMGR;

// PDF types
typedef void*	FPDF_DOCUMENT;
typedef void*	FPDF_PAGE;
typedef void*	FPDF_PAGEOBJECT;	// Page object(text, path, etc)
typedef void*	FPDF_PATH;
typedef void*	FPDF_CLIPPATH;
typedef void*	FPDF_BITMAP;
typedef void*	FPDF_FONT;

typedef void*	FPDF_TEXTPAGE;
typedef void*	FPDF_SCHHANDLE;
typedef void*	FPDF_PAGELINK;
typedef void*	FPDF_HMODULE;
typedef void*	FPDF_DOCSCHHANDLE;

typedef void*	FPDF_BOOKMARK;
typedef void*	FPDF_DEST;
typedef void*	FPDF_ACTION;
typedef void*	FPDF_LINK;

typedef void*	FPDF_BITMAP;


// Basic data types
typedef int				FPDF_BOOL;
typedef int				FPDF_ERROR;
typedef unsigned long	FPDF_DWORD;

// String types
typedef unsigned short			FPDF_WCHAR;
typedef unsigned char const*	FPDF_LPCBYTE;

// FPDFSDK may use three types of strings: byte string, wide string (UTF-16LE encoded), and platform dependant string
typedef const char*				FPDF_BYTESTRING;

typedef const unsigned short*	FPDF_WIDESTRING;		// Foxit PDF SDK always use UTF-16LE encoding wide string,
// each character use 2 bytes (except surrogation), with low byte first.

// For Windows programmers: for most case it's OK to treat FPDF_WIDESTRING as Windows unicode string,
//		 however, special care needs to be taken if you expect to process Unicode larger than 0xffff.
// For Linux/Unix programmers: most compiler/library environment uses 4 bytes for a Unicode character,
//		you have to convert between FPDF_WIDESTRING and system wide string by yourself.

#ifdef _WIN32_WCE
typedef const unsigned short* FPDF_STRING;
#else
typedef const char* FPDF_STRING;
#endif


#define FPDF_ANNOT			0x01		// Set if annotations are to be rendered
#define FPDF_LCD_TEXT		0x02		// Set if using text rendering optimized for LCD display
#define FPDF_NO_GDIPLUS		0x04		// Set if you don't want to use GDI+ (for fast rendering with poorer graphic quality)



typedef void (WINAPI *Pdf_InitLib)(void*);
typedef void (WINAPI *Pdf_DestroyLib)();
typedef int (WINAPI *Pdf_PDFToText)(const char *, const char *, int, char*);
typedef void (WINAPI *Pdf_UnlockDLL)(char *, char *);
typedef FPDF_DOCUMENT(WINAPI *FPDF_LoadDocument)(FPDF_STRING file_path,
	FPDF_BYTESTRING password);
typedef int (WINAPI *FPDF_GetPageCount)(FPDF_DOCUMENT document);
typedef int (WINAPI *FPDFText_PageToText)(FPDF_DOCUMENT doc, int page_index, wchar_t* buf, int size, int flag);
typedef FPDF_PAGE(WINAPI *FPDF_LoadPage)(FPDF_DOCUMENT document, int page_index);
typedef void	(WINAPI *FPDF_ClosePage)(FPDF_PAGE page);
typedef FPDF_TEXTPAGE(WINAPI *FPDFText_LoadPage)(FPDF_PAGE page);
typedef int (WINAPI *FPDFText_CountChars)(FPDF_TEXTPAGE text_page);
typedef unsigned int(WINAPI * FPDFText_GetUnicode)(FPDF_TEXTPAGE text_page, int index);
typedef int (WINAPI *FPDFText_GetText)(FPDF_TEXTPAGE text_page, int start_index, int count, unsigned short* result);
typedef void (WINAPI *FPDFText_ClosePage)(FPDF_TEXTPAGE text_page);
typedef void (WINAPI *FPDF_CloseDocument)(FPDF_DOCUMENT document);
typedef void (WINAPI *FPDF_RenderPage)(HDC dc, FPDF_PAGE page, int start_x, int start_y, int size_x, int size_y,
	int rotate, int flags);
typedef void (WINAPI *FPDF_RenderPageBitmap)(FPDF_BITMAP bitmap, FPDF_PAGE page, int start_x, int start_y,
	int size_x, int size_y, int rotate, int flags);

typedef double (WINAPI *FPDF_GetPageWidth)(FPDF_PAGE page);
typedef double (WINAPI *FPDF_GetPageHeight)(FPDF_PAGE page);

typedef int (WINAPI *FPDFText_PDFToText)(const char * sour_file, const char * dest_file, int flag, FPDF_BYTESTRING password);

typedef FPDF_BITMAP(WINAPI *FPDFBitmap_Create)(int width, int height, int alpha);
typedef void (WINAPI *FPDFBitmap_FillRect)(FPDF_BITMAP bitmap, int left, int top, int width, int height,
	int red, int green, int blue, int alpha);
typedef void* (WINAPI *FPDFBitmap_GetBuffer)(FPDF_BITMAP bitmap);
typedef int (WINAPI *FPDFBitmap_GetWidth)(FPDF_BITMAP bitmap);
typedef int (WINAPI *FPDFBitmap_GetHeight)(FPDF_BITMAP bitmap);
typedef void (WINAPI *FPDFBitmap_Destroy)(FPDF_BITMAP bitmap);




Pdf_InitLib pdf_init;
Pdf_PDFToText pdf_pdftotxt;
Pdf_DestroyLib pdf_destroy;
Pdf_UnlockDLL pdf_unlock;
FPDF_LoadDocument pFpdfLoadDocument;
FPDF_CloseDocument pFpdfCloseDocument;
FPDF_GetPageCount pFPdfGetPageCount;
FPDFText_PDFToText pFPDFTextPdfToText;
FPDFText_PageToText pFPDFTextPageToText;
FPDF_LoadPage pFPDFLoadPage;
FPDF_ClosePage pFPDFClosePage;

FPDF_RenderPage pFPDFRenderPage;
FPDF_RenderPageBitmap pFPDFRenderPageBitmap;

FPDF_GetPageWidth pFPDFGetPageWidth;
FPDF_GetPageHeight pFPDFGetPageHeight;

FPDFText_LoadPage pFPDFTextLoadPage;
FPDFText_CountChars pFPDFTextCountChars;
FPDFText_GetUnicode pFPDFTextGetUnicode;
FPDFText_GetText pFPDFTextGetText;
FPDFText_ClosePage pFPDFTextClosePage;

FPDFBitmap_Create pFPDFBitmapCreate;
FPDFBitmap_FillRect pFPDFBitmapFillRect;
FPDFBitmap_GetBuffer pFPDFBitmapGetBuffer;
FPDFBitmap_GetWidth pFPDFBitmapGetWidth;
FPDFBitmap_GetHeight pFPDFBitmapGetHeight;
FPDFBitmap_Destroy pFPDFBitmapDestroy;



HINSTANCE hdll;



int InitFPdfSdk();
int ConvertPdf2Txt(char *sourcefile, char *outfile);
byte * Ontrun(bool bLeftRight, BITMAPINFOHEADER *bmiHeader, BITMAPFILEHEADER &bmfh, byte* bmData);
bool BMP_SaveFile(const char* szFile, const void* pBmp, int width, int height, int bitCount = 32);
int ConvertPdf2Png(char *sourcefile, char *outpath, int &pagenum);
int ConvertPdf(char *sourcefile, char *outpath, char *cid, int isdelsrc, int converttype, vector<int> pages, int isgetpagenum, int &pageNum, int txtSize, bool bToTxt = true, bool bToHtml = true);

// void ParseOutFilePath(const char *filepath, const char *outpath, char *outfilepath, char *filename);
int ConvertPdf2Png_Thumb(char *sourcefile);

int ExtractPdftoPng(const char* pdffile, const char* outpath, int beginpage = 0, int lastpage = 0);

int InitFPdfSdk()
{
	hdll = LoadLibrary(L"fpdfsdk.dll");
	if (NULL == hdll)
	{
		return -1;
	}

	pdf_init = (Pdf_InitLib)GetProcAddress(hdll, "FPDF_InitLibrary");
	if (NULL == pdf_init)
	{
		return -1;
	}

	pdf_destroy = (Pdf_DestroyLib)GetProcAddress(hdll, "FPDF_DestroyLibrary");
	if (NULL == pdf_destroy)
	{
		return -1;
	}

	pdf_pdftotxt = (Pdf_PDFToText)GetProcAddress(hdll, "FPDFText_PDFToText");
	if (NULL == pdf_pdftotxt)
	{
		return -1;
	}

	pdf_unlock = (Pdf_UnlockDLL)GetProcAddress(hdll, "FPDF_UnlockDLL");
	if (NULL == pdf_unlock)
	{
		return -1;
	}

	pFpdfLoadDocument = (FPDF_LoadDocument)GetProcAddress(hdll, "FPDF_LoadDocument");
	if (NULL == pFpdfLoadDocument)
	{
		return -1;
	}

	pFpdfCloseDocument = (FPDF_CloseDocument)GetProcAddress(hdll, "FPDF_CloseDocument");
	if (NULL == pFpdfCloseDocument)
	{
		return -1;
	}

	pFPdfGetPageCount = (FPDF_GetPageCount)GetProcAddress(hdll, "FPDF_GetPageCount");
	if (NULL == pFPdfGetPageCount)
	{
		return -1;
	}

	pFPDFRenderPage = (FPDF_RenderPage)GetProcAddress(hdll, "FPDF_RenderPage");
	if (NULL == pFPDFRenderPage)
	{
		return -1;
	}

	pFPDFRenderPageBitmap = (FPDF_RenderPageBitmap)GetProcAddress(hdll, "FPDF_RenderPageBitmap");
	if (NULL == pFPDFRenderPageBitmap)
	{
		return -1;
	}

	pFPDFGetPageWidth = (FPDF_GetPageWidth)GetProcAddress(hdll, "FPDF_GetPageWidth");
	if (NULL == pFPDFGetPageWidth)
	{
		return -1;
	}

	pFPDFGetPageHeight = (FPDF_GetPageHeight)GetProcAddress(hdll, "FPDF_GetPageHeight");
	if (NULL == pFPDFGetPageHeight)
	{
		return -1;
	}

	pFPDFTextPdfToText = (FPDFText_PDFToText)GetProcAddress(hdll, "FPDFText_PDFToText");
	if (NULL == pFPDFTextPdfToText)
	{
		return -1;
	}

	pFPDFTextPageToText = (FPDFText_PageToText)GetProcAddress(hdll, "FPDFText_PageToText");
	if (NULL == pFPDFTextPageToText)
	{
		return -1;
	}

	pFPDFLoadPage = (FPDF_LoadPage)GetProcAddress(hdll, "FPDF_LoadPage");
	if (NULL == pFPDFLoadPage)
	{
		return -1;
	}

	pFPDFClosePage = (FPDF_ClosePage)GetProcAddress(hdll, "FPDF_ClosePage");
	if (NULL == pFPDFClosePage)
	{
		return -1;
	}

	pFPDFTextLoadPage = (FPDFText_LoadPage)GetProcAddress(hdll, "FPDFText_LoadPage");
	if (NULL == pFPDFTextLoadPage)
	{
		return -1;
	}

	pFPDFTextCountChars = (FPDFText_CountChars)GetProcAddress(hdll, "FPDFText_CountChars");
	if (NULL == pFPDFTextCountChars)
	{
		return -1;
	}

	pFPDFTextGetUnicode = (FPDFText_GetUnicode)GetProcAddress(hdll, "FPDFText_GetUnicode");
	if (NULL == pFPDFTextGetUnicode)
	{
		return -1;
	}

	pFPDFTextGetText = (FPDFText_GetText)GetProcAddress(hdll, "FPDFText_GetText");
	if (NULL == pFPDFTextGetText)
	{
		return -1;
	}

	pFPDFTextClosePage = (FPDFText_ClosePage)GetProcAddress(hdll, "FPDFText_ClosePage");
	if (NULL == pFPDFTextClosePage)
	{
		return -1;
	}

	pFPDFBitmapCreate = (FPDFBitmap_Create)GetProcAddress(hdll, "FPDFBitmap_Create");
	if (NULL == pFPDFBitmapCreate)
	{
		return -1;
	}

	pFPDFBitmapFillRect = (FPDFBitmap_FillRect)GetProcAddress(hdll, "FPDFBitmap_FillRect");
	if (NULL == pFPDFBitmapFillRect)
	{
		return -1;
	}

	pFPDFBitmapGetBuffer = (FPDFBitmap_GetBuffer)GetProcAddress(hdll, "FPDFBitmap_GetBuffer");
	if (NULL == pFPDFBitmapGetBuffer)
	{
		return -1;
	}

	pFPDFBitmapGetWidth = (FPDFBitmap_GetWidth)GetProcAddress(hdll, "FPDFBitmap_GetWidth");
	if (NULL == pFPDFBitmapGetWidth)
	{
		return -1;
	}

	pFPDFBitmapGetHeight = (FPDFBitmap_GetHeight)GetProcAddress(hdll, "FPDFBitmap_GetHeight");
	if (NULL == pFPDFBitmapGetHeight)
	{
		return -1;
	}

	pFPDFBitmapDestroy = (FPDFBitmap_Destroy)GetProcAddress(hdll, "FPDFBitmap_Destroy");
	if (NULL == pFPDFBitmapDestroy)
	{
		return -1;
	}

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// 	HINSTANCE hInstance= ::AfxGetInstanceHandle();

	pdf_unlock("SDKRDTEMP", "921315A06BD486EBC0792D60A826A5C4455E33A8");
	pdf_init(hInstance);

	return 0;
}



int ConvertPdf2Txt(char *sourcefile, char *outfile)
{
	char txtfile[200] = { 0 };
	char filename[128] = { 0 };
	ParseOutFilePath(sourcefile, outfile, txtfile, filename);
	strcat(txtfile, filename);
	strcat(txtfile, "_txt\\");
	CreateMultipleDirectory(txtfile);

	int bok = 0;
#if 0
	strcat(txtfile, filename);
	strcat(txtfile, ".txt");
	bok = pFPDFTextPdfToText(sourcefile, txtfile, 0, NULL);
#endif

	void* pDoc = pFpdfLoadDocument(sourcefile, NULL);
	if (pDoc != NULL)
	{
		int npages = pFPdfGetPageCount(pDoc);

		for (int i = 0; i<npages; i++)
		{
			//不转换1000页以后的页面
			if (i >= 1000) break;

			char cfilename[200] = { 0 };
			int len = pFPDFTextPageToText(pDoc, i, NULL, 0, 0);
			if (len>0)
			{
				wchar_t* buf = new wchar_t[len];
				memset(buf, 0, len);
				pFPDFTextPageToText(pDoc, i, buf, len, 0);
				int nlen = 0;
				char *c_buf = WCharToChar(buf, len);

				sprintf(cfilename, "%spage-%03d.txt", txtfile, i + 1);
				ofstream in;
				in.open(cfilename, ios::trunc);
				in << c_buf;
				in.close();

				delete[] buf;
				delete[] c_buf;
			}
		}
		pFpdfCloseDocument(pDoc);
		bok = 1;
	}


	if (bok)
		return 0;
	else
		return 1;
}


byte * Ontrun(bool bLeftRight, BITMAPINFOHEADER *bmiHeader, BITMAPFILEHEADER &bmfh, byte* bmData)
{
	byte * bEddyData = NULL;
	int byteCount = bmiHeader->biBitCount / 8;
	bEddyData = new byte[bmfh.bfSize - bmfh.bfOffBits];
	int iBmpAddLineData = (4 - bmiHeader->biWidth * byteCount % 4) % 4;

	//对数据进行操作
	int iImageSize = (bmiHeader->biWidth * byteCount + iBmpAddLineData) * bmiHeader->biHeight;

	for (int i = 0; i < bmiHeader->biHeight; i++)
	{
		int m = 0;
		for (int j = 0; j < bmiHeader->biWidth * byteCount; j += byteCount)
		{
			int bmDataIndex;
			int bEddyDataRowIndex;
			int bEddyDataColIndex;
			int locIndex;
			if (bLeftRight)
			{
				bmDataIndex = (bmiHeader->biWidth*byteCount + iBmpAddLineData)*i;
				bEddyDataRowIndex = i*((bmfh.bfSize - bmfh.bfOffBits) / bmiHeader->biHeight);
				bEddyDataColIndex = (bmiHeader->biWidth - 1 - m)*byteCount;
				locIndex = bEddyDataRowIndex + bEddyDataColIndex;
			}
			else
			{
				bmDataIndex = (bmiHeader->biWidth * byteCount + iBmpAddLineData) * i;
				bEddyDataRowIndex = (bmiHeader->biHeight - 1 - i)*((bmfh.bfSize - bmfh.bfOffBits) / bmiHeader->biHeight);
				bEddyDataColIndex = (m)*byteCount;
				locIndex = bEddyDataRowIndex + bEddyDataColIndex;
			}
			bEddyData[locIndex] = bmData[j + bmDataIndex];     //B
			bEddyData[locIndex + 1] = bmData[j + bmDataIndex + 1]; //G
			bEddyData[locIndex + 2] = bmData[j + bmDataIndex + 2]; //R
			m++;
		}
	}

	return bEddyData;
}

bool BMP_SaveFile(const char* szFile, const void* pBmp, int width, int height, int bitCount)
{
	FILE* pFile = fopen(szFile, "wb");
	ASSERT(pFile != NULL);

	int bmp_size = width*height*(bitCount / 8);

	// 【写位图文件头】
	BITMAPFILEHEADER bmpHeader;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmp_size;    // BMP图像文件的大小
	bmpHeader.bfType = 0x4D42;    // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);            // BMP图像数据的偏移位置
	bmpHeader.bfReserved1 = 0;    // 总为0
	bmpHeader.bfReserved2 = 0;    // 总为0
	fwrite(&bmpHeader, sizeof(bmpHeader), 1, pFile);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(bmiHeader);                // 本结构所占用字节数，即sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;                            // 位图宽度（单位：像素）
	bmiHeader.biHeight = height;                        // 位图高度（单位：像素）
	bmiHeader.biPlanes = 1;                                // 目标设备的级别，必须为1
	bmiHeader.biBitCount = bitCount;                    // 像素的位数（每个像素所需的位数，范围：1、4、8、24、32）
	bmiHeader.biCompression = 0;                        // 压缩类型（0：不压缩 1：BI_RLE8压缩类型 2：BI_RLE4压缩类型）
	bmiHeader.biSizeImage = bmp_size;                    // 位图大小（单位：字节）
	bmiHeader.biXPelsPerMeter = 0;                        // 水平分辨率(像素/米)
	bmiHeader.biYPelsPerMeter = 0;                        // 垂直分辨率(像素/米)
	bmiHeader.biClrUsed = 0;                            // 位图实际使用的彩色表中的颜色索引数
	bmiHeader.biClrImportant = 0;                        // 对图象显示有重要影响的颜色索引的数目

	// 【写位图信息头（BITMAPINFO的bmiHeader成员）】
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, pFile);

	byte *newData = NULL;
	newData = Ontrun(0, &bmiHeader, bmpHeader, (byte*)pBmp);
	// 【写像素内容】
	fwrite(newData, bmp_size, 1, pFile);
	fclose(pFile);

	delete newData;
	return true;
}

bool ChangeImgName(string imgPath)
{
	if (imgPath == "" || imgPath.empty())
	{
		return false;
	}
	int times = 1;
	int jpgNum = 1;
	if (imgPath[imgPath.length() - 1] != '\\' && imgPath[imgPath.length() - 1] != '/')
	{
		imgPath.append("\\");
	}
	string findFile = imgPath + "*.png";
	while (jpgNum == 1 && times > 0)
	{
		_finddata_t fileInfo;
		long handle = _findfirst(findFile.c_str(), &fileInfo);

		if (handle == -1L)
		{
			return false;
		}

		do
		{
			int num = findnum(fileInfo.name);

			char newname[200] = { 0 };
			sprintf(newname, "%s%d.png", imgPath.c_str(), num);

			// 			remove(newname);

			jpgNum++;
			char filepath[500] = { 0 };// new char[strlen(fileInfo.name)+pptPath.length()];
			sprintf(filepath, "%s%s", imgPath.c_str(), fileInfo.name);

			rename(filepath, newname);
		} while (_findnext(handle, &fileInfo) == 0);
		_findclose(handle);
		times--;
		if (jpgNum == 1)
			Sleep(300);
	}

	if (jpgNum == 0)
		return false;

	return true;
}

int ExtractPdftoTxt(const char* pdffile, const char* outfile)
{
	try
	{
		char szAppPath[MAX_PATH];
		GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
		string pdftotextPath(szAppPath);
		int nPos = pdftotextPath.find_last_of("\\");
		if (nPos < 0)
			return -1;
		else
		{
			pdftotextPath = pdftotextPath.substr(0, nPos);
			pdftotextPath = pdftotextPath + "\\xpdf\\pdftotext.exe";
		}

		char paramstr[1024] = { 0 };
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		si.cbReserved2 = NULL;
		si.lpReserved2 = NULL;

		sprintf(paramstr, "%s %s %s", pdftotextPath.c_str(), pdffile, outfile);

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			WaitForSingleObject(pi.hThread, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return 0;
		}
	}
	catch (...)
	{
		return -1;
	}
	return 1;
}

int ExtractPdftoPng(const char* pdffile, const char* outpath, int beginpage, int lastpage)
{
	try
	{
		char szAppPath[MAX_PATH];
		GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
		string pdftotextPath(szAppPath);
		int nPos = pdftotextPath.find_last_of("\\");
		if (nPos < 0)
			return -1;
		else
		{
			pdftotextPath = pdftotextPath.substr(0, nPos);
			pdftotextPath = pdftotextPath + "\\xpdf\\pdftopng.exe";
		}

		char paramstr[1024] = { 0 };
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		si.cbReserved2 = NULL;
		si.lpReserved2 = NULL;

		if (beginpage == 0 && lastpage == 0)
		{
			sprintf(paramstr, "%s -r 70 %s %s", pdftotextPath.c_str(), pdffile, outpath);
		}
		else
		{
			sprintf(paramstr, "%s -r 70 -f %d -l %d %s %s", pdftotextPath.c_str(), beginpage, lastpage, pdffile, outpath);
		}

		BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		if (ret)
		{
			WaitForSingleObject(pi.hThread, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return 0;
		}
	}
	catch (...)
	{
		return -1;
	}
	return 1;
}

int ResolveTxtFile(const string srcfile, const string dstfile, const long len)
{
	try
	{
		char *buf = new char[len];
		CFile file(CharToCString(srcfile.c_str()), CFile::modeReadWrite);
		if (file.GetLength() < len)
			return 0;
		file.Read(buf, len);
		file.Close();
		CFile newfile(CharToCString(dstfile.c_str()), CFile::modeCreate | CFile::modeWrite);
		newfile.Write(buf, len);
		delete[] buf;
		newfile.Close();
	}
	catch (CMemoryException* e)
	{
		return 1;
	}
	catch (CFileException* e)
	{
		return 2;
	}
	catch (CException* e)
	{
		return 3;
	}
	return 0;
}

void RegexTxtFile(char *sourcefile, char *path, char *cid)
{
#if 0
	fstream out;
	out.open(sourcefile, ios::in);
	char newfile[1024] = { 0 };
	sprintf(newfile, "%s\\%s.txt", path, cid);
	ofstream in;
	in.open(newfile, ios::trunc);
	char outstr[1024] = { 0 };
	while (!out.eof())
	{
		out.getline(outstr, 1024);
		const std::tr1::regex pattern("-*(p|P)age.*-*");
		std::smatch result;
		std::string strtemp(outstr);
		bool match = std::regex_search(strtemp, result, pattern);
		if (!match)
		{
			in << outstr << "\n";
		}
	}
	out.close();
	in.close();
#else
	CStdioFile file;
	CStdioFile rfile;
	if (file.Open(CharToCString(sourcefile), CStdioFile::typeBinary | CStdioFile::modeRead))
	{
		char newfile[1024] = { 0 };
		sprintf(newfile, "%s\\%s.txt", path, cid);
		rfile.Open(CharToCString(newfile), CStdioFile::typeBinary | CStdioFile::modeCreate | CStdioFile::modeWrite);
		CString str;
#if 1

		char *pBuf;
		int iLen = file.GetLength();
		pBuf = new char[iLen + 1];
		file.Read(pBuf, iLen);
		pBuf[iLen] = 0;

		const std::tr1::regex pattern("-*(p|P)age.*-*");
		std::smatch result;
		std::string strtemp(pBuf);
		bool match = std::regex_search(strtemp, result, pattern);
		if (match)
		{
			string strr = std::regex_replace(strtemp, pattern, "");
			rfile.Write(strr.c_str(), strr.length());
		}

#else


		while (file.ReadString(str))
		{
			const std::tr1::regex pattern("-*(p|P)age.*-*");
			std::smatch result;
			char *temp = CStringToChar(str);
			std::string strtemp(temp);
			delete[] temp;
			bool match = std::regex_search(strtemp, result, pattern);
			if (match)
			{
				// 				string strr = std::regex_replace(strtemp, pattern, "");
				// 				TRACE("%s\n", strr.c_str());
			}
			else
			{
				rfile.WriteString(str);
				rfile.WriteString(L"\n");
			}
		}
#endif
		file.Close();
		rfile.Close();
	}
#endif
	remove(sourcefile);
}


void RegexTxt(char *sourcefile, char *filter, char *replacestr)
{
	char paramstr[1024] = { 0 };
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;

	sprintf(paramstr, "regexTxt.exe %s %s %s", sourcefile, filter, replacestr);

	BOOL ret = CreateProcessA(NULL, paramstr, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

#if 0
	BOOL bConvertTimeOut = FALSE;

	if (ret)
	{
		int ntimes = 0;
		while (WaitForSingleObject(pi.hThread, 200) != WAIT_OBJECT_0)
		{
			if (ntimes > 100)
			{
				bConvertTimeOut = TRUE;
				break;
			}
		}
		//转换超时
		if (bConvertTimeOut)
		{
			TerminateProcess(pi.hProcess, 0);
			CloseHandle(pi.hProcess);
		}
		else
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
#endif
}

int ConvertPdfToTxt(const char *sourcefile, const char *outpath, const int &fileid, const int &minpage, const int &txtSize, int &pageNum)
{
	int nReturn = 3;

	if (!IsFileExist(sourcefile))
	{
		nReturn = FileLoss;
		return nReturn;
	}

	int nToTxtStatusOk = 0;
	int nToHtmlStatusOk = 0;
	int nToTxtStatusError = 0;
	int nToHtmlStatusError = 0;

	void* pDoc = pFpdfLoadDocument(sourcefile, NULL);

	if (pDoc == NULL)
	{
		pFpdfCloseDocument(pDoc);
		nReturn = TotxtFailed;
		return nReturn;
	}
	else
	{
		int ncount = pFPdfGetPageCount(pDoc);

		if (ncount < minpage)
		{
			pFpdfCloseDocument(pDoc);
			return PageLess;
		}

		FPDF_PAGE m_pPage;
		char txtfile[1024] = { 0 };
		if (outpath[strlen(outpath) - 1] == '\\' || outpath[strlen(outpath) - 1] == '/')
		{
			sprintf(txtfile, "%s%d.txt", outpath, fileid);
		}
		else
		{
			sprintf(txtfile, "%s\\%d.txt", outpath, fileid);
		}

		int i = 0;

		for (i; i<ncount; i++)
		{
			//totxt
			try
			{
				int len = pFPDFTextPageToText(pDoc, i, NULL, 0, 0);
				if (len > 0)
				{
					wchar_t* buf = new wchar_t[len];
					memset(buf, 0, len);
					pFPDFTextPageToText(pDoc, i, buf, len, 0);
					int nlen = 0;
					char *c_buf = WCharToChar(buf, len);

					//判读文字是否乱码
					if (CheckCodeIsUnrecognizable(c_buf))
					{
						//如果是乱码就调用xpdf转换

						int btxt = ExtractPdftoTxt(sourcefile, txtfile);

						//////正则表达式替换-----page 1-----
						// 								RegexTxtFile(cfilename, txtfile, cid);
						RegexTxt(txtfile, "-*(p|P)age.*-*", "NULL");

						if (txtSize)
						{
							ResolveTxtFile(txtfile, txtfile, txtSize * 1024);
						}
						//////////////////////////////////////////////////////////////////////////

						delete[] buf;
						delete[] c_buf;

						pFpdfCloseDocument(pDoc);

						return ConvertSuccess;
					}
					else
					{
						int bok = pFPDFTextPdfToText(sourcefile, txtfile, 1, NULL);
						if (bok)
						{
							nToTxtStatusOk++;
							//////正则表达式替换-----page 1-----
							// 									RegexTxtFile(cfilename, txtfile, cid);
							RegexTxt(txtfile, "-*(p|P)age.*-*", "NULL");

							if (txtSize)
							{
								ResolveTxtFile(txtfile, txtfile, txtSize * 1024);
							}
							//////////////////////////////////////////////////////////////////////////

						}

						delete[] buf;
						delete[] c_buf;
						break;
					}
				}
			}
			catch (...)
			{
				nToTxtStatusError++;
			}
		}
		pFpdfCloseDocument(pDoc);
	}


	if (nToTxtStatusOk > 0)
	{
		nReturn = ConvertSuccess;
	}

	return nReturn;
}



//pdf文件路径  保存路径  线程id   窗口句柄  是否转txt  是否转html
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 7)
	{
		InitFPdfSdk();

		char c_pdffile[200] = { 0 };
		char c_pdfsave[200] = { 0 };
		int fileid;
		wchar_t temp[200] = { 0 };

		wcscpy(temp, argv[1]);
		char *ctemp = WCharToChar(temp, wcslen(temp));
		strcpy(c_pdffile, ctemp);
		delete[] ctemp;

		memset(temp, 0, 200);
		wcscpy(temp, argv[2]);
		ctemp = WCharToChar(temp, wcslen(temp));
		strcpy(c_pdfsave, ctemp);
		delete[] ctemp;

		memset(temp, 0, 200);
		wcscpy(temp, argv[3]);
		ctemp = WCharToChar(temp, wcslen(temp));
		int nthreadid = atoi(ctemp);
		delete[] ctemp;

		memset(temp, 0, 200);
		wcscpy(temp, argv[4]);
		ctemp = WCharToChar(temp, wcslen(temp));
		fileid = atoi(ctemp);
		delete[] ctemp;

		memset(temp, 0, 200);
		wcscpy(temp, argv[5]);
		ctemp = WCharToChar(temp, wcslen(temp));
		int minpage = atoi(ctemp);
		delete[] ctemp;

		memset(temp, 0, 200);
		wcscpy(temp, argv[6]);
		ctemp = WCharToChar(temp, wcslen(temp));
		int txtsize = atoi(ctemp);
		delete[] ctemp;

#if _DEBUG
		char now[64] = { 0 };
		GetNowTime(now);
		printf("start %s \n", now);
#endif
		int pageNum = 0;
		int status = ConvertPdfToTxt(c_pdffile, c_pdfsave, fileid, minpage, txtsize, pageNum);
#if _DEBUG
		GetNowTime(now);
		printf("end %s \n", now);
#endif
		PostThreadMessage(nthreadid, WM_MSG_PDF2TXT_STATUS, status, pageNum);

		Sleep(50);

		pdf_destroy();
		FreeLibrary(hdll);

#if _DEBUG
		getchar();
#endif
	}

	return 0;
}

