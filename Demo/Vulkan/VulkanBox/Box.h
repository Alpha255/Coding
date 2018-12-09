#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"

class AppBox : public RenderApp
{
public:
	AppBox()
	{
		m_bRenderGUI = false;
	}

	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_Box;
};
