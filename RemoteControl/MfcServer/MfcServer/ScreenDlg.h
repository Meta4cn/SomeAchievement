#pragma once


// CScreenDlg 对话框

class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(SOCKET sClient, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScreenDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
protected:
	afx_msg LRESULT OnMyScreenData(WPARAM wParam, LPARAM lParam);

	SOCKET m_sClient;
public:
  afx_msg void OnScrnSht();
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  BOOL SaveBitmapToFile(HBITMAP hBitmap, CString& szfilename);
private:
  CString m_strBuf;
  int m_nBufLen;
};
