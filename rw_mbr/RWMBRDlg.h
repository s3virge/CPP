
// RWMBRDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CRWMBRDlg dialog
class CRWMBRDlg : public CDialogEx
{
// Construction
public:
	CRWMBRDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RWMBR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	int m_nBytesPerRow;
	BOOL m_bShowAllChars;
	
public:
	afx_msg void OnBnClickedBtnReadmbr();

	void GetOffsetString(TCHAR* sBuffer, int nOffset);
	void GetHexString(TCHAR* sBuffer, BYTE* lpBuffer, int nCount);
	void GetChrString(TCHAR* sBuffer, BYTE* lpBuffer, int nCount);
};
