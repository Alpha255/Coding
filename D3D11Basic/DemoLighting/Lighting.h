#pragma once

#include <IApplication.h>

class ApplicationLighting : public IApplication
{
public:
	ApplicationLighting()
	{
		m_bDrawGUI = true;
	}
	~ApplicationLighting() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
	static void SetWireframe(const void *pData, void*)
	{
		m_Wireframe = *static_cast<const bool*>(pData);
	}

	static void GetWireframe(void* data, void*)
	{
		*static_cast<bool*>(data) = m_Wireframe;
	}
private:
	uint32_t m_PreLightCount = 1U;
	uint32_t m_CurLightCount = 1U;
	static bool m_Wireframe;
};
