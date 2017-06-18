
// BlockInputDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "BDialog.h"
#include "TransparentStatic.h"

// CBlockInputDlg dialog
class CBlockInputDlg : public CBDialog
{
// Construction
public:
	CBlockInputDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BLOCKINPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	HICON m_hIcon;
	int iTimeSeconds;
	CTransparentStatic m_wndStatic1, m_wndStatic2, m_wndStatic3, m_wndStatic4, m_wndTimeTowait;

	DECLARE_MESSAGE_MAP()
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnBlock();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	
};
