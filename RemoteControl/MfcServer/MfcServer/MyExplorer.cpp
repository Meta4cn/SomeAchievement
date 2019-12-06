// MyExplorer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MfcServer.h"
#include "MyExplorer.h"
#include "afxdialogex.h"

#define MYDIRECTORY '0'
#define MYFILE      '1'

CString g_strTmpDownload;

// CMyExplorer �Ի���

IMPLEMENT_DYNAMIC(CMyExplorer, CDialogEx)

CMyExplorer::CMyExplorer(SOCKET sClient, CWnd* pParent /*=NULL*/)
: CDialogEx(CMyExplorer::IDD, pParent)
{
	m_sClient = sClient;
}

CMyExplorer::~CMyExplorer()
{
}

void CMyExplorer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, CMB_LOCAL, m_cmbLocalExplorer);
	DDX_Control(pDX, CMB_CLIENT, m_cmbRemoteExplorer);
	DDX_Control(pDX, LST_FILES_LOCAL, m_lstLocal);
	DDX_Control(pDX, LST_FILES_CLIENT, m_lstClient);
}


BEGIN_MESSAGE_MAP(CMyExplorer, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMyExplorer::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, LST_FILES_LOCAL, &CMyExplorer::OnNMDblclkFilesLocal)
	ON_CBN_SELCHANGE(CMB_LOCAL, &CMyExplorer::OnCbnSelchangeLocal)
	ON_MESSAGE(WM_MYEXPLORERDRIVEDATA, &CMyExplorer::OnMyExplorerDriveData)
	ON_MESSAGE(WM_MYEXPLORERFILELIST, &CMyExplorer::OnMyExplorerFileList)
	ON_NOTIFY(NM_DBLCLK, LST_FILES_CLIENT, &CMyExplorer::OnNMDblclkFilesClient)
	ON_NOTIFY(NM_RCLICK, LST_FILES_LOCAL, &CMyExplorer::OnNMRClickFilesLocal)
	ON_NOTIFY(NM_RCLICK, LST_FILES_CLIENT, &CMyExplorer::OnNMRClickFilesClient)
	ON_COMMAND(IDM_DOWNLOAD, &CMyExplorer::OnDownload)
	ON_COMMAND(IDM_UPLOAD, &CMyExplorer::OnUpload)
	ON_MESSAGE(WM_MYDOWNLOAD, &CMyExplorer::OnMydownload)
	ON_CBN_SELCHANGE(CMB_CLIENT, &CMyExplorer::OnCbnSelchangeClient)
	ON_MESSAGE(WM_MYUPLOAD, &CMyExplorer::OnMyUpload)
END_MESSAGE_MAP()


// CMyExplorer ��Ϣ�������


void CMyExplorer::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
}


BOOL CMyExplorer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitLocalView();	//���ؽ���
	InitClientView();   //�û�����

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

//
void CMyExplorer::InitLocalView()
{
	// ö�������̷�
	ListLogicalDrive(m_aryLogicDrive);
	for (int i = 0; i < m_aryLogicDrive.GetCount(); i++)
	{
		m_cmbLocalExplorer.InsertString(i, m_aryLogicDrive[i].GetString());
	}

	m_cmbLocalExplorer.SetCurSel(0);

	CString strDrive;
	int nIndex = m_cmbLocalExplorer.GetCurSel();
	m_cmbLocalExplorer.GetLBText(nIndex, strDrive);

	// listctrl ������
	m_lstLocal.InsertColumn(0, "  �ļ�����  ");
	m_lstLocal.InsertColumn(1, "  �ļ�����  ");

	m_lstLocal.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_lstLocal.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	//����List���
	m_lstLocal.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_lstLocal.GetExtendedStyle());

	// listctrl �ͻ���
	m_lstClient.InsertColumn(0, "  �ļ�����  ");
	m_lstClient.InsertColumn(1, "  �ļ�����  ");

	m_lstClient.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_lstClient.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	//����List���
	m_lstClient.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_lstClient.GetExtendedStyle());

	UpdateData(FALSE);



