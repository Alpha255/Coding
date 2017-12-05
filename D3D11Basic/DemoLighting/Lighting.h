#pragma once

#include <IApplication.h>

class ApplicationLighting : public IApplication
{
public:
	ApplicationLighting() = default;
	~ApplicationLighting() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
	static void __stdcall SetWireframe(const void *pData, void*)
	{
		m_Wireframe = *static_cast<const bool*>(pData);
	}

	static void __stdcall GetWireframe(void* pData, void*)
	{
		*static_cast<bool*>(pData) = m_Wireframe;
	}
private:
	int32_t m_PreLightCount = 1;
	int32_t m_CurLightCount = 1;
	static bool m_Wireframe;
};
