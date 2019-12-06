
// MainFrm.cpp : CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
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
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));


	//加载插件
	if (!LoadPlugin()) {
		AfxMessageBox(_T("加载插件产生错误"));
	}


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

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


// CMainFrame 消息处理程序
bool CMainFrame::LoadPlugin(void)
{
	//CMenu *pMenu = CMenu::FromHandle(::GetMenu(this->GetSafeHwnd()));
	CMenu *pMenu=this->GetMenu();
	CMenu *pSubMenu = NULL;
	int nId = PLUGIN_MENUID_START;

	// 在菜单栏上添加新菜单
	pMenu->AppendMenu(MF_STRING | MF_POPUP,
		(UINT)pMenu->GetSafeHmenu(),
		_T("插件(&P)"));

	pSubMenu = pMenu->GetSubMenu(pMenu->GetMenuItemCount() - 1);

	// 遍历文件，添加子菜单
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("Plugins\\*.dll"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// 加载插件
		HMODULE hModule = ::LoadLibrary(finder.GetFilePath());
		if (hModule == NULL)
		{
			continue;
		}

		// 获取插件函数指针
		GETPLUGINOBJECT pGetObject = NULL;
		pGetObject = (GETPLUGINOBJECT)::GetProcAddress(hModule,"GetPluginObject");
		if (pGetObject == NULL)
		{
			FreeLibrary(hModule);
			continue;
		}

		// 生成新的图形插件对象
		IShape* pShape = pGetObject();
		if (pShape == NULL)
		{
			FreeLibrary(hModule);
			continue;
		}
		// 添加子菜单
		pSubMenu->AppendMenu(MF_STRING,
			nId,
			pShape->GetName());

		// 保存到哈希表
		m_PluginList[nId++] = pShape;

	}


	return true;
}