#ifdef _DEBUG
	m_ServerCurPath = "E:\\";
	MyFindFile(m_ServerCurPath);
#endif

	return;
}

// ö�ٴ���
void CMyExplorer::ListLogicalDrive(CStringArray& aryLogicDrive)
{
	TCHAR buf[100];
	DWORD len = GetLogicalDriveStrings(sizeof(buf) / sizeof(TCHAR), buf);
	TCHAR *s = buf;
	UINT nDriveType(0);
	CString strDisks;

	for (; *s; s += _tcslen(s) + 1)
	{
		strDisks = s; //�����̷� 
		nDriveType = GetDriveType(strDisks);
		switch (nDriveType)
		{
		case DRIVE_FIXED:
			OutputDebugStringA(strDisks);
			aryLogicDrive.Add(strDisks);
		}
	}

	return;
}

// ����Ŀ¼�µ��ļ����ļ���
void CMyExplorer::MyFindFile(CString& strFilePath)
{
	CFileFind findFile;
	CString Dir = strFilePath + "\\*.*";

	BOOL res = findFile.FindFile(Dir);
	if (!res)
	{
		return;
	}

	while (res)
	{
		CString strFilename;
		CString strTmp;
		res = findFile.FindNextFile();
		//if (findPath.IsDirectory()/* && !strFind.IsDots()*/) //Ŀ¼���ļ��� 
		{
			strFilename = findFile.GetFileName();
			strTmp = Dir.Left(Dir.GetLength() - 3) + strFilename;

			//ִ�к������� 
			OutputDebugStringA(strFilename);
			int nIndex = m_lstLocal.InsertItem(m_lstLocal.GetItemCount(), strFilename);
			if (findFile.IsDirectory())
			{
				m_lstLocal.SetItemText(nIndex, 1, "�ļ���");
			}
			else
			{
				m_lstLocal.SetItemText(nIndex, 1, "�ļ�");
				continue;
			}
			continue;
		}
	}
}

// ������Դ�����˫���¼�
void CMyExplorer::OnNMDblclkFilesLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//��ȡ��ǰ��ѡ����
	int nIndex = m_lstLocal.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	TRACE("%d", nIndex);

	CString strFileType = m_lstLocal.GetItemText(nIndex, 1);
	if (strFileType.Compare("�ļ�") == 0)
	{
		// ���Ҳ��༭�ļ�����
		return;
	}

	// ��ȡѡ������
	CString strDirName = m_lstLocal.GetItemText(nIndex, 0);

	// ��ձ�����Դ����������
	m_lstLocal.DeleteAllItems();

	// ����
	if ('\\' != m_ServerCurPath.GetAt(m_ServerCurPath.GetLength() - 1))
	{
		m_ServerCurPath.Append("\\");
	}
	m_ServerCurPath.Append(strDirName.GetString());

	MyFindFile(m_ServerCurPath);

	*pResult = 0;
}

// ��Ӧ�����˵�ѡ��ı�
void CMyExplorer::OnCbnSelchangeLocal()
{
	m_lstLocal.DeleteAllItems();
	int nIndex = m_cmbLocalExplorer.GetCurSel();
	m_cmbLocalExplorer.GetLBText(nIndex, m_ServerCurPath);

	MyFindFile(m_ServerCurPath);
}

// ��ͻ��˷��ʹ�����Ϣ����
void CMyExplorer::InitClientView()
{
	SendPacket(m_sClient, FILE_LOGICDRIVE_DATA_REQ);
}

// ��Ӧ��Ϣ��ʾ-�ͻ���-�Ĵ�����Ϣ
afx_msg LRESULT CMyExplorer::OnMyExplorerDriveData(WPARAM wParam, LPARAM lParam)
{
	char* pData = reinterpret_cast<char*>(wParam);

	CString strTmp(pData);
	CStringArray aryDrives;

	while (!strTmp.IsEmpty())
	{
		CString strSingleDrive = strTmp.Left(strTmp.Find("||"));
		aryDrives.Add(strSingleDrive);
		strTmp.Delete(0, strTmp.Find("||") + 2);
	}

	for (int i = 0; i < aryDrives.GetCount(); i++)
	{
		m_cmbRemoteExplorer.InsertString(0, aryDrives[i].GetString());
	}

	// �տ�ʼѡ���һ��
	m_cmbRemoteExplorer.SetCurSel(0);

	// ������Ϣ���ͻ��ˣ������ļ����ļ�����
	m_ClientCurPath.Format("%s", aryDrives[aryDrives.GetCount() - 1]);
	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetAllocLength());

	return 0;
}

