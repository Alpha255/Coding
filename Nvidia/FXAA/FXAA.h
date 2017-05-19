#pragma once

#include <IApplication.h>

class ApplicationFXAA : public IApplication
{
public:
	ApplicationFXAA();
	~ApplicationFXAA() {}

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
	void CreateTextures();
	void CreateViews();
	void CreateInputLayoutAndShaders();
	void CreateMesh();
	void CreateStates();
	void CreateConstantsBuffers();
	void SetupD3D();
private:
};
