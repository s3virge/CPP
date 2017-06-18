
// RWMBRDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RWMBR.h"
#include "RWMBRDlg.h"
#include "afxdialogex.h"
#include "WMIDiskDrives.h"

#include <strsafe.h>

#include "HexString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}
// CAboutDlg dialog used for App About //////////////////////////////////

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CRWMBRDlg dialog ///////////////////////////////////////////////////////


CRWMBRDlg::CRWMBRDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RWMBR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRWMBRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRWMBRDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ReadMBR, &CRWMBRDlg::OnBnClickedBtnReadmbr)
END_MESSAGE_MAP()


// CRWMBRDlg message handlers

BOOL CRWMBRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	
	///////////////////////////////////////////////////////////////

	WMIDiskDrives diskDrives;
	wstring wstrError = diskDrives.Initialize();

	if (!wstrError.empty())
	{
		MessageBox(wstrError.c_str(), L"An error occurred");
	}

	diskDrives.Cleanup();

	//-----------------------------------------------------------
	CComboBox* wndDrives = (CComboBox*)GetDlgItem(IDC_DRIVES);
	wstring str;

	for (int i = 0; i < diskDrives.GetCount(); i++)
	{
		str = diskDrives.m_vDrivesCaption[i].c_str();
		str += L"  ("; 
		str += diskDrives.m_vDrivesSize[i].c_str();
		str += L" GB)";

		wndDrives->InsertString(i, str.c_str());
	}

	wndDrives->SetCurSel(0);

	///////////////////////////////////////////////////
	m_nBytesPerRow = 0x08;
	m_bShowAllChars = TRUE;


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRWMBRDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRWMBRDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRWMBRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRWMBRDlg::OnBnClickedBtnReadmbr()
{
	///Read mbr from selected drive in combobox
	CComboBox* wndDrives = (CComboBox*)GetDlgItem(IDC_DRIVES);
	int iCurSel = wndDrives->GetCurSel();

	//объект создается несклько раз.
	WMIDiskDrives disk;
	disk.Initialize();

	HANDLE hFile =	CreateFile(
		disk.m_vDrivesName[iCurSel].c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorExit(L"CreateFile()");
	}

	unsigned char cBuffer[1024] = {0};
	DWORD dwBytesRead; // number of bytes that were read
	
	ReadFile(hFile, cBuffer, 512, &dwBytesRead, NULL);

	CString csHex, buff;

	for (int i = 0; i < dwBytesRead; i++)
	{
		for (size_t c = 0; c < 16; c++)
		{
			buff.Format(L"%02hX ", cBuffer[i]);
			csHex += buff;
			
			if (c == 7)
			{
				csHex += " ";
			}

			i++;
		}

		csHex += "\r\n";
	}
	
	GetHexString((TCHAR*)csHex.GetString(), (BYTE*)csHex.GetLength(), 24);

	SetDlgItemText(IDC_EDIT, csHex);

	disk.Cleanup();

	CloseHandle(hFile);
}

// Hex character lookup table
static const TCHAR szHexDigits[] = _T("0123456789ABCDEF");

//// Creates a 4 digit, hexidecimal representation of nOffset
void CRWMBRDlg::GetOffsetString(TCHAR* sBuffer, int nOffset)
{
	*sBuffer++ = szHexDigits[(nOffset >> (4 * 3)) & 0xf];
	*sBuffer++ = szHexDigits[(nOffset >> (4 * 2)) & 0xf];
	*sBuffer++ = szHexDigits[(nOffset >> (4 * 1)) & 0xf];
	*sBuffer++ = szHexDigits[nOffset & 0xf];
}


// Creates a string with two hex characters followed by a space for each byte
// in lpBuffer. (Spaces are appended if nCount < bytes per row.)
void CRWMBRDlg::GetHexString(TCHAR* sBuffer, BYTE* lpBuffer, int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		int j = (TCHAR)*lpBuffer++;
		*sBuffer++ = szHexDigits[(j >> 4) & 0xf];
		*sBuffer++ = szHexDigits[j & 0xf];
		*sBuffer++ = ' ';
	}
	wmemset(sBuffer, ' ', (m_nBytesPerRow - nCount) * 3);
}

// Creates a string with an ASCII character for each byte in lpBuffer.
// (Spaces are appended if nCount < bytes per row.)
void CRWMBRDlg::GetChrString(TCHAR* sBuffer, BYTE* lpBuffer, int nCount)
{
	if (m_bShowAllChars)
	{
		for (int i = 0; i < nCount; i++)
			*sBuffer++ = (TCHAR)*lpBuffer++;
		wmemset(sBuffer, ' ', (m_nBytesPerRow - nCount) * 3);
	}
	else
	{
		for (int i = 0; i < nCount; i++)
		{
			TCHAR c = (TCHAR)*lpBuffer++;
			if (c < ' ' || c > '~')
				c = '.';
			*sBuffer++ = c;
		}
		wmemset(sBuffer, ' ', (m_nBytesPerRow - nCount) * 3);
	}
}


