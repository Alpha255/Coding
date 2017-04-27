#pragma once

#include <IApplication.h>

class ApplicationFXAA : public IApplication
{
public:
	ApplicationFXAA(HINSTANCE hInstance, LPCWSTR lpTitle);
	~ApplicationFXAA() {}

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
	void CreateTextures();
	void CreateViews();
	void CreateShaders();
	void CreateSamplerStates();
	void CreateBlendStates();
	void CreateRasterizerStates();
	void CreateConstantsBuffers();
private:
};
