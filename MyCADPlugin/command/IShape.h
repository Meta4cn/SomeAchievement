#ifndef __ISHAPE__H__
#define __ISHAPE__H__
#if _MSC_VER > 1000
#pragma once
#endif

#define PLUGIN_MENUID_START			10000
#define PLUGIN_MENUID_RANGE			20000

class IShape 
{
public:
	virtual void OnLButtonDown(UINT nFlags, CPoint point) = 0;
	virtual void OnLButtonUp(UINT nFlags, CPoint point) = 0;
	virtual void OnMouseMove(UINT nFlags, CPoint point) = 0;
	virtual void OnDraw(CDC* pDC) = 0;
	virtual void onDrawSelect(CDC* pDC) = 0;
	virtual BOOL IsSelected(CPoint pt) = 0;
	virtual void SetPenBrush(LOGPEN& logpen, LOGBRUSH& logbrush) = 0;
	virtual void SetMovePoint(CPoint& selectPt,CPoint nowPt) = 0;
	virtual void SetPoint(CPoint& startPt,CPoint& endPt) = 0;
	virtual void GetPoint(CPoint& startPt,CPoint& endPt) = 0;
	virtual void LeftRotate() = 0;
	virtual const TCHAR* GetName() = 0;
	virtual IShape* CreateNew() = 0;
	virtual void Release() = 0;
};

IShape* GetPluginObject();
typedef IShape* (*GETPLUGINOBJECT)();

#endif