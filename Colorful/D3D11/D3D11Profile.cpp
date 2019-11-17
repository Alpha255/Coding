#include "D3D11Profile.h"
#include "D3D11Engine.h"

D3D11Event::D3D11Event(D3D11Context *pContext)
{
	if (!IsValid())
	{
		D3D11Context *pCtx = nullptr;
		if (pContext && pContext->IsValid())
		{
			pCtx = pContext;
		}
		else
		{
			pCtx = &D3D11Engine::Instance().GetIMContext();
		}

		ID3DUserDefinedAnnotation *pUserDefineAnnotaion = nullptr;
		Check(pCtx->Get()->QueryInterface(__uuidof(pUserDefineAnnotaion), reinterpret_cast<void**>(&pUserDefineAnnotaion)));
		Reset(pUserDefineAnnotaion);
	}
}

void D3D11Event::Begin(const std::string &)
{
	//if (IsValid())
	//{
	//	std::wstring wInfo(info.cbegin(), info.cend());
	//	m_Object->BeginEvent(wInfo.c_str());
	//}
}

void D3D11Event::End()
{
	//if (IsValid())
	//{
	//	m_Object->EndEvent();
	//}
}