// MyExplorer.cpp : 实现文件
//

#include "stdafx.h"
#include "MfcServer.h"
#include "MyExplorer.h"
#include "afxdialogex.h"

#define MYDIRECTORY '0'
#define MYFILE      '1'

CString g_strTmpDownload;

// CMyExplorer 对话框

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


// CMyExplorer 消息处理程序


void CMyExplorer::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}


BOOL CMyExplorer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitLocalView();	//本地界面
	InitClientView();   //用户界面

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//
void CMyExplorer::InitLocalView()
{
	// 枚举所有盘符
	ListLogicalDrive(m_aryLogicDrive);
	for (int i = 0; i < m_aryLogicDrive.GetCount(); i++)
	{
		m_cmbLocalExplorer.InsertString(i, m_aryLogicDrive[i].GetString());
	}

	m_cmbLocalExplorer.SetCurSel(0);

	CString strDrive;
	int nIndex = m_cmbLocalExplorer.GetCurSel();
	m_cmbLocalExplorer.GetLBText(nIndex, strDrive);

	// listctrl 服务器
	m_lstLocal.InsertColumn(0, "  文件名称  ");
	m_lstLocal.InsertColumn(1, "  文件类型  ");

	m_lstLocal.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_lstLocal.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	//设置List风格
	m_lstLocal.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_lstLocal.GetExtendedStyle());

	// listctrl 客户端
	m_lstClient.InsertColumn(0, "  文件名称  ");
	m_lstClient.InsertColumn(1, "  文件类型  ");

	m_lstClient.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_lstClient.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	//设置List风格
	m_lstClient.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_lstClient.GetExtendedStyle());

	UpdateData(FALSE);



#ifdef _DEBUG
	m_ServerCurPath = "E:\\";
	MyFindFile(m_ServerCurPath);
#endif

	return;
}

// 枚举磁盘
void CMyExplorer::ListLogicalDrive(CStringArray& aryLogicDrive)
{
	TCHAR buf[100];
	DWORD len = GetLogicalDriveStrings(sizeof(buf) / sizeof(TCHAR), buf);
	TCHAR *s = buf;
	UINT nDriveType(0);
	CString strDisks;

	for (; *s; s += _tcslen(s) + 1)
	{
		strDisks = s; //单个盘符 
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

// 遍历目录下的文件和文件夹
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
		//if (findPath.IsDirectory()/* && !strFind.IsDots()*/) //目录是文件夹 
		{
			strFilename = findFile.GetFileName();
			strTmp = Dir.Left(Dir.GetLength() - 3) + strFilename;

			//执行后续操作 
			OutputDebugStringA(strFilename);
			int nIndex = m_lstLocal.InsertItem(m_lstLocal.GetItemCount(), strFilename);
			if (findFile.IsDirectory())
			{
				m_lstLocal.SetItemText(nIndex, 1, "文件夹");
			}
			else
			{
				m_lstLocal.SetItemText(nIndex, 1, "文件");
				continue;
			}
			continue;
		}
	}
}

// 本地资源浏览器双击事件
void CMyExplorer::OnNMDblclkFilesLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//获取当前的选中行
	int nIndex = m_lstLocal.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	TRACE("%d", nIndex);

	CString strFileType = m_lstLocal.GetItemText(nIndex, 1);
	if (strFileType.Compare("文件") == 0)
	{
		// 暂且不编辑文件内容
		return;
	}

	// 获取选中内容
	CString strDirName = m_lstLocal.GetItemText(nIndex, 0);

	// 清空本地资源管理器内容
	m_lstLocal.DeleteAllItems();

	// 更新
	if ('\\' != m_ServerCurPath.GetAt(m_ServerCurPath.GetLength() - 1))
	{
		m_ServerCurPath.Append("\\");
	}
	m_ServerCurPath.Append(strDirName.GetString());

	MyFindFile(m_ServerCurPath);

	*pResult = 0;
}

// 响应下拉菜单选项改变
void CMyExplorer::OnCbnSelchangeLocal()
{
	m_lstLocal.DeleteAllItems();
	int nIndex = m_cmbLocalExplorer.GetCurSel();
	m_cmbLocalExplorer.GetLBText(nIndex, m_ServerCurPath);

	MyFindFile(m_ServerCurPath);
}

// 向客户端发送磁盘信息请求
void CMyExplorer::InitClientView()
{
	SendPacket(m_sClient, FILE_LOGICDRIVE_DATA_REQ);
}

// 响应消息显示-客户端-的磁盘信息
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

	// 刚开始选择第一项
	m_cmbRemoteExplorer.SetCurSel(0);

	// 发送信息给客户端，请求文件夹文件数据
	m_ClientCurPath.Format("%s", aryDrives[aryDrives.GetCount() - 1]);
	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetAllocLength());

	return 0;
}

