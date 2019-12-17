#pragma once

#include "stdafx.h"
#include <BaseAudioProcessingObject.h>

class __declspec (uuid("61a2c80b-e2cc-40a1-8f0b-6702c5fa2831")) AnnoyingFilterAPO :
	public CBaseAudioProcessingObject
{
public:

	AnnoyingFilterAPO(IUnknown* pUnknownOuter);
	~AnnoyingFilterAPO();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;

	// IAudioProcessingObject
	HRESULT __stdcall GetLatency(HNSTIME* pTime) override;
	HRESULT __stdcall Initialize(UINT32 cbDataSize, BYTE* pbyData) override;
	HRESULT __stdcall IsInputFormatSupported(IAudioMediaType* pOutputFormat,
		IAudioMediaType* pRequestedInputFormat, IAudioMediaType** ppSupportedInputFormat) override;

	// IAudioProcessingObjectConfiguration
	HRESULT __stdcall LockForProcess(UINT32 u32NumInputConnections,
		APO_CONNECTION_DESCRIPTOR** ppInputConnections, UINT32 u32NumOutputConnections,
		APO_CONNECTION_DESCRIPTOR** ppOutputConnections) override;
	HRESULT __stdcall UnlockForProcess(void) override;

	// IAudioProcessingObjectRT
	void __stdcall APOProcess(UINT32 u32NumInputConnections,
		APO_CONNECTION_PROPERTY** ppInputConnections, UINT32 u32NumOutputConnections,
		APO_CONNECTION_PROPERTY** ppOutputConnections) override;


	static long InstanceCounter;
	static const CRegAPOProperties<1> regProperties;

private:
	long ReferenceCounter;
	IUnknown* pUnknownOuter;
	int unknown_flag = 0;
	unsigned channelCount;
	double previous_val = 0.0;

};
