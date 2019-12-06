
// MfcServerDlg.h : 头文件
//

#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"
#include "afxcmn.h"
#include <map>
#include "../../common/common.h"
#include "Iocp.h"

using namespace std;

#define BUF_SIZE 0xFF0000

class CCmdDlg;
class CScreenDlg;
class CProcess;
class CMyExplorer;

struct TagClientContext
{
	TagClientContext()
	{
		m_hThread = INVALID_HANDLE_VALUE;
		m_sClient = INVALID_SOCKET;
		m_pCmdDlg = NULL;
		m_pScreenDlg = NULL;
		m_pProcessDlg = NULL;
		m_pExplorerDlg = NULL;
		m_MainDlg = NULL;

		m_hEvt = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			TRUE,              // 初始状态没有信号
			NULL                // object name
			);

	}

	~TagClientContext()
	{
		if (m_pCmdDlg != NULL)
		{
			((CWnd*)m_pCmdDlg)->DestroyWindow();
		}

		if (m_pScreenDlg != NULL)
		{
			((CWnd*)m_pScreenDlg)->DestroyWindow();
		}

		if (m_pProcessDlg != NULL)
		{
			((CWnd*)m_pProcessDlg)->DestroyWindow();
		}

		if (m_pExplorerDlg != NULL)
		{
			((CWnd*)m_pExplorerDlg)->DestroyWindow();
		}

		m_pProcessDlg = NULL;
		m_pCmdDlg = NULL;
		m_pScreenDlg = NULL;
		m_pExplorerDlg = NULL;

		m_hThread = INVALID_HANDLE_VALUE;
		m_sClient = INVALID_SOCKET;
	}

	void release()
	{
		delete this;
	}


	CMfcServerDlg* m_MainDlg;

	HANDLE m_hThread;
	SOCKET m_sClient;

	CProcess* m_pProcessDlg;
	CCmdDlg* m_pCmdDlg;
	CScreenDlg* m_pScreenDlg;
	CMyExplorer* m_pExplorerDlg;

	CBuffer m_recvBuf;
	CBuffer m_sendBuf; //待发送的缓冲区
	CCriticalSection m_cs;
	char m_TmpBuf[BUF_SIZE];

	clock_t   m_curClock;
	int       m_nListItemIndex;
	HANDLE    m_hEvt;
};


// CMfcServerDlg 对话框
class CMfcServerDlg : public CDialogEx
{
	// 构造
public:
	CMfcServerDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_MFCSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg LRESULT OnCreateCmdDlg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedStart();
	bool OnAccept();
	bool OnRecvData(TagClientContext* pContext, char nType, CBuffer& strRecv, int nBufSize);

	CComboBox m_ComboIP;
	CEdit m_EdPort;

	int m_nPort;
	CString m_strOld;
	SOCKET m_sListen;

	//使用hash表来存储每一个客户端的context
	map<SOCKET, TagClientContext*> m_ClientMap;

	HANDLE m_hAcceptThread;
	HANDLE m_hRecvThread;

	CIocp m_IOCP;

	CCriticalSection m_cs;
	CListCtrl m_Lst;

	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCmd();
	afx_msg void OnScreen();
protected:
	afx_msg LRESULT OnMyscreenstart(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
public:
	afx_msg void OnSetProcData();
protected:
	afx_msg LRESULT OnMyprocstart(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnMyExplorer();
	afx_msg void OnBnClickedStop();
protected:
	afx_msg LRESULT OnMyexplorerstart(WPARAM wParam, LPARAM lParam);
	//  afx_msg LRESULT OnMyexplorerfilelist(WPARAM wParam, LPARAM lParam);
};

typedef unsigned(__stdcall *PFN_start_address)(void*);