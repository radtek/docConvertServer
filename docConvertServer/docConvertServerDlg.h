
// docConvertServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CdocConvertServerDlg �Ի���
class CGetAPIDataThread;
class CGetConvertDataThread;
class CConvertThread;
class CPostResultThread;
class CdocConvertServerDlg : public CDialogEx
{
// ����
public:
	CdocConvertServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DOCCONVERTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

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
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bIsDelSrc;
//	BOOL m_bIsSaveLog;
	// ת���߳���
	int m_nConvertThreads;
	// ת����ʱ
	int m_nConvertTimeout;
	// ת��ʧ�ܵ��õ�api
	CString m_strFailedApi;
	// ��ȡ�б�api
	CString m_strGetApi;
	// img����ǰ׺
	CString m_strImgHttp;
	// img����·��
	CString m_strImgSavePath;
	CListBox m_wndMsgList;
	// ��־����·��
	CString m_strLogPath;
	// �ĵ���Сҳ��
	int m_nMinPages;
	// ת���ɹ�����api
	CString m_strSuccessApi;
	CTabCtrl m_wndTabCtrl;
	// txt����ǰ׺
	CString m_strTxtHttp;
	// txt�ļ�����·��
	CString m_strTxtSavePath;
	CButton m_wndBtSaveConfig;
	// api��������������	// api��������������
	int m_nSpanTime;
	// api��������������
	int m_nSpanTimeNull;
	CString m_strDownPath;
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtSaveconfig();
	afx_msg void OnBnClickedStart();
	// �Ƿ������غ�ת��
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
	// //�ļ���������ļ���
	int m_nMaxFileNumInFloder;
protected:
	afx_msg LRESULT OnMsgMsgLog(WPARAM wParam, LPARAM lParam);
public:
	CString m_strYZApi;
};
