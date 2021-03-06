// AnnoyingFilterAPO.cpp : Defines the exported functions for the DLL application.
//

#include "AnnoyingFilterAPO.h"

long AnnoyingFilterAPO::InstanceCounter = 0;
const CRegAPOProperties<1> AnnoyingFilterAPO::regProperties(
	__uuidof(AnnoyingFilterAPO), L"AnnoyingFilterAPO", L"" , 1, 0, __uuidof(IAudioProcessingObject),
	(APO_FLAG)(APO_FLAG_SAMPLESPERFRAME_MUST_MATCH | APO_FLAG_FRAMESPERSECOND_MUST_MATCH | APO_FLAG_BITSPERSAMPLE_MUST_MATCH | APO_FLAG_INPLACE));

AnnoyingFilterAPO::AnnoyingFilterAPO(IUnknown * pUnknownOuter) : CBaseAudioProcessingObject(regProperties)
{
	ReferenceCounter = 1;

	if (pUnknownOuter != NULL)
		this->pUnknownOuter = pUnknownOuter;
	else
	{
		this->pUnknownOuter = reinterpret_cast<IUnknown*>(this);
		unknown_flag = 1;
	}
		

	InterlockedIncrement(&InstanceCounter);
}


AnnoyingFilterAPO::~AnnoyingFilterAPO()
{
	InterlockedDecrement(&InstanceCounter);
}


HRESULT __stdcall AnnoyingFilterAPO::QueryInterface(const IID & iid, void ** ppv)
{
	if(unknown_flag=0)	return pUnknownOuter->QueryInterface(iid, ppv);
	else return S_FALSE;
}

ULONG __stdcall AnnoyingFilterAPO::AddRef()
{
	if (unknown_flag = 0)	return pUnknownOuter->AddRef();
	else return S_FALSE;
}

ULONG __stdcall AnnoyingFilterAPO::Release()
{
	if (unknown_flag = 0)	return pUnknownOuter->Release();
	else return S_FALSE;
}

HRESULT __stdcall AnnoyingFilterAPO::GetLatency(HNSTIME * pTime)
{
	if (!pTime)
		return E_POINTER;

	if (!m_bIsLocked)
		return APOERR_ALREADY_UNLOCKED;

	*pTime = 0;

	return S_OK;
}

HRESULT __stdcall AnnoyingFilterAPO::Initialize(UINT32 cbDataSize, BYTE * pbyData)
{
	if ((NULL == pbyData) && (0 != cbDataSize))
		return E_INVALIDARG;
	if ((NULL != pbyData) && (0 == cbDataSize))
		return E_POINTER;
	if (cbDataSize != sizeof(APOInitSystemEffects))
		return E_INVALIDARG;

	return S_OK;
}

HRESULT __stdcall AnnoyingFilterAPO::IsInputFormatSupported(IAudioMediaType * pOutputFormat, IAudioMediaType * pRequestedInputFormat, IAudioMediaType ** ppSupportedInputFormat)
{
	if (!pRequestedInputFormat)
		return E_POINTER;

	UNCOMPRESSEDAUDIOFORMAT inFormat;
	HRESULT hr = pRequestedInputFormat->GetUncompressedAudioFormat(&inFormat);
	if (FAILED(hr))
	{
		return hr;
	}

	UNCOMPRESSEDAUDIOFORMAT outFormat;
	hr = pOutputFormat->GetUncompressedAudioFormat(&outFormat);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = CBaseAudioProcessingObject::IsInputFormatSupported(pOutputFormat, pRequestedInputFormat, ppSupportedInputFormat);

	return hr;
}

