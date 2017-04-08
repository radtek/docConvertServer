
// docConvertServerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CdocConvertServerDlg 对话框
class CGetAPIDataThread;
class CGetConvertDataThread;
class CConvertThread;
class CPostResultThread;
class CdocConvertServerDlg : public CDialogEx
{
// 构造
public:
	CdocConvertServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DOCCONVERTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void InitConsoleWindow();

	char* GetApiNode(const char* apiurl);

	void ReadIni();
	void WriteIni();

	void DrawWindow();
	void UpTableShow();

	void StartGetApiThread();
	void StopGetApiThread();
	void StartGetConvertDataThread();
	void StopGetConvertDataThread();
	void StartConvertThread();
	void StopConvertThread();
	void StartPostResultThread();
	void StopPostResultThread();

	void InitListCtrlDomain();
	void AddListCtrlDomain(p_st_domain2path sm);

	void ShowMsgList(char *msg, BOOL bsavelog = FALSE);
	void InitListCtrl();
	void AddListCtrl(p_st_msg sm);
	void UpdateListCtrl(p_st_msg sm, OutStatus status);


public:
	CGetAPIDataThread* m_pGetApiThread;
	CGetConvertDataThread* m_pGetConvertDataThread;
	CConvertThread* m_pConvertThread[100];
	CPostResultThread* m_pPostResultThread;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bIsDelSrc;
//	BOOL m_bIsSaveLog;
	// 转换线程数
	int m_nConvertThreads;
	// 转换超时
	int m_nConvertTimeout;
	// 转换失败调用的api
	CString m_strFailedApi;
	// 获取列表api
	CString m_strGetApi;
	// img域名前缀
	CString m_strImgHttp;
	// img保存路径
	CString m_strImgSavePath;
	CListBox m_wndMsgList;
	// 日志保存路径
	CString m_strLogPath;
	// 文档最小页数
	int m_nMinPages;
	// 转换成功调用api
	CString m_strSuccessApi;
	CTabCtrl m_wndTabCtrl;
	// txt域名前缀
	CString m_strTxtHttp;
	// txt文件保存路径
	CString m_strTxtSavePath;
	CButton m_wndBtSaveConfig;
	// api请求间隔，有数据	// api请求间隔，有数据
	int m_nSpanTime;
	// api请求间隔，无数据
	int m_nSpanTimeNull;
	CString m_strDownPath;
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtSaveconfig();
	afx_msg void OnBnClickedStart();
	// 是否先下载后转换
	BOOL m_bIsDownfile;
protected:
	afx_msg LRESULT OnMsglistShow(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_wndListCtrl;
protected:
	afx_msg LRESULT OnMsgListctrlAdd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgListctrlUpdate(WPARAM wParam, LPARAM lParam);
public:
	BOOL m_bIsToImg;
	CListCtrl m_wndListCtrlDomain;
	afx_msg void OnBnClickedCkIsdownfile();
	afx_msg void OnBnClickedCkDelsrc();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtClearsqlite();
	// //文件夹中最大文件数
	int m_nMaxFileNumInFloder;
protected:
	afx_msg LRESULT OnMsgMsgLog(WPARAM wParam, LPARAM lParam);
public:
	CString m_strYZApi;
};
