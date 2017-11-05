#pragma once

#include <IApplication.h>

class ApplicationLighting : public IApplication
{
public:
	ApplicationLighting() = default;
	~ApplicationLighting() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
private:
	uint32_t m_PreLightCount = 1U;
	uint32_t m_CurLightCount = 1U;
	bool m_Wireframe = false;
};
