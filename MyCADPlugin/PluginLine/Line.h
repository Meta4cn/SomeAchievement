#pragma once
#include "../command//IShape.h"
#include "../command/ShapeFactory.h"

class CLine : public IShape
{
public:
	CLine(void);
	virtual ~CLine(void);

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
		return _T("Ö±Ïß");
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
	}
public:
	CPoint m_StartPt;
	CPoint m_EndPt;
	LOGPEN m_logpen;
};


class CLineFactory : public CShapeFactory
{
public:
	virtual IShape* CreateShape()
	{
		return new CLine();
	}
};

