#include "D3D11Profile.h"
#include "D3D11Engine.h"

D3D11Event::D3D11Event()
{
	if (!IsValid())
	{
		ID3DUserDefinedAnnotation *pUserDefineAnnotaion = nullptr;
		Check(D3D11Engine::Instance().GetIMContext()->QueryInterface(__uuidof(pUserDefineAnnotaion), reinterpret_cast<void**>(&pUserDefineAnnotaion)));
		Reset(pUserDefineAnnotaion);
	}
}

void D3D11Event::Begin(const std::string &info)
{
	if (IsValid())
	{
		std::wstring wInfo(info.cbegin(), info.cend());
		m_Object->BeginEvent(wInfo.c_str());
	}
}

void D3D11Event::End()
{
	if (IsValid())
	{
		m_Object->EndEvent();
	}
}