#pragma once

#include <IApplication.h>

class ApplicationBox : public IApplication
{
public:
	ApplicationBox(HINSTANCE hInstance, LPCWSTR lpTitle);
	~ApplicationBox() {}

	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
	void Setup();
private:
	bool m_bInited;
};
