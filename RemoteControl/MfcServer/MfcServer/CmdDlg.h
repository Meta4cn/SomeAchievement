#pragma once
#include "afxwin.h"
#include "MyEdit.h"

// CCmdDlg �Ի���

class CCmdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(SOCKET sClient, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCmdDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnSetCmdData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	CMyEdit m_EdContent;
	CString m_strOld;
	SOCKET m_sClient;
};
