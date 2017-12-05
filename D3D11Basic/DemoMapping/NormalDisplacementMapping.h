#pragma once

#include <IApplication.h>

class ApplicationMapping : public IApplication
{
public:
	ApplicationMapping() = default;
	~ApplicationMapping() = default;

	virtual void SetupScene();
	virtual void RenderScene();
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
