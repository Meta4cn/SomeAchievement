// ScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MfcServer.h"
#include "ScreenDlg.h"
#include "afxdialogex.h"

#include<Commdlg.h>  

#include "../../common/common.h"

// #include "Cryption/zlib.h"
// #include "Cryption/base64.h" 
// 
// #pragma comment(lib, "zlib.lib")

// CScreenDlg 对话框

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

CScreenDlg::CScreenDlg(SOCKET sClient, CWnd* pParent /*=NULL*/)
: CDialogEx(CScreenDlg::IDD, pParent)
{
  m_sClient = sClient;
}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
  ON_BN_CLICKED(IDOK, &CScreenDlg::OnBnClickedOk)
  ON_MESSAGE(WM_MYSCREENDATA, &CScreenDlg::OnMyScreenData)
  ON_COMMAND(ID_SCRNSHT, &CScreenDlg::OnScrnSht)
  ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序

// 过滤回车键
void CScreenDlg::OnBnClickedOk()
{
  // TODO:  在此添加控件通知处理程序代码
  //CDialogEx::OnOK();
}


afx_msg LRESULT CScreenDlg::OnMyScreenData(WPARAM wParam, LPARAM lParam)
{
  if (wParam == NULL || lParam <= 8)
  {
    return 0;
  }

  m_strBuf.ReleaseBuffer();

  //处理发送过来的屏幕数据
  char* pData = (char*)wParam;
  int nDataLength = lParam; // original data size
  
  m_nBufLen = lParam;
  char* pFromCString = m_strBuf.GetBuffer(m_nBufLen);

  memcpy(pFromCString, pData, nDataLength);

  int nWidth = (int&)*pData;
  int nHeight = (int&)*(pData + 4);
  char* pBuf = pData + 8;

  CDC memDC;
  CBitmap bitmap;

  CDC* pDC = GetDC();
  if (NULL == pDC)
  {
    return 0;
  }

  BOOL bRet = memDC.CreateCompatibleDC(pDC);
  if (!bRet)
  {
    ReleaseDC(pDC);
    return 0;
  }

  bRet = bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
  if (!bRet)
  {
    memDC.DeleteDC();
    ReleaseDC(pDC);
    return 0;
  }

  int nBitSize = bitmap.SetBitmapBits(nDataLength - 8, pBuf);
  if (0 == nBitSize)
  {
    memDC.DeleteDC();
    ReleaseDC(pDC);
    return 0;
  }

  memDC.SelectObject(bitmap.GetSafeHandle());

  pDC->BitBlt(0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);

  ReleaseDC(pDC);

  UpdateWindow();

  return 0;
}

//屏幕截图
void CScreenDlg::OnScrnSht()
{
  BOOL isOpen = FALSE;        //是否打开(否则为保存)  
  CString defaultDir = "D:\\Program Files";   //默认打开的文件路径  
  CString fileName = "1.bmp";         //默认打开的文件名  
  CString filter = "BMP文件 (*.bmp)|*.bmp||";   //文件过虑的类型  
  CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
  openFileDlg.GetOFN().lpstrInitialDir = "D:\\Program Files\\";
  INT_PTR result = openFileDlg.DoModal();
  CString filePath = defaultDir + "\\" + fileName;
  if (result == IDOK) {
    filePath = openFileDlg.GetPathName();
  }
  
  // 获取bitmap句柄

  char* pData = (PCHAR)m_strBuf.GetString();
  int nDataLength = m_nBufLen; // original data size

  int nWidth = (int&)*pData;
  int nHeight = (int&)*(pData + 4);
  char* pBuf = pData + 8;

  CDC memDC;
  CBitmap bitmap;

  CDC* pDC = GetDC();
  if (NULL == pDC)
  {
    return ;
  }

  BOOL bRet = memDC.CreateCompatibleDC(pDC);
  if (!bRet)
  {
    ReleaseDC(pDC);
    return ;
  }

  bRet = bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
  if (!bRet)
  {
    memDC.DeleteDC();
    ReleaseDC(pDC);
    return ;
  }

  int nBitSize = bitmap.SetBitmapBits(nDataLength - 8, pBuf);
  if (0 == nBitSize)
  {
    memDC.DeleteDC();
    ReleaseDC(pDC);
    return ;
  }

  HBITMAP hBmp;
  hBmp = (HBITMAP)bitmap.GetSafeHandle();

  SaveBitmapToFile(hBmp, filePath);
}

void CScreenDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
  //弹出右键菜单
  CMenu menu;
  menu.LoadMenu(IDR_MENU3);
  CMenu* pSubMenu = menu.GetSubMenu(0);

  POINT pt;
  GetCursorPos(&pt);

  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,
    pt.x,
    pt.y,
    this
    );

  CDialogEx::OnRButtonUp(nFlags, point);
}

//保存截图
BOOL CScreenDlg::SaveBitmapToFile(HBITMAP hBitmap, CString& szfilename)
{
  HDC     hDC;
  //当前分辨率下每象素所占字节数          
  int     iBits;
  //位图中每象素所占字节数          
  WORD     wBitCount;
  //定义调色板大小，     
  DWORD     dwPaletteSize = 0;
  // 位图中像素字节大小 
  DWORD     dwBmBitsSize = 0;
  // 位图文件大小
  DWORD     dwDIBSize = 0;
  //写入文件字节数  
  DWORD     dwWritten = 0;
  //位图属性结构              
  BITMAP     Bitmap;
  //位图文件头结构          
  BITMAPFILEHEADER     bmfHdr;
  //位图信息头结构              
  BITMAPINFOHEADER     bi;
  //指向位图信息头结构                  
  LPBITMAPINFOHEADER     lpbi;
  //定义文件，分配内存句柄，调色板句柄              
  HANDLE     fh, hDib, hPal, hOldPal = NULL;

  //计算位图文件每个像素所占字节数              
  hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  iBits = GetDeviceCaps(hDC, BITSPIXEL)*GetDeviceCaps(hDC, PLANES);
  DeleteDC(hDC);

  if (iBits <= 1)
    wBitCount = 1;
  else  if (iBits <= 4)
    wBitCount = 4;
  else if (iBits <= 8)
    wBitCount = 8;
  else
    wBitCount = 24;

  GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = Bitmap.bmWidth;
  bi.biHeight = Bitmap.bmHeight;
  bi.biPlanes = 1;
  bi.biBitCount = wBitCount;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrImportant = 0;
  bi.biClrUsed = 0;

  dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

  // 为位图内容分配内存              
  hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
  *lpbi = bi;

  // 处理调色板                  
  hPal = GetStockObject(DEFAULT_PALETTE);
  if (hPal)
  {
    hDC = ::GetDC(NULL);
    hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
    RealizePalette(hDC);
  }

  // 获取该调色板下新的像素值              
  GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
    (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
    (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

  // 恢复调色板                  
  if (hOldPal)
  {
    ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
    RealizePalette(hDC);
    ::ReleaseDC(NULL, hDC);
  }

  // 创建位图文件                  
  fh = CreateFile(szfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

  // 设置位图文件头              
  bmfHdr.bfType = 0x4D42;     // "BM"              
  dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
  bmfHdr.bfSize = dwDIBSize;
  bmfHdr.bfReserved1 = 0;
  bmfHdr.bfReserved2 = 0;
  bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

  BOOL bRet = FALSE;
  // 写入位图文件头              
  bRet = WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
  if (!bRet)
  {
    // 清除                  
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);

  }

  // 写入位图文件其余内容              
  bRet = WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
  if (!bRet)
  {
    // 清除                  
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
  }

  return TRUE;
}