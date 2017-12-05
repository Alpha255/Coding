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
protected:
	void InitGeometriesResource();
	void InitCubemapCameras(float cx, float cy, float cz);
	void InitCubemapResource();

	void DrawScene(const class Camera &cam, bool bDrawCenterSphere = false);
	void DrawDynamicCubemap();
private:
	bool m_bEnableReflection = true;
};
