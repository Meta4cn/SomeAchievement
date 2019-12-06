// Process.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\..\common\common.h"
#include "MfcServer.h"
#include "Process.h"
#include "afxdialogex.h"
#include "MfcServerDlg.h"

// CProcess �Ի���

IMPLEMENT_DYNAMIC(CProcess, CDialogEx)

CProcess::CProcess(SOCKET sClient, CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcess::IDD, pParent),
  m_sClient(sClient)
{
}

CProcess::~CProcess()
{
}

void CProcess::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, LST_PROC, m_lst);
}


BEGIN_MESSAGE_MAP(CProcess, CDialogEx)
  ON_BN_CLICKED(IDOK, &CProcess::OnBnClickedOk)
  ON_NOTIFY(NM_RCLICK, LST_PROC, &CProcess::OnNMRClickProc)
  ON_COMMAND(ID_CLSPRC, &CProcess::OnClsprc)
  ON_MESSAGE(WM_MYCLSPROCSCS, &CProcess::OnMyclsprocscs)
  ON_MESSAGE(WM_MYCLSPROCFAIL, &CProcess::OnMyclsprocfail)
END_MESSAGE_MAP()


// CProcess ��Ϣ�������


int CProcess::Mysetprocdata(WPARAM wParam, LPARAM lParam/*, CListCtrl& lst*/)
{
  m_lst.DeleteAllItems();

 char* pszData = reinterpret_cast<char*>(wParam);
 TagClientContext* pContext = reinterpret_cast<TagClientContext*>(lParam);

 CString strAllData;
 strAllData.Format("%s", pszData);

 // ��ʽ������ID0��������0|����ID1��������1|... 
 // �����Ϣ������������ʾ���ؼ���
 
 char delims[] = "|";
 char *result = NULL;
// strcpy(szBuf, pszData); // ���Bug������

 result = strtok(pszData, delims);
 while (result != NULL) 
 {
   CStringA strTmp;
   strTmp.Format("%s", result);
   int nFind = strTmp.Find(":");
   CStringA strProcName = strTmp.Left(nFind);
   CStringA strProcId = strTmp.Right(strTmp.GetLength() - nFind - 1);

   InsertList(strProcName, strProcId);

   result = strtok(NULL, delims);
 }
 return 0;
}

void CProcess::InsertList(CString strProcName, CString strProcId)
{
  CListCtrl* pLst = (CListCtrl*)GetDlgItem(LST_PROC);

  int nRet = pLst->InsertItem(0, strProcId.GetString());
  pLst->SetItemText(nRet, 1, strProcName.GetString());
}

void CProcess::OnBnClickedOk()
{}


BOOL CProcess::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  �ڴ���Ӷ���ĳ�ʼ��
  m_lst.InsertColumn(0, "����ID");
  m_lst.InsertColumn(1, "������");

  m_lst.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  m_lst.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

  //����List���
  m_lst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_lst.GetExtendedStyle());

  return TRUE;  // return TRUE unless you set the focus to a control
  // �쳣:  OCX ����ҳӦ���� FALSE
}


LRESULT CProcess::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (message == WM_MYSETPROCDATA)
  {
    Mysetprocdata(wParam, lParam);
  }    

  return CDialogEx::WindowProc(message, wParam, lParam);
}


void CProcess::OnNMRClickProc(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  //�����Ҽ��˵�
  CMenu menu;
  menu.LoadMenu(IDR_MENU2);
  CMenu* pSubMenu = menu.GetSubMenu(0);

  POINT pt;
  GetCursorPos(&pt); 
  
  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,
    pt.x,
    pt.y,
    this
    );

  *pResult = 0;
}


void CProcess::OnClsprc()
{
  CStringA strProcId;

  //��ȡ��ǰ��ѡ����
  int nIndex = m_lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  strProcId = m_lst.GetItemText(nIndex, 0);
  
  SendPacket(m_sClient, PROCESS_CLS, (char*)strProcId.GetString(), strProcId.GetLength());
}



afx_msg LRESULT CProcess::OnMyclsprocscs(WPARAM wParam, LPARAM lParam)
{
  CStringA strTmp;
  CStringA strProcId;
  strTmp.Format("%s", lParam);

  for (int i = 0; i < m_lst.GetItemCount();i++)
  {
    strProcId = m_lst.GetItemText(i, 0);
    if (0 == strProcId.Compare(strTmp))
    {
      m_lst.DeleteItem(i);
      break; 
    }
  }
  
  return 0;
}


afx_msg LRESULT CProcess::OnMyclsprocfail(WPARAM wParam, LPARAM lParam)
{
  AfxMessageBox("�����ͻ�������ʧ�ܣ�");
  return 0;
}
