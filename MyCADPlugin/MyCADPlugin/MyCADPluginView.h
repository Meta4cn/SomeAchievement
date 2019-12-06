
// MyCADPluginView.h : CMyCADPluginView ��Ľӿ�
//

#pragma once
#include "../command/IShape.h"


class CMyCADPluginView : public CView
{
protected: // �������л�����
	CMyCADPluginView();
	DECLARE_DYNCREATE(CMyCADPluginView)

// ����
public:
	CMyCADPluginDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMyCADPluginView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
private:
	LOGPEN m_logpen;
	LOGBRUSH m_logbrush;

	IShape* m_pCurFactory;
	IShape* m_pCurShape;
	IShape* m_pSelectShape; //��ѡ�е�ͼ��

	BOOL m_bAdd;
	BOOL m_bSelect;
	BOOL m_bIsMove; //��־,�ƶ�ͼ��

	CPoint m_selectPt;
	CList<IShape*,IShape*> m_lstShapes;
public:
	afx_msg void OnPlubinCommand(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MyCADPluginView.cpp �еĵ��԰汾
inline CMyCADPluginDoc* CMyCADPluginView::GetDocument() const
   { return reinterpret_cast<CMyCADPluginDoc*>(m_pDocument); }
#endif

