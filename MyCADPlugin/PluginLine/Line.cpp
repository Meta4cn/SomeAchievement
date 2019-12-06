#include "StdAfx.h"
#include "Line.h"
#include "../command/Global.h"


CLine::CLine(void)
{
	m_logpen.lopnStyle = PS_SOLID;
	m_logpen.lopnColor = RGB(0,0,0);
	m_logpen.lopnWidth.x = 1;
}


CLine::~CLine(void)
{
}

void CLine::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_StartPt = point;
}
void CLine::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_EndPt = point;
}
void CLine::OnMouseMove(UINT nFlags, CPoint point)
{
	m_EndPt = point;
}
void CLine::OnDraw(CDC* pDC)
{
	CPen pen;
	pen.CreatePenIndirect(&m_logpen);
	CPen* nOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(m_StartPt);
	pDC->LineTo(m_EndPt);
	pDC->SelectObject(nOldPen);
}


void CLine::onDrawSelect(CDC* pDC)
{
	const int nDelta = 5; //��Χֵ

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

	// ���Ͻ�
	rcSelect.left = m_StartPt.x- nDelta;
	rcSelect.top = m_StartPt.y - nDelta;
	rcSelect.right = m_StartPt.x + nDelta;
	rcSelect.bottom = m_StartPt.y  + nDelta;
	pDC->Rectangle(&rcSelect);

	// ���½�
	rcSelect.left = m_EndPt.x - nDelta;
	rcSelect.top = m_EndPt.y - nDelta;
	rcSelect.right = m_EndPt.x + nDelta;
	rcSelect.bottom = m_EndPt.y + nDelta;
	pDC->Rectangle(&rcSelect);

	DeleteObject(penForSelected);
	DeleteObject(brush);

	//�����Լ���ֱ��
	OnDraw(pDC);
}
BOOL CLine::IsSelected(CPoint pt)
{
	const int nDelta = 5; //��Χֵ
	const int nPointCnt = 4; //ƽ���ı��ε�ĸ���
	POINT aryPoints[nPointCnt];//�洢ƽ���ı��ε��ĸ���

	//��һ����
	aryPoints[0].x = m_StartPt.x + nDelta;
	aryPoints[0].y = m_StartPt.y - nDelta;


	//�ڶ�����
	aryPoints[1].x = m_EndPt.x + nDelta;
	aryPoints[1].y = m_EndPt.y - nDelta;

	//��������
	aryPoints[2].x = m_EndPt.x - nDelta;
	aryPoints[2].y = m_EndPt.y + nDelta;

	//���ĸ���
	aryPoints[3].x = m_StartPt.x - nDelta;
	aryPoints[3].y = m_StartPt.y + nDelta;

	//����һ��ƽ���ı���
	CRgn rgn;
	rgn.CreatePolygonRgn(aryPoints, nPointCnt, ALTERNATE);

	//�жϵ��Ƿ�����ƽ���ı����ڲ�
	return rgn.PtInRegion(pt);
}


void CLine::SetMovePoint(CPoint& selectPt,CPoint nowPt)
{
	int xPos = nowPt.x - selectPt.x;
	int yPos = nowPt.y - selectPt.y;
	m_StartPt.x += xPos;
	m_StartPt.y += yPos;
	m_EndPt.x += xPos;
	m_EndPt.y += yPos;
	selectPt.x += xPos;
	selectPt.y += yPos;
}


void CLine::LeftRotate()
{
	LineLeftRotate(m_StartPt, m_EndPt);
}

void CLine::SetPoint(CPoint& startPt,CPoint& endPt)
{
	m_StartPt = startPt;
	m_EndPt = endPt;
}

void CLine::GetPoint(CPoint& startPt,CPoint& endPt)
{
	startPt = m_StartPt;
	endPt = m_EndPt;
}

IShape* CLine::CreateNew()
{
	return new CLine();
}