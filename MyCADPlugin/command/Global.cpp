#include "stdafx.h"
#include "Global.h"

void LineLeftRotate(CPoint& ptBegin, CPoint& ptEnd)
{
	CPoint ptCenter;
	ptCenter.x = abs(ptEnd.x + ptBegin.x) / 2;
	ptCenter.y = abs(ptEnd.y + ptBegin.y) / 2;

	int nTempX = ptBegin.x;
	ptBegin.x = ptBegin.y - ptCenter.y + ptCenter.x;
	ptBegin.y = ptCenter.x - nTempX + ptCenter.y;

	nTempX = ptEnd.x;
	ptEnd.x = ptEnd.y - ptCenter.y + ptCenter.x;
	ptEnd.y = ptCenter.x - nTempX + ptCenter.y;
}


void LineRightRotate(CPoint& ptBegin, CPoint& ptEnd)
{
	CPoint ptCenter;
	ptCenter.x = abs(ptEnd.x + ptBegin.x) / 2;
	ptCenter.y = abs(ptEnd.y + ptBegin.y) / 2;

	int nTempX = ptBegin.x;
	ptBegin.x = ptCenter.y - ptBegin.y + ptCenter.x;
	ptBegin.y = nTempX - ptCenter.x + ptCenter.y;

	nTempX = ptEnd.x;
	ptEnd.x = ptCenter.y - ptEnd.y + ptCenter.x;
	ptEnd.y = nTempX - ptCenter.x + ptCenter.y;
}