// 响应消息显示-客户端-的文件信息
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
		// 插入队列
		if (chTmp == MYDIRECTORY)
		{
			CString strDirName = strSingleFile.Left(strSingleFile.GetLength() - strlen("--0"));
			nIndex = m_lstClient.InsertItem(0, strDirName);
			m_lstClient.SetItemText(nIndex, 1, "文件夹");
		}
		else if (chTmp == MYFILE)
		{
			CString strFileName = strSingleFile.Left(strSingleFile.GetLength() - strlen("--1"));
			nIndex = m_lstClient.InsertItem(0, strFileName);
			m_lstClient.SetItemText(nIndex, 1, "文件");
		}
		strTmp.Delete(0, strTmp.Find("||") + 2);

		continue;
	}

	return 0;
}

// 响应-客户端-列表控件双击消息
void CMyExplorer::OnNMDblclkFilesClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//获取当前的选中行
	int nIndex = m_lstClient.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	TRACE("%d", nIndex);

	CString strFileType = m_lstClient.GetItemText(nIndex, 1);
	if (strFileType.Compare("文件") == 0)
	{
		// 暂且不编辑文件内容
		return;
	}

	// 获取选中内容
	CString strFileName = m_lstClient.GetItemText(nIndex, 0);

	// 清空本地资源管理器内容
	m_lstClient.DeleteAllItems();

	// 更新
	//m_ClientCurPath.Append("\\");
	if ('\\' != m_ClientCurPath.GetAt(m_ClientCurPath.GetLength() - 1))
	{
		m_ClientCurPath.Append("\\");
	}
	m_ClientCurPath.Append(strFileName.GetString());

	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetLength());

	*pResult = 0;
}

// 响应-服务端-列表控件右键单击消息
void CMyExplorer::OnNMRClickFilesLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//弹出右键菜单
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

// 响应-客户端-列表控件右键单击消息
void CMyExplorer::OnNMRClickFilesClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//弹出右键菜单
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

// 响应客户端下拉菜单切换
void CMyExplorer::OnCbnSelchangeClient()
{
	m_ClientCurPath.Empty();
	int nIndex = m_cmbRemoteExplorer.GetCurSel();
	CString strIp;
	m_cmbRemoteExplorer.GetLBText(nIndex, m_ClientCurPath);
	SendPacket(m_sClient, FILE_LIST_DATA_REQ, (PCHAR)m_ClientCurPath.GetString(), m_ClientCurPath.GetLength());
}

// 下载文件到客户端
void CMyExplorer::OnDownload()
{
	// 先发文件名，等客户端创建新文件后，发内容
	//获取当前的选中行
	int nIndex = m_lstLocal.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	CString strType = m_lstLocal.GetItemText(nIndex, 1);
	if (0 == strType.CompareNoCase("文件夹"))
	{
		AfxMessageBox("暂不支持下载文件夹");
		return;
	}

	CString strDLFileName = m_lstLocal.GetItemText(nIndex, 0);

	strDLFileName = m_ClientCurPath + strDLFileName;

	// 通知客户端准备下载文件-1
	SendPacket(m_sClient, FILE_DOWNLOAD_REQ, (PCHAR)strDLFileName.GetString(), strDLFileName.GetLength());
}

// 下载线程回调函数
unsigned int __stdcall DownLoadProc(void* param)
{
	CMyExplorer* pThis = reinterpret_cast<CMyExplorer*>(param);

	int nIndex = pThis->m_lstLocal.GetSelectionMark();
	CString strFileName = pThis->m_lstLocal.GetItemText(nIndex, 0);
	strFileName = pThis->m_ServerCurPath + strFileName; // 下载的源文件全路径

	CFile fileDownLoad;
	BOOL bRet = fileDownLoad.Open(strFileName, CFile::modeReadWrite /*| CFile::modeNoTruncate*/);
	if (!bRet)
	{
		AfxMessageBox("打开本地文件失败！");
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

// 把客户端文件上传到服务器
void CMyExplorer::OnUpload()
{
	int nIndex = m_lstClient.GetSelectionMark();
	if (nIndex == -1)
	{
		return;
	}

	CString strType = m_lstClient.GetItemText(nIndex, 1);
	if (0 == strType.CompareNoCase("文件夹"))
	{
		AfxMessageBox("暂不支持文件夹");
		return;
	}

	// 要上传的文件名
	CString strULFilePath = m_lstClient.GetItemText(nIndex, 0);

	// 客户端的原文件全路径
	m_CurDownloadClientPath = m_ClientCurPath + strULFilePath;
	// 服务端的目标文件全路径
	m_CurDownloadServerPath = m_ServerCurPath + strULFilePath;

	// 在服务端生成目标文件
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
			// 如果已存在就删除
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

	// 向客户端请求该文件的数据
	SendPacket(m_sClient, FILE_UPLOAD_REQ,
		(PCHAR)m_CurDownloadClientPath.GetString(), m_CurDownloadClientPath.GetLength());

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
}

// 接收客户端发来的下载回复，开始下载
afx_msg LRESULT CMyExplorer::OnMydownload(WPARAM wParam, LPARAM lParam)
{
	// 发送下载内容
	_beginthreadex(0, 0, DownLoadProc, this, 0, 0);
	return 0;
}

// 处理客户端上传来的文件内容
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
