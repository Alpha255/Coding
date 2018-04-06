#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"

class AppBox : public D3DApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_Box;
};
