#include "stdafx.h"
#include "ClassFactory.h"
#include "AnnoyingFilterAPO.h"



ClassFactory::ClassFactory()
{
	refcounter=1;
}

HRESULT __stdcall ClassFactory::QueryInterface(const IID & iid, void ** ppv)
{
	if (iid == __uuidof(IUnknown) || iid == __uuidof(IClassFactory))
		*ppv = static_cast<IClassFactory*>(this);
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall ClassFactory::AddRef()
{
	return InterlockedIncrement(&refcounter);
}

ULONG __stdcall ClassFactory::Release()
{
	InterlockedDecrement(&refcounter);

	if (refcounter == 0)
	{
		delete this;
		return 0;
	}

	return refcounter;
}

HRESULT __stdcall ClassFactory::CreateInstance(IUnknown * pUnknownOuter, const IID & iid, void ** ppv)
{
	if (pUnknownOuter != NULL && iid != __uuidof(IUnknown))
		return E_NOINTERFACE;

	AnnoyingFilterAPO* apo = new AnnoyingFilterAPO(pUnknownOuter);
	if (apo == NULL)
		return E_OUTOFMEMORY;

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	
	return S_OK;
}

HRESULT __stdcall ClassFactory::LockServer(BOOL Lock)
{
	if (Lock)
		InterlockedIncrement(&lockcount);
	else
		InterlockedDecrement(&lockcount);

	return S_OK;
}
