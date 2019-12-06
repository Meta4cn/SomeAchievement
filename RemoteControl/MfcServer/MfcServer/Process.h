#pragma once
#include "afxcmn.h"


// CProcess 对话框

class CProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CProcess)

public:
  CProcess(SOCKET sClient, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcess();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
  SOCKET m_sClient;
protected:
public:
  afx_msg void OnBnClickedOk();
  // list control param
  virtual BOOL OnInitDialog();
  void InsertList(CString strProcName, CString strProcId);
  virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
  int Mysetprocdata(WPARAM wParam, LPARAM lParam/*, CListCtrl& lst*/);
  CListCtrl m_lst;
  afx_msg void OnNMRClickProc(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnClsprc();
protected:

  afx_msg LRESULT OnMyclsprocscs(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnMyclsprocfail(WPARAM wParam, LPARAM lParam);
};
