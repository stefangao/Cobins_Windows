﻿
// mmi_exeDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "mmi_exe.h"
#include "mmi_exeDlg.h"
#include "afxdialogex.h"
#include "llfsm/lianli.h"
#include "cobins.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

USING_NS_COB;

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CmmiexeDlg::CmmiexeDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MMI_EXE_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmmiexeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CmmiexeDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CmmiexeDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CmmiexeDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CmmiexeDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CmmiexeDlg::OnBnClickedButton4)
    ON_MESSAGE(WSH_MSG_KEY, &CmmiexeDlg::OnWshMsgKey)
    ON_BN_CLICKED(IDC_BUTTON5, &CmmiexeDlg::OnBnClickedButton5)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CmmiexeDlg 消息处理程序

BOOL CmmiexeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    Kb_StartHook(m_hWnd);
    m_pAppDelegate = new AppDelegate();
    m_pAppDelegate->create(m_hWnd, "HostApp").start();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmmiexeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmmiexeDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmmiexeDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

HWND CmmiexeDlg::GetWndByCursor()
{
    POINT point;
    GetCursorPos(&point);
    HWND hGameWnd = WBS_GetWindowFromPoint(NULL, point, 0);
    return hGameWnd;
}

void CmmiexeDlg::OnBnClickedButton1()
{
    EvtStream dataS;
    std::string robotName = "AutoRefresh";
    dataS << robotName;
    m_pAppDelegate->postEvent("StartRobotEvt", dataS);
}

void CmmiexeDlg::OnBnClickedButton2()
{
    EvtStream dataS;
    std::string robotName = "AutoRefresh";
    dataS << robotName;

    EvtStream embedDataS;
    embedDataS << 6789 << "Thinking widely instead of deeply";
    dataS << "TestEvt1" << embedDataS;
    m_pAppDelegate->postEvent("PostRobotEvt", dataS);
}

void CmmiexeDlg::OnBnClickedButton3()
{
	EvtStream dataS;
	std::string robotName = "AutoRefresh";
	dataS << robotName;

	EvtStream embedDataS;
	embedDataS << 'T';
	dataS << "PressKeyEvt" << embedDataS;
	m_pAppDelegate->postEvent("PostRobotEvt", dataS);

	/*
    ValueMap vm;
    vm.at("a/b") = 12;
    vm.at("a/c") = "hello";
    vm.at("a/d/e") = 789;
    auto str = vm.toJson();
    COBLOG("str=%s\n", str.c_str());*/
}

void CmmiexeDlg::OnBnClickedButton4()
{
    COBLOG("Wait E\n");
    WBS_Wait(2000);
    COBLOG("Wait X\n");

    /*
    EvtStream dataS;
    std::string robotName = "AutoRefresh";
    dataS << robotName;
    m_pAppDelegate->postEvent("StopRobotEvt", dataS);*/
}

afx_msg LRESULT CmmiexeDlg::OnWshMsgKey(WPARAM wParam, LPARAM lParam)
{
    if (lParam & 0x80000000)  //when key up
    {
        UINT keyCode = wParam;
        //COBLOG("OnWshMsgKey: keyCode=%x", keyCode);
        switch (keyCode)
        {
        case 'H':
        {
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                HWND hGameWnd = GetWndByCursor();
                if (hGameWnd)
                {
                    EvtStream data;
                    data << (DWORD)hGameWnd;
                    m_pAppDelegate->postEvent("BindEvt", data);
                }
            }
        }
        break;

        case 'K':
        {
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                HWND hGameWnd = GetWndByCursor();
				COBLOG("hGameWnd=%x\n", hGameWnd);

				EvtStream dataS;
				std::string robotName = "AutoRefresh";
				dataS << robotName;

				EvtStream embedDataS;
				embedDataS << 'F';
				dataS << "PressKeyEvt" << embedDataS;
				m_pAppDelegate->postEvent("PostRobotEvt", dataS);
            }
        }
        break;
        }
    }

    return 0;
}

void CmmiexeDlg::OnBnClickedButton5()
{

}

void CmmiexeDlg::OnDestroy()
{
	m_pAppDelegate->destroy();

	CDialogEx::OnDestroy();
}
