
// MyCADPluginView.cpp : CMyCADPluginView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(PLUGIN_MENUID_START,PLUGIN_MENUID_RANGE+PLUGIN_MENUID_START,OnPlubinCommand)
END_MESSAGE_MAP()

// CMyCADPluginView 构造/析构

CMyCADPluginView::CMyCADPluginView()
	:m_bAdd(FALSE),m_bSelect(FALSE),m_bIsMove(FALSE),m_pCurShape(NULL),m_pCurFactory(NULL),m_pSelectShape(NULL)
{
	// TODO: 在此处添加构造代码
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMyCADPluginView 绘制

void CMyCADPluginView::OnDraw(CDC* pDC)
{
	CMyCADPluginDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CRect rcClient;
	GetClientRect(&rcClient);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(pDC,rcClient.Width(),rcClient.Height());
	dcMem.SelectObject(&bmpMem);
	dcMem.FillSolidRect(rcClient,RGB(255,255,255));
	// 画己存放的
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

	// 画当前

	if (m_pCurShape)
	{
		m_pCurShape->OnDraw(&dcMem);
	}

	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),
		&dcMem,0,0,SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(dcMem);
}


// CMyCADPluginView 打印

BOOL CMyCADPluginView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMyCADPluginView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMyCADPluginView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMyCADPluginView 诊断

#ifdef _DEBUG
void CMyCADPluginView::AssertValid() const
{
	CView::AssertValid();
}

void CMyCADPluginView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyCADPluginDoc* CMyCADPluginView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyCADPluginDoc)));
	return (CMyCADPluginDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyCADPluginView 消息处理程序

// CCADPluginView 消息处理程序
void CMyCADPluginView::OnPlubinCommand(UINT nID)
{
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_pCurFactory = pMainFrame->m_PluginList[nID];
}


void CMyCADPluginView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bAdd)
	{
		m_pCurShape->OnLButtonUp(nFlags,point);
		m_lstShapes.AddTail(m_pCurShape);

		m_pCurShape = NULL;
		InvalidateRect(NULL,FALSE);
		ReleaseCapture();
		m_bAdd = FALSE;
	}
	m_bIsMove = FALSE; //停止移动
	CView::OnLButtonUp(nFlags, point);
}


void CMyCADPluginView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bAdd && m_pCurShape != NULL)
	{
		m_pCurShape->OnMouseMove(nFlags,point);
		InvalidateRect(NULL,FALSE);
	}
	CView::OnMouseMove(nFlags, point);
}