HRESULT __stdcall AnnoyingFilterAPO::LockForProcess(UINT32 u32NumInputConnections, APO_CONNECTION_DESCRIPTOR ** ppInputConnections, UINT32 u32NumOutputConnections, APO_CONNECTION_DESCRIPTOR ** ppOutputConnections)
{
	HRESULT hr;

	UNCOMPRESSEDAUDIOFORMAT outFormat;
	hr = ppOutputConnections[0]->pFormat->GetUncompressedAudioFormat(&outFormat);
	if (FAILED(hr))
		return hr;

	hr = CBaseAudioProcessingObject::LockForProcess(u32NumInputConnections, ppInputConnections,
		u32NumOutputConnections, ppOutputConnections);
	if (FAILED(hr))
		return hr;

	channelCount = outFormat.dwSamplesPerFrame;

	return hr;
}

HRESULT __stdcall AnnoyingFilterAPO::UnlockForProcess(void)
{
	return CBaseAudioProcessingObject::UnlockForProcess();
}

void __stdcall AnnoyingFilterAPO::APOProcess(UINT32 u32NumInputConnections, APO_CONNECTION_PROPERTY ** ppInputConnections, UINT32 u32NumOutputConnections, APO_CONNECTION_PROPERTY ** ppOutputConnections)
{

	for (UINT32 channel = 0; channel < u32NumInputConnections; channel++)
	{
		auto buffer_flag = ppInputConnections[channel]->u32BufferFlags;

		if (buffer_flag == BUFFER_VALID)
		{
			float* inputFrames = reinterpret_cast<float*>(ppInputConnections[0]->pBuffer);
			float* outputFrames = reinterpret_cast<float*>(ppOutputConnections[0]->pBuffer);

			

			for (unsigned j = 0; j < channelCount; j++)
			{
				for (unsigned i = 0; i < ppOutputConnections[0]->u32ValidFrameCount; i++)
				{

					float s = inputFrames[i * channelCount + j];

					// process audio sample
					
					s += previous_val;
					s *= 0.8;

					previous_val = s;

					outputFrames[i * channelCount + j] = s;

				}
			}

			ppOutputConnections[0]->u32ValidFrameCount = ppInputConnections[0]->u32ValidFrameCount;
			ppOutputConnections[0]->u32BufferFlags = ppInputConnections[0]->u32BufferFlags;

		}
		if (buffer_flag == BUFFER_INVALID)
		{
			ppOutputConnections[0]->u32ValidFrameCount = ppInputConnections[0]->u32ValidFrameCount;
			ppOutputConnections[0]->u32BufferFlags = ppInputConnections[0]->u32BufferFlags;
		}
		if (buffer_flag == BUFFER_SILENT)
		{
			ppOutputConnections[0]->u32ValidFrameCount = ppInputConnections[0]->u32ValidFrameCount;
			ppOutputConnections[0]->u32BufferFlags = ppInputConnections[0]->u32BufferFlags;
		}
	}

	switch (ppInputConnections[0]->u32BufferFlags)
	{
	case BUFFER_VALID:
	{
		float* inputFrames = reinterpret_cast<float*>(ppInputConnections[0]->pBuffer);
		float* outputFrames = reinterpret_cast<float*>(ppOutputConnections[0]->pBuffer);

		for (unsigned i = 0; i < ppOutputConnections[0]->u32ValidFrameCount; i++)
		{
			for (unsigned j = 0; j < channelCount; j++)
			{
				float s = inputFrames[i * channelCount + j];

				// process audio sample
				s *= 0.01f * abs(((int)i % 200) - 100);

				outputFrames[i * channelCount + j] = s;
			}
		}

		ppOutputConnections[0]->u32ValidFrameCount = ppInputConnections[0]->u32ValidFrameCount;
		ppOutputConnections[0]->u32BufferFlags = ppInputConnections[0]->u32BufferFlags;

		break;
	}
	case BUFFER_SILENT:
		ppOutputConnections[0]->u32ValidFrameCount = ppInputConnections[0]->u32ValidFrameCount;
		ppOutputConnections[0]->u32BufferFlags = ppInputConnections[0]->u32BufferFlags;

		break;
	}
	
}
