// ScreenDlg.cpp : ʵ���ļ�
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

// CScreenDlg �Ի���

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


// CScreenDlg ��Ϣ�������

// ���˻س���
void CScreenDlg::OnBnClickedOk()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  //CDialogEx::OnOK();
}


afx_msg LRESULT CScreenDlg::OnMyScreenData(WPARAM wParam, LPARAM lParam)
{
  if (wParam == NULL || lParam <= 8)
  {
    return 0;
  }

  m_strBuf.ReleaseBuffer();

  //�����͹�������Ļ����
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

//��Ļ��ͼ
void CScreenDlg::OnScrnSht()
{
  BOOL isOpen = FALSE;        //�Ƿ��(����Ϊ����)  
  CString defaultDir = "D:\\Program Files";   //Ĭ�ϴ򿪵��ļ�·��  
  CString fileName = "1.bmp";         //Ĭ�ϴ򿪵��ļ���  
  CString filter = "BMP�ļ� (*.bmp)|*.bmp||";   //�ļ����ǵ�����  
  CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
  openFileDlg.GetOFN().lpstrInitialDir = "D:\\Program Files\\";
  INT_PTR result = openFileDlg.DoModal();
  CString filePath = defaultDir + "\\" + fileName;
  if (result == IDOK) {
    filePath = openFileDlg.GetPathName();
  }
  
  // ��ȡbitmap���

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
  //�����Ҽ��˵�
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

//�����ͼ
BOOL CScreenDlg::SaveBitmapToFile(HBITMAP hBitmap, CString& szfilename)
{
  HDC     hDC;
  //��ǰ�ֱ�����ÿ������ռ�ֽ���          
  int     iBits;
  //λͼ��ÿ������ռ�ֽ���          
  WORD     wBitCount;
  //�����ɫ���С��     
  DWORD     dwPaletteSize = 0;
  // λͼ�������ֽڴ�С 
  DWORD     dwBmBitsSize = 0;
  // λͼ�ļ���С
  DWORD     dwDIBSize = 0;
  //д���ļ��ֽ���  
  DWORD     dwWritten = 0;
  //λͼ���Խṹ              
  BITMAP     Bitmap;
  //λͼ�ļ�ͷ�ṹ          
  BITMAPFILEHEADER     bmfHdr;
  //λͼ��Ϣͷ�ṹ              
  BITMAPINFOHEADER     bi;
  //ָ��λͼ��Ϣͷ�ṹ                  
  LPBITMAPINFOHEADER     lpbi;
  //�����ļ��������ڴ�������ɫ����              
  HANDLE     fh, hDib, hPal, hOldPal = NULL;

  //����λͼ�ļ�ÿ��������ռ�ֽ���              
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

  // Ϊλͼ���ݷ����ڴ�              
  hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
  *lpbi = bi;

  // �����ɫ��                  
  hPal = GetStockObject(DEFAULT_PALETTE);
  if (hPal)
  {
    hDC = ::GetDC(NULL);
    hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
    RealizePalette(hDC);
  }

  // ��ȡ�õ�ɫ�����µ�����ֵ              
  GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
    (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
    (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

  // �ָ���ɫ��                  
  if (hOldPal)
  {
    ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
    RealizePalette(hDC);
    ::ReleaseDC(NULL, hDC);
  }

  // ����λͼ�ļ�                  
  fh = CreateFile(szfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

  // ����λͼ�ļ�ͷ              
  bmfHdr.bfType = 0x4D42;     // "BM"              
  dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
  bmfHdr.bfSize = dwDIBSize;
  bmfHdr.bfReserved1 = 0;
  bmfHdr.bfReserved2 = 0;
  bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

  BOOL bRet = FALSE;
  // д��λͼ�ļ�ͷ              
  bRet = WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
  if (!bRet)
  {
    // ���                  
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);

  }

  // д��λͼ�ļ���������              
  bRet = WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
  if (!bRet)
  {
    // ���                  
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
  }

  return TRUE;
}