// ��Ӧ��Ϣ��ʾ-�ͻ���-���ļ���Ϣ
afx_msg LRESULT CMyExplorer::OnMyExplorerFileList(WPARAM wParam, LPARAM lParam)
{
	char* pData = reinterpret_cast<char*>(wParam);
	CString strTmp(pData);

	m_lstClient.DeleteAllItems();

	while (!strTmp.IsEmpty())
	{
		CString strSingleFile = strTmp.Left(strTmp.Find("||"));
		int nIndex = 0;
		char chTmp = strSingleFile.GetAt(strSingleFile.GetLength() - 1);
		// �������
		if (chTmp == MYDIRECTORY)
		{
			CString strDirName = strSingleFile.Left(strSingleFile.GetLength() - strlen("--0"));
			nIndex = m_lstClient.InsertItem(0, strDirName);
			m_lstClient.SetItemText(nIndex, 1, "�ļ���");
		}
		else if (chTmp == MYFILE)
		{
			CString strFileName = strSingleFile.Left(strSingleFile.GetLength() - strlen("--1"));
			nIndex = m_lstClient.InsertItem(0, strFileName);
			m_lstClient.SetItemText(nIndex, 1, "�ļ�");
		}
		strTmp.Delete(0, strTmp.Find("||") + 2);

		continue;
	}

	return 0;
}

// ��Ӧ-�ͻ���-�б�ؼ�˫����Ϣ
void CMyExplorer::OnNMDblclkFilesClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//��ȡ��ǰ��ѡ����
	int nIndex = m_lstClient.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	TRACE("%d", nIndex);

	CString strFileType = m_lstClient.GetItemText(nIndex, 1);
	if (strFileType.Compare("�ļ�") == 0)
	{
		// ���Ҳ��༭�ļ�����
		return;
	}

	// ��ȡѡ������
	CString strFileName = m_lstClient.GetItemText(nIndex, 0);

	// ��ձ�����Դ����������
	m_lstClient.DeleteAllItems();

	// ����
	//m_ClientCurPath.Append("\\");
	if ('\\' != m_ClientCurPath.GetAt(m_ClientCurPath.GetLength() - 1))
	{
		m_ClientCurPath.Append("\\");
	}
	m_ClientCurPath.Append(strFileName.GetString());

	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetLength());

	*pResult = 0;
}

// ��Ӧ-�����-�б�ؼ��Ҽ�������Ϣ
void CMyExplorer::OnNMRClickFilesLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//�����Ҽ��˵�
	CMenu menu;
	menu.LoadMenu(IDR_MENU4);
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

// ��Ӧ-�ͻ���-�б�ؼ��Ҽ�������Ϣ
void CMyExplorer::OnNMRClickFilesClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//�����Ҽ��˵�
	CMenu menu;
	menu.LoadMenu(IDR_MENU5);
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

// ��Ӧ�ͻ��������˵��л�
void CMyExplorer::OnCbnSelchangeClient()
{
	m_ClientCurPath.Empty();
	int nIndex = m_cmbRemoteExplorer.GetCurSel();
	CString strIp;
	m_cmbRemoteExplorer.GetLBText(nIndex, m_ClientCurPath);
	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetLength());
}

// �����ļ����ͻ���
void CMyExplorer::OnDownload()
{
	// �ȷ��ļ������ȿͻ��˴������ļ��󣬷�����
	//��ȡ��ǰ��ѡ����
	int nIndex = m_lstLocal.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	CString strType = m_lstLocal.GetItemText(nIndex, 1);
	if (0 == strType.CompareNoCase("�ļ���"))
	{
		AfxMessageBox("�ݲ�֧�������ļ���");
		return;
	}

	CString strDLFileName = m_lstLocal.GetItemText(nIndex, 0);

	strDLFileName = m_ClientCurPath + strDLFileName;

	// ֪ͨ�ͻ���׼�������ļ�-1
	SendPacket(m_sClient, FILE_DOWNLOAD_REQ, (PCHAR)strDLFileName.GetString(), strDLFileName.GetLength());
}

