#pragma once

#include "D3DApplication.h"

class ApplicationMapping : public D3DApplication
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
	int32_t m_MappingType = eNormalMap;
	bool m_bWireframe = false;
};
