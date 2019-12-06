
// MyCADPluginView.h : CMyCADPluginView 类的接口
//

#pragma once
#include "../command/IShape.h"


class CMyCADPluginView : public CView
{
protected: // 仅从序列化创建
	CMyCADPluginView();
	DECLARE_DYNCREATE(CMyCADPluginView)

// 特性
public:
	CMyCADPluginDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMyCADPluginView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
private:
	LOGPEN m_logpen;
	LOGBRUSH m_logbrush;

	IShape* m_pCurFactory;
	IShape* m_pCurShape;
	IShape* m_pSelectShape; //被选中的图形

	BOOL m_bAdd;
	BOOL m_bSelect;
	BOOL m_bIsMove; //标志,移动图形

	CPoint m_selectPt;
	CList<IShape*,IShape*> m_lstShapes;
public:
	afx_msg void OnPlubinCommand(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MyCADPluginView.cpp 中的调试版本
inline CMyCADPluginDoc* CMyCADPluginView::GetDocument() const
   { return reinterpret_cast<CMyCADPluginDoc*>(m_pDocument); }
#endif

