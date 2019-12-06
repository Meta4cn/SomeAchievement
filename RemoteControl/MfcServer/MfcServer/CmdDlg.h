#pragma once
#include "afxwin.h"
#include "MyEdit.h"

// CCmdDlg 对话框

class CCmdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(SOCKET sClient, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCmdDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnSetCmdData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	CMyEdit m_EdContent;
	CString m_strOld;
	SOCKET m_sClient;
};