// �����̻߳ص�����
unsigned int __stdcall DownLoadProc(void* param)
{
	CMyExplorer* pThis = reinterpret_cast<CMyExplorer*>(param);

	int nIndex = pThis->m_lstLocal.GetSelectionMark();
	CString strFileName = pThis->m_lstLocal.GetItemText(nIndex, 0);
	strFileName = pThis->m_ServerCurPath + strFileName; // ���ص�Դ�ļ�ȫ·��

	CFile fileDownLoad;
	BOOL bRet = fileDownLoad.Open(strFileName, CFile::modeReadWrite /*| CFile::modeNoTruncate*/);
	if (!bRet)
	{
		AfxMessageBox("�򿪱����ļ�ʧ�ܣ�");
		return 0;
	}

	ULONGLONG uFileContent = fileDownLoad.GetLength();
	char* buf = g_strTmpDownload.GetBuffer(uFileContent + 1);

	ZeroMemory(buf, uFileContent + 1);

	fileDownLoad.Read(buf, uFileContent);

	fileDownLoad.Close();

	SendPacket(pThis->m_sClient, FILE_DOWNLOAD_DATA_RLY, buf, uFileContent + 1);

	g_strTmpDownload.ReleaseBuffer(-1);

	return 0;
}

// �ѿͻ����ļ��ϴ���������
void CMyExplorer::OnUpload()
{
	int nIndex = m_lstClient.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	CString strType = m_lstClient.GetItemText(nIndex, 1);
	if (0 == strType.CompareNoCase("�ļ���"))
	{
		AfxMessageBox("�ݲ�֧���ļ���");
		return;
	}

	// Ҫ�ϴ����ļ���
	CString strULFilePath = m_lstClient.GetItemText(nIndex, 0);

	// �ͻ��˵�ԭ�ļ�ȫ·��
	m_CurDownloadClientPath = m_ClientCurPath + strULFilePath;
	// ����˵�Ŀ���ļ�ȫ·��
	m_CurDownloadServerPath = m_ServerCurPath + strULFilePath;

	// �ڷ��������Ŀ���ļ�
	HANDLE hFile = CreateFile(m_CurDownloadServerPath.GetString(),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_ARCHIVE,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		if (ERROR_ALREADY_EXISTS == GetLastError() | ERROR_FILE_EXISTS == GetLastError())
		{
			// ����Ѵ��ھ�ɾ��
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			DeleteFile(m_CurDownloadServerPath.GetString());

			hFile = CreateFile(m_CurDownloadServerPath.GetString(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_ARCHIVE,
				NULL);

		}
	}

	// ��ͻ���������ļ�������
	SendPacket(m_sClient, FILE_UPLOAD_REQ,
		(PCHAR)m_CurDownloadClientPath.GetString(), m_CurDownloadClientPath.GetLength());

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
}

// ���տͻ��˷��������ػظ�����ʼ����
afx_msg LRESULT CMyExplorer::OnMydownload(WPARAM wParam, LPARAM lParam)
{
	// ������������
	_beginthreadex(0, 0, DownLoadProc, this, 0, 0);
	return 0;
}

// ����ͻ����ϴ������ļ�����
afx_msg LRESULT CMyExplorer::OnMyUpload(WPARAM wParam, LPARAM lParam)
{
	PCHAR pszBuf = reinterpret_cast<PCHAR>(wParam);
	DWORD dwSize = lParam;

	CFile fileUpLoad;
	BOOL bRet = fileUpLoad.Open(m_CurDownloadServerPath.GetString(), CFile::modeReadWrite);
	if (!bRet)
	{
		return 0;
	}

	fileUpLoad.Write(pszBuf, dwSize);

	fileUpLoad.Close();

	return 0;
}
