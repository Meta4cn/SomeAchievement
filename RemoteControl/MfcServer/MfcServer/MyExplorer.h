#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMyExplorer 对话框

class CMyExplorer : public CDialogEx
{
	DECLARE_DYNAMIC(CMyExplorer)

public:
  CMyExplorer(SOCKET sClient, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyExplorer();

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
  void InitLocalView();
  void ListLogicalDrive(CStringArray& aryLogicDrive);
  void MyFindFile(CString& FilePath);

  //CTreeCtrl m_webTree;
  CComboBox m_cmbLocalExplorer;
  CComboBox m_cmbRemoteExplorer;
  
  CStringArray m_aryLogicDrive;
  CString m_ServerCurPath;
  CString m_ClientCurPath;
  SOCKET m_sClient;
  CString m_CurDownloadClientPath;
  CString m_CurDownloadServerPath;

//  afx_msg void OnCbnSelchangeLocal();
  CListCtrl m_lstLocal;
  CListCtrl m_lstClient;
  afx_msg void OnNMDblclkFilesLocal(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnCbnSelchangeLocal();
  void InitClientView();
protected:
  afx_msg LRESULT OnMyExplorerDriveData(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnMyExplorerFileList(WPARAM wParam, LPARAM lParam);
public:
  afx_msg void OnNMDblclkFilesClient(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMRClickFilesLocal(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMRClickFilesClient(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDownload();
  afx_msg void OnUpload();
protected:
  afx_msg LRESULT OnMydownload(WPARAM wParam, LPARAM lParam);
public:
  afx_msg void OnCbnSelchangeClient();
protected:
  afx_msg LRESULT OnMyUpload(WPARAM wParam, LPARAM lParam);
};
