#pragma once

#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <vector>

#include <iostream>

#pragma comment(lib, "wbemuuid.lib")

using namespace std;

class WMIDiskDrives
{
public:
	//WMIDiskDrives();
	//~WMIDiskDrives();

	//if error ocurs return string whith error
	wstring Initialize();
	wstring Init();

	void Cleanup();
	wstring GetName(vector<wstring> &);
	wstring GetCaption(vector<wstring> &);
	wstring GetSize(vector<wstring> &);

	int GetCount();

private:
	IWbemLocator *pLoc = 0;
	IWbemServices *pSvc = 0;
	IEnumWbemClassObject* pEnumerator = NULL;

	int m_DrivesCount = 0;

public:
	vector<wstring> m_vDrivesName;
	vector<wstring> m_vDrivesCaption;
	vector<wstring> m_vDrivesSize;
};


int WMIDiskDrives::GetCount()
{
	return m_DrivesCount;
}

wstring WMIDiskDrives::Init()
{
	HRESULT hres;

	// Initialize COM.
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	
	if (FAILED(hres))
	{
		return L"Failed to initialize COM library.";
	}

	// Initialize 
	hres = CoInitializeSecurity(
		NULL,
		-1,      // COM negotiates service                  
		NULL,    // Authentication services
		NULL,    // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
		RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
		NULL,             // Authentication info 
		EOAC_NONE,        // Additional capabilities
		NULL              // Reserved
	);

	if (FAILED(hres))
	{		
		//CoUninitialize();
		Cleanup();
		return L"Failed to initialize security.";
	}

	// Obtain the initial locator to Windows Management
	// on a particular host computer.
	//IWbemLocator *pLoc = 0;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		//CoUninitialize();
		Cleanup();
		return L"Failed to create IWbemLocator object.";
	}

	//IWbemServices *pSvc = 0;

	// Connect to the root\cimv2 namespace with the
	// current user and obtain pointer pSvc
	// to make IWbemServices calls.

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
		NULL,                    // User name
		NULL,                    // User password
		0,                       // Locale
		NULL,                    // Security flags                 
		0,                       // Authority       
		0,                       // Context object
		&pSvc                    // IWbemServices proxy
	);

	if (FAILED(hres))
	{		
		/*pLoc->Release();
		CoUninitialize();*/
		Cleanup();
		return L"Could not connect.";
	}

	// Set the IWbemServices proxy so that impersonation
	// of the user (client) occurs.
	hres = CoSetProxyBlanket(
		pSvc,                         // the proxy to set
		RPC_C_AUTHN_WINNT,            // authentication service
		RPC_C_AUTHZ_NONE,             // authorization service
		NULL,                         // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,       // authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
		NULL,                         // client identity 
		EOAC_NONE                     // proxy capabilities     
	);

	if (FAILED(hres))
	{		
		Cleanup();
		return L"Could not set proxy blanket.";
	}

	return L"";
}

wstring WMIDiskDrives::Initialize()
{
	wstring sError = Init();

	if (!sError.empty())
	{
		return sError;
	}

	GetName(m_vDrivesName);
	GetCaption(m_vDrivesCaption);
	GetSize(m_vDrivesSize);

	return L"";
}

void WMIDiskDrives::Cleanup()
{
	pSvc->Release();
	pLoc->Release();
	CoUninitialize();
}

wstring WMIDiskDrives::GetName(vector<wstring> &DrivesNames)
{
	m_DrivesCount = 0;

	HRESULT hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_DiskDrive"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		Cleanup();
		return L"GetName(). Query failed. ";
	}

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

		m_DrivesCount++;
	
		wstring strDriveName = (const wchar_t*)vtProp.bstrVal;
		DrivesNames.push_back(strDriveName);
			
		VariantClear(&vtProp);
	}

	return L"";
}

wstring WMIDiskDrives::GetCaption(vector<wstring> &DrivesCaption)
{
	m_DrivesCount = 0;

	HRESULT hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_DiskDrive"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		Cleanup();
		return L"GetCaption(). Query failed. ";
	}

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		hres = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		//wcout << "Caption: " << vtProp.bstrVal << endl;

		m_DrivesCount++;

		wstring wstrDrivesCaption = (const wchar_t*)vtProp.bstrVal;
		DrivesCaption.push_back(wstrDrivesCaption);
			
		VariantClear(&vtProp);
	}

	return L"";
}

wstring WMIDiskDrives::GetSize(vector<wstring> &DrivesSize)
{
	m_DrivesCount = 0;

	HRESULT hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_DiskDrive"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		Cleanup();
		return L"GetSize(). Query failed. ";
	}

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		hres = pclsObj->Get(L"Size", 0, &vtProp, 0, 0);
		//wcout << "Size: " << vtProp.bstrVal << endl;

		m_DrivesCount++;

		long long iSize = 0;

		//get drive Capasity in bytes
		if (vtProp.bstrVal != NULL)
		{
			iSize = _wtoi64(vtProp.bstrVal);
		}

		float iCapacityGB = ((iSize / 1024.0) / 1024.0) / 1024.0;
	
		wchar_t str[32];
		swprintf(str, L"%.2f", iCapacityGB);
		
		DrivesSize.push_back(str);
		//DrivesSize.push_back(to_wstring(iCapacityGB));

		VariantClear(&vtProp);
	}

	return L"";
}