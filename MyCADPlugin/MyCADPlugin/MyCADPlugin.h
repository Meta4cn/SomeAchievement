
// MyCADPlugin.h : MyCADPlugin Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMyCADPluginApp:
// �йش����ʵ�֣������ MyCADPlugin.cpp
//

class CMyCADPluginApp : public CWinApp
{
public:
	CMyCADPluginApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMyCADPluginApp theApp;
