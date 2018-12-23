#pragma once

#define UsingD3D11

#include "Definitions.h"
#include "IApplication.h"
#include "D3DEngine.h"
#include "D3D11ImGUI.h"

class ID3DApplication : public IApplication
{
public:
	virtual void SetupRenderEngine() override
	{
		m_Engine.reset(&D3DEngine::Instance());
		m_ImGUI.reset(new D3D11ImGUI());
	}
protected:
private:
};

typedef ID3DApplication RApplication;
