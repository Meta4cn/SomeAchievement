// MyEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MfcServer.h"
#include "MyEdit.h"
#include "MfcServerDlg.h"
#include "CmdDlg.h"
#include "../../common/common.h"

// CMyEdit
extern CMfcServerApp theApp;


IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
END_MESSAGE_MAP()



// CMyEdit ��Ϣ�������
BOOL CMyEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN)
	{
		//���û�����س������ͷ�������
		CCmdDlg* pDlg = reinterpret_cast<CCmdDlg*>(GetParent());
		CString strNew;
		((CEdit*)(pDlg->GetDlgItem(IDC_EDIT1)))->GetWindowText(strNew);

		strNew.Delete(0, pDlg->m_strOld.GetLength());

		strNew += '\n';

		CMfcServerDlg* pServerDlg = (CMfcServerDlg*)pDlg->GetParent();

		pServerDlg->m_IOCP.Send(pDlg->m_sClient, CMD_DATA, (BYTE*)strNew.GetBuffer(0), strNew.GetLength());

	}

	return CEdit::PreTranslateMessage(pMsg);
}
