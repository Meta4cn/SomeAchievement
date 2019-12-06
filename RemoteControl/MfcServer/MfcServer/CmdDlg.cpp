// CmdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MfcServer.h"
#include "MfcServerDlg.h"
#include "CmdDlg.h"
#include "afxdialogex.h"


// CCmdDlg �Ի���

IMPLEMENT_DYNAMIC(CCmdDlg, CDialogEx)

CCmdDlg::CCmdDlg(SOCKET sClient, CWnd* pParent /*=NULL*/)
: CDialogEx(CCmdDlg::IDD, pParent)
{
	m_sClient = sClient;
}

CCmdDlg::~CCmdDlg()
{
}

void CCmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EdContent);
}


BEGIN_MESSAGE_MAP(CCmdDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCmdDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MYCMDDATA, &CCmdDlg::OnSetCmdData)
END_MESSAGE_MAP()


// CCmdDlg ��Ϣ�������


void CCmdDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
}

//д�����̨�ı�
LRESULT CCmdDlg::OnSetCmdData(WPARAM wParam, LPARAM lParam)
{
	CBuffer* pData = (CBuffer*)wParam;
	TagClientContext* pContext = (TagClientContext*)lParam;

	CStringA strTmp("");

	//strTmp.Format("%s", pData->GetBuffer());
	strTmp.Format("%s", pData);

	m_EdContent.GetWindowText(m_strOld);
	m_strOld += strTmp;
	m_EdContent.SetWindowText(m_strOld);

	m_EdContent.LineScroll(m_EdContent.GetLineCount() - 1, 0);

	return true;
}
