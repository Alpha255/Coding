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

	static void __stdcall SetEnableDisplacementMap(const void *pData, void*)
	{
		m_bDisplacementMap = *static_cast<const bool*>(pData);
	}

	static void __stdcall GetEnableDisplacementMap(void *pData, void*)
	{
		*static_cast<bool*>(pData) = m_bDisplacementMap;
	}
private:
	static bool m_bDisplacementMap;
};
