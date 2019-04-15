
// WinUpgradeDlg.h: 头文件
//

#pragma once


// CWinUpgradeDlg 对话框
class CWinUpgradeDlg : public CDialogEx
{
// 构造
public:
	CWinUpgradeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINUPGRADE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// 输入的key
	CString m_key;
	// 输入的IV// 输入的IV
	CString m_IV;
	afx_msg void OnBnClickedButtonDecrypt();
};
