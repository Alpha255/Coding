#pragma once

#include <IApplication.h>

class ApplicationMapping : public IApplication
{
public:
	ApplicationMapping() = default;
	~ApplicationMapping() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapsedTime, float totalTime);
	virtual void ResizeWindow(uint32_t width, uint32_t height);

	virtual void MouseMove(WPARAM wParam, int x, int y);
protected:
	enum eMappingType
	{
		eNormalMap,
		eDisplacementMap
	};

	void InitGeometriesResource();
private:
	int m_MappingType = eNormalMap;
	bool m_bWireframe = false;
};
