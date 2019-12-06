#pragma once
#include "../command/IShape.h"

class CRectangle : public IShape
{
public:
	CRectangle(void);
	virtual ~CRectangle(void);

	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDraw(CDC* pDC);
	virtual void onDrawSelect(CDC* pDC);
	virtual BOOL IsSelected(CPoint pt);
	virtual void SetMovePoint(CPoint& selectPt,CPoint nowPt);
	virtual void LeftRotate();
	virtual void SetPoint(CPoint& startPt,CPoint& endPt);
	virtual void GetPoint(CPoint& startPt,CPoint& endPt);
	virtual const TCHAR* GetName()
	{
		return _T("¾ØÐÎ");
	}
	virtual IShape* CreateNew();
	virtual void Release()
	{
		delete this;
	}
public:
	void SetPenBrush(LOGPEN& logpen, LOGBRUSH& logbrush)
	{
		m_logpen = logpen;
		m_logbrush = logbrush;
	}
public:
	CRect m_Rect;
	LOGPEN m_logpen;
	LOGBRUSH m_logbrush;
};
