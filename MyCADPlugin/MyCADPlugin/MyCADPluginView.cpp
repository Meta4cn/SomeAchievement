
// MyCADPluginView.cpp : CMyCADPluginView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "MyCADPlugin.h"
#endif

#include "MyCADPluginDoc.h"
#include "MyCADPluginView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyCADPluginView

IMPLEMENT_DYNCREATE(CMyCADPluginView, CView)

BEGIN_MESSAGE_MAP(CMyCADPluginView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(PLUGIN_MENUID_START,PLUGIN_MENUID_RANGE+PLUGIN_MENUID_START,OnPlubinCommand)
END_MESSAGE_MAP()

// CMyCADPluginView ����/����

CMyCADPluginView::CMyCADPluginView()
	:m_bAdd(FALSE),m_bSelect(FALSE),m_bIsMove(FALSE),m_pCurShape(NULL),m_pCurFactory(NULL),m_pSelectShape(NULL)
{
	// TODO: �ڴ˴���ӹ������
	m_logpen.lopnStyle = PS_SOLID;
	m_logpen.lopnColor = RGB(0,0,0);
	m_logpen.lopnWidth.x = 1;

	m_logbrush.lbStyle = BS_NULL;
	m_logbrush.lbColor = RGB(0,0,0);
	m_selectPt.x = -1;
	m_selectPt.y = -1;
}

CMyCADPluginView::~CMyCADPluginView()
{
}

BOOL CMyCADPluginView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CMyCADPluginView ����

void CMyCADPluginView::OnDraw(CDC* pDC)
{
	CMyCADPluginDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CRect rcClient;
	GetClientRect(&rcClient);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(pDC,rcClient.Width(),rcClient.Height());
	dcMem.SelectObject(&bmpMem);
	dcMem.FillSolidRect(rcClient,RGB(255,255,255));
	// ������ŵ�
	POSITION pos = m_lstShapes.GetHeadPosition();
	while (pos)
	{
		IShape* pShape = m_lstShapes.GetNext(pos);
		if (pShape == m_pSelectShape)
		{
			pShape->onDrawSelect(&dcMem);
		}
		else
		{
			pShape->OnDraw(&dcMem);
		}
	}

	// ����ǰ

	if (m_pCurShape)
	{
		m_pCurShape->OnDraw(&dcMem);
	}

	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),
		&dcMem,0,0,SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(dcMem);
}


// CMyCADPluginView ��ӡ

BOOL CMyCADPluginView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CMyCADPluginView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CMyCADPluginView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CMyCADPluginView ���

#ifdef _DEBUG
void CMyCADPluginView::AssertValid() const
{
	CView::AssertValid();
}

void CMyCADPluginView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyCADPluginDoc* CMyCADPluginView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyCADPluginDoc)));
	return (CMyCADPluginDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyCADPluginView ��Ϣ�������

// CCADPluginView ��Ϣ�������
void CMyCADPluginView::OnPlubinCommand(UINT nID)
{
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_pCurFactory = pMainFrame->m_PluginList[nID];
}


void CMyCADPluginView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_pCurFactory)
	{
		m_pCurShape = m_pCurFactory->CreateNew();
		m_pCurShape->SetPenBrush(m_logpen,m_logbrush);
		m_pCurShape->OnLButtonDown(nFlags,point);
		SetCapture();
		m_bAdd = TRUE;
	} else
	{
		m_bAdd = FALSE;
	}
	
	CView::OnLButtonDown(nFlags, point);
}


void CMyCADPluginView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bAdd)
	{
		m_pCurShape->OnLButtonUp(nFlags,point);
		m_lstShapes.AddTail(m_pCurShape);

		m_pCurShape = NULL;
		InvalidateRect(NULL,FALSE);
		ReleaseCapture();
		m_bAdd = FALSE;
	}
	m_bIsMove = FALSE; //ֹͣ�ƶ�
	CView::OnLButtonUp(nFlags, point);
}


void CMyCADPluginView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bAdd && m_pCurShape != NULL)
	{
		m_pCurShape->OnMouseMove(nFlags,point);
		InvalidateRect(NULL,FALSE);
	}
	CView::OnMouseMove(nFlags, point);
}
