
#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DGeometryBuffer.h"
#include "LightController.h"

class AppDeferredShading : public D3DApp
{
public:
	AppDeferredShading() = default;
	~AppDeferredShading() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	D3DGeometryBuffer m_GBuffer;
	LightController m_LightController;
};
