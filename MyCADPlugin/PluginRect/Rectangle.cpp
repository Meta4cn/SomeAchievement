#include "StdAfx.h"
#include "Rectangle.h"
#include "../command/Global.h"

CRectangle::CRectangle(void)
{
	m_logpen.lopnStyle = PS_SOLID;
	m_logpen.lopnColor = RGB(0,0,0);
	m_logpen.lopnWidth.x = 1;

	m_logbrush.lbStyle = BS_NULL;
	m_logbrush.lbColor = RGB(0,0,0);
}


CRectangle::~CRectangle(void)
{
}


void CRectangle::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_Rect.TopLeft() = point;
}
void CRectangle::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Rect.BottomRight() = point;
}
void CRectangle::OnMouseMove(UINT nFlags, CPoint point)
{
	m_Rect.BottomRight() = point;
}
void CRectangle::OnDraw(CDC* pDC)
{
	CPen pen;
	pen.CreatePenIndirect(&m_logpen);
	CPen* nOldPen = pDC->SelectObject(&pen);
	CBrush brush;
	brush.CreateBrushIndirect(&m_logbrush);
	CBrush* nOldbrush = pDC->SelectObject(&brush);
	pDC->Rectangle(&m_Rect);
	pDC->SelectObject(nOldPen);
	pDC->SelectObject(nOldbrush);
}

void CRectangle::onDrawSelect(CDC* pDC)
{
	OnDraw(pDC);
	const int nDelta = 5; //·¶Î§Öµ

	CPen penForSelected;
	penForSelected.CreatePen(PS_DASH, 2, RGB(255, 0, 0));
	CBrush brush;
	LOGBRUSH logBrush;
	logBrush.lbColor = RGB(255,0,0);
	logBrush.lbStyle = BS_NULL;
	brush.CreateBrushIndirect(&logBrush);

	pDC->SelectObject(&penForSelected);
	pDC->SelectObject(&brush);
	CRect rcSelect;

	// ×óÉÏ½Ç
	rcSelect.left = m_Rect.left - nDelta;
	rcSelect.top = m_Rect.top - nDelta;
	rcSelect.right = m_Rect.left + nDelta;
	rcSelect.bottom = m_Rect.top + nDelta;
	pDC->Rectangle(&rcSelect);

	// ÓÒÉÏ½Ç
	rcSelect.left = m_Rect.right - nDelta;
	rcSelect.top = m_Rect.top - nDelta;
	rcSelect.right = m_Rect.right + nDelta;
	rcSelect.bottom = m_Rect.top + nDelta;
	pDC->Rectangle(&rcSelect);

	// ×óÏÂ½Ç
	rcSelect.left = m_Rect.left - nDelta;
	rcSelect.top = m_Rect.bottom- nDelta;
	rcSelect.right = m_Rect.left + nDelta;
	rcSelect.bottom = m_Rect.bottom + nDelta;
	pDC->Rectangle(&rcSelect);

	// ÓÒÏÂ½Ç
	rcSelect.left = m_Rect.right - nDelta;
	rcSelect.top = m_Rect.bottom - nDelta;
	rcSelect.right = m_Rect.right + nDelta;
	rcSelect.bottom = m_Rect.bottom + nDelta;
	pDC->Rectangle(&rcSelect);

	DeleteObject(penForSelected);
	DeleteObject(brush);

}
BOOL CRectangle::IsSelected(CPoint pt)
{
	return m_Rect.PtInRect(pt);
}

void CRectangle::SetMovePoint(CPoint& selectPt,CPoint nowPt)
{
	int xPos = nowPt.x - selectPt.x;
	int yPos = nowPt.y - selectPt.y;

	m_Rect.left += xPos;
	m_Rect.top += yPos;
	m_Rect.right += xPos;
	m_Rect.bottom += yPos;
	selectPt.x += xPos;
	selectPt.y += yPos;
}

void CRectangle::LeftRotate()
{
	LineLeftRotate(m_Rect.TopLeft(), m_Rect.BottomRight());
}


void CRectangle::SetPoint(CPoint& startPt,CPoint& endPt)
{
	m_Rect.TopLeft() = startPt;
	m_Rect.BottomRight() = endPt;
}

void CRectangle::GetPoint(CPoint& startPt,CPoint& endPt)
{
	startPt = m_Rect.TopLeft();
	endPt = m_Rect.BottomRight();
}


IShape* CRectangle::CreateNew()
{
	return new CRectangle();
}
