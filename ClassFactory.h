#pragma once
#include <Unknwnbase.h>


class ClassFactory :
	public IClassFactory
{
public:
	ClassFactory();

	//IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IClassFactory
	HRESULT __stdcall CreateInstance(IUnknown* pUnkown, const IID& iid, void** ppv) override;
	HRESULT __stdcall LockServer(BOOL Lock);

	static long lockcount;

private:
	long refcounter;
};

