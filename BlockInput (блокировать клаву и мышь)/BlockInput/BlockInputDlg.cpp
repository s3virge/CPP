
// BlockInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BlockInput.h"
#include "BlockInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int iTimerID = 12;
const int iTimerSecondsID = 11;

class CAboutDlg : public CBDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CBDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBDialog)
END_MESSAGE_MAP()


// CBlockInputDlg dialog




CBlockInputDlg::CBlockInputDlg(CWnd* pParent /*=NULL*/)
	: CBDialog(CBlockInputDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBlockInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CBDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
	DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
	DDX_Control(pDX, IDC_STATIC3, m_wndStatic3);
	DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);
	DDX_Control(pDX, IDC_TIMETOWAIT, m_wndTimeTowait);
}

BEGIN_MESSAGE_MAP(CBlockInputDlg, CBDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BLOCK, &CBlockInputDlg::OnBnClickedBtnBlock)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CBlockInputDlg::OnBnClickedCancel)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CBlockInputDlg message handlers

BOOL CBlockInputDlg::OnInitDialog()
{
	CBDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetBitmapStyle(StyleTile);
	SetBitmap(IDB_BITMAP1);

	GetDlgItem(IDC_EDIT_TIME)->SetWindowText(L"2");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBlockInputDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBlockInputDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBlockInputDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBlockInputDlg::OnBnClickedBtnBlock()
{
	CString csTime;

	//GetDlgItem(IDC_EDIT_TIME)->GetWindowTextW(csTime);
	GetDlgItem(IDC_EDIT_TIME)->GetWindowText(csTime);

	//-----------------------------------------------
	//проверяем на пустоту
	if ( csTime == "0" )
	{
		MessageBox(L"Не установлена продолжительность блокировки.");
		GetDlgItem(IDC_EDIT_TIME)->SetFocus();
		return;
	}

	//-----------------------------------------------
	int iTime = _wtoi(csTime.GetString());
	//int iTime = atoi(csTime.GetString());
	
	iTimeSeconds = iTime * 60;
	
	CString csSeconds;
	csSeconds.Format(_T("%d"), iTimeSeconds);
	
	//GetDlgItem(IDC_TIMETOWAIT)->SetWindowTextW(csSeconds);
	GetDlgItem(IDC_TIMETOWAIT)->SetWindowText(csSeconds);
	GetDlgItem(IDC_TIMETOWAIT)->SetFocus();

	iTime *= 60000;
	//-----------------------------------------------
	
	if ( SetTimer(iTimerID, iTime, NULL) )
	{
		SetTimer(iTimerSecondsID, 1000, NULL);

		::BlockInput(TRUE);
	}
	else
	{
		MessageBox(L"Не удалось создать таймер.");
		//MessageBox("Не удалось создать таймер.");
	}
}

void CBlockInputDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == iTimerID )
	{
		::BlockInput(FALSE);
	}

	if ( nIDEvent == iTimerSecondsID )
	{
		iTimeSeconds--;

		CString csTimeToWait;
		csTimeToWait.Format(_T("%d"), iTimeSeconds);
		
		//GetDlgItem(IDC_TIMETOWAIT)->SetWindowTextW(csTimeToWait);
		GetDlgItem(IDC_TIMETOWAIT)->SetWindowText(csTimeToWait);

		if ( iTimeSeconds == 0 )
		{
			KillTimer(iTimerSecondsID);
			KillTimer(iTimerID);
		}
	}

	CBDialog::OnTimer(nIDEvent);
}

void CBlockInputDlg::OnBnClickedCancel()
{
	KillTimer(iTimerSecondsID);
	KillTimer(iTimerID);
	OnCancel();
}

//перемещаем окно за любую область
void CBlockInputDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//можно перемещать окно мышкой
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	KillTimer(iTimerSecondsID);
	KillTimer(iTimerID);

	GetDlgItem(IDC_TIMETOWAIT)->SetWindowTextW(_T("0"));
	//GetDlgItem(IDC_EDIT_TIME)->SetFocus();

	CBDialog::OnLButtonDown(nFlags, point);
}

HBRUSH CBlockInputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ( nCtlColor ) 
	{
	//case CTLCOLOR_STATIC:
	case CTLCOLOR_EDIT:
	case CTLCOLOR_MSGBOX:
	//case CTLCOLOR_BTN:
		// Set color to green on black and return the backgroundbrush.
		//pDC->SetTextColor(RGB(0, 0, 0));
		//pDC->SetBkColor(RGB(200, 200, 200));
		pDC->SetBkMode(TRANSPARENT);

		hbr = ::CreateSolidBrush(RGB(200, 200, 200)); //задает цвет фона
		break;	
	}

	return hbr;
}
