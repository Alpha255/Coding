#pragma once

#include <IApplication.h>

class ApplicationCubemap : public IApplication
{
public:
	ApplicationCubemap() = default;
	~ApplicationCubemap() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
private:
};
