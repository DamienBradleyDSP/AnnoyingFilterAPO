// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <string>

#include "AnnoyingFilterAPO.h"
#include "ClassFactory.h"

static HINSTANCE hModule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		::hModule = hModule;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

STDAPI DllCanUnloadNow()
{
	if (AnnoyingFilterAPO::InstanceCounter == 0) return S_OK;
	else return S_FALSE;
}

STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
	if (clsid != __uuidof(AnnoyingFilterAPO))
		return CLASS_E_CLASSNOTAVAILABLE;

	ClassFactory* factory = new ClassFactory();
	if (factory == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = factory->QueryInterface(iid, ppv);
	factory->Release();

	return hr;
}

STDAPI DllRegisterServer()
{

	wchar_t filename[1024];
	GetModuleFileNameW(hModule, filename, sizeof(filename) / sizeof(wchar_t));

	HRESULT hr = RegisterAPO(AnnoyingFilterAPO::regProperties);
	if (FAILED(hr))
	{
		UnregisterAPO(__uuidof(AnnoyingFilterAPO));
		return hr;
	}

	wchar_t* guid;
	StringFromCLSID(__uuidof(AnnoyingFilterAPO), &guid);
	std::wstring guidString(guid);
	CoTaskMemFree(guid);

	HKEY keyHandle;
	RegCreateKeyExW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Classes\\CLSID\\" + guidString).c_str(), 0, NULL, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, NULL, &keyHandle, NULL);
	const wchar_t* value = L"AnnoyingFilterAPO";
	RegSetValueExW(keyHandle, L"", 0, REG_SZ, (const BYTE*)value, (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
	RegCloseKey(keyHandle);

	RegCreateKeyExW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Classes\\CLSID\\" + guidString + L"\\InprocServer32").c_str(), 0, NULL, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, NULL, &keyHandle, NULL);
	value = filename;
	RegSetValueExW(keyHandle, L"", 0, REG_SZ, (const BYTE*)value, (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
	value = L"Both";
	RegSetValueExW(keyHandle, L"ThreadingModel", 0, REG_SZ, (const BYTE*)value, (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
	RegCloseKey(keyHandle);

	return S_OK;

}

STDAPI DllUnregisterServer()
{
	wchar_t* guid;
	StringFromCLSID(__uuidof(AnnoyingFilterAPO), &guid);
	std::wstring guidString(guid);
	CoTaskMemFree(guid);

	RegDeleteKeyExW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Classes\\CLSID\\" + guidString + L"\\InprocServer32").c_str(), KEY_WOW64_64KEY, 0);
	RegDeleteKeyExW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Classes\\CLSID\\" + guidString).c_str(), KEY_WOW64_64KEY, 0);

	HRESULT hr = UnregisterAPO(__uuidof(AnnoyingFilterAPO));

	return hr;
}

