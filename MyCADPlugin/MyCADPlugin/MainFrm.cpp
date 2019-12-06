
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "MyCADPlugin.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));


	//���ز��
	if (!LoadPlugin()) {
		AfxMessageBox(_T("���ز����������"));
	}


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������
bool CMainFrame::LoadPlugin(void)
{
	//CMenu *pMenu = CMenu::FromHandle(::GetMenu(this->GetSafeHwnd()));
	CMenu *pMenu=this->GetMenu();
	CMenu *pSubMenu = NULL;
	int nId = PLUGIN_MENUID_START;

	// �ڲ˵���������²˵�
	pMenu->AppendMenu(MF_STRING | MF_POPUP,
		(UINT)pMenu->GetSafeHmenu(),
		_T("���(&P)"));

	pSubMenu = pMenu->GetSubMenu(pMenu->GetMenuItemCount() - 1);

	// �����ļ�������Ӳ˵�
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("Plugins\\*.dll"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// ���ز��
		HMODULE hModule = ::LoadLibrary(finder.GetFilePath());
		if (hModule == NULL)
		{
			continue;
		}

		// ��ȡ�������ָ��
		GETPLUGINOBJECT pGetObject = NULL;
		pGetObject = (GETPLUGINOBJECT)::GetProcAddress(hModule,"GetPluginObject");
		if (pGetObject == NULL)
		{
			FreeLibrary(hModule);
			continue;
		}

		// �����µ�ͼ�β������
		IShape* pShape = pGetObject();
		if (pShape == NULL)
		{
			FreeLibrary(hModule);
			continue;
		}
		// ����Ӳ˵�
		pSubMenu->AppendMenu(MF_STRING,
			nId,
			pShape->GetName());

		// ���浽��ϣ��
		m_PluginList[nId++] = pShape;

	}


	return true;
}

